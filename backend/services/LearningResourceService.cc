#include "services/LearningResourceService.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
int toPercent(double score)
{
    return static_cast<int>(std::round(std::clamp(score, 0.0, 1.0) * 100.0));
}

std::string readLearningResourceFilePath()
{
    const auto &config = drogon::app().getCustomConfig();
    if (config.isMember("learning_resource_file") &&
        config["learning_resource_file"].isString())
    {
        const auto filePath = config["learning_resource_file"].asString();
        if (!filePath.empty())
        {
            return filePath;
        }
    }

    return "../config/learning_resources.json";
}

Json::Value buildStringArrayPayload(const Json::Value &jsonArray)
{
    Json::Value payload(Json::arrayValue);
    if (!jsonArray.isArray())
    {
        return payload;
    }

    for (const auto &item : jsonArray)
    {
        if (item.isString())
        {
            payload.append(item.asString());
        }
    }

    return payload;
}

Json::Value buildSingleResourcePayload(const Json::Value &resourceJson)
{
    Json::Value payload;
    payload["title"] = resourceJson["title"].asString();
    payload["type"] = resourceJson["type"].asString();
    payload["source"] = resourceJson["source"].asString();
    payload["url"] = resourceJson["url"].asString();
    payload["description"] = resourceJson["description"].asString();
    payload["recommendedUsage"] = resourceJson["recommendedUsage"].asString();
    payload["recommendedPhase"] = resourceJson["recommendedPhase"].asString();
    payload["focusTags"] = buildStringArrayPayload(resourceJson["focusTags"]);
    payload["focusNodeCode"] = resourceJson["focusNodeCode"].asString();
    payload["focusNodeLabel"] = resourceJson["focusNodeLabel"].asString();
    payload["selectionReason"] = "";
    return payload;
}

std::string buildRolePhrase(
    const algorithm::planner::LearningPathItem &item)
{
    if (item.isTarget)
    {
        return "当前学习目标";
    }

    if (item.isPrerequisite)
    {
        return "目标前置知识点";
    }

    return "当前学习链路中的相关节点";
}

std::string buildStatusPhrase(
    const algorithm::planner::LearningPathItem &item)
{
    if (item.status == "mastered")
    {
        return "当前已作为已掌握节点保留";
    }

    if (item.status == "scheduled")
    {
        return "已纳入本轮学习安排";
    }

    if (item.status == "deferred")
    {
        return "受当前时间预算限制，建议放入下一轮学习";
    }

    return "已纳入当前学习路径";
}

std::string buildPhaseGuidance(
    const std::string &recommendedPhase,
    const algorithm::planner::LearningPathItem &item)
{
    if (recommendedPhase.empty())
    {
        return "";
    }

    if (recommendedPhase == "预习")
    {
        return item.status == "deferred"
                   ? "这份资源更适合在进入下一轮前先做概念预热。"
                   : "这份资源更适合在正式展开本轮学习前先做概念预热。";
    }

    if (recommendedPhase == "主学")
    {
        return item.status == "deferred"
                   ? "进入下一轮后，可把它作为该节点的主学材料。"
                   : "可直接把它作为当前节点的主学材料。";
    }

    if (recommendedPhase == "巩固")
    {
        return "适合在完成核心学习后对照概念、步骤和实现细节。";
    }

    if (recommendedPhase == "难点突破")
    {
        return "适合在理解卡点时集中突破关键定义、推导或算法过程。";
    }

    if (recommendedPhase == "答辩复盘")
    {
        return "适合在实现完成后整理术语、定义与答辩讲解顺序。";
    }

    if (recommendedPhase == "延伸学习")
    {
        return "适合在主链路掌握后继续补充背景与应用场景。";
    }

    return "适合作为当前节点的配套学习材料。";
}

std::string buildSelectionReason(
    const algorithm::planner::LearningPathItem &item,
    const Json::Value &resourcePayload)
{
    std::vector<std::string> parts;

    {
        std::ostringstream summary;
        summary << item.point.name << "属于" << buildRolePhrase(item)
                << "，当前掌握度约为 " << toPercent(item.masteryScore)
                << "%，" << buildStatusPhrase(item) << "。";
        parts.push_back(summary.str());
    }

    if (!item.reasonTrace.triggerReasons.empty())
    {
        std::ostringstream evidence;
        evidence << "规划依据显示："
                 << item.reasonTrace.triggerReasons.front();

        if (item.reasonTrace.triggerReasons.size() >= 2)
        {
            evidence << item.reasonTrace.triggerReasons[1];
        }

        parts.push_back(evidence.str());
    }

    {
        std::ostringstream metrics;
        metrics << "该节点与当前目标的相关度约为 "
                << toPercent(item.reasonTrace.relevanceScore) << "%";

        if (item.status != "mastered")
        {
            metrics << "，仍有 "
                    << toPercent(item.reasonTrace.masteryGap)
                    << "% 的补强空间";
        }

        if (item.reasonTrace.timeCostPenalty > 0.0 &&
            item.status != "mastered")
        {
            metrics << "，预计学习时间约占当前预算的 "
                    << toPercent(item.reasonTrace.timeCostPenalty) << "%";
        }

        metrics << "。";
        parts.push_back(metrics.str());
    }

    const auto recommendedPhase =
        resourcePayload["recommendedPhase"].asString();
    if (!recommendedPhase.empty())
    {
        parts.push_back("资源阶段定位为“" + recommendedPhase + "”。" +
                        buildPhaseGuidance(recommendedPhase, item));
    }

    const auto focusNodeLabel = resourcePayload["focusNodeLabel"].asString();
    if (!focusNodeLabel.empty())
    {
        parts.push_back("若继续查看细化图谱，可优先关注“" + focusNodeLabel +
                        "”这个子节点。");
    }

    std::ostringstream output;
    bool firstPart = true;
    for (std::size_t index = 0; index < parts.size(); ++index)
    {
        if (parts[index].empty())
        {
            continue;
        }

        if (!firstPart)
        {
            output << ' ';
        }

        output << parts[index];
        firstPart = false;
    }

    return output.str();
}

std::unordered_map<std::string, Json::Value> loadLearningResourceCatalog()
{
    const auto filePath = readLearningResourceFilePath();
    std::ifstream input(filePath);
    if (!input.is_open())
    {
        LOG_WARN << "未找到学习资源目录配置文件: " << filePath;
        return {};
    }

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    if (!Json::parseFromStream(builder, input, &root, &errors))
    {
        throw std::runtime_error("学习资源目录配置解析失败: " + errors);
    }

    if (!root.isMember("knowledgePointResources") ||
        !root["knowledgePointResources"].isArray())
    {
        throw std::runtime_error(
            "学习资源目录配置格式错误：knowledgePointResources 必须为数组。");
    }

    std::unordered_map<std::string, Json::Value> catalog;
    for (const auto &itemJson : root["knowledgePointResources"])
    {
        const auto knowledgePointCode =
            itemJson["knowledgePointCode"].asString();
        if (knowledgePointCode.empty())
        {
            throw std::runtime_error("学习资源目录存在空的 knowledgePointCode。");
        }

        if (!itemJson.isMember("resources") || !itemJson["resources"].isArray())
        {
            throw std::runtime_error("学习资源目录缺少 resources 数组: " +
                                     knowledgePointCode);
        }

        Json::Value resources(Json::arrayValue);
        for (const auto &resourceJson : itemJson["resources"])
        {
            resources.append(buildSingleResourcePayload(resourceJson));
        }

        catalog[knowledgePointCode] = resources;
    }

    return catalog;
}

const std::unordered_map<std::string, Json::Value> &getLearningResourceCatalog()
{
    static const auto catalog = loadLearningResourceCatalog();
    return catalog;
}
}

namespace services
{
Json::Value LearningResourceService::buildResourcesForKnowledgePoint(
    const std::string &knowledgePointCode)
{
    const auto &catalog = getLearningResourceCatalog();
    const auto it = catalog.find(knowledgePointCode);
    if (it == catalog.end())
    {
        return Json::Value(Json::arrayValue);
    }

    return it->second;
}

Json::Value LearningResourceService::buildResourcesForLearningPathItem(
    const algorithm::planner::LearningPathItem &item)
{
    const auto &catalog = getLearningResourceCatalog();
    const auto it = catalog.find(item.point.code);
    if (it == catalog.end())
    {
        return Json::Value(Json::arrayValue);
    }

    Json::Value resources(Json::arrayValue);
    for (const auto &resourcePayload : it->second)
    {
        auto enrichedResource = resourcePayload;
        enrichedResource["selectionReason"] =
            buildSelectionReason(item, enrichedResource);
        resources.append(enrichedResource);
    }

    return resources;
}

Json::Value LearningResourceService::buildResourceRecommendations(
    const std::vector<algorithm::planner::LearningPathItem> &pathItems)
{
    Json::Value recommendations(Json::arrayValue);

    for (const auto &item : pathItems)
    {
        if (item.status != "scheduled" && item.status != "deferred")
        {
            continue;
        }

        const auto resources = buildResourcesForLearningPathItem(item);
        if (!resources.isArray() || resources.empty())
        {
            continue;
        }

        Json::Value recommendation;
        recommendation["code"] = item.point.code;
        recommendation["name"] = item.point.name;
        recommendation["status"] = item.status;
        recommendation["scheduledNow"] = item.scheduledNow;
        recommendation["resourceCount"] = static_cast<int>(resources.size());
        recommendation["recommendedUsage"] =
            item.status == "scheduled"
                ? "建议与当前轮次学习同步使用，优先先看视频建立概念，再配合文本巩固。"
                : "建议在进入下一轮前提前预习，先熟悉概念与依赖关系。";
        recommendation["resources"] = resources;
        recommendations.append(recommendation);
    }

    return recommendations;
}
}
