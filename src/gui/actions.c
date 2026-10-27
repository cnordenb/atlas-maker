#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "app.h"
#include "atlas.h"
#include "layout.h"

#define OPEN_BUFFER_LEN 65536 /* room for a few thousand file names from one Add */
#define PATH_LEN 1024
#define TEXT_LEN 1024

static void show_error(const wchar_t *text)
{
    MessageBoxW(app.hwnd, text, APP_TITLE, MB_ICONERROR | MB_OK);
}

static int contains(const wchar_t *path)
{
    for (int i = 0; i < app.count; i++) {
        if (CompareStringOrdinal(app.paths[i], -1, path, -1, TRUE) == CSTR_EQUAL) return 1;
    }
    return 0;
}

/* Takes ownership of path, which may be NULL after a failed allocation. Returns 0, or -1 if memory ran out. */
static int add_path(wchar_t *path)
{
    const wchar_t **grown;

    if (path == NULL) return -1;
    if (contains(path)) {
        free(path);
        return 0;
    }
    grown = realloc(app.paths, (size_t)(app.count + 1) * sizeof *grown);
    if (grown == NULL) {
        free(path);
        return -1;
    }
    app.paths = grown;
    app.paths[app.count++] = path;
    return 0;
}

static wchar_t *join_path(const wchar_t *dir, const wchar_t *name)
{
    size_t dir_len = wcslen(dir);
    size_t name_len = wcslen(name);
    size_t slash = dir_len > 0 && dir[dir_len - 1] != L'\\'; /* a drive root like C:\ already ends in one */
    wchar_t *path = malloc((dir_len + slash + name_len + 1) * sizeof *path);

    if (path == NULL) return NULL;
    memcpy(path, dir, dir_len * sizeof *path);
    if (slash) path[dir_len] = L'\\';
    memcpy(path + dir_len + slash, name, (name_len + 1) * sizeof *path);
    return path;
}

/* The open dialog returns either one full path, or a folder followed by file names, each NUL-terminated. */
static int add_selection(const wchar_t *buffer)
{
    const wchar_t *name = buffer + wcslen(buffer) + 1;

    if (*name == L'\0') return add_path(join_path(L"", buffer));
    for (; *name != L'\0'; name += wcslen(name) + 1) {
        if (add_path(join_path(buffer, name)) != 0) return -1;
    }
    return 0;
}

void inputs_free(void)
{
    for (int i = 0; i < app.count; i++) free((void *)app.paths[i]);
    free(app.paths);
    app.paths = NULL;
    app.count = 0;
}

/* The tile size: the inputs' own size, or the requested width with the height that keeps their aspect ratio. */
static int tile_size(int *tile_w, int *tile_h)
{
    BOOL ok = TRUE;
    UINT width = (UINT)app.src_w;

    if (app.src_w == 0) return -1;
    if (IsDlgButtonChecked(app.hwnd, ID_RESIZE) == BST_CHECKED) width = GetDlgItemInt(app.hwnd, ID_WIDTH, &ok, FALSE);
    if (!ok || width < 1 || width > LAYOUT_MAX_DIM) return -1;
    *tile_w = (int)width;
    *tile_h = layout_scaled_height(app.src_w, app.src_h, *tile_w);
    return 0;
}

void settings_changed(void)
{
    wchar_t summary[TEXT_LEN] = L"";
    wchar_t height[TEXT_LEN] = L"";
    int resize = IsDlgButtonChecked(app.hwnd, ID_RESIZE) == BST_CHECKED;
    int tile_w = 0;
    int tile_h = 0;
    Layout layout;
    int ok = 0;

    if (tile_size(&tile_w, &tile_h) == 0) {
        if (resize) swprintf(height, TEXT_LEN, L"x %d px (aspect ratio kept)", tile_h);
        ok = layout_compute(app.count, tile_w, tile_h, &layout) == 0;
        if (ok) {
            swprintf(summary, TEXT_LEN,
                     L"Atlas: %d x %d grid of %d x %d px tiles = %d x %d px, lossy WebP, quality %d.",
                     layout.cols, layout.rows, tile_w, tile_h, layout.width, layout.height, (int)ATLAS_WEBP_QUALITY);
        } else {
            swprintf(summary, TEXT_LEN, L"Too large: WebP allows at most %d x %d px. Use smaller tiles.",
                     LAYOUT_MAX_DIM, LAYOUT_MAX_DIM);
        }
    } else if (app.src_w > 0) {
        swprintf(summary, TEXT_LEN, L"Enter a width from 1 to %d px.", LAYOUT_MAX_DIM);
    }
    EnableWindow(app.width, resize);
    SetWindowTextW(app.height, height);
    SetWindowTextW(app.summary, summary);
    EnableWindow(app.create, ok && GetWindowTextLengthW(app.output) > 0);
}

void inputs_changed(void)
{
    wchar_t text[TEXT_LEN];

    atlas_sort(app.paths, app.count);
    SendMessageW(app.list, WM_SETREDRAW, FALSE, 0);
    SendMessageW(app.list, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < app.count; i++) {
        SendMessageW(app.list, LB_ADDSTRING, 0, (LPARAM)atlas_file_name(app.paths[i]));
    }
    SendMessageW(app.list, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(app.list, NULL, TRUE);

    app.src_w = 0;
    app.src_h = 0;
    if (app.count == 0) {
        swprintf(text, TEXT_LEN, L"Add PNG images. They must all be the same size.");
    } else if (atlas_inspect(app.paths, app.count, &app.src_w, &app.src_h, text, TEXT_LEN) == 0) {
        swprintf(text, TEXT_LEN, L"%d image%ls, %d x %d px each.", app.count, app.count == 1 ? L"" : L"s",
                 app.src_w, app.src_h);
        if (GetWindowTextLengthW(app.width) == 0) SetDlgItemInt(app.hwnd, ID_WIDTH, (UINT)app.src_w, FALSE);
    }
    SetWindowTextW(app.info, text);
    settings_changed();
}

void on_add(void)
{
    wchar_t *buffer = calloc(OPEN_BUFFER_LEN, sizeof *buffer);
    OPENFILENAMEW ofn;

    if (buffer == NULL) {
        show_error(L"Not enough memory.");
        return;
    }
    ZeroMemory(&ofn, sizeof ofn);
    ofn.lStructSize = sizeof ofn;
    ofn.hwndOwner = app.hwnd;
    ofn.lpstrFilter = L"PNG images (*.png)\0*.png\0";
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = OPEN_BUFFER_LEN;
    ofn.lpstrTitle = L"Add PNG images";
    ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (GetOpenFileNameW(&ofn)) {
        if (add_selection(buffer) != 0) show_error(L"Not enough memory: some files were not added.");
        inputs_changed();
    } else if (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL) {
        show_error(L"Too many files were selected at once. Add them in smaller batches.");
    }
    free(buffer);
}

void on_remove(void)
{
    for (int i = app.count - 1; i >= 0; i--) {
        if (SendMessageW(app.list, LB_GETSEL, (WPARAM)i, 0) > 0) {
            free((void *)app.paths[i]);
            memmove(&app.paths[i], &app.paths[i + 1], (size_t)(app.count - i - 1) * sizeof *app.paths);
            app.count--;
        }
    }
    inputs_changed();
}

void on_clear(void)
{
    inputs_free();
    inputs_changed();
}

void on_browse(void)
{
    wchar_t path[PATH_LEN] = L"atlas.webp";
    wchar_t dir[PATH_LEN] = L"";
    OPENFILENAMEW ofn;

    if (GetWindowTextLengthW(app.output) > 0) {
        GetWindowTextW(app.output, path, PATH_LEN);
    } else if (app.count > 0) {
        /* Start next to the input images. */
        size_t len = (size_t)(atlas_file_name(app.paths[0]) - app.paths[0]);

        if (len < PATH_LEN) wmemcpy(dir, app.paths[0], len);
    }
    ZeroMemory(&ofn, sizeof ofn);
    ofn.lStructSize = sizeof ofn;
    ofn.hwndOwner = app.hwnd;
    ofn.lpstrFilter = L"WebP images (*.webp)\0*.webp\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = PATH_LEN;
    ofn.lpstrInitialDir = dir[0] != L'\0' ? dir : NULL;
    ofn.lpstrDefExt = L"webp";
    ofn.lpstrTitle = L"Save atlas as";
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN;
    if (GetSaveFileNameW(&ofn)) {
        SetWindowTextW(app.output, path);
        settings_changed();
    }
}

void on_create(void)
{
    wchar_t output[PATH_LEN];
    wchar_t text[TEXT_LEN];
    wchar_t err[ATLAS_ERR_LEN] = L"";
    AtlasRequest req = {app.paths, app.count, 0, 0, output};
    Layout layout;
    HCURSOR cursor;
    int result;

    if (tile_size(&req.tile_w, &req.tile_h) != 0) return;
    if (layout_compute(req.count, req.tile_w, req.tile_h, &layout) != 0) return;
    GetWindowTextW(app.output, output, PATH_LEN);
    SetWindowTextW(app.status, L"Creating atlas...");
    UpdateWindow(app.status);
    cursor = SetCursor(LoadCursorW(NULL, IDC_WAIT));
    result = atlas_build(&req, err, ATLAS_ERR_LEN);
    SetCursor(cursor);
    if (result == 0) {
        swprintf(text, TEXT_LEN, L"Saved %d x %d px atlas to %ls", layout.width, layout.height, output);
        SetWindowTextW(app.status, text);
    } else {
        SetWindowTextW(app.status, L"The atlas was not created.");
        show_error(err);
    }
}
