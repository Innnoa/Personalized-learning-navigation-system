#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace services
{
struct DetailScopeNode
{
    std::string code;
    std::string label;
    int chapterNo = 0;
    std::string chapterName;
    std::string nodeType;
    int difficultyLevel = 1;
    int estimatedMinutes = 20;
    int displayOrder = 0;
    bool isCore = true;
    std::string description;
    bool hasDetailGraph = false;
    std::string detailScopeCode;
};

struct DetailScopeEdge
{
    std::string id;
    std::string source;
    std::string target;
    std::string relationType;
    std::string description;
};

struct DetailScope
{
    std::string scopeCode;
    std::string scopeName;
    std::string parentScopeCode;
    std::string parentNodeCode;
    std::string description;
    std::string sourceReference;
    std::vector<DetailScopeNode> nodes;
    std::vector<DetailScopeEdge> edges;
};

struct DetailScopeCatalog
{
    std::unordered_map<std::string, DetailScope> scopesByCode;
};

class DetailScopeCatalogService
{
  public:
    static const DetailScopeCatalog &getCatalog();
};
}
