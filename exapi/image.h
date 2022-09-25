#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "npkdef.h"
#include "matrix.h"
#include <stdio.h>
#include "exarray.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IMAGE_NAME_LEN 256

typedef struct tagPaletteManager PaletteManager;
typedef struct tagNpkObject NpkObject;
typedef struct tagImageObject ImageObject;

typedef struct tagImageIndex {
    int offset;
    int size;
    char name[256];
} ImageIndex;

typedef struct tagFrame {
    enum ColorFormat colorFormat;
    union {
        enum CompressType compressType;
        unsigned int linkTo;
    };
    int width;
    int height;
    int datasize;
    int basePosX;
    int basePosY;
    int frameWidth;
    int frameHeight;
    char* data;    // 如果是dds格式为空
    int unknow1;   // v5
    int ddsIndex;  // v5
    int ddsLeft;   // v5
    int ddsTop;    // v5
    int ddsRight;  // v5
    int ddsBotton; // v5
    int unknow2;   // v5

    ImageObject* parent;
} Frame;

typedef struct tagDDSFrame {
    int head;
    int ddsFormat;
    int index;
    int beforeCompressSize;
    int behindCompressSize;
    int width;
    int height;
    char* data;

    ImageObject* parent;
} DDSFrame;

typedef struct tagImageObject {
    unsigned int indexSize;
    unsigned int version;
    unsigned int imageSize; // v5
    PaletteManager* paletteManager; // V2为NULL
    ExArray* frameArray;            // frame array
    ExArray* ddsFrameArray;
    char name[IMAGE_NAME_LEN];     // 不包含路径的文件名，只作为显示用，不写在文件里
    char pathName[IMAGE_NAME_LEN]; // 写在文件里的包含路径的文件名
    NpkObject* parent;
} ImageObject;

EXAPI_EXPORT ImageObject* ImageLoad(FILE* _file, ImageIndex* _index);
EXAPI_EXPORT void ImageFree(ImageObject* _image);
EXAPI_EXPORT Frame* ImageGetFrame(ImageObject* _image, int _frameIndex);
EXAPI_EXPORT int ImageGetFrameCount(ImageObject* _image);
EXAPI_EXPORT int ImageGetVersion(ImageObject* _image);
EXAPI_EXPORT int ImageGetDDSFrameCount(ImageObject* _image);
EXAPI_EXPORT DDSFrame* ImageGetDDSFrame(ImageObject* _image, int _frameIndex);

int ImageSave(ImageObject* _image, FILE* _file, ImageIndex* _index);
ExByteArray* ImageToByteArray(ImageObject* _image);

EXAPI_EXPORT ExMatrix* FrameToMatrix(Frame* _frame, int _palette);
EXAPI_EXPORT int FrameIsLink(Frame* _frame);
EXAPI_EXPORT Frame* FrameGetLink(Frame* _frame);

#ifdef __cplusplus
}
#endif

#endif