#pragma once
#include <NEString.h>
#include <json/json.h>

class Charactor {
public:
    Charactor(Json::Value _configuration);
    Charactor(const Charactor& _charactor);
    Charactor(Charactor&& _charactor) noexcept;

    neapu::String GetIndexName()
    {
        return m_indexName;
    }

    neapu::String GetShowName()
    {
        return m_showName;
    }
private:
    neapu::String m_indexName;
    neapu::String m_showName;
};