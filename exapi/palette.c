#include "palette.h"
#include <stdlib.h>
#include <memory.h>
#include "npkdef.h"
#include "matrix.h"
#include <assert.h>

Palette* PaletteInit(int _count, const char* data);
void PaletteFree(Palette* palette);

PaletteManager* PaletteManagerInit(int _count)
{
    if (_count == 0) return NULL;
    PaletteManager* rst = (PaletteManager*)malloc(sizeof(PaletteManager));
    assert(rst);
    memset(rst, 0, sizeof(PaletteManager));
    rst->paletteArray = ArrayInit(_count);
    return rst;
}

void PaletteManagerFree(PaletteManager* palette)
{
    if (palette) {
        int len = ArrayLength(palette->paletteArray);
        for (int i = 0; i < len; i++) {
            PaletteFree((Palette*)ArrayGetData(palette->paletteArray, i));
        }
        free(palette);
    }
}

int PMAddPalette(PaletteManager* _context, int _colorCount, const char* data)
{
    if (_context == NULL) {
        return -1;
    }
    Palette* palette = PaletteInit(_colorCount, data);
    return ArrayAppend(_context->paletteArray, palette);
}

ExColor PMGetColor(PaletteManager* _context, int _paletteIndex, int _colorCount)
{
    ExColor rst;
    memset(&rst, 0, sizeof(ExColor));
    if (!_context) {
        return rst;
    }

    Palette* palette = (Palette*)ArrayGetData(_context->paletteArray, _paletteIndex);
    if (!palette) {
        return rst;
    }

    if (_colorCount >= palette->colorCount) {
        return rst;
    }

    return palette->colorList[_colorCount];
}

int PMGetPaletteCount(PaletteManager* _context)
{
    if (!_context) {
        return -1;
    }
    return ArrayLength(_context->paletteArray);
}

int PMGetColorCount(PaletteManager* _context, int _paletteIndex)
{
    if (!_context) {
        return -1;
    }
    Palette* palette = (Palette*)ArrayGetData(_context->paletteArray, _paletteIndex);
    if (!palette) {
        return -1;
    }
    return palette->colorCount;
}

PaletteManager* PMCopy(PaletteManager* _context)
{
    if (!_context) {
        return NULL;
    }
    int len = ArrayLength(_context->paletteArray);
    if (len <= 0)return NULL;
    PaletteManager* rst = PaletteManagerInit(len);
    for (int i = 0; i < len; i++) {
        Palette* src = (Palette*)ArrayGetData(_context->paletteArray, i);
        assert(src);
        PMAddPalette(rst, src->colorCount, (const char*)src->colorList);
    }
    return rst;
}

Palette* PaletteInit(int _count, const char* data)
{
    if (_count == 0) return NULL;
    Palette* rst = (Palette*)malloc(sizeof(Palette));
    assert(rst);
    memset(rst, 0, sizeof(Palette));
    size_t len = sizeof(ExColor) * _count;
    rst->colorCount = _count;
    rst->colorList = (ExColor*)malloc(len);
    assert(rst->colorList);
    memcpy_s(rst->colorList, len, data, len);
    return rst;
}

void PaletteFree(Palette* palette)
{
    if (palette) {
        if (palette->colorList) {
            free(palette->colorList);
        }
        free(palette);
    }
}

const void* PMGetPaletteData(PaletteManager* _context, int _paletteIndex)
{
    Palette* palette = ArrayGetData(_context->paletteArray, _paletteIndex);
    if (!palette)return NULL;
    return palette->colorList;
}