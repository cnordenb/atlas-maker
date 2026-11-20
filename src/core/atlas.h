#ifndef ATLAS_H
#define ATLAS_H

#include <stddef.h>
#include <wchar.h>

#include "layout.h"

#define ATLAS_WEBP_QUALITY 90.0f
#define ATLAS_ERR_LEN 512

typedef struct {
    const wchar_t *const *paths;
    int count;
    int tile_w;
    int tile_h;
    const wchar_t *out_path;
} AtlasRequest;

const wchar_t *atlas_file_name(const wchar_t *path);

void atlas_sort(const wchar_t **paths, int count);

int atlas_inspect(const wchar_t *const *paths, int count, int *src_w, int *src_h, wchar_t *err, size_t err_len);

int atlas_build(const AtlasRequest *req, wchar_t *err, size_t err_len);

#endif
