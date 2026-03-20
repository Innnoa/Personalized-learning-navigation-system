#include "services/KnowledgeGraphService.h"

#include "repositories/KnowledgeGraphRepository.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <fstream>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace
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

std::string readSelectedCourseName()
{
    const auto &config = drogon::app().getCustomConfig();
    if (config.isMember("selected_course") && config["selected_course"].isString())
    {
        const auto courseName = config["selected_course"].asString();
        if (!courseName.empty())
        {
            return courseName;
        }
    }

    return "数据结构";
}

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

std::unordered_map<std::string, DetailScope> loadDetailScopes()
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

    std::unordered_map<std::string, DetailScope> scopes;

    for (const auto &scopeJson : root["detailScopes"])
    {
        DetailScope scope;
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
            DetailScopeNode node;
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
            scope.edges.push_back(DetailScopeEdge{
                edgeJson["id"].asString(),
                edgeJson["source"].asString(),
                edgeJson["target"].asString(),
                edgeJson["relationType"].asString(),
                edgeJson["description"].asString()});
        }

        scopes[scope.scopeCode] = scope;
    }

    return scopes;
}

Json::Value buildBreadcrumbs(
    const std::string &courseName,
    const std::string &scopeCode,
    const std::unordered_map<std::string, DetailScope> &detailScopes)
{
    Json::Value breadcrumbs(Json::arrayValue);
    std::vector<std::pair<std::string, std::string>> chain = {
        {"root", courseName + "课程主线"}};

    if (!scopeCode.empty() && scopeCode != "root")
    {
        std::vector<std::pair<std::string, std::string>> scopeChain;
        auto currentScopeCode = scopeCode;

        while (!currentScopeCode.empty() && currentScopeCode != "root")
        {
            const auto scopeIt = detailScopes.find(currentScopeCode);
            if (scopeIt == detailScopes.end())
            {
                throw std::invalid_argument("未找到指定的细化图谱范围: " +
                                            currentScopeCode);
            }

            scopeChain.push_back(
                {scopeIt->second.scopeCode, scopeIt->second.scopeName});
            currentScopeCode = scopeIt->second.parentScopeCode;
        }

        std::reverse(scopeChain.begin(), scopeChain.end());
        chain.insert(chain.end(), scopeChain.begin(), scopeChain.end());
    }

    for (const auto &[code, label] : chain)
    {
        Json::Value item;
        item["scopeCode"] = code;
        item["label"] = label;
        breadcrumbs.append(item);
    }

    return breadcrumbs;
}

Json::Value buildRootPayload(
    const algorithm::graph::KnowledgeGraph &graph,
    const std::unordered_map<std::string, DetailScope> &detailScopes)
{
    Json::Value nodes(Json::arrayValue);
    Json::Value edges(Json::arrayValue);
    std::set<int> chapterNumbers;
    int coreKnowledgePointCount = 0;
    int detailScopeCount = 0;

    for (const auto &point : graph.points)
    {
        const bool hasDetailGraph = detailScopes.count(point.code) > 0;
        if (hasDetailGraph)
        {
            ++detailScopeCount;
        }

        Json::Value node;
        node["id"] = point.code;
        node["dbId"] = point.dbId;
        node["code"] = point.code;
        node["label"] = point.name;
        node["chapterNo"] = point.chapterNo;
        node["chapterName"] = point.chapterName;
        node["nodeType"] = point.nodeType;
        node["difficultyLevel"] = point.difficultyLevel;
        node["estimatedMinutes"] = point.estimatedMinutes;
        node["displayOrder"] = point.displayOrder;
        node["isCore"] = point.isCore;
        node["description"] = point.description;
        node["hasDetailGraph"] = hasDetailGraph;
        node["detailScopeCode"] = hasDetailGraph ? point.code : "";

        nodes.append(node);
        chapterNumbers.insert(point.chapterNo);

        if (point.isCore)
        {
            ++coreKnowledgePointCount;
        }
    }

    for (const auto &dependency : graph.dependencies)
    {
        Json::Value edge;
        edge["id"] = "dep-" + std::to_string(dependency.dbId);
        edge["source"] = dependency.sourceCode;
        edge["target"] = dependency.targetCode;
        edge["relationType"] = dependency.relationType;
        edge["description"] = dependency.description;
        edges.append(edge);
    }

    Json::Value payload;
    payload["status"] = "ok";
    payload["message"] = "知识图谱主视图已从 SQLite 演示数据加载。";
    payload["course"]["id"] = graph.courseId;
    payload["course"]["code"] = graph.courseCode;
    payload["course"]["name"] = graph.courseName;
    payload["course"]["description"] = graph.courseDescription;
    payload["course"]["targetAudience"] = graph.targetAudience;
    payload["view"]["scopeCode"] = "root";
    payload["view"]["scopeName"] = graph.courseName + "课程主线";
    payload["view"]["parentScopeCode"] = "";
    payload["view"]["parentNodeCode"] = "";
    payload["view"]["isRoot"] = true;
    payload["view"]["description"] =
        "当前显示课程级知识图谱。双击带“可细化”标记的节点可进入更细粒度图谱。";
    payload["view"]["sourceReference"] = "";
    payload["breadcrumbs"] =
        buildBreadcrumbs(graph.courseName, "root", detailScopes);
    payload["nodes"] = nodes;
    payload["edges"] = edges;
    payload["meta"]["knowledgePointCount"] =
        static_cast<int>(graph.points.size());
    payload["meta"]["dependencyCount"] =
        static_cast<int>(graph.dependencies.size());
    payload["meta"]["chapterCount"] = static_cast<int>(chapterNumbers.size());
    payload["meta"]["coreKnowledgePointCount"] = coreKnowledgePointCount;
    payload["meta"]["detailScopeCount"] = detailScopeCount;

    return payload;
}

Json::Value buildDetailScopePayload(
    const algorithm::graph::KnowledgeGraph &graph,
    const DetailScope &scope,
    const std::unordered_map<std::string, DetailScope> &detailScopes)
{
    Json::Value nodes(Json::arrayValue);
    Json::Value edges(Json::arrayValue);
    std::set<int> chapterNumbers;
    int coreKnowledgePointCount = 0;
    int detailScopeCount = 0;

    for (const auto &nodeItem : scope.nodes)
    {
        if (nodeItem.hasDetailGraph)
        {
            ++detailScopeCount;
        }

        Json::Value node;
        node["id"] = nodeItem.code;
        node["dbId"] = 0;
        node["code"] = nodeItem.code;
        node["label"] = nodeItem.label;
        node["chapterNo"] = nodeItem.chapterNo;
        node["chapterName"] = nodeItem.chapterName;
        node["nodeType"] = nodeItem.nodeType;
        node["difficultyLevel"] = nodeItem.difficultyLevel;
        node["estimatedMinutes"] = nodeItem.estimatedMinutes;
        node["displayOrder"] = nodeItem.displayOrder;
        node["isCore"] = nodeItem.isCore;
        node["description"] = nodeItem.description;
        node["hasDetailGraph"] = nodeItem.hasDetailGraph;
        node["detailScopeCode"] =
            nodeItem.hasDetailGraph ? nodeItem.detailScopeCode : "";

        nodes.append(node);
        chapterNumbers.insert(nodeItem.chapterNo);

        if (nodeItem.isCore)
        {
            ++coreKnowledgePointCount;
        }
    }

    for (const auto &edgeItem : scope.edges)
    {
        Json::Value edge;
        edge["id"] = edgeItem.id;
        edge["source"] = edgeItem.source;
        edge["target"] = edgeItem.target;
        edge["relationType"] = edgeItem.relationType;
        edge["description"] = edgeItem.description;
        edges.append(edge);
    }

    Json::Value payload;
    payload["status"] = "ok";
    payload["message"] = "知识图谱细化视图已加载。";
    payload["course"]["id"] = graph.courseId;
    payload["course"]["code"] = graph.courseCode;
    payload["course"]["name"] = graph.courseName;
    payload["course"]["description"] = graph.courseDescription;
    payload["course"]["targetAudience"] = graph.targetAudience;
    payload["view"]["scopeCode"] = scope.scopeCode;
    payload["view"]["scopeName"] = scope.scopeName;
    payload["view"]["parentScopeCode"] = scope.parentScopeCode;
    payload["view"]["parentNodeCode"] = scope.parentNodeCode;
    payload["view"]["isRoot"] = false;
    payload["view"]["description"] = scope.description;
    payload["view"]["sourceReference"] = scope.sourceReference;
    payload["breadcrumbs"] =
        buildBreadcrumbs(graph.courseName, scope.scopeCode, detailScopes);
    payload["nodes"] = nodes;
    payload["edges"] = edges;
    payload["meta"]["knowledgePointCount"] =
        static_cast<int>(scope.nodes.size());
    payload["meta"]["dependencyCount"] =
        static_cast<int>(scope.edges.size());
    payload["meta"]["chapterCount"] = static_cast<int>(chapterNumbers.size());
    payload["meta"]["coreKnowledgePointCount"] = coreKnowledgePointCount;
    payload["meta"]["detailScopeCount"] = detailScopeCount;

    return payload;
}
}

namespace services
{
algorithm::graph::KnowledgeGraph KnowledgeGraphService::loadKnowledgeGraph()
{
    const auto courseName = readSelectedCourseName();
    const auto course =
        repositories::KnowledgeGraphRepository::findCourseByName(courseName);

    if (!course.has_value())
    {
        throw std::runtime_error("未找到当前课程的图谱数据，请先执行数据库初始化脚本。");
    }

    const auto pointRecords =
        repositories::KnowledgeGraphRepository::listKnowledgePointsByCourseId(
            course->id);
    const auto dependencyRecords =
        repositories::KnowledgeGraphRepository::listDependenciesByCourseId(
            course->id);

    algorithm::graph::KnowledgeGraph graph;
    graph.courseId = course->id;
    graph.courseCode = course->code;
    graph.courseName = course->name;
    graph.courseDescription = course->description;
    graph.targetAudience = course->targetAudience;

    graph.points.reserve(pointRecords.size());
    for (const auto &point : pointRecords)
    {
        graph.points.push_back(algorithm::graph::KnowledgePoint{
            point.id,
            point.code,
            point.name,
            point.chapterNo,
            point.chapterName,
            point.nodeType,
            point.difficultyLevel,
            point.estimatedMinutes,
            point.displayOrder,
            point.isCore,
            point.description});
    }

    std::unordered_map<int, std::string> codeById;
    codeById.reserve(pointRecords.size());
    for (const auto &point : pointRecords)
    {
        codeById[point.id] = point.code;
    }

    graph.dependencies.reserve(dependencyRecords.size());
    for (const auto &dependency : dependencyRecords)
    {
        const auto prerequisiteIt = codeById.find(dependency.prerequisitePointId);
        const auto targetIt = codeById.find(dependency.targetPointId);

        if (prerequisiteIt == codeById.end() || targetIt == codeById.end())
        {
            throw std::runtime_error("知识图谱依赖边引用了不存在的知识点。");
        }

        graph.dependencies.push_back(algorithm::graph::KnowledgeDependency{
            dependency.id,
            prerequisiteIt->second,
            targetIt->second,
            dependency.relationType,
            dependency.description});
    }

    return graph;
}

Json::Value KnowledgeGraphService::buildGraphPayload(const std::string &scopeCode)
{
    const auto graph = loadKnowledgeGraph();
    const auto detailScopes = loadDetailScopes();
    const auto normalizedScopeCode =
        (scopeCode.empty() || scopeCode == "root") ? "root" : scopeCode;

    if (normalizedScopeCode == "root")
    {
        return buildRootPayload(graph, detailScopes);
    }

    const auto scopeIt = detailScopes.find(normalizedScopeCode);
    if (scopeIt == detailScopes.end())
    {
        throw std::invalid_argument("未找到指定的细化图谱范围。");
    }

    return buildDetailScopePayload(graph, scopeIt->second, detailScopes);
}
}
