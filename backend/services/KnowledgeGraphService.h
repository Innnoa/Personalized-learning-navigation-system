#pragma once

#include "algorithm/graph/KnowledgeGraph.h"

#include <json/json.h>

namespace services
{
class KnowledgeGraphService
{
  public:
    static algorithm::graph::KnowledgeGraph loadKnowledgeGraph();
    static Json::Value buildGraphPayload(const std::string &scopeCode = "");
};
}
