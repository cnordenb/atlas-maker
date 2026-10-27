#define COBJMACROS
#include <windows.h>
#include <wincodec.h>
#include <limits.h>
#include <stdlib.h>

#include <webp/encode.h>

#include "image.h"

/* WICBitmapInterpolationModeHighQualityCubic (Windows 10 and later) is missing from MinGW's wincodec.h. */
#define INTERPOLATION_HIGH_QUALITY_CUBIC ((WICBitmapInterpolationMode)4)

static IWICImagingFactory *factory;

int image_startup(void)
{
    HRESULT hr = CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory,
                                  (LPVOID *)&factory);
    return SUCCEEDED(hr) ? IMAGE_OK : IMAGE_ERR_READ;
}

void image_shutdown(void)
{
    if (factory != NULL) IWICImagingFactory_Release(factory);
    factory = NULL;
}

static HRESULT open_frame(const wchar_t *path, IWICBitmapFrameDecode **frame)
{
    IWICBitmapDecoder *decoder = NULL;
    HRESULT hr = IWICImagingFactory_CreateDecoderFromFilename(factory, path, NULL, GENERIC_READ,
                                                              WICDecodeMetadataCacheOnDemand, &decoder);

    if (SUCCEEDED(hr)) {
        hr = IWICBitmapDecoder_GetFrame(decoder, 0, frame);
        IWICBitmapDecoder_Release(decoder);
    }
    return hr;
}

static HRESULT frame_size(IWICBitmapFrameDecode *frame, int *width, int *height)
{
    UINT w = 0;
    UINT h = 0;
    HRESULT hr = IWICBitmapFrameDecode_GetSize(frame, &w, &h);

    if (SUCCEEDED(hr) && (w == 0 || h == 0 || w > INT_MAX || h > INT_MAX)) hr = E_FAIL;
    if (SUCCEEDED(hr)) {
        *width = (int)w;
        *height = (int)h;
    }
    return hr;
}

int image_probe(const wchar_t *path, int *width, int *height)
{
    IWICBitmapFrameDecode *frame = NULL;
    HRESULT hr = open_frame(path, &frame);

    if (SUCCEEDED(hr)) {
        hr = frame_size(frame, width, height);
        IWICBitmapFrameDecode_Release(frame);
    }
    return SUCCEEDED(hr) ? IMAGE_OK : IMAGE_ERR_READ;
}

/* On success, releases *src and replaces it with next. On failure, leaves *src alone. */
static HRESULT advance(IWICBitmapSource **src, HRESULT hr, IWICBitmapSource *next)
{
    if (SUCCEEDED(hr)) {
        IWICBitmapSource_Release(*src);
        *src = next;
    } else if (next != NULL) {
        IWICBitmapSource_Release(next);
    }
    return hr;
}

static HRESULT convert(IWICBitmapSource **src, REFWICPixelFormatGUID format)
{
    IWICFormatConverter *converter = NULL;
    HRESULT hr = IWICImagingFactory_CreateFormatConverter(factory, &converter);

    if (FAILED(hr)) return hr;
    hr = IWICFormatConverter_Initialize(converter, *src, format, WICBitmapDitherTypeNone, NULL, 0.0,
                                        WICBitmapPaletteTypeCustom);
    return advance(src, hr, (IWICBitmapSource *)converter);
}

static HRESULT scale(IWICBitmapSource **src, int width, int height)
{
    IWICBitmapScaler *scaler = NULL;
    HRESULT hr = IWICImagingFactory_CreateBitmapScaler(factory, &scaler);

    if (FAILED(hr)) return hr;
    hr = IWICBitmapScaler_Initialize(scaler, *src, (UINT)width, (UINT)height, INTERPOLATION_HIGH_QUALITY_CUBIC);
    return advance(src, hr, (IWICBitmapSource *)scaler);
}

static HRESULT copy_pixels(IWICBitmapSource *src, int width, int height, Image *out)
{
    size_t stride = (size_t)width * 4;
    size_t size = stride * (size_t)height;
    unsigned char *rgba;
    HRESULT hr;

    if (size > UINT_MAX) return E_OUTOFMEMORY;
    rgba = malloc(size);
    if (rgba == NULL) return E_OUTOFMEMORY;
    hr = IWICBitmapSource_CopyPixels(src, NULL, (UINT)stride, (UINT)size, rgba);
    if (FAILED(hr)) {
        free(rgba);
        return hr;
    }
    out->width = width;
    out->height = height;
    out->rgba = rgba;
    return S_OK;
}

int image_load(const wchar_t *path, int width, int height, Image *out)
{
    IWICBitmapFrameDecode *frame = NULL;
    IWICBitmapSource *src;
    int src_w = 0;
    int src_h = 0;
    HRESULT hr = open_frame(path, &frame);

    if (FAILED(hr)) return IMAGE_ERR_READ;
    src = (IWICBitmapSource *)frame;
    hr = frame_size(frame, &src_w, &src_h);
    if (SUCCEEDED(hr) && (src_w != width || src_h != height)) {
        /* Scale premultiplied pixels so colors hidden under transparent pixels don't bleed into the edges. */
        hr = convert(&src, &GUID_WICPixelFormat32bppPBGRA);
        if (SUCCEEDED(hr)) hr = scale(&src, width, height);
    }
    if (SUCCEEDED(hr)) hr = convert(&src, &GUID_WICPixelFormat32bppRGBA);
    if (SUCCEEDED(hr)) hr = copy_pixels(src, width, height, out);
    IWICBitmapSource_Release(src);
    return SUCCEEDED(hr) ? IMAGE_OK : IMAGE_ERR_READ;
}

void image_free(Image *img)
{
    free(img->rgba);
    img->rgba = NULL;
}

static int write_file(const wchar_t *path, const void *data, size_t size)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD written = 0;
    BOOL ok;

    if (file == INVALID_HANDLE_VALUE) return IMAGE_ERR_WRITE;
    ok = size <= MAXDWORD && WriteFile(file, data, (DWORD)size, &written, NULL) && written == size;
    if (!CloseHandle(file)) ok = FALSE;
    if (!ok) DeleteFileW(path); /* don't leave a truncated file behind */
    return ok ? IMAGE_OK : IMAGE_ERR_WRITE;
}

int image_save_webp(const Image *img, float quality, const wchar_t *path)
{
    uint8_t *data = NULL;
    size_t size = WebPEncodeRGBA(img->rgba, img->width, img->height, img->width * 4, quality, &data);
    int result = size > 0 ? write_file(path, data, size) : IMAGE_ERR_ENCODE;

    WebPFree(data);
    return result;
}
