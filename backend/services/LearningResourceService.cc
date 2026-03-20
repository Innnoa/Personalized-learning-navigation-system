#include "services/LearningResourceService.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
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

int buildPhasePriority(const std::string &recommendedPhase,
                       const algorithm::planner::LearningPathItem &item)
{
    const bool scheduledNow = item.status == "scheduled";

    if (recommendedPhase == "主学")
    {
        return scheduledNow ? 600 : 520;
    }

    if (recommendedPhase == "预习")
    {
        return scheduledNow ? 520 : 600;
    }

    if (recommendedPhase == "难点突破")
    {
        return scheduledNow ? 460 : 440;
    }

    if (recommendedPhase == "巩固")
    {
        return scheduledNow ? 380 : 360;
    }

    if (recommendedPhase == "答辩复盘")
    {
        return 300;
    }

    if (recommendedPhase == "延伸学习")
    {
        return 220;
    }

    return 180;
}

int buildTypePriority(const std::string &type)
{
    if (type == "video")
    {
        return 30;
    }

    if (type == "article")
    {
        return 20;
    }

    if (type == "document")
    {
        return 10;
    }

    return 0;
}

int buildFocusPriority(const Json::Value &resourcePayload,
                       const algorithm::planner::LearningPathItem &item)
{
    const auto focusNodeCode = resourcePayload["focusNodeCode"].asString();
    if (focusNodeCode.empty())
    {
        return 20;
    }

    if (focusNodeCode == item.point.code)
    {
        return 30;
    }

    return 10;
}

int buildResourcePriorityScore(
    const algorithm::planner::LearningPathItem &item,
    const Json::Value &resourcePayload)
{
    return buildPhasePriority(resourcePayload["recommendedPhase"].asString(), item) +
           buildTypePriority(resourcePayload["type"].asString()) +
           buildFocusPriority(resourcePayload, item);
}

std::string buildPriorityLabel(bool isPrimaryRecommendation,
                               const Json::Value &resourcePayload)
{
    if (isPrimaryRecommendation)
    {
        return "优先看";
    }

    const auto recommendedPhase = resourcePayload["recommendedPhase"].asString();
    if (recommendedPhase == "巩固")
    {
        return "巩固看";
    }

    if (recommendedPhase == "答辩复盘")
    {
        return "答辩看";
    }

    if (recommendedPhase == "延伸学习")
    {
        return "延伸看";
    }

    return "随后看";
}

std::string buildResourceLayer(const Json::Value &resourcePayload,
                               bool isPrimaryRecommendation)
{
    const auto recommendedPhase = resourcePayload["recommendedPhase"].asString();
    const auto type = resourcePayload["type"].asString();

    if (recommendedPhase == "答辩复盘")
    {
        return "答辩复盘";
    }

    if (isPrimaryRecommendation || type == "video" ||
        recommendedPhase == "主学" || recommendedPhase == "预习" ||
        recommendedPhase == "难点突破")
    {
        return "课程风格优先";
    }

    return "图文补充";
}

std::string buildResourceLayerHint(const std::string &resourceLayer)
{
    if (resourceLayer == "课程风格优先")
    {
        return "适合作为当前节点的主线学习入口，优先建立整体理解。";
    }

    if (resourceLayer == "图文补充")
    {
        return "适合在主线学习后补概念、查步骤或整理笔记。";
    }

    if (resourceLayer == "答辩复盘")
    {
        return "适合在实现完成后回顾术语、定义和讲解顺序。";
    }

    return "适合作为当前节点的补充学习材料。";
}

Json::Value buildResourceLayerSummary(const Json::Value &resources)
{
    Json::Value summary(Json::objectValue);
    summary["课程风格优先"] = 0;
    summary["图文补充"] = 0;
    summary["答辩复盘"] = 0;

    if (!resources.isArray())
    {
        return summary;
    }

    for (const auto &resource : resources)
    {
        const auto layer = resource["resourceLayer"].asString();
        if (layer.empty())
        {
            continue;
        }

        summary[layer] = summary[layer].asInt() + 1;
    }

    return summary;
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

Json::Value findRawResourcesForKnowledgePoint(
    const std::string &knowledgePointCode)
{
    const auto &catalog = getLearningResourceCatalog();
    const auto directIt = catalog.find(knowledgePointCode);
    if (directIt != catalog.end())
    {
        return directIt->second;
    }

    Json::Value matchedResources(Json::arrayValue);
    for (const auto &[_, resources] : catalog)
    {
        if (!resources.isArray())
        {
            continue;
        }

        for (const auto &resourcePayload : resources)
        {
            if (resourcePayload["focusNodeCode"].asString() == knowledgePointCode)
            {
                matchedResources.append(resourcePayload);
            }
        }
    }

    return matchedResources;
}
}

namespace services
{
Json::Value LearningResourceService::buildResourcesForKnowledgePoint(
    const std::string &knowledgePointCode)
{
    return findRawResourcesForKnowledgePoint(knowledgePointCode);
}

Json::Value LearningResourceService::buildResourcesForLearningPathItem(
    const algorithm::planner::LearningPathItem &item)
{
    const auto rawResources =
        findRawResourcesForKnowledgePoint(item.point.code);
    if (!rawResources.isArray() || rawResources.empty())
    {
        return Json::Value(Json::arrayValue);
    }

    std::vector<Json::Value> resourceItems;
    for (const auto &resourcePayload : rawResources)
    {
        auto enrichedResource = resourcePayload;
        enrichedResource["selectionReason"] =
            buildSelectionReason(item, enrichedResource);
        enrichedResource["recommendationScore"] =
            buildResourcePriorityScore(item, enrichedResource);
        resourceItems.push_back(std::move(enrichedResource));
    }

    std::stable_sort(resourceItems.begin(),
                     resourceItems.end(),
                     [](const Json::Value &left, const Json::Value &right) {
                         const auto leftScore =
                             left["recommendationScore"].asInt();
                         const auto rightScore =
                             right["recommendationScore"].asInt();
                         if (leftScore != rightScore)
                         {
                             return leftScore > rightScore;
                         }

                         return left["title"].asString() <
                                right["title"].asString();
                     });

    Json::Value resources(Json::arrayValue);
    for (std::size_t index = 0; index < resourceItems.size(); ++index)
    {
        auto resourcePayload = resourceItems[index];
        const bool isPrimaryRecommendation = index == 0;
        const auto resourceLayer =
            buildResourceLayer(resourcePayload, isPrimaryRecommendation);
        resourcePayload["recommendationRank"] =
            static_cast<int>(index + 1);
        resourcePayload["isPrimaryRecommendation"] =
            isPrimaryRecommendation;
        resourcePayload["priorityLabel"] =
            buildPriorityLabel(isPrimaryRecommendation, resourcePayload);
        resourcePayload["resourceLayer"] = resourceLayer;
        resourcePayload["resourceLayerHint"] =
            buildResourceLayerHint(resourceLayer);
        resources.append(resourcePayload);
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
        recommendation["primaryResourceTitle"] =
            resources[0]["title"].asString();
        recommendation["primaryResourcePriorityLabel"] =
            resources[0]["priorityLabel"].asString();
        recommendation["resourceLayerSummary"] =
            buildResourceLayerSummary(resources);
        recommendation["resources"] = resources;
        recommendations.append(recommendation);
    }

    return recommendations;
}
}
