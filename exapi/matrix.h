#pragma once
#include <stdlib.h>
#include "npkdef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagExColor
{
	union {
		struct {
			unsigned char a;
			unsigned char r;
			unsigned char g;
			unsigned char b;
		};
		unsigned int color;
	};
	
}ExColor;

typedef struct tagExMatrix
{
	ExColor* data;
	int width;
	int height;
	int size; // color count
	int baseX;
	int baseY;
	int frameWidth;
	int frameHeight;
}ExMatrix;

EXAPI_EXPORT ExMatrix* MatrixInit(int _width, int _height);
EXAPI_EXPORT void MatrixFree(ExMatrix* _matrix);
EXAPI_EXPORT ExColor MatrixGetColor(ExMatrix* _matrix, int _x, int _y);
EXAPI_EXPORT int MatrixSetColor(ExMatrix* _matrix, int _x, int _y, ExColor color);
EXAPI_EXPORT ExMatrix* MatrixClip(ExMatrix* _src, int _left, int _top, int _right, int _botton);

#ifdef __cplusplus
}
#endif