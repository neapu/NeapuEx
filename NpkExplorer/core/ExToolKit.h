#pragma once
#include <memory>
#include <string>
#include <vector>
#include "FrameHandle.h"

namespace extoolkit {
class ExToolKit {
public:
    virtual ~ExToolKit()
    {}

    virtual int Open(std::string _fileName) = 0;
    virtual void Close() = 0;
    
    virtual int GetFrameCount() = 0;
    virtual int GetCurentFrame() = 0;
    virtual void SetCurentFrame(int _frameIndex) = 0;
    virtual std::shared_ptr<FrameHandle> GetCurentFrameHandle() = 0;
    virtual void NextFrame() = 0;
    virtual void PreviousFrame() = 0;

    virtual int GetImageVersion() = 0;
    virtual std::string GetImagePath() = 0;
    virtual std::vector<std::string> GetImageNameList() = 0;
    virtual void SetCurentImageIndex(int _index) = 0;

    virtual int GetDDSFrameCount() = 0;
    virtual std::string GetDDSFormat() = 0;
    virtual std::pair<int, int> GetDDSFrameSize() = 0;

    virtual int GetPaletteCount() = 0;
    virtual std::vector<Color> GetPaletteColors() = 0;
    virtual void SetCurentPalette(int _index) = 0;
};

std::shared_ptr<ExToolKit> CreateExToolKit();
} // namespace extoolkit
