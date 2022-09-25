#include "exarray.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>

#define MIN_LEN 128

typedef struct tagExArray {
    void** data;
    int len;
    int maxLen;
} ExArray;

typedef struct tagExByteArray {
    unsigned char* data;
    int len;
    int maxLen;
} ByteArray;

int ArrayExtend(ExArray* _array)
{
    if (!_array || !_array->data) {
        return -1;
    }
    int newLen = _array->maxLen + MIN_LEN;
    void** newData = (void**)malloc(sizeof(void*) * newLen);
    assert(newData);
    memcpy_s(newData, sizeof(void*) * newLen, _array->data, sizeof(void*) * _array->len);
    free(_array->data);
    _array->data = newData;
    _array->maxLen = newLen;
    return 0;
}

ExArray* ArrayInit(int _len)
{
    ExArray* rst = (ExArray*)malloc(sizeof(ExArray));
    assert(rst);

    memset(rst, 0, sizeof(ExArray));

    // MIN_LEN的整数倍
    rst->maxLen = _len + MIN_LEN - (_len % MIN_LEN);
    rst->len = 0;
    rst->data = (void**)malloc(sizeof(void*) * rst->maxLen);
    assert(rst->data);
    memset(rst->data, 0, sizeof(void*) * rst->maxLen);

    return rst;
}

void ArrayFree(ExArray* _array)
{
    if (!_array)return;
    if (_array->data) {
        free(_array->data);
    }
    free(_array);
}

int ArrayLength(ExArray* _array)
{
    if (!_array) {
        return -1;
    }
    return _array->len;
}

int ArrayAppend(ExArray* _array, void* _data)
{
    if (!_array || !_array->data) {
        return -1;
    }
    if (_array->len >= _array->maxLen) {
        int rst = ArrayExtend(_array);
        if (rst < 0) {
            return rst;
        }
    }

    _array->data[_array->len] = _data;
    _array->len += 1;
    return 0;
}

void* ArrayGetData(ExArray* _array, int _index)
{
    if (!_array || !_array->data) {
        return NULL;
    }

    if (_array->len <= _index) {
        return NULL;
    }

    return _array->data[_index];
}

void* ArrayRemoveData(ExArray* _array, int _index)
{
    if (!_array || !_array->data) {
        return NULL;
    }

    if (_array->len <= _index) {
        return NULL;
    }

    void* data = _array->data[_index];

    for (int i = _index; i < _array->len - 1; i++) {
        _array->data[i] = _array->data[i + 1];
    }
    _array->len -= 1;

    return data;
}

int ByteArrayExtend(ExByteArray* _array, int _len)
{
    if (!_array || !_array->data) {
        return -1;
    }
    int newLen = _array->maxLen + MIN_LEN;
    unsigned char* newData = (unsigned char*)malloc(newLen);
    assert(newData);
    memcpy_s(newData,  newLen, _array->data,  _array->len);
    free(_array->data);
    _array->data = newData;
    _array->maxLen = newLen;
    return 0;
}

ExByteArray* ByteArrayInit(int _len)
{
    ExByteArray* rst = (ExByteArray*)malloc(sizeof(ExByteArray));
    assert(rst);

    memset(rst, 0, sizeof(ExByteArray));

    // MIN_LEN的整数倍
    rst->maxLen = _len + MIN_LEN - (_len % MIN_LEN);
    rst->len = 0;
    rst->data = (unsigned char*)malloc(rst->maxLen);
    assert(rst->data);
    memset(rst->data, 0, rst->maxLen);

    return rst;
}

void ByteArrayFree(ExByteArray* _array)
{
    if (!_array)return;
    if (_array->data) {
        free(_array->data);
    }
    free(_array);
}

int ByteArrayLength(ExByteArray* _array)
{
    if (!_array)return -1;
    return _array->len;
    return 0;
}

int ByteArrayAppend(ExByteArray* _array, const unsigned char* _data, int _dataLen)
{
    if (!_array)return -1;
    int newLen = _dataLen + _array->len;
    if (newLen > _array->maxLen) {
        int rst = ByteArrayExtend(_array, newLen);
        if (rst < 0) {
            return rst;
        }
    }
    memcpy_s(_array->data + _array->len, _array->maxLen - _array->len, _data, _dataLen);
    _array->len = newLen;
    return 0;
}

int ByteArrayGetData(ExByteArray* _array, int _offset, const unsigned char* _buf, int _bufLen)
{
    if (!_array || _buf)return -1;
    if (_offset >= _array->len)return -1;
    int readSize = _array->len - _offset > _bufLen ? _bufLen : _array->len - _offset;
    memcpy_s(_buf, _bufLen, _array->data + _offset, readSize);
    return readSize;
}
