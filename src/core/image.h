#ifndef IMAGE_H
#define IMAGE_H

#include <wchar.h>

enum {
    IMAGE_OK = 0,
    IMAGE_ERR_READ = -1,   /* missing, unreadable or not an image, or out of memory */
    IMAGE_ERR_ENCODE = -2,
    IMAGE_ERR_WRITE = -3
};

/* Pixels in RGBA order, 4 bytes each, rows top to bottom, alpha not premultiplied. */
typedef struct {
    int width;
    int height;
    unsigned char *rgba;
} Image;

/* Call once on a COM-initialized thread before any other image function. Returns IMAGE_OK or IMAGE_ERR_READ. */
int image_startup(void);
void image_shutdown(void);

/* Reads only the size of the image at path. */
int image_probe(const wchar_t *path, int *width, int *height);

/* Decodes the image at path, scaled to width x height if it has another size. Free it with image_free. */
int image_load(const wchar_t *path, int width, int height, Image *out);

void image_free(Image *img);

/* Encodes img as lossy WebP at quality (0-100) and writes it to path, replacing any existing file. */
int image_save_webp(const Image *img, float quality, const wchar_t *path);

#endif
