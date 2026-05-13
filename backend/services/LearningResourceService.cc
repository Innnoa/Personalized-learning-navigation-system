#include "services/LearningResourceService.h"

#include "algorithm/explainer/LearningPathExplainer.h"
#include "repositories/LearnerProfileRepository.h"
#include "repositories/ResourceViewRepository.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
struct DetailScopeSummary
{
    std::string scopeCode;
    std::string scopeName;
    std::string parentScopeCode;
    std::string parentNodeCode;
};

struct DetailScopeCatalog
{
    std::unordered_map<std::string, DetailScopeSummary> scopesByCode;
    std::unordered_map<std::string, std::string> nodeScopeByCode;
    std::unordered_map<std::string, std::string> nodeLabelByCode;
    std::unordered_map<std::string, int> nodeDisplayOrderByCode;
};

struct RecommendationStage
{
    std::string code;
    std::string label;
    std::string guidance;
};

const std::unordered_map<std::string, Json::Value> &getLearningResourceCatalog();
const std::unordered_map<std::string, Json::Value> &
getCourseChapterLinkCatalog();

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

std::string readCourseChapterLinkPath()
{
    const auto &config = drogon::app().getCustomConfig();
    if (config.isMember("course_chapter_link_file") &&
        config["course_chapter_link_file"].isString())
    {
        const auto detailPath = config["course_chapter_link_file"].asString();
        if (!detailPath.empty())
        {
            return detailPath;
        }
    }

    return "../config/course_chapter_links.json";
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

void appendUnique(std::vector<std::string> &values, const std::string &value)
{
    if (value.empty())
    {
        return;
    }

    if (std::find(values.begin(), values.end(), value) == values.end())
    {
        values.push_back(value);
    }
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
    payload["lastInteractionType"] = "";
    payload["lastInteractionLabel"] = "";
    payload["behaviorAdjustmentHint"] = "";
    payload["resourceCoverageType"] = "configured";
    payload["inheritedFromCode"] = "";
    payload["inheritedFromLabel"] = "";
    return payload;
}

DetailScopeCatalog loadDetailScopeCatalog()
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

    DetailScopeCatalog catalog;
    for (const auto &scopeJson : root["detailScopes"])
    {
        DetailScopeSummary scope;
        scope.scopeCode = scopeJson["scopeCode"].asString();
        scope.scopeName = scopeJson["scopeName"].asString();
        scope.parentScopeCode = scopeJson["parentScopeCode"].asString();
        scope.parentNodeCode = scopeJson["parentNodeCode"].asString();

        if (scope.scopeCode.empty())
        {
            continue;
        }

        catalog.scopesByCode[scope.scopeCode] = scope;

        if (scope.parentScopeCode == "root" &&
            !scope.parentNodeCode.empty() &&
            catalog.nodeLabelByCode.count(scope.parentNodeCode) == 0)
        {
            catalog.nodeLabelByCode[scope.parentNodeCode] = scope.scopeName;
        }

        if (!scopeJson.isMember("nodes") || !scopeJson["nodes"].isArray())
        {
            continue;
        }

        for (const auto &nodeJson : scopeJson["nodes"])
        {
            const auto nodeCode = nodeJson["code"].asString();
            if (nodeCode.empty())
            {
                continue;
            }

            catalog.nodeScopeByCode[nodeCode] = scope.scopeCode;
            catalog.nodeLabelByCode[nodeCode] = nodeJson["label"].asString();
            catalog.nodeDisplayOrderByCode[nodeCode] =
                nodeJson["displayOrder"].asInt();
        }
    }

    return catalog;
}

const DetailScopeCatalog &getDetailScopeCatalog()
{
    static const auto catalog = loadDetailScopeCatalog();
    return catalog;
}

std::string resolveNodeLabel(const std::string &nodeCode)
{
    const auto &catalog = getDetailScopeCatalog();
    const auto labelIt = catalog.nodeLabelByCode.find(nodeCode);
    if (labelIt != catalog.nodeLabelByCode.end() && !labelIt->second.empty())
    {
        return labelIt->second;
    }

    return nodeCode;
}

Json::Value buildCourseChapterResourcePayload(const Json::Value &linkJson)
{
    const auto knowledgePointCode = linkJson["knowledgePointCode"].asString();
    const auto knowledgePointLabel =
        linkJson.isMember("knowledgePointLabel") &&
                linkJson["knowledgePointLabel"].isString() &&
                !linkJson["knowledgePointLabel"].asString().empty()
            ? linkJson["knowledgePointLabel"].asString()
            : resolveNodeLabel(knowledgePointCode);
    const auto chapterDisplayIndex =
        linkJson["chapterDisplayIndex"].asString();
    const auto chapterTitle = linkJson["chapterTitle"].asString();
    const auto linkMode = linkJson.isMember("linkMode") &&
                                  linkJson["linkMode"].isString()
                              ? linkJson["linkMode"].asString()
                              : "direct";
    const bool isCandidateLink = linkMode == "candidate";

    Json::Value payload;
    payload["title"] = chapterTitle;
    payload["type"] = "document";
    payload["source"] =
        isCandidateLink ? "超星课程原文章节候选" : "超星课程原文章节";
    payload["url"] = linkJson["url"].asString();

    std::string description =
        isCandidateLink
            ? "根据课程章节标题与当前知识点做高置信弱匹配得到，可作为课程内候选原文入口。"
            : "直接回到本课原文章节查看教师组织的课程内容。";
    if (!chapterDisplayIndex.empty() && !chapterTitle.empty())
    {
        description =
            isCandidateLink
                ? "候选课程章节“" + chapterDisplayIndex + " " + chapterTitle +
                      "”，可先核对它是否与当前知识点完全对应。"
                : "对应课程章节“" + chapterDisplayIndex + " " + chapterTitle +
                      "”，适合先对齐课堂语境。";
    }
    else if (!chapterTitle.empty())
    {
        description = isCandidateLink
                          ? "候选课程章节“" + chapterTitle +
                                "”，可先核对它是否与当前知识点完全对应。"
                          : "对应课程章节“" + chapterTitle +
                                "”，适合先对齐课堂语境。";
    }

    payload["description"] = description;
    payload["recommendedUsage"] = isCandidateLink
                                      ? "建议先打开这条课程候选原文快速核对课堂表达；若完全对应，再继续结合外部资源补充。"
                                      : "建议先打开课程原文章节，对齐本课教师给出的定义、案例和任务安排，再结合外部资源补充。";
    payload["recommendedPhase"] = "主学";
    payload["focusTags"] = Json::Value(Json::arrayValue);
    payload["focusTags"].append(isCandidateLink ? "课程候选" : "课程原文");
    if (!chapterDisplayIndex.empty())
    {
        payload["focusTags"].append(chapterDisplayIndex);
    }
    payload["focusNodeCode"] = knowledgePointCode;
    payload["focusNodeLabel"] = knowledgePointLabel;
    payload["selectionReason"] = "";
    payload["lastInteractionType"] = "";
    payload["lastInteractionLabel"] = "";
    payload["behaviorAdjustmentHint"] = "";
    payload["resourceCoverageType"] =
        isCandidateLink ? "course-candidate" : "course-direct";
    payload["inheritedFromCode"] = "";
    payload["inheritedFromLabel"] = "";
    payload["chapterId"] = linkJson["chapterId"].asString();
    payload["chapterDisplayIndex"] = chapterDisplayIndex;
    payload["mappingSource"] = linkJson["mappingSource"].asString();
    payload["linkMode"] = linkMode;
    return payload;
}

std::vector<std::pair<std::string, std::string>>
resolveFallbackAncestorNodes(const std::string &knowledgePointCode)
{
    const auto &catalog = getDetailScopeCatalog();
    const auto scopeCodeIt = catalog.nodeScopeByCode.find(knowledgePointCode);
    if (scopeCodeIt == catalog.nodeScopeByCode.end())
    {
        return {};
    }

    std::vector<std::pair<std::string, std::string>> ancestors;
    std::unordered_set<std::string> visitedCodes;
    auto currentScopeCode = scopeCodeIt->second;

    while (!currentScopeCode.empty() && currentScopeCode != "root")
    {
        const auto scopeIt = catalog.scopesByCode.find(currentScopeCode);
        if (scopeIt == catalog.scopesByCode.end())
        {
            break;
        }

        const auto &scope = scopeIt->second;
        if (!scope.parentNodeCode.empty() &&
            scope.parentNodeCode != knowledgePointCode &&
            visitedCodes.insert(scope.parentNodeCode).second)
        {
            ancestors.push_back(
                {scope.parentNodeCode, resolveNodeLabel(scope.parentNodeCode)});
        }

        currentScopeCode = scope.parentScopeCode;
    }

    return ancestors;
}

Json::Value cloneAndMarkResources(const Json::Value &resources,
                                  const std::string &resourceCoverageType,
                                  const std::string &inheritedFromCode = "",
                                  const std::string &inheritedFromLabel = "")
{
    Json::Value payload(Json::arrayValue);
    if (!resources.isArray())
    {
        return payload;
    }

    for (const auto &resource : resources)
    {
        auto enriched = resource;
        enriched["resourceCoverageType"] = resourceCoverageType;
        enriched["inheritedFromCode"] = inheritedFromCode;
        enriched["inheritedFromLabel"] = inheritedFromLabel;
        payload.append(enriched);
    }

    return payload;
}

Json::Value buildDirectCatalogResources(const std::string &knowledgePointCode)
{
    const auto &catalog = getLearningResourceCatalog();
    const auto directIt = catalog.find(knowledgePointCode);
    if (directIt == catalog.end())
    {
        return Json::Value(Json::arrayValue);
    }

    return cloneAndMarkResources(directIt->second, "configured");
}

Json::Value buildFocusedCatalogResources(const std::string &knowledgePointCode)
{
    Json::Value matchedResources(Json::arrayValue);
    std::unordered_set<std::string> seenUrls;
    const auto &catalog = getLearningResourceCatalog();

    for (const auto &[_, resources] : catalog)
    {
        if (!resources.isArray())
        {
            continue;
        }

        for (const auto &resourcePayload : resources)
        {
            if (resourcePayload["focusNodeCode"].asString() != knowledgePointCode)
            {
                continue;
            }

            const auto resourceUrl = resourcePayload["url"].asString();
            if (!seenUrls.insert(resourceUrl).second)
            {
                continue;
            }

            auto enriched = resourcePayload;
            enriched["resourceCoverageType"] = "focused";
            matchedResources.append(enriched);
        }
    }

    return matchedResources;
}

Json::Value buildInheritedCatalogResources(const std::string &knowledgePointCode)
{
    Json::Value inheritedResources(Json::arrayValue);
    std::unordered_set<std::string> seenUrls;

    for (const auto &[ancestorCode, ancestorLabel] :
         resolveFallbackAncestorNodes(knowledgePointCode))
    {
        const auto ancestorResources = buildDirectCatalogResources(ancestorCode);
        if (!ancestorResources.isArray() || ancestorResources.empty())
        {
            continue;
        }

        for (const auto &resourcePayload : ancestorResources)
        {
            const auto focusNodeCode = resourcePayload["focusNodeCode"].asString();
            if (!focusNodeCode.empty() && focusNodeCode != knowledgePointCode)
            {
                continue;
            }

            const auto resourceUrl = resourcePayload["url"].asString();
            if (!seenUrls.insert(resourceUrl).second)
            {
                continue;
            }

            auto enriched = resourcePayload;
            enriched["resourceCoverageType"] = "inherited";
            enriched["inheritedFromCode"] = ancestorCode;
            enriched["inheritedFromLabel"] = ancestorLabel;
            inheritedResources.append(enriched);
        }

        if (inheritedResources.size() >= 6)
        {
            break;
        }
    }

    return inheritedResources;
}

std::vector<std::pair<std::string, std::string>>
resolveRelatedScopeNodes(const std::string &knowledgePointCode)
{
    const auto &catalog = getDetailScopeCatalog();
    const auto scopeCodeIt = catalog.nodeScopeByCode.find(knowledgePointCode);
    if (scopeCodeIt == catalog.nodeScopeByCode.end())
    {
        return {};
    }

    const auto nodeOrderIt =
        catalog.nodeDisplayOrderByCode.find(knowledgePointCode);
    const int currentDisplayOrder =
        nodeOrderIt == catalog.nodeDisplayOrderByCode.end()
            ? 0
            : nodeOrderIt->second;
    const auto &scopeCode = scopeCodeIt->second;

    struct RelatedNode
    {
        std::string code;
        std::string label;
        int distance = 0;
        int displayOrder = 0;
    };

    std::vector<RelatedNode> relatedNodes;
    for (const auto &[nodeCode, nodeScopeCode] : catalog.nodeScopeByCode)
    {
        if (nodeCode == knowledgePointCode || nodeScopeCode != scopeCode)
        {
            continue;
        }

        const auto labelIt = catalog.nodeLabelByCode.find(nodeCode);
        const auto orderIt = catalog.nodeDisplayOrderByCode.find(nodeCode);
        const int nodeDisplayOrder =
            orderIt == catalog.nodeDisplayOrderByCode.end()
                ? 0
                : orderIt->second;
        relatedNodes.push_back(RelatedNode{
            nodeCode,
            labelIt == catalog.nodeLabelByCode.end() ? nodeCode : labelIt->second,
            std::abs(nodeDisplayOrder - currentDisplayOrder),
            nodeDisplayOrder,
        });
    }

    std::sort(relatedNodes.begin(),
              relatedNodes.end(),
              [](const RelatedNode &left, const RelatedNode &right) {
                  if (left.distance != right.distance)
                  {
                      return left.distance < right.distance;
                  }

                  if (left.displayOrder != right.displayOrder)
                  {
                      return left.displayOrder < right.displayOrder;
                  }

                  return left.code < right.code;
              });

    std::vector<std::pair<std::string, std::string>> payload;
    for (const auto &node : relatedNodes)
    {
        payload.push_back({node.code, node.label});
        if (payload.size() >= 4)
        {
            break;
        }
    }

    return payload;
}

Json::Value buildRelatedCatalogResources(const std::string &knowledgePointCode)
{
    Json::Value relatedResources(Json::arrayValue);
    std::unordered_set<std::string> seenUrls;
    const auto &catalog = getLearningResourceCatalog();

    for (const auto &[relatedCode, relatedLabel] :
         resolveRelatedScopeNodes(knowledgePointCode))
    {
        for (const auto &[_, resources] : catalog)
        {
            if (!resources.isArray())
            {
                continue;
            }

            for (const auto &resourcePayload : resources)
            {
                if (resourcePayload["focusNodeCode"].asString() != relatedCode)
                {
                    continue;
                }

                const auto resourceUrl = resourcePayload["url"].asString();
                if (!seenUrls.insert(resourceUrl).second)
                {
                    continue;
                }

                auto enriched = resourcePayload;
                enriched["resourceCoverageType"] = "related";
                enriched["inheritedFromCode"] = relatedCode;
                enriched["inheritedFromLabel"] = relatedLabel;
                relatedResources.append(enriched);

                if (relatedResources.size() >= 4)
                {
                    return relatedResources;
                }
            }
        }
    }

    return relatedResources;
}

Json::Value mergeResourceArrays(const Json::Value &primaryResources,
                                const Json::Value &secondaryResources)
{
    Json::Value merged(Json::arrayValue);
    std::unordered_set<std::string> seenUrls;

    const auto appendBatch = [&](const Json::Value &resources) {
        if (!resources.isArray())
        {
            return;
        }

        for (const auto &resourcePayload : resources)
        {
            const auto resourceUrl = resourcePayload["url"].asString();
            if (!seenUrls.insert(resourceUrl).second)
            {
                continue;
            }

            merged.append(resourcePayload);
        }
    };

    appendBatch(primaryResources);
    appendBatch(secondaryResources);
    return merged;
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

int buildCoveragePriority(const Json::Value &resourcePayload)
{
    const auto coverageType =
        resourcePayload["resourceCoverageType"].asString();

    if (coverageType == "course-direct")
    {
        return 180;
    }

    if (coverageType == "course-candidate")
    {
        return 96;
    }

    if (coverageType == "focused")
    {
        return 40;
    }

    if (coverageType == "configured")
    {
        return 32;
    }

    if (coverageType == "inherited")
    {
        return 18;
    }

    if (coverageType == "related")
    {
        return 12;
    }

    return 0;
}

int buildResourcePriorityScore(
    const algorithm::planner::LearningPathItem &item,
    const Json::Value &resourcePayload)
{
    return buildPhasePriority(resourcePayload["recommendedPhase"].asString(), item) +
           buildTypePriority(resourcePayload["type"].asString()) +
           buildFocusPriority(resourcePayload, item) +
           buildCoveragePriority(resourcePayload);
}

int buildRepeatedAttributePenalty(
    const std::unordered_map<std::string, int> &counts,
    const std::string &value,
    const int unitPenalty)
{
    if (value.empty())
    {
        return 0;
    }

    const auto countIt = counts.find(value);
    if (countIt == counts.end())
    {
        return 0;
    }

    return countIt->second * unitPenalty;
}

int buildDiversityAdjustment(
    const Json::Value &resourcePayload,
    const std::unordered_map<std::string, int> &sourceCounts,
    const std::unordered_map<std::string, int> &typeCounts,
    const std::unordered_map<std::string, int> &phaseCounts,
    const std::unordered_map<std::string, int> &focusNodeCounts,
    const std::unordered_map<std::string, int> &coverageCounts)
{
    int penalty = 0;
    penalty += buildRepeatedAttributePenalty(
        sourceCounts,
        resourcePayload["source"].asString(),
        28);
    penalty += buildRepeatedAttributePenalty(
        typeCounts,
        resourcePayload["type"].asString(),
        14);
    penalty += buildRepeatedAttributePenalty(
        phaseCounts,
        resourcePayload["recommendedPhase"].asString(),
        10);
    penalty += buildRepeatedAttributePenalty(
        focusNodeCounts,
        resourcePayload["focusNodeCode"].asString(),
        8);
    penalty += buildRepeatedAttributePenalty(
        coverageCounts,
        resourcePayload["resourceCoverageType"].asString(),
        6);
    return -penalty;
}

void markSelectedResourceAttributes(
    const Json::Value &resourcePayload,
    std::unordered_map<std::string, int> &sourceCounts,
    std::unordered_map<std::string, int> &typeCounts,
    std::unordered_map<std::string, int> &phaseCounts,
    std::unordered_map<std::string, int> &focusNodeCounts,
    std::unordered_map<std::string, int> &coverageCounts)
{
    const auto incrementIfNonEmpty =
        [](std::unordered_map<std::string, int> &counts,
           const std::string &value) {
            if (value.empty())
            {
                return;
            }

            counts[value] += 1;
        };

    incrementIfNonEmpty(sourceCounts, resourcePayload["source"].asString());
    incrementIfNonEmpty(typeCounts, resourcePayload["type"].asString());
    incrementIfNonEmpty(
        phaseCounts,
        resourcePayload["recommendedPhase"].asString());
    incrementIfNonEmpty(
        focusNodeCounts,
        resourcePayload["focusNodeCode"].asString());
    incrementIfNonEmpty(
        coverageCounts,
        resourcePayload["resourceCoverageType"].asString());
}

std::vector<Json::Value> buildDiversifiedResourceOrder(
    std::vector<Json::Value> resourceItems)
{
    std::vector<Json::Value> orderedResources;
    orderedResources.reserve(resourceItems.size());

    std::unordered_map<std::string, int> sourceCounts;
    std::unordered_map<std::string, int> typeCounts;
    std::unordered_map<std::string, int> phaseCounts;
    std::unordered_map<std::string, int> focusNodeCounts;
    std::unordered_map<std::string, int> coverageCounts;

    while (!resourceItems.empty())
    {
        auto bestIt = resourceItems.begin();
        int bestEffectiveScore = std::numeric_limits<int>::min();
        int bestBaseScore = std::numeric_limits<int>::min();
        int bestDiversityAdjustment = std::numeric_limits<int>::min();

        for (auto candidateIt = resourceItems.begin();
             candidateIt != resourceItems.end();
             ++candidateIt)
        {
            const auto baseScore =
                (*candidateIt)["baseRecommendationScore"].asInt();
            const auto diversityAdjustment =
                buildDiversityAdjustment(*candidateIt,
                                         sourceCounts,
                                         typeCounts,
                                         phaseCounts,
                                         focusNodeCounts,
                                         coverageCounts);
            const auto effectiveScore = baseScore + diversityAdjustment;

            const bool shouldReplace =
                effectiveScore > bestEffectiveScore ||
                (effectiveScore == bestEffectiveScore &&
                 baseScore > bestBaseScore) ||
                (effectiveScore == bestEffectiveScore &&
                 baseScore == bestBaseScore &&
                 diversityAdjustment > bestDiversityAdjustment) ||
                (effectiveScore == bestEffectiveScore &&
                 baseScore == bestBaseScore &&
                 diversityAdjustment == bestDiversityAdjustment &&
                 (*candidateIt)["title"].asString() <
                     (*bestIt)["title"].asString());

            if (shouldReplace)
            {
                bestIt = candidateIt;
                bestEffectiveScore = effectiveScore;
                bestBaseScore = baseScore;
                bestDiversityAdjustment = diversityAdjustment;
            }
        }

        auto selectedResource = *bestIt;
        selectedResource["diversityAdjustment"] = bestDiversityAdjustment;
        selectedResource["recommendationScore"] = bestEffectiveScore;
        orderedResources.push_back(selectedResource);
        markSelectedResourceAttributes(selectedResource,
                                       sourceCounts,
                                       typeCounts,
                                       phaseCounts,
                                       focusNodeCounts,
                                       coverageCounts);
        resourceItems.erase(bestIt);
    }

    return orderedResources;
}

std::vector<Json::Value> buildCappedResourceItems(
    const std::vector<Json::Value> &resourceItems)
{
    if (resourceItems.size() <= 4)
    {
        return resourceItems;
    }

    return std::vector<Json::Value>(resourceItems.begin(),
                                    resourceItems.begin() + 4);
}

std::string buildInteractionLabel(const std::string &interactionType)
{
    if (interactionType == "viewed")
    {
        return "已看过";
    }

    if (interactionType == "completed")
    {
        return "已学完";
    }

    if (interactionType == "save_for_later")
    {
        return "稍后再看";
    }

    if (interactionType == "opened")
    {
        return "已打开";
    }

    return "";
}

int buildInteractionPriorityAdjustment(
    const std::string &interactionType,
    const algorithm::planner::LearningPathItem &item)
{
    if (interactionType == "completed")
    {
        return -280;
    }

    if (interactionType == "viewed")
    {
        return -120;
    }

    if (interactionType == "opened")
    {
        return -40;
    }

    if (interactionType == "save_for_later")
    {
        return item.status == "deferred" ? 25 : -60;
    }

    return 0;
}

std::string buildBehaviorAdjustmentHint(
    const std::string &interactionType,
    const algorithm::planner::LearningPathItem &item)
{
    if (interactionType == "completed")
    {
        return "你已将这条资源记为学完，系统会自动后移，优先展示仍未完成的材料。";
    }

    if (interactionType == "viewed")
    {
        return "你之前已看过这条资源，系统会适度后移，把未接触的材料放在更前面。";
    }

    if (interactionType == "opened")
    {
        return "你之前已打开过这条资源，系统会略微后移，避免重复置顶。";
    }

    if (interactionType == "save_for_later")
    {
        return item.status == "deferred"
                   ? "你曾将这条资源标记为稍后再看，进入下一轮时系统会继续保留它。"
                   : "你曾将这条资源标记为稍后再看，当前轮次会优先展示更适合立即使用的材料。";
    }

    return "";
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

bool explanationHasLabel(
    const algorithm::explainer::PathExplanation &explanation,
    const std::string &label)
{
    return std::find(explanation.labels.begin(),
                     explanation.labels.end(),
                     label) != explanation.labels.end();
}

Json::Value buildLinkedReasonLabels(
    const algorithm::planner::LearningPathItem &item,
    const algorithm::explainer::PathExplanation &explanation,
    const Json::Value &resourcePayload)
{
    std::vector<std::string> orderedLabels;
    const auto recommendedPhase = resourcePayload["recommendedPhase"].asString();

    if (item.status == "scheduled")
    {
        appendUnique(orderedLabels, "本轮学习");
    }
    else if (item.status == "deferred")
    {
        appendUnique(orderedLabels, "下一轮建议");
    }

    if (recommendedPhase == "预习")
    {
        appendUnique(orderedLabels, "前置依赖");
        appendUnique(orderedLabels, "目标节点");
        appendUnique(orderedLabels, "高目标相关");
    }
    else if (recommendedPhase == "主学" || recommendedPhase == "难点突破")
    {
        appendUnique(orderedLabels, "掌握度待提升");
        appendUnique(orderedLabels, "高目标相关");
        appendUnique(orderedLabels, "目标节点");
    }
    else if (recommendedPhase == "巩固")
    {
        appendUnique(orderedLabels, "掌握度待提升");
        appendUnique(orderedLabels, "高目标相关");
    }
    else if (recommendedPhase == "答辩复盘")
    {
        appendUnique(orderedLabels, "目标节点");
        appendUnique(orderedLabels, "高目标相关");
    }

    if (item.isPrerequisite)
    {
        appendUnique(orderedLabels, "前置依赖");
    }

    if (item.isTarget)
    {
        appendUnique(orderedLabels, "目标节点");
    }

    if (item.reasonTrace.masteryGap >= 0.5 && item.status != "mastered")
    {
        appendUnique(orderedLabels, "掌握度待提升");
    }

    if (item.reasonTrace.relevanceScore >= 0.8)
    {
        appendUnique(orderedLabels, "高目标相关");
    }

    Json::Value labels(Json::arrayValue);
    for (const auto &label : orderedLabels)
    {
        if (explanationHasLabel(explanation, label))
        {
            labels.append(label);
        }

        if (labels.size() >= 3)
        {
            return labels;
        }
    }

    for (const auto &label : explanation.labels)
    {
        bool exists = false;
        for (const auto &existing : labels)
        {
            if (existing.asString() == label)
            {
                exists = true;
                break;
            }
        }

        if (!exists)
        {
            labels.append(label);
        }

        if (labels.size() >= 3)
        {
            break;
        }
    }

    return labels;
}

std::string buildLinkedReasonLabelSummary(const Json::Value &labels)
{
    if (!labels.isArray() || labels.empty())
    {
        return "";
    }

    std::ostringstream output;
    output << "对应推荐理由：";
    bool first = true;
    for (const auto &label : labels)
    {
        if (!label.isString() || label.asString().empty())
        {
            continue;
        }

        if (!first)
        {
            output << " / ";
        }

        output << label.asString();
        first = false;
    }

    return output.str();
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

RecommendationStage buildRecommendationStage(
    const algorithm::planner::LearningPathItem &item)
{
    if (item.masteryScore < 0.30)
    {
        return {"foundation",
                "入门讲解",
                "当前掌握度较低，建议先用入门讲解型资源建立基础概念与核心直觉。"};
    }

    if (item.masteryScore < 0.60)
    {
        return {"consolidation",
                "巩固理解",
                "当前掌握度正在提升，建议优先使用巩固理解型资源补齐关键定义与步骤。"};
    }

    if (item.masteryScore < 0.85)
    {
        return {"practice",
                "应用训练",
                "当前掌握度已具备基础，建议通过应用训练型资源加强迁移与实操。"};
    }

    return {"advanced",
            "提升拓展",
            "当前掌握度较高，建议优先查看提升拓展型资源补充进阶视角与延伸应用。"};
}

std::string resolveCourseChapterRecommendedPhase(
    const algorithm::planner::LearningPathItem &item)
{
    if (item.status == "mastered")
    {
        return "巩固";
    }

    return "预习";
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

    const auto resourceCoverageType =
        resourcePayload["resourceCoverageType"].asString();
    if (resourceCoverageType == "course-direct")
    {
        parts.push_back(
            "这条资源直接对应当前课程原文章节，能先帮助你回到教师课程语境确认定义、案例和任务安排。");
    }
    else if (resourceCoverageType == "course-candidate")
    {
        parts.push_back(
            "这条资源是根据课程章节标题做出的高置信弱匹配候选，适合先快速核对课堂原文是否与当前知识点完全对应。");
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

    const auto inheritedFromLabel =
        resourcePayload["inheritedFromLabel"].asString();
    if (!inheritedFromLabel.empty())
    {
        const auto coverageType =
            resourcePayload["resourceCoverageType"].asString();
        if (coverageType == "related")
        {
            parts.push_back("当前节点会补充同一细化范围内关联节点“" +
                            inheritedFromLabel +
                            "”的配套材料，帮助从相邻知识点切入理解。");
        }
        else
        {
            parts.push_back("当前节点暂未配置完全专属资源，先复用上层节点“" +
                            inheritedFromLabel + "”的配套材料。");
        }
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

std::unordered_map<std::string, Json::Value> loadCourseChapterLinkCatalog()
{
    const auto filePath = readCourseChapterLinkPath();
    std::ifstream input(filePath);
    if (!input.is_open())
    {
        LOG_WARN << "未找到课程直链配置文件: " << filePath;
        return {};
    }

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    if (!Json::parseFromStream(builder, input, &root, &errors))
    {
        throw std::runtime_error("课程直链配置解析失败: " + errors);
    }

    if (!root.isMember("courseChapterLinks") ||
        !root["courseChapterLinks"].isArray())
    {
        throw std::runtime_error(
            "课程直链配置格式错误：courseChapterLinks 必须为数组。");
    }

    std::unordered_map<std::string, Json::Value> catalog;
    for (const auto &itemJson : root["courseChapterLinks"])
    {
        const auto knowledgePointCode =
            itemJson["knowledgePointCode"].asString();
        if (knowledgePointCode.empty())
        {
            continue;
        }

        const auto url = itemJson["url"].asString();
        const auto title = itemJson["chapterTitle"].asString();
        if (url.empty() || title.empty())
        {
            continue;
        }

        catalog[knowledgePointCode].append(
            buildCourseChapterResourcePayload(itemJson));
    }

    return catalog;
}

const std::unordered_map<std::string, Json::Value> &
getCourseChapterLinkCatalog()
{
    static const auto catalog = loadCourseChapterLinkCatalog();
    return catalog;
}

Json::Value buildCourseChapterResources(const std::string &knowledgePointCode)
{
    const auto &catalog = getCourseChapterLinkCatalog();
    const auto it = catalog.find(knowledgePointCode);
    if (it == catalog.end())
    {
        return Json::Value(Json::arrayValue);
    }

    return it->second;
}

Json::Value findRawResourcesForKnowledgePoint(
    const std::string &knowledgePointCode)
{
    const auto courseChapterResources =
        buildCourseChapterResources(knowledgePointCode);
    const auto directResources =
        buildDirectCatalogResources(knowledgePointCode);
    if (directResources.isArray() && !directResources.empty())
    {
        return mergeResourceArrays(courseChapterResources, directResources);
    }

    const auto focusedResources =
        buildFocusedCatalogResources(knowledgePointCode);
    const auto inheritedResources =
        buildInheritedCatalogResources(knowledgePointCode);
    const auto relatedResources =
        buildRelatedCatalogResources(knowledgePointCode);

    return mergeResourceArrays(
        courseChapterResources,
        mergeResourceArrays(
            mergeResourceArrays(focusedResources, inheritedResources),
            relatedResources));
}

std::unordered_map<std::string, std::string> loadLatestInteractionByUrl(
    const std::string &learnerCode,
    const algorithm::planner::LearningPathItem &item)
{
    if (learnerCode.empty() || item.point.dbId <= 0)
    {
        return {};
    }

    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(learnerCode);
    if (!learner.has_value())
    {
        return {};
    }

    const auto interactionRecords =
        repositories::ResourceViewRepository::
            listLatestResourceInteractionsByLearnerAndKnowledgePointId(
                learner->id, item.point.dbId);

    std::unordered_map<std::string, std::string> latestInteractionByUrl;
    latestInteractionByUrl.reserve(interactionRecords.size());

    for (const auto &record : interactionRecords)
    {
        latestInteractionByUrl[record.resourceUrl] = record.interactionType;
    }

    return latestInteractionByUrl;
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
    const algorithm::planner::LearningPathItem &item,
    const std::string &learnerCode)
{
    const auto rawResources =
        findRawResourcesForKnowledgePoint(item.point.code);
    if (!rawResources.isArray() || rawResources.empty())
    {
        return Json::Value(Json::arrayValue);
    }

    const auto explanation =
        algorithm::explainer::explainLearningPathItem(item);
    const auto recommendationStage = buildRecommendationStage(item);
    const auto latestInteractionByUrl =
        loadLatestInteractionByUrl(learnerCode, item);

    std::vector<Json::Value> resourceItems;
    for (const auto &resourcePayload : rawResources)
    {
        auto enrichedResource = resourcePayload;
        if (enrichedResource["resourceCoverageType"].asString() ==
            "course-direct")
        {
            enrichedResource["recommendedPhase"] =
                resolveCourseChapterRecommendedPhase(item);
        }

        const auto resourceUrl = enrichedResource["url"].asString();
        const auto latestInteractionIt =
            latestInteractionByUrl.find(resourceUrl);
        const auto latestInteractionType =
            latestInteractionIt == latestInteractionByUrl.end()
                ? ""
                : latestInteractionIt->second;

        enrichedResource["selectionReason"] =
            buildSelectionReason(item, enrichedResource);
        enrichedResource["lastInteractionType"] = latestInteractionType;
        enrichedResource["lastInteractionLabel"] =
            buildInteractionLabel(latestInteractionType);
        enrichedResource["behaviorAdjustmentHint"] =
            buildBehaviorAdjustmentHint(latestInteractionType, item);
        const auto baseRecommendationScore =
            buildResourcePriorityScore(item, enrichedResource) +
            buildInteractionPriorityAdjustment(latestInteractionType, item);
        enrichedResource["baseRecommendationScore"] =
            baseRecommendationScore;
        enrichedResource["diversityAdjustment"] = 0;
        enrichedResource["recommendationScore"] = baseRecommendationScore;
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

    resourceItems = buildDiversifiedResourceOrder(std::move(resourceItems));
    resourceItems = buildCappedResourceItems(resourceItems);

    Json::Value resources(Json::arrayValue);
    for (std::size_t index = 0; index < resourceItems.size(); ++index)
    {
        auto resourcePayload = resourceItems[index];
        const bool isPrimaryRecommendation = index == 0;
        const auto resourceLayer =
            buildResourceLayer(resourcePayload, isPrimaryRecommendation);
        const auto linkedReasonLabels =
            buildLinkedReasonLabels(item, explanation, resourcePayload);
        resourcePayload["recommendationRank"] =
            static_cast<int>(index + 1);
        resourcePayload["isPrimaryRecommendation"] =
            isPrimaryRecommendation;
        resourcePayload["priorityLabel"] =
            buildPriorityLabel(isPrimaryRecommendation, resourcePayload);
        resourcePayload["resourceLayer"] = resourceLayer;
        resourcePayload["resourceLayerHint"] =
            buildResourceLayerHint(resourceLayer);
        resourcePayload["recommendationStageCode"] = recommendationStage.code;
        resourcePayload["recommendationStageLabel"] = recommendationStage.label;
        resourcePayload["stageGuidance"] = recommendationStage.guidance;
        resourcePayload["linkedReasonLabels"] = linkedReasonLabels;
        resourcePayload["linkedReasonLabelSummary"] =
            buildLinkedReasonLabelSummary(linkedReasonLabels);
        resources.append(resourcePayload);
    }

    return resources;
}

Json::Value LearningResourceService::buildResourceRecommendations(
    const std::vector<algorithm::planner::LearningPathItem> &pathItems,
    const std::string &learnerCode)
{
    // Learning resource recommendation entrypoint.
    // This service boundary converts scheduled or deferred learning path items
    // into a resource recommendation payload for the frontend recommendation page.
    // Search terms kept here on purpose: learning resource recommendation,
    // resource recommendation flow, recommendation service boundary.
    Json::Value recommendations(Json::arrayValue);

    for (const auto &item : pathItems)
    {
        if (item.status != "scheduled" && item.status != "deferred")
        {
            continue;
        }

        const auto resources =
            buildResourcesForLearningPathItem(item, learnerCode);
        if (!resources.isArray() || resources.empty())
        {
            continue;
        }

        Json::Value recommendation;
        recommendation["code"] = item.point.code;
        recommendation["name"] = item.point.name;
        recommendation["status"] = item.status;
        recommendation["scheduledNow"] = item.scheduledNow;
        recommendation["masteryScore"] = item.masteryScore;
        recommendation["masteryPercent"] =
            static_cast<int>(std::round(item.masteryScore * 100.0));
        recommendation["difficultyLevel"] = item.point.difficultyLevel;
        recommendation["estimatedMinutes"] = item.point.estimatedMinutes;
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
