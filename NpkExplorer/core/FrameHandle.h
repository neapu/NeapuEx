#pragma once
#include <string>

typedef struct tagFrame Frame;
typedef struct tagExMatrix ExMatrix;
typedef struct tagExColor ExColor;

namespace extoolkit {
using Color = struct tagColor
{
    tagColor() : color(0) {}
    tagColor(const ExColor& exColor);
    union {
        struct {
            unsigned char a;
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };
        unsigned int color;
    };
};

class FrameHandle {
public:
    FrameHandle();
    virtual ~FrameHandle();

    int Init(Frame* _frame, int _palette);

    int Width();
    int Height();
    int PosX();
    int PosY();
    int FrameWidth();
    int FrameHeight();

    int DDSIndex();
    int DDSLeft();
    int DDSTop();
    int DDSRight();
    int DDSBotton();

    int ColorFormat();

    bool IsDDSFormat();

    int LinkTo();

    Color GetColor(unsigned int x, unsigned int y);

protected:
    Frame* m_frame = nullptr; // ImageObject持有，不需要释放
    ExMatrix* m_matrix = nullptr;
    int m_linkTo = -1;
};

std::string ColorFormatToString(int _colorFormat);
} // namespace extoolkit