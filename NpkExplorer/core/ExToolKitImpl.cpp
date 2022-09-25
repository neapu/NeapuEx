#include "ExToolKitImpl.h"
#include "npk.h"
#include "image.h"
#include "palette.h"

using namespace extoolkit;

constexpr size_t NAME_LEN = 256;

std::shared_ptr<ExToolKit> extoolkit::CreateExToolKit()
{
    return std::shared_ptr<ExToolKit>(new extoolkit::ExToolKitImpl);
}

ExToolKitImpl::~ExToolKitImpl()
{
    Close();
}

int ExToolKitImpl::Open(std::string _fileName)
{
    if (m_npkObject) {
        Close();
    }
    m_npkObject = ::NpkLoad(_fileName.c_str());
    if (m_npkObject == nullptr) {
        return -1;
    }
    return 0;
}

void ExToolKitImpl::Close()
{
    m_curentImage = nullptr; // npk持有不需要单独释放

    if (m_npkObject) {
        ::NpkFree(m_npkObject);
        m_npkObject = nullptr;
    }
    
    m_curentFrameIndex = 0;
    m_curentImageIndex = 0;
    m_curentPaletteIndex = 0;
}

int ExToolKitImpl::GetFrameCount()
{
    if (m_npkObject == nullptr) {
        return -1;
    }

    auto img = ::NpkGetImage(m_npkObject, m_curentImageIndex);

    return ::ImageGetFrameCount(img);
}

int ExToolKitImpl::GetCurentFrame()
{
    return m_curentFrameIndex;
}

void ExToolKitImpl::SetCurentFrame(int _frameIndex)
{
    if (GetFrameCount() <= _frameIndex) {
        return;
    }
    m_curentFrameIndex = _frameIndex;
}

std::shared_ptr<FrameHandle> extoolkit::ExToolKitImpl::GetCurentFrameHandle()
{
    if (m_npkObject == nullptr) {
        return nullptr;
    }
    auto img = ::NpkGetImage(m_npkObject, m_curentImageIndex);
    Frame* frame = ::ImageGetFrame(img, m_curentFrameIndex);
    auto rst = std::make_shared<FrameHandle>();
    if (rst->Init(frame, m_curentPaletteIndex) < 0) {
        return nullptr;
    }
    return rst;
}

void extoolkit::ExToolKitImpl::NextFrame()
{
    if (m_npkObject == nullptr) {
        return;
    }
    m_curentFrameIndex++;
    if (m_curentFrameIndex >= this->GetFrameCount()) {
        m_curentFrameIndex = 0;
    }
}

void extoolkit::ExToolKitImpl::PreviousFrame()
{
    if (m_npkObject == nullptr) {
        return;
    }
    m_curentFrameIndex--;
    if (m_curentFrameIndex < 0) {
        m_curentFrameIndex = this->GetFrameCount() - 1;
    }
}

int extoolkit::ExToolKitImpl::GetImageVersion()
{
    return ::ImageGetVersion(m_curentImage);
}

std::string ExToolKitImpl::GetImagePath()
{
    return m_curentImage->pathName;
}

std::vector<std::string> ExToolKitImpl::GetImageNameList()
{
    std::vector<std::string> rst;
    if (m_npkObject == nullptr) {
        return rst;
    }

    int count = ::NpkGetImageCount(m_npkObject);
    for (int i = 0; i < count; i++) {
        auto img = ::NpkGetImage(m_npkObject, i);
        rst.push_back(img->name);
    }
    return rst;
}

void ExToolKitImpl::SetCurentImageIndex(int _index)
{
    m_curentImageIndex = _index;
    m_curentImage = ::NpkGetImage(m_npkObject, _index);
    m_curentFrameIndex = 0;
    m_curentPaletteIndex = 0;
}

int extoolkit::ExToolKitImpl::GetDDSFrameCount()
{
    return ::ImageGetDDSFrameCount(m_curentImage);
}

std::string extoolkit::ExToolKitImpl::GetDDSFormat()
{
    auto ddsFrame = this->GetDDSFrame();
    if (!ddsFrame)return "";
    switch (ddsFrame->ddsFormat) {
    case DDS_DXT1:
        return "DDS_DXT1";
    case DDS_DXT3:
        return "DDS_DXT3";
    case DDS_DXT5:
        return "DDS_DXT5";
    default:
        break;
    }
    return std::string();
}

std::pair<int, int> extoolkit::ExToolKitImpl::GetDDSFrameSize()
{
    auto ddsFrame = this->GetDDSFrame();
    if (!ddsFrame) {
        return { 0,0 };
        return std::make_pair(0, 0);
    }
    return { ddsFrame->width, ddsFrame->height };
}

int extoolkit::ExToolKitImpl::GetPaletteCount()
{
    return ::PMGetPaletteCount(m_curentImage->paletteManager);
}

std::vector<Color> extoolkit::ExToolKitImpl::GetPaletteColors()
{
    std::vector<Color> rst;
    auto paletteManager = m_curentImage->paletteManager;
    int colorCount = ::PMGetColorCount(paletteManager, m_curentPaletteIndex);
    for (int i = 0; i < colorCount; i++) {
        auto temp = ::PMGetColor(paletteManager, m_curentPaletteIndex, i);
        rst.emplace_back(temp);
    }
    return rst;
}

void extoolkit::ExToolKitImpl::SetCurentPalette(int _index)
{
    int paletteCount = GetPaletteCount();
    if (paletteCount <= 0)return;
    m_curentPaletteIndex = _index % paletteCount;
}

DDSFrame* extoolkit::ExToolKitImpl::GetDDSFrame()
{
    auto frame = this->GetCurentFrameHandle();
    if (frame->IsDDSFormat()) {
        return ::ImageGetDDSFrame(m_curentImage, frame->DDSIndex());
    }
    return nullptr;
}
