#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atlas.h"
#include "image.h"

const wchar_t *atlas_file_name(const wchar_t *path)
{
    const wchar_t *name = path;

    for (const wchar_t *p = path; *p != L'\0'; p++) {
        if (*p == L'\\' || *p == L'/') name = p + 1;
    }
    return name;
}

static int compare_paths(const void *a, const void *b)
{
    const wchar_t *path_a = *(const wchar_t *const *)a;
    const wchar_t *path_b = *(const wchar_t *const *)b;
    int order = CompareStringOrdinal(atlas_file_name(path_a), -1, atlas_file_name(path_b), -1, TRUE);

    /* Same name in two folders: fall back to the whole path so the order is still fixed. */
    if (order == CSTR_EQUAL) order = CompareStringOrdinal(path_a, -1, path_b, -1, TRUE);
    return order - CSTR_EQUAL;
}

void atlas_sort(const wchar_t **paths, int count)
{
    if (count > 1) qsort(paths, (size_t)count, sizeof *paths, compare_paths);
}

int atlas_inspect(const wchar_t *const *paths, int count, int *src_w, int *src_h, wchar_t *err, size_t err_len)
{
    int first_w = 0;
    int first_h = 0;

    if (count < 1) {
        swprintf(err, err_len, L"Add at least one PNG image.");
        return -1;
    }
    for (int i = 0; i < count; i++) {
        int w = 0;
        int h = 0;

        if (image_probe(paths[i], &w, &h) != IMAGE_OK) {
            swprintf(err, err_len, L"Could not read %ls as an image.", atlas_file_name(paths[i]));
            return -1;
        }
        if (i == 0) {
            first_w = w;
            first_h = h;
        } else if (w != first_w || h != first_h) {
            swprintf(err, err_len, L"%ls is %d x %d px but %ls is %d x %d px. All images must be the same size.",
                     atlas_file_name(paths[i]), w, h, atlas_file_name(paths[0]), first_w, first_h);
            return -1;
        }
    }
    *src_w = first_w;
    *src_h = first_h;
    return 0;
}

static void blit(Image *canvas, const Image *tile, int x, int y)
{
    size_t row_bytes = (size_t)tile->width * 4;

    for (int row = 0; row < tile->height; row++) {
        unsigned char *dst = canvas->rgba + ((size_t)(y + row) * (size_t)canvas->width + (size_t)x) * 4;

        memcpy(dst, tile->rgba + (size_t)row * row_bytes, row_bytes);
    }
}

static int draw_tiles(const wchar_t *const *paths, int count, const Layout *layout, Image *canvas, wchar_t *err,
                      size_t err_len)
{
    for (int i = 0; i < count; i++) {
        Image tile = {0, 0, NULL};

        if (image_load(paths[i], layout->tile_w, layout->tile_h, &tile) != IMAGE_OK) {
            swprintf(err, err_len, L"Could not read %ls.", atlas_file_name(paths[i]));
            return -1;
        }
        blit(canvas, &tile, (i % layout->cols) * layout->tile_w, (i / layout->cols) * layout->tile_h);
        image_free(&tile);
    }
    return 0;
}

static int save(const Image *canvas, const wchar_t *path, wchar_t *err, size_t err_len)
{
    int status = image_save_webp(canvas, ATLAS_WEBP_QUALITY, path);

    if (status == IMAGE_ERR_ENCODE) swprintf(err, err_len, L"WebP encoding failed.");
    else if (status != IMAGE_OK) swprintf(err, err_len, L"Could not write %ls.", path);
    return status == IMAGE_OK ? 0 : -1;
}

/* Draws the already sorted and checked paths and saves the atlas. */
static int compose(const wchar_t *const *paths, const AtlasRequest *req, wchar_t *err, size_t err_len)
{
    Layout layout;
    Image canvas = {0, 0, NULL};
    int result = -1;

    if (layout_compute(req->count, req->tile_w, req->tile_h, &layout) != 0) {
        swprintf(err, err_len, L"The tile size is invalid or the atlas would exceed %d x %d px.", LAYOUT_MAX_DIM,
                 LAYOUT_MAX_DIM);
        return -1;
    }
    canvas.width = layout.width;
    canvas.height = layout.height;
    canvas.rgba = calloc((size_t)layout.width * (size_t)layout.height, 4); /* zero: empty cells stay transparent */
    if (canvas.rgba == NULL) {
        swprintf(err, err_len, L"Not enough memory for a %d x %d px atlas.", layout.width, layout.height);
        return -1;
    }
    if (draw_tiles(paths, req->count, &layout, &canvas, err, err_len) == 0) {
        result = save(&canvas, req->out_path, err, err_len);
    }
    image_free(&canvas);
    return result;
}

int atlas_build(const AtlasRequest *req, wchar_t *err, size_t err_len)
{
    const wchar_t **sorted;
    int src_w = 0;
    int src_h = 0;
    int result = -1;

    /* Let atlas_inspect explain an empty request before anything is allocated for it. */
    if (req->count < 1) return atlas_inspect(req->paths, req->count, &src_w, &src_h, err, err_len);
    sorted = malloc((size_t)req->count * sizeof *sorted);
    if (sorted == NULL) {
        swprintf(err, err_len, L"Not enough memory.");
        return -1;
    }
    memcpy(sorted, req->paths, (size_t)req->count * sizeof *sorted);
    atlas_sort(sorted, req->count);
    if (atlas_inspect(sorted, req->count, &src_w, &src_h, err, err_len) == 0) {
        result = compose(sorted, req, err, err_len);
    }
    free(sorted);
    return result;
}
