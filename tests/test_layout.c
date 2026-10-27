#include "check.h"
#include "layout.h"

static void expect_grid(int count, int tile_w, int tile_h, int cols, int rows)
{
    Layout layout = {0, 0, 0, 0, 0, 0};

    CHECK(layout_compute(count, tile_w, tile_h, &layout) == 0);
    CHECK(layout.cols == cols);
    CHECK(layout.rows == rows);
    CHECK(layout.tile_w == tile_w && layout.tile_h == tile_h);
    CHECK(layout.width == cols * tile_w && layout.height == rows * tile_h);
}

static void test_grids(void)
{
    expect_grid(4, 100, 100, 2, 2);   /* the example: four 100x100 images make 200x200 */
    expect_grid(4, 60, 60, 2, 2);     /* resized to 60x60 they make 120x120 */
    expect_grid(1, 100, 100, 1, 1);
    expect_grid(2, 100, 100, 2, 1);   /* 200x100 beats 100x200 only by being wider */
    expect_grid(3, 100, 100, 2, 2);   /* square with one empty cell beats a 3:1 strip */
    expect_grid(5, 100, 100, 3, 2);   /* 3x2 and 2x3 tie on ratio and cells; wider wins */
    expect_grid(9, 100, 100, 3, 3);
    expect_grid(56, 104, 104, 8, 7);
    expect_grid(2, 200, 100, 1, 2);   /* wide tiles stack into a square */
    expect_grid(4, 100, 50, 2, 2);    /* 1x4 (100x200) and 2x2 (200x100) tie; the wider wins */
}

static void test_limits(void)
{
    Layout layout;

    CHECK(layout_compute(0, 100, 100, &layout) == -1);
    CHECK(layout_compute(4, 0, 100, &layout) == -1);
    CHECK(layout_compute(4, 100, -1, &layout) == -1);
    CHECK(layout_compute(4, 100, 100, NULL) == -1);
    CHECK(layout_compute(1, LAYOUT_MAX_DIM, LAYOUT_MAX_DIM, &layout) == 0);
    CHECK(layout_compute(2, LAYOUT_MAX_DIM, LAYOUT_MAX_DIM, &layout) == -1);
    CHECK(layout_compute(2, LAYOUT_MAX_DIM + 1, 1, &layout) == -1);
}

static void test_scaled_height(void)
{
    CHECK(layout_scaled_height(100, 100, 60) == 60);
    CHECK(layout_scaled_height(104, 104, 104) == 104);
    CHECK(layout_scaled_height(200, 100, 60) == 30);
    CHECK(layout_scaled_height(3, 2, 4) == 3);      /* 2.67 rounds to 3 */
    CHECK(layout_scaled_height(1000, 1, 10) == 1);  /* never collapses to 0 */
    CHECK(layout_scaled_height(0, 100, 60) == -1);
    CHECK(layout_scaled_height(100, 100, 0) == -1);
}

int main(void)
{
    test_grids();
    test_limits();
    test_scaled_height();
    if (check_failures == 0) printf("test_layout: all checks passed\n");
    return check_failures;
}
