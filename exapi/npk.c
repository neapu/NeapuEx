#include "npk.h"
#include <string.h>
#include <zlib.h>
#include "sha256.h"
#include "image.h"
#include "logger.h"
#include <assert.h>



#define FLAG_LEN 16
#define LOAD_NPK_FAILED()       \
    {                           \
        if (rst) NpkFree(rst);  \
        if (file) fclose(file); \
        file = NULL;            \
        return NULL;            \
    }
#define NPK_READ(buf, len)                        \
    {                                             \
        read = fread((buf), 1, (len), file);      \
        if (read != (len)) { LOAD_NPK_FAILED(); } \
    }
NpkObject* NpkLoad(const char* _fileName)
{
    NpkObject* rst = NULL;
    size_t pos = 0;
    char szFlag[FLAG_LEN];
    unsigned int imageCount;
    FILE* file = NULL;
    size_t read;
    ImageIndex* imageIndexList = NULL;

    if (fopen_s(&file, _fileName, "rb") != 0) {
        return NULL;
    }

    NPK_READ(szFlag, FLAG_LEN);
    if (memcmp(szFlag, "NeoplePack_Bill", FLAG_LEN) != 0) {
        LOG_ERROR("Load Npk Error:File Head Error.");
        LOAD_NPK_FAILED();
    }

    NPK_READ(&imageCount, 4);
    imageIndexList = (ImageIndex*)malloc(imageCount * sizeof(ImageIndex));
    if (!imageIndexList) {
        LOG_DEADLY("Load Npk Error:malloc failed.");
        LOAD_NPK_FAILED();
    }
    for (unsigned int i = 0; i < imageCount; i++) {
        NPK_READ(&imageIndexList[i], sizeof(ImageIndex));
    }

    rst = (NpkObject*)malloc(sizeof(NpkObject));
    if (!rst) {
        LOG_DEADLY("Load Npk Error:malloc failed.");
        LOAD_NPK_FAILED();
    }

    memset(rst, 0, sizeof(NpkObject));

    rst->imageList = ArrayInit(imageCount);

    // rst->_imageCount = imageCount;
    // rst->_imageObjectList = (ImageObject **)malloc(imageCount * sizeof(ImageObject *));
    if (!rst->imageList) {
        LOG_DEADLY("Load Npk Error:malloc failed.");
        LOAD_NPK_FAILED();
    }

    for (unsigned int i = 0; i < imageCount; i++) {
        ImageObject* imageObject = ImageLoad(file, &imageIndexList[i]);
        if (!imageCount) {
            LOG_ERROR("Load Npk Error:File Head Error.");
        }
        ArrayAppend(rst->imageList, imageObject);
    }

    fclose(file);

    return rst;
}

void NpkFree(NpkObject* _npk)
{
    if (!_npk) {
        return;
    }

    int len = ArrayLength(_npk->imageList);
    for (int i = 0; i < len; i++) {
        ImageFree((ImageObject*)ArrayGetData(_npk->imageList, i));
    }
    ArrayFree(_npk->imageList);
    free(_npk);
}

int NpkGetImageCount(NpkObject* _npk)
{
    if (!_npk) return -1;
    return ArrayLength(_npk->imageList);
}

ImageObject* NpkGetImage(NpkObject* _npk, unsigned int _imageIndex)
{
    if (!_npk) {
        return NULL;
    }
    return (ImageObject*)ArrayGetData(_npk->imageList, _imageIndex);
}

ImageObject* NpkRemoveImage(NpkObject* _npk, unsigned int _imageIndex)
{
    if (!_npk) {
        return NULL;
    }
    return (ImageObject*)ArrayRemoveData(_npk->imageList, _imageIndex);
}

NpkObject* NpkCreate()
{
    NpkObject* rst = (NpkObject*)malloc(sizeof(NpkObject));
    assert(rst);
    rst->imageList = ArrayInit(0);
    return rst;
}

int NpkAppendImage(NpkObject* _npk, ImageObject* _image)
{
    return ArrayAppend(_npk->imageList, _image);
}

int NpkSave(NpkObject* _npk, const char* _fileName)
{
    if (!_npk)return -1;
    FILE* file = NULL;
    int imageCount = 0;

    if (fopen_s(&file, _fileName, "wb") != 0) {
        return -1;
    }

    fwrite("NeoplePack_Bill", FLAG_LEN, 1, file);
    imageCount = ArrayLength(_npk->imageList);
    fwrite(&imageCount, 4, 1, file);

    
    
    
    return 0;
}
