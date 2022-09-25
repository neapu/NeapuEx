#pragma once
#include "ExToolKit.h"

typedef struct tagNpkObject NpkObject;
typedef struct tagImageObject ImageObject;
typedef struct tagDDSFrame DDSFrame;
namespace extoolkit {
class ExToolKitImpl : public ExToolKit {
public:
    virtual ~ExToolKitImpl();

    virtual int Open(std::string _fileName) override;
    virtual void Close() override;

    virtual int GetFrameCount() override;
    virtual int GetCurentFrame() override;
    virtual void SetCurentFrame(int _frameIndex) override;
    virtual std::shared_ptr<FrameHandle> GetCurentFrameHandle() override;
    virtual void NextFrame() override;
    virtual void PreviousFrame() override;

    virtual int GetImageVersion() override;
    virtual std::string GetImagePath() override;
    virtual std::vector<std::string> GetImageNameList() override;
    virtual void SetCurentImageIndex(int _index) override;
    
    virtual int GetDDSFrameCount() override;
    virtual std::string GetDDSFormat() override;
    virtual std::pair<int, int> GetDDSFrameSize() override;

    virtual int GetPaletteCount() override;
    virtual std::vector<Color> GetPaletteColors() override;
    virtual void SetCurentPalette(int _index) override;

private:
    DDSFrame* GetDDSFrame();

private:
    NpkObject *m_npkObject = nullptr;
    ImageObject* m_curentImage = nullptr;
    int m_curentImageIndex = 0;
    int m_curentFrameIndex = 0;
    int m_curentPaletteIndex = 0;
};
} // namespace extoolkit