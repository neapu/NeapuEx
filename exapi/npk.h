#ifndef _NPK_H_
#define _NPK_H_
#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"
#include "npkdef.h"
#include "exarray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagNpkObject {
    ExArray* imageList;
} NpkObject;
typedef struct tagImageObject ImageObject;

/*
 * 从文件中读取Npk文件，成功返回0，失败返回负值
 */
EXAPI_EXPORT NpkObject* NpkLoad(const char* _fileName);

/*
 * 从内存中释放Npk对象，包含的Image对象一起释放
 */
EXAPI_EXPORT void NpkFree(NpkObject* _npk);

/*
 * 获取Npk对象包含的Image对象个数
 */
EXAPI_EXPORT int NpkGetImageCount(NpkObject* _npk);

/*
 * 获取Npk对象包含的Image对象，如果要单独释放Image对象，要先调用RemoveImage移除Image
 * 成功返回Image对象指针，失败返回NULL
 */
EXAPI_EXPORT ImageObject* NpkGetImage(NpkObject* _npk, unsigned int _imageIndex);

/*
 * 从Npk对象中移除Image，这个动作并不会释放Image对象
 * 成功返回Image对象指针，失败返回NULL
 */
EXAPI_EXPORT ImageObject* NpkRemoveImage(NpkObject* _npk, unsigned int _imageIndex);

NpkObject* NpkCreate();

int NpkAppendImage(NpkObject* _npk, ImageObject* _image);

int NpkSave(NpkObject* _npk, const char* _fileName);

#ifdef __cplusplus
}
#endif

#endif