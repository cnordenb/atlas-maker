#include "check.h"
#include "layout.h"

static int grid_is(int count, int tile_w, int tile_h, int cols, int rows)
{
    Layout layout = {0, 0, 0, 0, 0, 0};

    return layout_compute(count, tile_w, tile_h, &layout) == 0 && layout.cols == cols && layout.rows == rows;
}

static int one_tile_fits(int tile_w, int tile_h)
{
    Layout layout = {0, 0, 0, 0, 0, 0};

    return layout_compute(1, tile_w, tile_h, &layout) == 0;
}

static int four_squares_make_two_by_two(void)
{
    return grid_is(4, 100, 100, 2, 2);
}

static int three_squares_make_two_by_two(void)
{
    return grid_is(3, 100, 100, 2, 2);
}

static int two_wide_tiles_make_one_column(void)
{
    return grid_is(2, 200, 100, 1, 2);
}

static int equally_square_grids_give_more_columns(void)
{
    return grid_is(2, 100, 100, 2, 1);
}

static int largest_atlas_fits(void)
{
    return one_tile_fits(LAYOUT_MAX_DIM, LAYOUT_MAX_DIM);
}

static int too_wide_atlas_is_refused(void)
{
    return !one_tile_fits(LAYOUT_MAX_DIM + 1, 1);
}

static int too_tall_atlas_is_refused(void)
{
    return !one_tile_fits(1, LAYOUT_MAX_DIM + 1);
}

static int height_keeps_aspect_ratio(void)
{
    return layout_scaled_height(200, 100, 60) == 30;
}

static int height_rounds_to_nearest(void)
{
    return layout_scaled_height(3, 2, 4) == 3;
}

static int height_is_at_least_one(void)
{
    return layout_scaled_height(1000, 1, 10) == 1;
}

const Test layout_tests[] = {
    four_squares_make_two_by_two,
    three_squares_make_two_by_two,
    two_wide_tiles_make_one_column,
    equally_square_grids_give_more_columns,
    largest_atlas_fits,
    too_wide_atlas_is_refused,
    too_tall_atlas_is_refused,
    height_keeps_aspect_ratio,
    height_rounds_to_nearest,
    height_is_at_least_one,
};

const int layout_test_count = (int)(sizeof layout_tests / sizeof layout_tests[0]);
