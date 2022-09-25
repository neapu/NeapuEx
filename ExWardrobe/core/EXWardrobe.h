#pragma once
#include <string>
#include <vector>
#include <memory>

// first:英文名, secend:中文名，中文名用于显示，英文名用于索引
using CharactorName = std::pair<std::string, std::string>;
class EXWardrobe
{
public:
    virtual int Init() = 0;
    virtual std::vector<CharactorName> GetCharactorList() = 0;
};

std::shared_ptr<EXWardrobe> CreateEXWardrobe();
