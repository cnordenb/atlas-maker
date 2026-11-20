#ifndef LAYOUT_H
#define LAYOUT_H

#define LAYOUT_MAX_DIM 16383

typedef struct {
    int cols;
    int rows;
    int tile_w;
    int tile_h;
    int width;
    int height;
} Layout;

int layout_compute(int count, int tile_w, int tile_h, Layout *out);

int layout_scaled_height(int src_w, int src_h, int new_w);

#endif
