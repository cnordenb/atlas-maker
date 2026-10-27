#include <windows.h>

#include "app.h"

App app;

#define CLASS_NAME L"AtlasMakerWindow"
#define WINDOW_STYLE (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define CLIENT_W 520 /* client size at 96 DPI; everything is scaled to the system DPI */
#define CLIENT_H 440

#define BUTTON (WS_TABSTOP | BS_PUSHBUTTON)
#define LABEL (SS_LEFT | SS_NOPREFIX)

typedef struct {
    const wchar_t *cls;
    const wchar_t *text;
    DWORD style;
    DWORD ex_style;
    int id;
    int x, y, w, h; /* at 96 DPI */
    HWND *handle;   /* where to keep the control's handle, if it is needed later */
} Control;

static const Control controls[] = {
    {L"STATIC", L"Input images, in atlas order (left to right, row by row):", LABEL, 0, 0, 12, 10, 380, 18, NULL},
    {L"LISTBOX", L"", WS_TABSTOP | WS_VSCROLL | LBS_EXTENDEDSEL | LBS_NOINTEGRALHEIGHT, WS_EX_CLIENTEDGE, ID_LIST,
     12, 30, 380, 220, &app.list},
    {L"BUTTON", L"Add PNGs...", BUTTON, 0, ID_ADD, 404, 30, 104, 28, NULL},
    {L"BUTTON", L"Remove", BUTTON, 0, ID_REMOVE, 404, 64, 104, 28, NULL},
    {L"BUTTON", L"Clear", BUTTON, 0, ID_CLEAR, 404, 98, 104, 28, NULL},
    {L"STATIC", L"", LABEL, 0, 0, 12, 258, 496, 32, &app.info},
    {L"BUTTON", L"Resize each image to width:", WS_TABSTOP | BS_AUTOCHECKBOX, 0, ID_RESIZE, 12, 298, 184, 22,
     &app.resize},
    {L"EDIT", L"", WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, ID_WIDTH, 200, 298, 56, 22,
     &app.width},
    {L"STATIC", L"", LABEL, 0, 0, 262, 301, 246, 18, &app.height},
    {L"STATIC", L"Output file:", LABEL, 0, 0, 12, 337, 70, 18, NULL},
    {L"EDIT", L"", ES_READONLY | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, 0, 84, 334, 308, 22, &app.output},
    {L"BUTTON", L"Browse...", BUTTON, 0, ID_BROWSE, 404, 331, 104, 28, NULL},
    {L"STATIC", L"", LABEL, 0, 0, 12, 368, 496, 18, &app.summary},
    {L"STATIC", L"", LABEL | SS_PATHELLIPSIS, 0, 0, 12, 403, 380, 18, &app.status},
    {L"BUTTON", L"Create atlas", BUTTON, 0, ID_CREATE, 404, 396, 104, 30, &app.create},
};

static int dpi = 96;

static int scale(int value)
{
    return MulDiv(value, dpi, 96);
}

static int create_controls(HWND hwnd)
{
    HINSTANCE instance = (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);

    for (size_t i = 0; i < sizeof controls / sizeof controls[0]; i++) {
        const Control *c = &controls[i];
        HWND control = CreateWindowExW(c->ex_style, c->cls, c->text, WS_CHILD | WS_VISIBLE | c->style,
                                       scale(c->x), scale(c->y), scale(c->w), scale(c->h), hwnd,
                                       (HMENU)(INT_PTR)c->id, instance, NULL);

        if (control == NULL) return -1;
        SendMessageW(control, WM_SETFONT, (WPARAM)app.font, FALSE);
        if (c->handle != NULL) *c->handle = control;
    }
    SendMessageW(app.width, EM_SETLIMITTEXT, 5, 0); /* LAYOUT_MAX_DIM has 5 digits */
    return 0;
}

static void on_command(int id, int code)
{
    switch (id) {
    case ID_ADD: on_add(); break;
    case ID_REMOVE: on_remove(); break;
    case ID_CLEAR: on_clear(); break;
    case ID_RESIZE: settings_changed(); break;
    case ID_WIDTH:
        if (code == EN_CHANGE) settings_changed();
        break;
    case ID_BROWSE: on_browse(); break;
    case ID_CREATE: on_create(); break;
    default: break;
    }
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case WM_CREATE:
        app.hwnd = hwnd;
        if (create_controls(hwnd) != 0) return -1;
        inputs_changed();
        return 0;
    case WM_COMMAND:
        on_command(LOWORD(wparam), HIWORD(wparam));
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}

static int start(HINSTANCE instance, int show)
{
    WNDCLASSEXW wc = {0};
    NONCLIENTMETRICSW metrics = {0};
    RECT rect = {0, 0, 0, 0};
    HDC screen = GetDC(NULL);

    dpi = screen != NULL ? GetDeviceCaps(screen, LOGPIXELSY) : 96;
    if (screen != NULL) ReleaseDC(NULL, screen);
    metrics.cbSize = sizeof metrics;
    if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof metrics, &metrics, 0)) return -1;
    app.font = CreateFontIndirectW(&metrics.lfMessageFont);
    wc.cbSize = sizeof wc;
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = CLASS_NAME;
    if (app.font == NULL || !RegisterClassExW(&wc)) return -1;
    rect.right = scale(CLIENT_W);
    rect.bottom = scale(CLIENT_H);
    AdjustWindowRectEx(&rect, WINDOW_STYLE, FALSE, 0);
    if (CreateWindowExW(0, CLASS_NAME, APP_TITLE, WINDOW_STYLE, CW_USEDEFAULT, CW_USEDEFAULT,
                        rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, instance, NULL) == NULL) {
        return -1;
    }
    ShowWindow(app.hwnd, show);
    return 0;
}

int window_run(HINSTANCE instance, int show)
{
    MSG msg = {0};
    BOOL got = FALSE;

    if (start(instance, show) != 0) {
        MessageBoxW(NULL, L"The window could not be created.", APP_TITLE, MB_ICONERROR);
        if (app.font != NULL) DeleteObject(app.font);
        return 1;
    }
    /* Runs until the window is closed. IsDialogMessage gives Tab navigation between the controls. */
    while ((got = GetMessageW(&msg, NULL, 0, 0)) > 0) {
        if (!IsDialogMessageW(app.hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    inputs_free();
    DeleteObject(app.font);
    return got == 0 ? (int)msg.wParam : 1;
}
