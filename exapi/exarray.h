#ifndef _EXARRAY_H_
#define _EXARRAY_H_
#include "npkdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_ERROR_NULLPTR -1
#define ARRAY_ERROR_MALLOC_FAILED -2
#define ARRAY_ERROR_INDEX_OVERFLOW -3

typedef struct tagExArray ExArray;
typedef struct tagExByteArray ExByteArray;

ExArray* ArrayInit(int _len);
void ArrayFree(ExArray* _array);
int ArrayLength(ExArray* _array);
int ArrayAppend(ExArray* _array, void* _data);
void* ArrayGetData(ExArray* _array, int _index);
void* ArrayRemoveData(ExArray* _array, int _index);

ExByteArray* ByteArrayInit(int _len);
void ByteArrayFree(ExByteArray* _array);
int ByteArrayLength(ExByteArray* _array);
int ByteArrayAppend(ExByteArray* _array, const unsigned char* _data, int _dataLen);
int ByteArrayGetData(ExByteArray* _array, int _offset, const unsigned char* buf, int _bufLen);

#ifdef __cplusplus
}
#endif

#endif