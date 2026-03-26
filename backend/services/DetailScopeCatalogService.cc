#include "services/DetailScopeCatalogService.h"

#include <drogon/drogon.h>

#include <fstream>
#include <mutex>
#include <stdexcept>

namespace
{
std::string readDetailCatalogPath()
{
    const auto &config = drogon::app().getCustomConfig();
    if (config.isMember("knowledge_graph_detail_file") &&
        config["knowledge_graph_detail_file"].isString())
    {
        const auto detailPath = config["knowledge_graph_detail_file"].asString();
        if (!detailPath.empty())
        {
            return detailPath;
        }
    }

    return "../config/knowledge_graph_details.json";
}

bool readBoolOrDefault(const Json::Value &json,
                       const std::string &fieldName,
                       bool defaultValue)
{
    if (!json.isMember(fieldName))
    {
        return defaultValue;
    }

    return json[fieldName].asBool();
}

services::DetailScopeCatalog loadCatalog()
{
    const auto detailCatalogPath = readDetailCatalogPath();
    std::ifstream input(detailCatalogPath);
    if (!input.is_open())
    {
        LOG_WARN << "未找到知识图谱细化配置文件: " << detailCatalogPath;
        return {};
    }

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    if (!Json::parseFromStream(builder, input, &root, &errors))
    {
        throw std::runtime_error("知识图谱细化配置解析失败: " + errors);
    }

    if (!root.isMember("detailScopes") || !root["detailScopes"].isArray())
    {
        throw std::runtime_error(
            "知识图谱细化配置格式错误：detailScopes 必须为数组。");
    }

    services::DetailScopeCatalog catalog;

    for (const auto &scopeJson : root["detailScopes"])
    {
        services::DetailScope scope;
        scope.scopeCode = scopeJson["scopeCode"].asString();
        scope.scopeName = scopeJson["scopeName"].asString();
        scope.parentScopeCode = scopeJson["parentScopeCode"].asString();
        scope.parentNodeCode = scopeJson["parentNodeCode"].asString();
        scope.description = scopeJson["description"].asString();
        scope.sourceReference = scopeJson["sourceReference"].asString();

        if (scope.scopeCode.empty() || scope.scopeName.empty())
        {
            throw std::runtime_error("知识图谱细化配置存在空的 scopeCode 或 scopeName。");
        }

        if (!scopeJson.isMember("nodes") || !scopeJson["nodes"].isArray())
        {
            throw std::runtime_error("细化图谱 scope 缺少 nodes 数组: " +
                                     scope.scopeCode);
        }

        if (!scopeJson.isMember("edges") || !scopeJson["edges"].isArray())
        {
            throw std::runtime_error("细化图谱 scope 缺少 edges 数组: " +
                                     scope.scopeCode);
        }

        scope.nodes.reserve(scopeJson["nodes"].size());
        for (const auto &nodeJson : scopeJson["nodes"])
        {
            services::DetailScopeNode node;
            node.code = nodeJson["code"].asString();
            node.label = nodeJson["label"].asString();
            node.chapterNo = nodeJson["chapterNo"].asInt();
            node.chapterName = nodeJson["chapterName"].asString();
            node.nodeType = nodeJson["nodeType"].asString();
            node.difficultyLevel = nodeJson["difficultyLevel"].asInt();
            node.estimatedMinutes = nodeJson["estimatedMinutes"].asInt();
            node.displayOrder = nodeJson["displayOrder"].asInt();
            node.isCore = readBoolOrDefault(nodeJson, "isCore", true);
            node.description = nodeJson["description"].asString();
            node.hasDetailGraph = readBoolOrDefault(nodeJson, "hasDetailGraph", false);
            node.detailScopeCode = nodeJson["detailScopeCode"].asString();

            if (!node.detailScopeCode.empty())
            {
                node.hasDetailGraph = true;
            }

            scope.nodes.push_back(node);
        }

        scope.edges.reserve(scopeJson["edges"].size());
        for (const auto &edgeJson : scopeJson["edges"])
        {
            scope.edges.push_back(services::DetailScopeEdge{
                edgeJson["id"].asString(),
                edgeJson["source"].asString(),
                edgeJson["target"].asString(),
                edgeJson["relationType"].asString(),
                edgeJson["description"].asString()});
        }

        catalog.scopesByCode[scope.scopeCode] = scope;
    }

    return catalog;
}
}

namespace services
{
const DetailScopeCatalog &DetailScopeCatalogService::getCatalog()
{
    static std::once_flag onceFlag;
    static DetailScopeCatalog catalog;

    std::call_once(onceFlag, []() { catalog = loadCatalog(); });

    return catalog;
}
}
