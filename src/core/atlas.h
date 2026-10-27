#ifndef ATLAS_H
#define ATLAS_H

#include <stddef.h>
#include <wchar.h>

#include "layout.h"

#define ATLAS_WEBP_QUALITY 90.0f
#define ATLAS_ERR_LEN 512 /* enough for any message the atlas functions write */

typedef struct {
    const wchar_t *const *paths; /* input images, in any order */
    int count;
    int tile_w;                  /* every image is drawn at tile_w x tile_h */
    int tile_h;
    const wchar_t *out_path;     /* the .webp file to write */
} AtlasRequest;

/* The file name part of path, after the last slash or backslash. */
const wchar_t *atlas_file_name(const wchar_t *path);

/* Puts paths in atlas order: alphabetical by file name, ignoring case, the same on every machine. */
void atlas_sort(const wchar_t **paths, int count);

/*
 * Checks that every file is a readable image and that all share one resolution, which it returns.
 * Returns 0 on success, or -1 with a message for the user in err.
 */
int atlas_inspect(const wchar_t *const *paths, int count, int *src_w, int *src_h, wchar_t *err, size_t err_len);

/*
 * Draws the images in atlas order, left to right and row by row, on the most square grid (see layout.h),
 * leaving unused cells transparent, and saves the result as lossy WebP at ATLAS_WEBP_QUALITY.
 * Returns 0 on success, or -1 with a message for the user in err.
 */
int atlas_build(const AtlasRequest *req, wchar_t *err, size_t err_len);

#endif
