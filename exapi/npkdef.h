#ifndef _NPKDEF_H_
#define _NPKDEF_H_

#ifdef EXAPI_EXPORTS
#define EXAPI_EXPORT _declspec(dllexport)
#else
#define EXAPI_EXPORT _declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum ColorFormat
{
    ARGB8888 = 0x10,
    ARGB4444 = 0x0F,
    ARGB1555 = 0x0E,
    LINK = 0x11,
    DDS_DXT1 = 0x12,
    DDS_DXT3 = 0x13,
    DDS_DXT5 = 0x14,
    COLOR_UDEF = 0,
    V4_FMT,
    RGB565
};

enum CompressType {
    NONE = 0x05,
    ZLIB = 0x06,
    ZLIB2 = 0x07
};

#define INT_LEN sizeof(int)

#ifdef __cplusplus
}
#endif

#endif