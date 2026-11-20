#ifndef IMAGE_H
#define IMAGE_H

#include <wchar.h>

enum {
    IMAGE_OK = 0,
    IMAGE_ERR_READ = -1,
    IMAGE_ERR_ENCODE = -2,
    IMAGE_ERR_WRITE = -3
};

typedef struct {
    int width;
    int height;
    unsigned char *rgba;
} Image;

int image_startup(void);
void image_shutdown(void);

int image_probe(const wchar_t *path, int *width, int *height);

int image_load(const wchar_t *path, int width, int height, Image *out);

void image_free(Image *img);

int image_save_webp(const Image *img, float quality, const wchar_t *path);

#endif
