#include "dds.h"
#include <memory.h>

typedef struct DDSPixelFormat {
    unsigned int size;        //像素格式数据大小，固定为32(0x20)
    unsigned int flags;       //标志位，DNF里一般用DDPF_FOURCC（0x04）
    unsigned int fourCC;      // DXT格式，flag里有DDPF_FOURCC有效，DNF里一般为字符串DXT1-5（0x31545844 - 0x35545844）
    unsigned int rgbBitCount; //一个RGB(A)包含的位数，flag里有DDPF_RGB、DDPF_YUV、DDPF_LUMINANCE有效，DNF里一般无效
    unsigned int rBitMask;    // R通道(Y通道或亮度通道)掩码，DNF用不上
    unsigned int gBitMask;    // G通道(U通道)掩码，DNF用不上
    unsigned int bBitMask;    // B通道(V通道)掩码，DNF用不上
    unsigned int aBitMask;    // A通道掩码，DNF用不上
} DDSPixelFormat;
//像素头
typedef struct DDSHeader {
    unsigned int magic;             //标识"DDS "，固定为542327876（0x20534444）
    unsigned int headSize;          //首部大小，固定为124（0x7C）
    unsigned int flags;             //标志位（见DDSHeaderFlags），DNF里经常采用的是“压缩纹理”，一般为0x81007
    unsigned int height;            //高度
    unsigned int width;             //宽度
    unsigned int pitchOrLinearSize; //间距
    unsigned int depth;             // DNF里没用，标志位DDSD_DEPTH有效时有用
    unsigned int mipMapCount;       // DNF里没用，标志位DDSD_MIPMAPCOUNT有效时有用
    unsigned int reserved1[11];     //保留位，DNF里第十双字一般为"NVTT"（0x5454564E），第十一双字一般为0x20008，其他为零
    DDSPixelFormat pixelFormat;     //像素格式数据，见上文
    unsigned int caps1;             //曲面的复杂性（见DDSCaps），DNF里一般为0x1000
    unsigned int caps2;             //曲面的其他信息（主要是三维性），DNF里不用，为零
    unsigned int caps3;             //默认零
    unsigned int caps4;             //默认零
    unsigned int reserved2;         //默认零
} DDSHeader;
//枚举
typedef enum DDSHeaderFlag {
    DDSD_CAPS = 0x00000001U,        // Required in every .dds file.
    DDSD_HEIGHT = 0x00000002U,      // Required in every .dds file.
    DDSD_WIDTH = 0x00000004U,       // Required in every .dds file.
    DDSD_PITCH = 0x00000008U,       // Required when pitch is provided for an uncompressed texture.
    DDSD_PIXELFORMAT = 0x00001000U, // Required in every .dds file.
    DDSD_MIPMAPCOUNT = 0x00020000U, // Required in a mipmapped texture.
    DDSD_LINEARSIZE = 0x00080000U,  // Required when pitch is provided for a compressed texture.
    DDSD_DEPTH = 0x00800000U        // Required in a depth texture.
} DDSHeaderFlag;
typedef enum DDSCap {
    DDSCAPS_COMPLEX = 0x00000008U, // Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or mipmapped volume texture).
    DDSCAPS_MIPMAP = 0x00400000U,  // Optional; should be used for a mipmap.
    DDSCAPS_TEXTURE = 0x00001000U, // Required
} DDSCap;
typedef enum DDSCap2 {
    DDSCAPS2_CUBEMAP = 0x00000200U,
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400U,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800U,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000U,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000U,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000U,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000U,
    DDSCAPS2_VOLUME = 0x00200000U
} DDSCap2;
typedef enum DDSPixelFormatFlag {
    DDPF_ALPHAPIXELS = 0x00000001U,
    DDPF_ALPHA = 0x00000002U,
    DDPF_FOURCC = 0x00000004U,
    DDPF_RGB = 0x00000040U,
    DDPF_YUV = 0x00000200U,
    DDPF_LUMINANCE = 0x00002000U
} DDSPixelFormatFlag;

ExColor RGB565TO8888(unsigned short _c565)
{
    ExColor rst;
    rst.a = 0xFF;
    rst.r = (_c565 & 0xf800) >> 8;
    rst.g = (_c565 & 0x07e0) >> 3;
    rst.b = (_c565 & 0x001f) << 3;
    return rst;
}

void DXT1UncompressUnit(const unsigned char _data[], ExColor _colorList[])
{
    unsigned int clrPart = (unsigned int)_data[0] | ((unsigned int)_data[1] << 8) | ((unsigned int)_data[2] << 16) | ((unsigned int)_data[3] << 24);
    unsigned int idxPart = (unsigned int)_data[4] | ((unsigned int)_data[5] << 8) | ((unsigned int)_data[6] << 16) | ((unsigned int)_data[7] << 24);
    unsigned short uc1 = (unsigned short)(clrPart & 0xffff);
    unsigned short uc2 = (unsigned short)(clrPart >> 16);
    ExColor clrList[4];
    clrList[0] = RGB565TO8888(uc1);
    clrList[1] = RGB565TO8888(uc2);
    if (uc1 <= uc2) {
        clrList[2].a = 0xff;
        clrList[2].r = (clrList[0].r + clrList[1].r) / 2;
        clrList[2].g = (clrList[0].g + clrList[1].g) / 2;
        clrList[2].b = (clrList[0].b + clrList[1].b) / 2;
        clrList[3].a = 0x00;
        clrList[3].r = 0x00;
        clrList[3].g = 0x00;
        clrList[3].b = 0x00;
    }
    else {
        clrList[2].a = 0xff;
        clrList[2].r = (2 * clrList[0].r + clrList[1].r) / 3;
        clrList[2].g = (2 * clrList[0].g + clrList[1].g) / 3;
        clrList[2].b = (2 * clrList[0].b + clrList[1].b) / 3;
        clrList[3].a = 0xff;
        clrList[3].r = (clrList[0].r + 2 * clrList[1].r) / 3;
        clrList[3].g = (clrList[0].g + 2 * clrList[1].g) / 3;
        clrList[3].b = (clrList[0].b + 2 * clrList[1].b) / 3;
    }
    for (unsigned int i = 0; i < 16; i++) {
        unsigned char index = idxPart & 0x0003;
        idxPart >>= 2;
        _colorList[i] = clrList[index];
    }
}

#define DXT1_UINT_LEN 8
void DXT1Uncompress(const unsigned char* _data, size_t _len, ExMatrix* matrix)
{
    size_t pos = 0;
    unsigned int blockWidth = matrix->width / 4;
    unsigned int blockHeight = matrix->height / 4;
    unsigned char data[DXT1_UINT_LEN];
    ExColor colorList[16];
    for (unsigned int y = 0; y < blockHeight; y++) {
        for (unsigned int x = 0; x < blockWidth; x++) {
            memcpy_s(data, DXT1_UINT_LEN, _data + pos, DXT1_UINT_LEN);
            pos += DXT1_UINT_LEN;
            DXT1UncompressUnit(data, colorList);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 0, colorList[0]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 0, colorList[1]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 0, colorList[2]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 0, colorList[3]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 1, colorList[4]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 1, colorList[5]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 1, colorList[6]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 1, colorList[7]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 2, colorList[8]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 2, colorList[9]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 2, colorList[10]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 2, colorList[11]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 3, colorList[12]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 3, colorList[13]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 3, colorList[14]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 3, colorList[15]);
        }
    }
}

void DXT3UncompressUnit(const unsigned char _data[], ExColor _colorList[])
{
    unsigned int clrPart = (unsigned int)_data[8] | ((unsigned int)_data[9] << 8) | ((unsigned int)_data[10] << 16) | ((unsigned int)_data[11] << 24);
    unsigned int idxPart = (unsigned int)_data[12] | ((unsigned int)_data[13] << 8) | ((unsigned int)_data[14] << 16) | ((unsigned int)_data[15] << 24);
    unsigned short uc1 = (unsigned short)(clrPart & 0xffff);
    unsigned short uc2 = (unsigned short)(clrPart >> 16);
    ExColor clrList[4];
    clrList[0] = RGB565TO8888(uc1);
    clrList[1] = RGB565TO8888(uc2);
    clrList[2].a = 0xff;
    clrList[2].r = (2 * clrList[0].r + clrList[1].r) / 3;
    clrList[2].g = (2 * clrList[0].g + clrList[1].g) / 3;
    clrList[2].b = (2 * clrList[0].b + clrList[1].b) / 3;
    clrList[3].a = 0xff;
    clrList[3].r = (clrList[0].r + 2 * clrList[1].r) / 3;
    clrList[3].g = (clrList[0].g + 2 * clrList[1].g) / 3;
    clrList[3].b = (clrList[0].b + 2 * clrList[1].b) / 3;


    for (unsigned int i = 0; i < 16; i++) {
        unsigned char index = idxPart & 0x0003;
        idxPart >>= 2;
        _colorList[i] = clrList[index];
        if (i % 2 == 0) {
            _colorList[i].a = (_data[i / 2] & 0x0f) * 0x11;
        }
        else {
            _colorList[i].a = ((_data[i / 2] >> 4) & 0x0f) * 0x11;
        }
        
    }
}

#define DXT3_UNIT_LEN 16
void DXT3Uncompress(const unsigned char* _data, size_t _len, ExMatrix* matrix)
{
    size_t pos = 0;
    unsigned int blockWidth = matrix->width / 4;
    unsigned int blockHeight = matrix->height / 4;
    unsigned char data[DXT3_UNIT_LEN];
    ExColor colorList[16];
    for (unsigned int y = 0; y < blockHeight; y++) {
        for (unsigned int x = 0; x < blockWidth; x++) {
            memcpy_s(data, DXT3_UNIT_LEN, _data + pos, DXT3_UNIT_LEN);
            pos += DXT3_UNIT_LEN;
            DXT3UncompressUnit(data, colorList);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 0, colorList[0]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 0, colorList[1]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 0, colorList[2]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 0, colorList[3]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 1, colorList[4]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 1, colorList[5]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 1, colorList[6]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 1, colorList[7]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 2, colorList[8]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 2, colorList[9]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 2, colorList[10]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 2, colorList[11]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 3, colorList[12]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 3, colorList[13]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 3, colorList[14]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 3, colorList[15]);
        }
    }
}

void DXT5UncompressUnit(const unsigned char _data[], ExColor _colorList[])
{
    unsigned int clrPart = (unsigned int)_data[8] | ((unsigned int)_data[9] << 8) | ((unsigned int)_data[10] << 16) | ((unsigned int)_data[11] << 24);
    unsigned int idxPart = (unsigned int)_data[12] | ((unsigned int)_data[13] << 8) | ((unsigned int)_data[14] << 16) | ((unsigned int)_data[15] << 24);
    unsigned short uc1 = (unsigned short)(clrPart & 0xffff);
    unsigned short uc2 = (unsigned short)(clrPart >> 16);
    ExColor clrList[4];
    clrList[0] = RGB565TO8888(uc1);
    clrList[1] = RGB565TO8888(uc2);
    clrList[2].a = 0xff;
    clrList[2].r = (2 * clrList[0].r + clrList[1].r) / 3;
    clrList[2].g = (2 * clrList[0].g + clrList[1].g) / 3;
    clrList[2].b = (2 * clrList[0].b + clrList[1].b) / 3;
    clrList[3].a = 0xff;
    clrList[3].r = (clrList[0].r + 2 * clrList[1].r) / 3;
    clrList[3].g = (clrList[0].g + 2 * clrList[1].g) / 3;
    clrList[3].b = (clrList[0].b + 2 * clrList[1].b) / 3;

    // 透明列表
    unsigned char alphaList[8];
    unsigned char ua1 = _data[0];
    unsigned char ua2 = _data[1];
    alphaList[0] = ua1;
    alphaList[1] = ua2;
    if (ua1 <= ua2) {
        alphaList[2] = (4 * ua1 + 1 * ua2) / 5;
        alphaList[3] = (3 * ua1 + 2 * ua2) / 5;
        alphaList[4] = (2 * ua1 + 3 * ua2) / 5;
        alphaList[5] = (1 * ua1 + 4 * ua2) / 5;
        alphaList[6] = 0;
        alphaList[7] = 0xff;
    }
    else {
        alphaList[2] = (6 * ua1 + 1 * ua2) / 7;
        alphaList[3] = (5 * ua1 + 2 * ua2) / 7;
        alphaList[4] = (4 * ua1 + 3 * ua2) / 7;
        alphaList[5] = (3 * ua1 + 4 * ua2) / 7;
        alphaList[6] = (2 * ua1 + 5 * ua2) / 7;
        alphaList[7] = (1 * ua1 + 6 * ua2) / 7;
    }

    //透明度索引
    unsigned short alphaIndex[16];
    alphaIndex[0] = ((_data[2] & 0x07) >> 0);
    alphaIndex[1] = ((_data[2] & 0x38) >> 3);
    alphaIndex[2] = ((_data[2] & 0xC0) >> 6) | ((_data[3] & 0x01) << 2);
    alphaIndex[3] = ((_data[3] & 0x0E) >> 1);
    alphaIndex[4] = ((_data[3] & 0x70) >> 4);
    alphaIndex[5] = ((_data[3] & 0x80) >> 7) | ((_data[4] & 0x03) << 1);
    alphaIndex[6] = ((_data[4] & 0x1C) >> 2);
    alphaIndex[7] = ((_data[4] & 0xE0) >> 5);
    alphaIndex[8] = ((_data[5] & 0x07) >> 0);
    alphaIndex[9] = ((_data[5] & 0x38) >> 3);
    alphaIndex[10] = ((_data[5] & 0xC0) >> 6) | ((_data[6] & 0x01) << 2);
    alphaIndex[11] = ((_data[6] & 0x0E) >> 1);
    alphaIndex[12] = ((_data[6] & 0x70) >> 4);
    alphaIndex[13] = ((_data[6] & 0x80) >> 7) | ((_data[7] & 0x03) << 1);
    alphaIndex[14] = ((_data[7] & 0x1C) >> 2);
    alphaIndex[15] = ((_data[7] & 0xE0) >> 5);

    for (unsigned int i = 0; i < 16; i++) {
        unsigned char index = idxPart & 0x0003;
        idxPart >>= 2;
        _colorList[i] = clrList[index];
        _colorList[i].a = alphaList[alphaIndex[i]];

    }
}

#define DXT5_UNIT_LEN 16
void DXT5Uncompress(const unsigned char* _data, size_t _len, ExMatrix* matrix)
{
    size_t pos = 0;
    unsigned int blockWidth = matrix->width / 4;
    unsigned int blockHeight = matrix->height / 4;
    unsigned char data[DXT5_UNIT_LEN];
    ExColor colorList[16];
    for (unsigned int y = 0; y < blockHeight; y++) {
        for (unsigned int x = 0; x < blockWidth; x++) {
            memcpy_s(data, DXT5_UNIT_LEN, _data + pos, DXT5_UNIT_LEN);
            pos += DXT5_UNIT_LEN;
            DXT5UncompressUnit(data, colorList);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 0, colorList[0]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 0, colorList[1]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 0, colorList[2]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 0, colorList[3]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 1, colorList[4]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 1, colorList[5]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 1, colorList[6]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 1, colorList[7]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 2, colorList[8]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 2, colorList[9]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 2, colorList[10]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 2, colorList[11]);
            MatrixSetColor(matrix, 4 * x + 0, 4 * y + 3, colorList[12]);
            MatrixSetColor(matrix, 4 * x + 1, 4 * y + 3, colorList[13]);
            MatrixSetColor(matrix, 4 * x + 2, 4 * y + 3, colorList[14]);
            MatrixSetColor(matrix, 4 * x + 3, 4 * y + 3, colorList[15]);
        }
    }
}

ExMatrix* DDSUncompress(const unsigned char *_data, size_t _len)
{
    DDSHeader header;
    size_t headerLen = sizeof(DDSHeader);
    if (headerLen >= _len) {
        return NULL;
    }
    memcpy_s(&header, headerLen, _data, headerLen);

    if (header.magic != 0x20534444)return NULL;
    if (header.headSize != 0x7c)return NULL;
    if (header.flags != 0x81007)return NULL;

    ExMatrix* matrix = MatrixInit(header.width, header.height);
    if (!matrix)return NULL;
    unsigned char* imgData = (unsigned char*)malloc(header.pitchOrLinearSize);
    memcpy_s(imgData, header.pitchOrLinearSize, _data + headerLen, header.pitchOrLinearSize);
    switch (header.pixelFormat.fourCC) {
    case 0x31545844:
        DXT1Uncompress(imgData, header.pitchOrLinearSize, matrix);
        break;
    case 0x33545844:
        DXT3Uncompress(imgData, header.pitchOrLinearSize, matrix);
        break;
    case 0x35545844:
        DXT5Uncompress(imgData, header.pitchOrLinearSize, matrix);
        break;
    default:
        MatrixFree(matrix);
        matrix = NULL;
    }
    free(imgData);
    return matrix;
}
