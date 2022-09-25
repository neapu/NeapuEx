#include "image.h"
#include "palette.h"
#include "logger.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <zlib.h>
#include "dds.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

char szIMGNameMask[IMAGE_NAME_LEN] = "puchikon@neople dungeon and fighter DNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNFDNF";

#define LOAD_IMAGE_FAILED()      \
    {                            \
        if (rst) ImageFree(rst); \
        return NULL;             \
    }
#define IMG_READ(buf, len)                          \
    {                                               \
        read = fread((buf), 1, (len), file);        \
        if (read != (len)) { LOAD_IMAGE_FAILED(); } \
    }
#define FLAG_LEN 16
ImageObject* ImageLoad(FILE* _file, ImageIndex* _index)
{
    FILE* file = _file;
    size_t read = 0;
    char szFlag[FLAG_LEN];
    ImageObject* rst = NULL;
    unsigned int indexSize = 0;
    unsigned int reserve = 0;
    unsigned int version = 0;
    unsigned int indexCount = 0;
    unsigned int ddsIndexCount = 0; // v5
    unsigned int imageSize = 0;     // v5

    fseek(file, _index->offset, SEEK_SET);
    IMG_READ(szFlag, FLAG_LEN);
    if (memcmp(szFlag, "Neople Img File", FLAG_LEN) != 0) {
        LOG_ERROR("Load Image Error:HEAD ERROR");
        LOAD_IMAGE_FAILED();
    }

    IMG_READ(&indexSize, INT_LEN);
    IMG_READ(&reserve, INT_LEN);
    IMG_READ(&version, INT_LEN);
    IMG_READ(&indexCount, INT_LEN);

    if (version != 2 && version != 4 && version != 5 && version != 6) {
        LOG_ERROR("Load Image Error:Unknow version");
        LOAD_IMAGE_FAILED();
    }

    if (version == 5) {
        IMG_READ(&ddsIndexCount, INT_LEN);
        IMG_READ(&imageSize, INT_LEN);
    }

    rst = (ImageObject*)malloc(sizeof(ImageObject));
    assert(rst);
    memset(rst, 0, sizeof(ImageObject));

    rst->indexSize = indexSize;
    rst->imageSize = imageSize;

    if (version == 4 || version == 5) {
        unsigned int colorCount = 0;
        rst->paletteManager = PaletteManagerInit(1);
        IMG_READ(&colorCount, INT_LEN);
        if (colorCount != 0) {
            unsigned int len = colorCount * INT_LEN;
            char* buf = (char*)malloc(len);
            if (!buf) {
                LOG_DEADLY("Load Image Error:malloc failed");
                exit(-1);
            }
            memset(buf, 0, len);
            IMG_READ(buf, len);
            PMAddPalette(rst->paletteManager, colorCount, buf);
            free(buf);
        }
    } else if (version == 6) {
        unsigned int paletteCount = 0;
        IMG_READ(&paletteCount, INT_LEN);
        rst->paletteManager = PaletteManagerInit(paletteCount);
        for (unsigned int i = 0; i < paletteCount; i++) {
            unsigned int colorCount = 0;
            IMG_READ(&colorCount, INT_LEN);
            if (colorCount == 0) continue;
            unsigned int len = colorCount * INT_LEN;
            char* buf = (char*)malloc(len);
            if (!buf) {
                LOG_DEADLY("Load Image Error:malloc failed");
                exit(-1);
            }
            memset(buf, 0, len);
            IMG_READ(buf, len);
            PMAddPalette(rst->paletteManager, colorCount, buf);
            free(buf);
        }
    }

    rst->version = version;
    if (version == 5) {
        rst->ddsFrameArray = ArrayInit(ddsIndexCount);
        for (unsigned int i = 0; i < ddsIndexCount; i++) {
            DDSFrame* ddsFrame = (DDSFrame*)malloc(sizeof(DDSFrame));
            assert(ddsFrame);
            memset(ddsFrame, 0, sizeof(DDSFrame));
            IMG_READ(ddsFrame, INT_LEN * 7);
            ddsFrame->parent = rst;
            ArrayAppend(rst->ddsFrameArray, ddsFrame);
        }
    }

    rst->frameArray = ArrayInit(indexCount);

    for (unsigned int i = 0; i < indexCount; i++) {
        Frame* frame = (Frame*)malloc(sizeof(Frame));
        assert(frame);
        memset(frame, 0, sizeof(Frame));
        IMG_READ(&frame->colorFormat, INT_LEN);
        if (frame->colorFormat == 0) {
            continue;
        }
        IMG_READ(&frame->compressType, INT_LEN);
        if (frame->colorFormat != LINK) {
            IMG_READ(&frame->width, INT_LEN);
            IMG_READ(&frame->height, INT_LEN);
            IMG_READ(&frame->datasize, INT_LEN);
            IMG_READ(&frame->basePosX, INT_LEN);
            IMG_READ(&frame->basePosY, INT_LEN);
            IMG_READ(&frame->frameWidth, INT_LEN);
            IMG_READ(&frame->frameHeight, INT_LEN);
        }
        // dds data
        if (frame->colorFormat > LINK) {
            IMG_READ(&frame->unknow1, INT_LEN);
            IMG_READ(&frame->ddsIndex, INT_LEN);
            IMG_READ(&frame->ddsLeft, INT_LEN);
            IMG_READ(&frame->ddsTop, INT_LEN);
            IMG_READ(&frame->ddsRight, INT_LEN);
            IMG_READ(&frame->ddsBotton, INT_LEN);
            IMG_READ(&frame->unknow2, INT_LEN);
        }
        frame->parent = rst;
        ArrayAppend(rst->frameArray, frame);
    }

    if (version == 5) {
        for (unsigned int i = 0; i < ddsIndexCount; i++) {
            DDSFrame* ddsFrame = (DDSFrame*)ArrayGetData(rst->ddsFrameArray, i);
            assert(ddsFrame);
            ddsFrame->data = (unsigned char*)malloc(ddsFrame->beforeCompressSize);
            assert(ddsFrame->data);
            memset(ddsFrame->data, 0, ddsFrame->beforeCompressSize);
            IMG_READ(ddsFrame->data, ddsFrame->beforeCompressSize);
        }
    }
    for (unsigned int i = 0; i < indexCount; i++) {
        Frame* frame = (Frame*)ArrayGetData(rst->frameArray, i);
        if (frame->colorFormat < LINK) {
            if (frame->datasize == 0) continue;
            frame->data = (unsigned char*)malloc(frame->datasize);
            assert(frame->data);
            IMG_READ(frame->data, frame->datasize);
        }
    }

    memcpy_s(rst->pathName, IMAGE_NAME_LEN, _index->name, IMAGE_NAME_LEN);
    for (int i = 0; i < IMAGE_NAME_LEN; i++) {
        rst->pathName[i] ^= szIMGNameMask[i];
    }

    // 取文件名
    size_t nameLen = strlen(rst->pathName);
    if (nameLen != 0) {
        size_t begin = nameLen - 1;
        for (; begin > 0; begin--) {
            if (rst->pathName[begin] == '/') {
                begin += 1;
                break;
            }
        }
        strcpy_s(rst->name, IMAGE_NAME_LEN, rst->pathName + begin);
    }

    return rst;
}

void FrameFree(Frame* frame)
{
    if (!frame) return;
    if (frame->data) {
        free(frame->data);
    }
    free(frame);
}

void DDSFrameFree(DDSFrame* frame)
{
    if (!frame) return;
    if (frame->data) {
        free(frame->data);
    }
    free(frame);
}

void ImageFree(ImageObject* _image)
{
    if (!_image) return;
    if (_image->paletteManager) {
        PaletteManagerFree(_image->paletteManager);
    }

    if (_image->frameArray) {
        int len = ArrayLength(_image->frameArray);
        for (int i = 0; i < len; i++) {
            FrameFree((Frame*)ArrayGetData(_image->frameArray, i));
        }
    }
    ArrayFree(_image->frameArray);

    if (_image->ddsFrameArray) {
        int len = ArrayLength(_image->ddsFrameArray);
        for (int i = 0; i < len; i++) {
            DDSFrameFree((DDSFrame*)ArrayGetData(_image->ddsFrameArray, i));
        }
    }
    ArrayFree(_image->ddsFrameArray);

    free(_image);
}

Frame* ImageGetFrame(ImageObject* _image, int _frameIndex)
{
    if (!_image) return NULL;
    return (Frame*)ArrayGetData(_image->frameArray, _frameIndex);
}

static void ToColor(ExColor* dst, unsigned long src, int format)
{
    if (format == V4_FMT) {
        dst->a = (unsigned char)((src & 0xff000000) >> 24);
        dst->r = (unsigned char)((src & 0x000000ff) >> 0);
        dst->g = (unsigned char)((src & 0x0000ff00) >> 8);
        dst->b = (unsigned char)((src & 0x00ff0000) >> 16);
    } else if (format == ARGB1555) {
        dst->a = (unsigned char)(((src & 0x8000) >> 15) * 0xff);
        dst->r = (unsigned char)((src & 0x7c00) >> 10 << 3);
        dst->g = (unsigned char)((src & 0x03e0) >> 5 << 3);
        dst->b = (unsigned char)((src & 0x001f) >> 0 << 3);
    } else if (format == ARGB4444) {
        dst->a = (unsigned char)(((src & 0xf000) >> 12) * 0x11);
        dst->r = (unsigned char)((src & 0x0f00) >> 8 << 4);
        dst->g = (unsigned char)((src & 0x00f0) >> 4 << 4);
        dst->b = (unsigned char)((src & 0x000f) >> 0 << 4);
    } else if (format == RGB565) {
        dst->a = 0xff;
        dst->r = (unsigned char)((src & 0xf800) >> 11 << 3);
        dst->g = (unsigned char)((src & 0x07e0) >> 5 << 2);
        dst->b = (unsigned char)((src & 0x001f) >> 0 << 3);
    }
}

static unsigned int GetDecompressionSize(Frame* frame)
{
    int version = ImageGetVersion(frame->parent);
    // v4和v6是每个像素1字节
    if (version == 4 || version == 6) {
        return (unsigned int)(frame->width * frame->height);
    }
    // ARGB8888每个像素4字节
    if (frame->colorFormat == ARGB8888) {
        return (unsigned int)(frame->width * frame->height * 4);
    }
    //其他颜色格式没像素2字节
    return (unsigned int)(frame->width * frame->height * 2);
}

static ExMatrix* DDSToMatrix(DDSFrame* _ddsFrame)
{
    unsigned int dataLen = _ddsFrame->behindCompressSize;
    unsigned char* data = (unsigned char*)malloc(dataLen);
    if (!data) return NULL;
    memset(data, 0, dataLen);
    int rst = uncompress(data, &dataLen, _ddsFrame->data, _ddsFrame->beforeCompressSize);
    if (rst != Z_OK) {
        free(data);
        return NULL;
    }
    ExMatrix* rstValue = DDSUncompress(data, dataLen);
    free(data);
    return rstValue;
}

ExMatrix* FrameToMatrix(Frame* _frame, int _palette)
{
    if (!_frame) {
        LOG_ERROR("GetFrameError");
        return NULL;
    }

    Frame* linkTarget = FrameGetLink(_frame);
    if (linkTarget) {
        return FrameToMatrix(linkTarget, _palette);
    }

    ImageObject* parent = _frame->parent;
    if (!parent) {
        return NULL;
    }

    if (_frame->colorFormat > LINK) {
        DDSFrame* ddsFrame = (DDSFrame*)ArrayGetData(parent->ddsFrameArray, _frame->ddsIndex);
        if (!ddsFrame) {
            return NULL;
        }
        ExMatrix* rawMatrix = DDSToMatrix(ddsFrame);
        if (!rawMatrix) {
            return NULL;
        }
        ExMatrix* rstMatrix = MatrixClip(rawMatrix, _frame->ddsLeft, _frame->ddsTop, _frame->ddsRight, _frame->ddsBotton);
        MatrixFree(rawMatrix);
        if (!rstMatrix) {
            return NULL;
        }
        rstMatrix->baseX = _frame->basePosX;
        rstMatrix->baseY = _frame->basePosY;
        rstMatrix->frameWidth = _frame->frameWidth;
        rstMatrix->frameHeight = _frame->frameHeight;
        return rstMatrix;
    }

    unsigned int dataLen = 0;
    unsigned char* data = NULL;

    if (_frame->compressType == ZLIB || _frame->compressType == ZLIB2) {
        unsigned int dataLen = GetDecompressionSize(_frame);
        if (dataLen == 0) {
            return NULL;
        }
        data = (unsigned char*)malloc(dataLen);
        assert(data);
        int rst = uncompress(data, &dataLen, _frame->data, _frame->datasize);
        if (rst != Z_OK) {
            LOG_ERROR("unzip error");
            free(data);
            return NULL;
        }
        // 解压会把dataLen清零
        dataLen = GetDecompressionSize(_frame);
    } else {
        dataLen = _frame->datasize;
        data = (unsigned char*)malloc(dataLen);
        assert(data);
        errno_t err = memcpy_s(data, dataLen, _frame->data, dataLen);
        if (err != 0) {
            LOG_ERROR("memcpy_s error");
            free(data);
            return NULL;
        }
    }
    ExMatrix* matrix = MatrixInit(_frame->width, _frame->height);
    if (!matrix) {
        free(data);
        return 0;
    }

    if ((parent->version == 4 || parent->version == 6) && PMGetPaletteCount(parent->paletteManager) > 0) {
        int paletteIndex = parent->version == 4 ? 0 : _palette;
        for (int i = 0; i < matrix->size; i++) {
            matrix->data[i] = PMGetColor(parent->paletteManager, paletteIndex, (int)data[i]);
        }
    } else if (_frame->colorFormat == ARGB1555 || _frame->colorFormat == ARGB4444) {
        for (int i = 0; i < matrix->size && i * 2 < (int)dataLen; i++) {
            ToColor(&matrix->data[i], (data[i * 2 + 1] << 8 | data[i * 2]), _frame->colorFormat);
        }
    } else {
        for (int i = 0; i < matrix->size && i * 4 < (int)dataLen; i++) {
            matrix->data[i].a = data[i * 4 + 3];
            matrix->data[i].r = data[i * 4 + 2];
            matrix->data[i].g = data[i * 4 + 1];
            matrix->data[i].b = data[i * 4 + 0];
        }
    }
    matrix->baseX = _frame->basePosX;
    matrix->baseY = _frame->basePosY;
    matrix->frameWidth = _frame->frameWidth;
    matrix->frameHeight = _frame->frameHeight;
    free(data);
    return matrix;
}

int FrameIsLink(Frame* _frame)
{
    if (_frame && _frame->colorFormat == LINK) {
        return _frame->linkTo;
    }
    return -1;
}

Frame* FrameGetLink(Frame* _frame)
{
    if (!_frame || _frame->colorFormat != LINK) {
        return NULL;
    }
    ImageObject* parent = _frame->parent;
    if (!parent) return NULL;
    Frame* target = ImageGetFrame(parent, _frame->linkTo);
    return target;
}

int ImageGetFrameCount(ImageObject* _image)
{
    if (!_image) {
        return -1;
    }
    return ArrayLength(_image->frameArray);
}

int ImageGetVersion(ImageObject* _image)
{
    if (!_image) {
        return -1;
    }

    return (int)_image->version;
}

int ImageGetDDSFrameCount(ImageObject* _image)
{
    if (!_image) {
        return -1;
    }
    return ArrayLength(_image->ddsFrameArray);
}

DDSFrame* ImageGetDDSFrame(ImageObject* _image, int _frameIndex)
{
    if (!_image) {
        return NULL;
    }
    return (DDSFrame*)ArrayGetData(_image->ddsFrameArray, _frameIndex);
}

int ImageSave(ImageObject* _image, FILE* _file, ImageIndex* _index)
{
    if (!_image) {
        return -1;
    }
    FILE* file = _file;
    size_t read = 0;
    char szFlag[FLAG_LEN + 1];

    _index->offset = ftell(file);
    memcpy_s(szFlag, FLAG_LEN, "Neople Img File", FLAG_LEN);

    fwrite(file, FLAG_LEN, 1, szFlag);

    unsigned int indexSize = _image->indexSize;
    unsigned int reserve = 0;
    unsigned int version = _image->version;
    unsigned int indexCount = 0;
    

    indexCount = ArrayLength(_image->frameArray);
    if (indexCount <= 0)return -1;

    fwrite(&indexSize, 1, INT_LEN, file);
    fwrite(&reserve, 1, INT_LEN, file);
    fwrite(&version, 1, INT_LEN, file);
    fwrite(&indexCount, 1, INT_LEN, file);

    if (version == 5) {
        unsigned int ddsIndexCount = ArrayLength(_image->ddsFrameArray);
        unsigned int imageSize = _image->imageSize;
        if (ddsIndexCount <= 0)return -1;
        fwrite(&ddsIndexCount, 1, INT_LEN, file);
        fwrite(&imageSize, 1, INT_LEN, file);
    }

    // 调色板
    switch (version)
    {
    case 4:
    case 5:
    {
        int colorCount = PMGetColorCount(_image->paletteManager, 0);
        if (colorCount < 0) {
            colorCount = 0;
        }
        fwrite(&colorCount, 1, 4, file);
        const void* paletteData = PMGetPaletteData(_image->paletteManager, 0);
        if (paletteData) {
            fwrite(paletteData, colorCount * INT_LEN, 1, file);
        }
    }
    break;
    case 6:
    {
        int paletteCount = PMGetPaletteCount(_image->paletteManager);
        for (int i = 0; i < paletteCount; i++) {
            int colorCount = PMGetColorCount(_image->paletteManager, i);
            if (colorCount < 0) {
                colorCount = 0;
            }
            fwrite(&colorCount, 1, 4, file);
            const void* paletteData = PMGetPaletteData(_image->paletteManager, i);
            if (paletteData) {
                fwrite(paletteData, colorCount * INT_LEN, 1, file);
            }
        }
    }
    break;
    default:
        break;
    }

    if (version == 5) {
        unsigned int ddsIndexCount = ArrayLength(_image->ddsFrameArray);
        for (int i = 0; i < ddsIndexCount; i++) {
            DDSFrame* ddsFrame = ArrayGetData(_image->ddsFrameArray, i);
            if (!ddsFrame)return -1;
            fwrite(ddsFrame, 28, 1, file);
        }
    }
    
    for (int i = 0; i < indexCount; i++) {
        Frame* frame = ArrayGetData(_image->frameArray, i);
        if (!frame) {
            return -1;
        }

        fwrite(&frame->colorFormat, 1, INT_LEN, file);
        fwrite(&frame->compressType, 1, INT_LEN, file);

        if (frame->colorFormat != LINK) {
            fwrite(&frame->width, 1, INT_LEN, file);
            fwrite(&frame->height, 1, INT_LEN, file);
            fwrite(&frame->datasize, 1, INT_LEN, file);
            fwrite(&frame->basePosX, 1, INT_LEN, file);
            fwrite(&frame->basePosY, 1, INT_LEN, file);
            fwrite(&frame->frameWidth, 1, INT_LEN, file);
            fwrite(&frame->frameHeight, 1, INT_LEN, file);
        }

        if (frame->compressType > LINK) {
            fwrite(&frame->unknow1, 1, INT_LEN, file);
            fwrite(&frame->ddsIndex, 1, INT_LEN, file);
            fwrite(&frame->ddsLeft, 1, INT_LEN, file);
            fwrite(&frame->ddsTop, 1, INT_LEN, file);
            fwrite(&frame->ddsRight, 1, INT_LEN, file);
            fwrite(&frame->ddsBotton, 1, INT_LEN, file);
            fwrite(&frame->unknow2, 1, INT_LEN, file);
        }
    }

    if (version == 5) {
        unsigned int ddsIndexCount = ArrayLength(_image->ddsFrameArray);
        for (int i = 0; i < ddsIndexCount; i++) {
            DDSFrame* ddsFrame = ArrayGetData(_image->ddsFrameArray, i);
            if (!ddsFrame || !ddsFrame->data)continue;
            fwrite(ddsFrame->data, ddsFrame->beforeCompressSize, 1, file);
        }
    }

    int frameCount = ArrayLength(_image->frameArray);
    for (int i = 0; i < frameCount; i++) {
        Frame* frame = ArrayGetData(_image->frameArray, i);
        if (!frame || !frame->data)continue;
        fwrite(frame->data, frame->datasize, 1, file);
    }
    _index->size = ftell(file) - _index->offset;
    return 0;
}

#ifdef __cplusplus
}
#endif