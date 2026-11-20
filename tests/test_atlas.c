#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <webp/decode.h>

#include "atlas.h"
#include "check.h"
#include "image.h"

#define PATH_LEN 1024
#define OUT_FILE L"test_atlas_out.webp"
#define NEVER_WRITTEN_FILE L"test_atlas_never_written.webp"
#define COLOR_SLACK 24

enum { FIX_DATE, FIX_BANANA, FIX_APPLE, FIX_CHERRY, FIX_ODD, FIXTURES };
static const wchar_t *const fixture_names[FIXTURES] = {
    L"date.png", L"Banana.png", L"apple.png", L"cherry.png", L"odd_50x50.png"
};
static wchar_t fixture_paths[FIXTURES][PATH_LEN];
static const wchar_t *fixtures[FIXTURES];

typedef struct {
    int r, g, b, a;
} Color;

static const Color APPLE_RED = {220, 40, 40, 255};
static const Color BANANA_GREEN = {40, 180, 60, 255};
static const Color CHERRY_BLUE = {40, 60, 200, 255};
static const Color DATE_YELLOW = {230, 200, 40, 255};

typedef struct {
    int width, height;
    uint8_t *rgba;
} Decoded;

static Decoded built;

static void decode_output(void)
{
    FILE *file = _wfopen(OUT_FILE, L"rb");
    static uint8_t data[1 << 20];
    size_t size;

    if (file == NULL) return;
    size = fread(data, 1, sizeof data, file);
    fclose(file);
    built.rgba = WebPDecodeRGBA(data, size, &built.width, &built.height);
}

static const Decoded *build(int count, int tile)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    AtlasRequest req = {fixtures, count, tile, tile, OUT_FILE};

    WebPFree(built.rgba);
    built = (Decoded){0, 0, NULL};
    if (atlas_build(&req, err, ATLAS_ERR_LEN) == 0) decode_output();
    else fwprintf(stderr, L"atlas_build: %ls\n", err);
    return &built;
}

static int size_is(const Decoded *img, int width, int height)
{
    return img->rgba != NULL && img->width == width && img->height == height;
}

static const uint8_t *pixel(const Decoded *img, int x, int y)
{
    return img->rgba + ((size_t)y * (size_t)img->width + (size_t)x) * 4;
}

static int close_to(int actual, int expected)
{
    return abs(actual - expected) <= COLOR_SLACK;
}

static int pixel_is(const Decoded *img, int x, int y, Color want)
{
    const uint8_t *p;

    if (img->rgba == NULL) return 0;
    p = pixel(img, x, y);
    return close_to(p[0], want.r) && close_to(p[1], want.g) && close_to(p[2], want.b) && close_to(p[3], want.a);
}

static int mismatch_build_fails(const wchar_t *out_path)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    AtlasRequest req = {fixtures, FIXTURES, 100, 100, out_path};

    return atlas_build(&req, err, ATLAS_ERR_LEN) == -1;
}

static int images_sort_by_name_ignoring_case(void)
{
    const wchar_t *paths[4] = {fixtures[FIX_DATE], fixtures[FIX_BANANA], fixtures[FIX_APPLE], fixtures[FIX_CHERRY]};
    const wchar_t *sorted[4] = {fixtures[FIX_APPLE], fixtures[FIX_BANANA], fixtures[FIX_CHERRY], fixtures[FIX_DATE]};

    atlas_sort(paths, 4);
    return memcmp(paths, sorted, sizeof paths) == 0;
}

static int shared_resolution_is_found(void)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    int w = 0;
    int h = 0;

    return atlas_inspect(fixtures, 4, &w, &h, err, ATLAS_ERR_LEN) == 0 && w == 100 && h == 100;
}

static int differing_image_is_named(void)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    int w = 0;
    int h = 0;

    atlas_inspect(fixtures, FIXTURES, &w, &h, err, ATLAS_ERR_LEN);
    return wcsstr(err, L"odd_50x50.png") != NULL;
}

static int empty_list_is_refused(void)
{
    wchar_t err[ATLAS_ERR_LEN] = L"";
    int w = 0;
    int h = 0;

    return atlas_inspect(fixtures, 0, &w, &h, err, ATLAS_ERR_LEN) == -1;
}

static int four_images_make_a_square_atlas(void)
{
    return size_is(build(4, 100), 200, 200);
}

static int first_image_is_top_left(void)
{
    return pixel_is(build(4, 100), 50, 50, APPLE_RED);
}

static int second_image_is_top_right(void)
{
    return pixel_is(build(4, 100), 150, 50, BANANA_GREEN);
}

static int third_image_is_bottom_left(void)
{
    return pixel_is(build(4, 100), 50, 150, CHERRY_BLUE);
}

static int fourth_image_is_bottom_right(void)
{
    return pixel_is(build(4, 100), 150, 150, DATE_YELLOW);
}

static int resized_images_make_a_smaller_atlas(void)
{
    return size_is(build(4, 60), 120, 120);
}

static int resized_image_fills_its_cell(void)
{
    return pixel_is(build(4, 60), 115, 115, DATE_YELLOW);
}

static int empty_cell_is_transparent(void)
{
    const Decoded *img = build(3, 100);

    return img->rgba != NULL && pixel(img, 150, 150)[3] == 0;
}

static int differing_image_stops_the_atlas(void)
{
    return mismatch_build_fails(OUT_FILE);
}

static int failed_atlas_writes_no_file(void)
{
    DeleteFileW(NEVER_WRITTEN_FILE);
    mismatch_build_fails(NEVER_WRITTEN_FILE);
    return GetFileAttributesW(NEVER_WRITTEN_FILE) == INVALID_FILE_ATTRIBUTES;
}

int atlas_tests_start(const wchar_t *project_dir)
{
    for (int i = 0; i < FIXTURES; i++) {
        swprintf(fixture_paths[i], PATH_LEN, L"%ls/tests/data/%ls", project_dir, fixture_names[i]);
        fixtures[i] = fixture_paths[i];
    }
    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) return -1;
    return image_startup() == IMAGE_OK ? 0 : -1;
}

void atlas_tests_stop(void)
{
    WebPFree(built.rgba);
    built.rgba = NULL;
    image_shutdown();
    CoUninitialize();
}

const Test atlas_tests[] = {
    images_sort_by_name_ignoring_case,
    shared_resolution_is_found,
    differing_image_is_named,
    empty_list_is_refused,
    four_images_make_a_square_atlas,
    first_image_is_top_left,
    second_image_is_top_right,
    third_image_is_bottom_left,
    fourth_image_is_bottom_right,
    resized_images_make_a_smaller_atlas,
    resized_image_fills_its_cell,
    empty_cell_is_transparent,
    differing_image_stops_the_atlas,
    failed_atlas_writes_no_file,
};

const int atlas_test_count = (int)(sizeof atlas_tests / sizeof atlas_tests[0]);
