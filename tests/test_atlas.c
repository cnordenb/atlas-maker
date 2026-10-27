#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <webp/decode.h>

#include "atlas.h"
#include "check.h"
#include "image.h"

#define PATH_LEN 1024
#define OUT_FILE L"test_atlas_out.webp"

/* Fixtures in tests/data, listed out of order: atlas order is apple, Banana, cherry, date. */
enum { FIX_DATE, FIX_BANANA, FIX_APPLE, FIX_CHERRY, FIX_ODD, FIXTURES };
static const wchar_t *const fixture_names[FIXTURES] = {
    L"date.png", L"Banana.png", L"apple.png", L"cherry.png", L"odd_50x50.png"
};
static wchar_t fixture_paths[FIXTURES][PATH_LEN];
static const wchar_t *fixtures[FIXTURES];

typedef struct {
    int r, g, b, a;
} Color;

static const Color RED = {220, 40, 40, 255};      /* apple */
static const Color GREEN = {40, 180, 60, 255};    /* Banana */
static const Color BLUE = {40, 60, 200, 255};     /* cherry */
static const Color YELLOW = {230, 200, 40, 255};  /* date */

typedef struct {
    int width, height;
    uint8_t *rgba;
} Decoded;

static int decode_output(Decoded *out)
{
    FILE *file = _wfopen(OUT_FILE, L"rb");
    static uint8_t data[1 << 20];
    size_t size;

    if (file == NULL) return -1;
    size = fread(data, 1, sizeof data, file);
    fclose(file);
    out->rgba = WebPDecodeRGBA(data, size, &out->width, &out->height);
    return out->rgba != NULL ? 0 : -1;
}

/* Lossy WebP shifts colors a little, so allow some slack per channel. */
static int close_to(int actual, int expected)
{
    return abs(actual - expected) <= 24;
}

static void check_pixel(const Decoded *img, int x, int y, Color want)
{
    const uint8_t *p = img->rgba + ((size_t)y * (size_t)img->width + (size_t)x) * 4;
    int matches = close_to(p[0], want.r) && close_to(p[1], want.g) && close_to(p[2], want.b) && close_to(p[3], want.a);

    if (!matches) {
        fprintf(stderr, "pixel (%d,%d) is %d,%d,%d,%d, expected %d,%d,%d,%d\n", x, y, p[0], p[1], p[2], p[3],
                want.r, want.g, want.b, want.a);
    }
    CHECK(matches);
}

static int build(int count, int tile, Decoded *out)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    AtlasRequest req = {fixtures, count, tile, tile, OUT_FILE};
    int result = atlas_build(&req, err, ATLAS_ERR_LEN);

    if (result != 0) fwprintf(stderr, L"atlas_build: %ls\n", err);
    return result == 0 ? decode_output(out) : -1;
}

static void test_sort(void)
{
    const wchar_t *paths[4] = {fixtures[FIX_DATE], fixtures[FIX_BANANA], fixtures[FIX_APPLE], fixtures[FIX_CHERRY]};

    atlas_sort(paths, 4);
    CHECK(wcscmp(atlas_file_name(paths[0]), L"apple.png") == 0);
    CHECK(wcscmp(atlas_file_name(paths[1]), L"Banana.png") == 0);
    CHECK(wcscmp(atlas_file_name(paths[2]), L"cherry.png") == 0);
    CHECK(wcscmp(atlas_file_name(paths[3]), L"date.png") == 0);
}

static void test_inspect(void)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    int w = 0;
    int h = 0;

    CHECK(atlas_inspect(fixtures, 4, &w, &h, err, ATLAS_ERR_LEN) == 0);
    CHECK(w == 100 && h == 100);
    CHECK(atlas_inspect(fixtures, 5, &w, &h, err, ATLAS_ERR_LEN) == -1);
    CHECK(wcsstr(err, L"odd_50x50.png") != NULL);
    CHECK(atlas_inspect(fixtures, 0, &w, &h, err, ATLAS_ERR_LEN) == -1);
}

/* Four 100x100 images make a 2x2 atlas, drawn alphabetically left to right, row by row. */
static void test_four_tiles(int tile)
{
    Decoded img = {0, 0, NULL};
    int c = tile / 2;

    CHECK(build(4, tile, &img) == 0);
    if (img.rgba == NULL) return;
    CHECK(img.width == 2 * tile && img.height == 2 * tile);
    check_pixel(&img, c, c, RED);
    check_pixel(&img, tile + c, c, GREEN);
    check_pixel(&img, c, tile + c, BLUE);
    check_pixel(&img, tile + c, tile + c, YELLOW);
    WebPFree(img.rgba);
}

/* Three images (date, Banana, apple) still make a square; the unused last cell is transparent. */
static void test_empty_cell(void)
{
    Decoded img = {0, 0, NULL};

    CHECK(build(3, 100, &img) == 0);
    if (img.rgba == NULL) return;
    CHECK(img.width == 200 && img.height == 200);
    check_pixel(&img, 50, 50, RED);
    check_pixel(&img, 150, 50, GREEN);
    check_pixel(&img, 50, 150, YELLOW);
    CHECK(img.rgba[((size_t)150 * 200 + 150) * 4 + 3] == 0);
    WebPFree(img.rgba);
}

static void test_mismatch_is_rejected(void)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    AtlasRequest req = {fixtures, 5, 100, 100, L"test_atlas_never_written.webp"};

    CHECK(atlas_build(&req, err, ATLAS_ERR_LEN) == -1);
    CHECK(wcsstr(err, L"same size") != NULL);
    CHECK(GetFileAttributesW(L"test_atlas_never_written.webp") == INVALID_FILE_ATTRIBUTES);
}

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: test_atlas <tests/data directory>\n");
        return 2;
    }
    for (int i = 0; i < FIXTURES; i++) {
        swprintf(fixture_paths[i], PATH_LEN, L"%ls/%ls", argv[1], fixture_names[i]);
        fixtures[i] = fixture_paths[i];
    }
    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)) || image_startup() != IMAGE_OK) {
        fprintf(stderr, "could not start WIC\n");
        return 2;
    }
    test_sort();
    test_inspect();
    test_four_tiles(100); /* the example: 4 x 100x100 -> 200x200 */
    test_four_tiles(60);  /* resized: 4 x 60x60 -> 120x120 */
    test_empty_cell();
    test_mismatch_is_rejected();
    image_shutdown();
    CoUninitialize();
    if (check_failures == 0) printf("test_atlas: all checks passed\n");
    return check_failures;
}
