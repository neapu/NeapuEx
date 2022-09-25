#include "matrix.h"
#include <memory.h>
#include <assert.h>

ExMatrix* MatrixInit(int _width, int _height)
{
    ExMatrix* rst = (ExMatrix*)malloc(sizeof(ExMatrix));
    assert(rst);
    memset(rst, 0, sizeof(ExMatrix));
    rst->width = _width;
    rst->height = _height;
    rst->size = _width * _height;
    if (rst->size) {
        rst->data = (ExColor*)malloc(rst->size * sizeof(ExColor));
        if (!rst->data) {
            free(rst);
            return NULL;
        }
        memset(rst->data, 0, rst->size * sizeof(ExColor));
    }
    return rst;
}

void MatrixFree(ExMatrix* _matrix)
{
    if (!_matrix)return;
    if (_matrix->data) {
        free(_matrix->data);
    }
    free(_matrix);
}

ExColor MatrixGetColor(ExMatrix* _matrix, int _x, int _y)
{
    if (!_matrix || _x >= _matrix->width || _y >= _matrix->height) {
        ExColor rst;
        memset(&rst, 0, sizeof(ExColor));
        return rst;
    }

    return _matrix->data[_x + _y * _matrix->width];
}

int MatrixSetColor(ExMatrix* _matrix, int _x, int _y, ExColor color)
{
    if (!_matrix || _x >= _matrix->width || _y >= _matrix->height) {
        return -1;
    }
    _matrix->data[_x + _y * _matrix->width] = color;
    return 0;
}

ExMatrix* MatrixClip(ExMatrix* _src, int _left, int _top, int _right, int _botton)
{
    if (!_src || _src->size == 0) {
        return NULL;
    }
    if (_right >= _src->width) {
        _right = _src->width;
    }
    if (_botton >= _src->height) {
        _botton = _src->height;
    }

    if (_left >= _right || _top >= _botton) {
        return NULL;
    }
    ExMatrix* rst = MatrixInit(_right - _left, _botton - _top);
    if (!rst)return NULL;
    int index = 0;
    for (int y = _top; y < _botton; y++) {
        for (int x = _left; x < _right; x++) {
            rst->data[index] = MatrixGetColor(_src, x, y);
            ++index;
        }
    }
    return rst;
}
