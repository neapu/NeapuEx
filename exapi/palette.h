#ifndef _PALETTE_H_
#define _PALETTE_H_

#include "npkdef.h"
#include "exarray.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct tagExColor ExColor;
typedef struct tagPalette {
    ExColor* colorList; // color array
    int colorCount;
} Palette;

typedef struct tagPaletteManager {
    ExArray* paletteArray;
} PaletteManager;

EXAPI_EXPORT PaletteManager* PaletteManagerInit(int _count);
EXAPI_EXPORT void PaletteManagerFree(PaletteManager* _palette);
EXAPI_EXPORT int PMAddPalette(PaletteManager* _context, int _colorCount, const char* data);
EXAPI_EXPORT ExColor PMGetColor(PaletteManager* _context, int _paletteIndex, int _colorCount);
EXAPI_EXPORT int PMGetPaletteCount(PaletteManager* _context);
EXAPI_EXPORT int PMGetColorCount(PaletteManager* _context, int _paletteIndex);
EXAPI_EXPORT PaletteManager* PMCopy(PaletteManager* _context);
const void* PMGetPaletteData(PaletteManager* _context, int _paletteIndex);

#ifdef __cplusplus
}
#endif

#endif