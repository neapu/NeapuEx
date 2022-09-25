#pragma once
#include "EXWardrobe.h"
#include <map>
#include "NEString.h"
#include "Charactor.h"

class EXWardrobeImpl : public EXWardrobe {
public:
    virtual int Init() override;
    virtual std::vector<CharactorName> GetCharactorList() override;
private:
    std::map<neapu::String, Charactor> m_charactorList;

};