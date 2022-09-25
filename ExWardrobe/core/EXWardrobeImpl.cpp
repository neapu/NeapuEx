#include "EXWardrobeImpl.h"
#include <json/json.h>
#include <NEFile.h>
#include <NELogger.h>

using namespace neapu;

std::shared_ptr<EXWardrobe> CreateEXWardrobe()
{
    return std::shared_ptr<EXWardrobe>(new EXWardrobeImpl());
}

int EXWardrobeImpl::Init()
{
    Logger::setLogLevel(LM_DEBUG, "./log");
    Json::Value root;
    Json::Reader reader;

    File file;
    if (!file.Open("charactor.json", File::OpenMode::ReadOnly)) {
        LOG_DEADLY << "charactor.json open failed!";
        return -1;
    }

    String charactorData = file.Read();
    file.Close();

    if (!reader.parse(charactorData.ToStdString(), root, false)) {
        LOG_DEADLY << "charactor.json parse failed!";
        return -2;
    }

    for (auto& jsonCharactor : root) {
        Charactor chr(jsonCharactor);
        m_charactorList.emplace(chr.GetIndexName(), chr);
    }

    return 0;
}

std::vector<CharactorName> EXWardrobeImpl::GetCharactorList()
{
    std::vector<CharactorName> rst;
    for (auto& ct : m_charactorList) {
        rst.push_back(std::make_pair(ct.first.ToStdString(), ct.second.GetShowName().ToStdString()));
    }
    return rst;
}
