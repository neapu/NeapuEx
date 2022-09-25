#include "Charactor.h"

Charactor::Charactor(Json::Value _configuration)
{
    m_indexName = _configuration.get("name_en", "").asString();
    m_showName = _configuration.get("name", "").asString();
}

Charactor::Charactor(const Charactor& _charactor)
{
    m_indexName = _charactor.m_indexName;
    m_showName = _charactor.m_showName;
}

Charactor::Charactor(Charactor&& _charactor) noexcept
{
    m_indexName = std::move(_charactor.m_indexName);
    m_showName = std::move(_charactor.m_showName);
}
