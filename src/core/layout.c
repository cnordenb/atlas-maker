#include <stddef.h>

#include "layout.h"

static long long longer_side(const Layout *l)
{
    return l->width > l->height ? l->width : l->height;
}

static long long shorter_side(const Layout *l)
{
    return l->width < l->height ? l->width : l->height;
}

/* Nonzero if a is closer to square than b, or ties and wastes fewer cells, or ties again and is wider. */
static int better(const Layout *a, const Layout *b)
{
    /* Compare longer/shorter ratios by cross-multiplying, so equal ratios compare exactly equal. */
    long long ratio_a = longer_side(a) * shorter_side(b);
    long long ratio_b = longer_side(b) * shorter_side(a);
    long long cells_a = (long long)a->cols * a->rows;
    long long cells_b = (long long)b->cols * b->rows;

    if (ratio_a != ratio_b) return ratio_a < ratio_b;
    if (cells_a != cells_b) return cells_a < cells_b;
    return a->cols > b->cols;
}

int layout_compute(int count, int tile_w, int tile_h, Layout *out)
{
    Layout best = {0, 0, 0, 0, 0, 0};

    if (count < 1 || tile_w < 1 || tile_h < 1 || out == NULL) return -1;
    /* Widening the grid only grows the width, so stop at the first column count that is too wide. */
    for (int cols = 1; cols <= count && (long long)cols * tile_w <= LAYOUT_MAX_DIM; cols++) {
        int rows = (count + cols - 1) / cols;
        Layout candidate = {cols, rows, tile_w, tile_h, cols * tile_w, 0};

        if ((long long)rows * tile_h > LAYOUT_MAX_DIM) continue;
        candidate.height = rows * tile_h;
        if (best.cols == 0 || better(&candidate, &best)) best = candidate;
    }
    if (best.cols == 0) return -1;
    *out = best;
    return 0;
}

int layout_scaled_height(int src_w, int src_h, int new_w)
{
    long long height;

    if (src_w < 1 || src_h < 1 || new_w < 1) return -1;
    height = ((long long)new_w * src_h * 2 + src_w) / ((long long)src_w * 2);
    if (height < 1) return 1;
    return height > LAYOUT_MAX_DIM ? LAYOUT_MAX_DIM + 1 : (int)height;
}
