#ifndef LAYOUT_H
#define LAYOUT_H

/* WebP cannot store an image wider or taller than this. */
#define LAYOUT_MAX_DIM 16383

/* A grid of equally sized tiles. */
typedef struct {
    int cols;
    int rows;
    int tile_w;
    int tile_h;
    int width;  /* cols * tile_w */
    int height; /* rows * tile_h */
} Layout;

/*
 * Chooses the grid for count tiles whose atlas is closest to square.
 * Ties go to the grid with fewer empty cells, then to the wider grid.
 * Returns 0 on success, or -1 if an argument is invalid or no grid fits within LAYOUT_MAX_DIM.
 */
int layout_compute(int count, int tile_w, int tile_h, Layout *out);

/*
 * The height that keeps the src_w:src_h ratio at width new_w, rounded and at least 1.
 * Heights beyond LAYOUT_MAX_DIM come back as LAYOUT_MAX_DIM + 1, which layout_compute rejects.
 * Returns -1 if an argument is invalid.
 */
int layout_scaled_height(int src_w, int src_h, int new_w);

#endif
