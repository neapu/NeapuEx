#include "FrameHandle.h"
#include <matrix.h>
#include <image.h>
#include <npkdef.h>

using namespace extoolkit;

extoolkit::tagColor::tagColor(const ExColor& exColor)
{
    this->color = exColor.color;
}

extoolkit::FrameHandle::FrameHandle()
{
}

extoolkit::FrameHandle::~FrameHandle()
{
    if (m_matrix) {
        ::MatrixFree(m_matrix);
    }
}

int extoolkit::FrameHandle::Init(Frame* _frame, int _palette)
{
    if (!_frame) return -1;
    m_frame = _frame;
    if (m_frame->colorFormat == LINK) {
        m_linkTo = m_frame->linkTo;
    }
    while (m_frame->colorFormat == LINK) {
        m_frame = ::ImageGetFrame(m_frame->parent, m_frame->linkTo);
    }

    m_matrix = ::FrameToMatrix(_frame, _palette);
    if (!m_matrix) return -1;
    return 0;
}

int extoolkit::FrameHandle::Width()
{
    return m_frame->width;
}

int extoolkit::FrameHandle::Height()
{
    return m_frame->height;
}

int extoolkit::FrameHandle::PosX()
{
    return m_frame->basePosX;
}

int extoolkit::FrameHandle::PosY()
{
    return m_frame->basePosY;
}

int extoolkit::FrameHandle::FrameWidth()
{
    return m_frame->frameWidth;
}

int extoolkit::FrameHandle::FrameHeight()
{
    return m_frame->frameHeight;
}

int extoolkit::FrameHandle::DDSIndex()
{
    return m_frame->ddsIndex;
}

int extoolkit::FrameHandle::DDSLeft()
{
    return m_frame->ddsLeft;
}

int extoolkit::FrameHandle::DDSTop()
{
    return m_frame->ddsTop;
}

int extoolkit::FrameHandle::DDSRight()
{
    return m_frame->ddsRight;
}

int extoolkit::FrameHandle::DDSBotton()
{
    return m_frame->ddsBotton;
}

int extoolkit::FrameHandle::ColorFormat()
{
    return m_frame->colorFormat;
}

bool extoolkit::FrameHandle::IsDDSFormat()
{
    if (m_frame->colorFormat == DDS_DXT1 || m_frame->colorFormat == DDS_DXT3 || m_frame->colorFormat == DDS_DXT5) {
        return true;
    }
    return false;
}

int extoolkit::FrameHandle::LinkTo()
{
    return m_linkTo;
}

Color extoolkit::FrameHandle::GetColor(unsigned int x, unsigned int y)
{
    if (!m_matrix) return Color();
    return ::MatrixGetColor(m_matrix, x, y);
}

std::string extoolkit::ColorFormatToString(int _colorFormat)
{
    switch (_colorFormat) {
    case ARGB8888:
        return "ARGB8888";
    case ARGB4444:
        return "ARGB4444";
    case ARGB1555:
        return "ARGB1555";
    case LINK:
        return "LINK";
    case DDS_DXT1:
    case DDS_DXT3:
    case DDS_DXT5:
        return "ARGB8888";
    case COLOR_UDEF:
        return "COLOR_UDEF";
    default:
        return "Unknow";
        break;
    }
}