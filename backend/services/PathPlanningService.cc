#include "services/PathPlanningService.h"

#include "algorithm/adjuster/LearningPathAdjuster.h"
#include "algorithm/explainer/LearningPathExplainer.h"
#include "algorithm/planner/LearningPathPlanner.h"
#include "services/KnowledgeGraphService.h"
#include "services/LearningResourceService.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace
{
algorithm::planner::PlanningRequest parsePlanningRequest(
    const Json::Value &requestJson)
{
    if (!requestJson.isObject())
    {
        throw std::invalid_argument("请求体必须是 JSON 对象。");
    }

    algorithm::planner::PlanningRequest request;

    if (requestJson.isMember("targetCodes"))
    {
        if (!requestJson["targetCodes"].isArray())
        {
            throw std::invalid_argument("targetCodes 必须是数组。");
        }

        for (const auto &code : requestJson["targetCodes"])
        {
            if (!code.isString())
            {
                throw std::invalid_argument("targetCodes 数组中的元素必须是字符串。");
            }

            request.targetCodes.push_back(code.asString());
        }
    }

    if (request.targetCodes.empty() && requestJson.isMember("targetCode"))
    {
        if (!requestJson["targetCode"].isString())
        {
            throw std::invalid_argument("targetCode 必须是字符串。");
        }

        request.targetCodes.push_back(requestJson["targetCode"].asString());
    }

    if (request.targetCodes.empty())
    {
        throw std::invalid_argument("至少需要提供 targetCode 或 targetCodes。");
    }

    if (requestJson.isMember("availableMinutes"))
    {
        if (!requestJson["availableMinutes"].isInt())
        {
            throw std::invalid_argument("availableMinutes 必须是整数。");
        }

        request.availableMinutes = requestJson["availableMinutes"].asInt();
    }

    if (requestJson.isMember("masteryByCode"))
    {
        if (!requestJson["masteryByCode"].isObject())
        {
            throw std::invalid_argument("masteryByCode 必须是对象。");
        }

        const auto memberNames = requestJson["masteryByCode"].getMemberNames();
        for (const auto &code : memberNames)
        {
            const auto &value = requestJson["masteryByCode"][code];
            if (!value.isNumeric())
            {
                throw std::invalid_argument(
                    "masteryByCode 中的掌握度必须是数值。");
            }

            request.masteryByCode[code] = value.asDouble();
        }
    }

    return request;
}

algorithm::planner::PlanningRequest parseDetailPlanningRequest(
    const Json::Value &requestJson)
{
    if (!requestJson.isObject())
    {
        throw std::invalid_argument("请求体必须是 JSON 对象。");
    }

    algorithm::planner::PlanningRequest request;

    if (requestJson.isMember("targetNodeCodes"))
    {
        if (!requestJson["targetNodeCodes"].isArray())
        {
            throw std::invalid_argument("targetNodeCodes 必须是数组。");
        }

        for (const auto &code : requestJson["targetNodeCodes"])
        {
            if (!code.isString())
            {
                throw std::invalid_argument(
                    "targetNodeCodes 数组中的元素必须是字符串。");
            }

            request.targetCodes.push_back(code.asString());
        }
    }

    if (request.targetCodes.empty() && requestJson.isMember("targetNodeCode"))
    {
        if (!requestJson["targetNodeCode"].isString())
        {
            throw std::invalid_argument("targetNodeCode 必须是字符串。");
        }

        request.targetCodes.push_back(requestJson["targetNodeCode"].asString());
    }

    if (requestJson.isMember("availableMinutes"))
    {
        if (!requestJson["availableMinutes"].isInt())
        {
            throw std::invalid_argument("availableMinutes 必须是整数。");
        }

        request.availableMinutes = requestJson["availableMinutes"].asInt();
    }

    if (requestJson.isMember("masteryByCode"))
    {
        if (!requestJson["masteryByCode"].isObject())
        {
            throw std::invalid_argument("masteryByCode 必须是对象。");
        }

        const auto memberNames = requestJson["masteryByCode"].getMemberNames();
        for (const auto &code : memberNames)
        {
            const auto &value = requestJson["masteryByCode"][code];
            if (!value.isNumeric())
            {
                throw std::invalid_argument(
                    "masteryByCode 中的掌握度必须是数值。");
            }

            request.masteryByCode[code] = value.asDouble();
        }
    }

    return request;
}

std::string parseDetailScopeCode(const Json::Value &requestJson)
{
    if (!requestJson.isMember("scopeCode") || !requestJson["scopeCode"].isString())
    {
        throw std::invalid_argument("scopeCode 必须是字符串。");
    }

    const auto scopeCode = requestJson["scopeCode"].asString();
    if (scopeCode.empty() || scopeCode == "root")
    {
        throw std::invalid_argument("scopeCode 必须指定非 root 的细化图谱范围。");
    }

    return scopeCode;
}

std::vector<algorithm::adjuster::FeedbackItem> parseFeedbackItems(
    const Json::Value &requestJson)
{
    if (!requestJson.isMember("feedbackItems") || !requestJson["feedbackItems"].isArray())
    {
        throw std::invalid_argument("feedbackItems 必须是数组。");
    }

    std::vector<algorithm::adjuster::FeedbackItem> feedbackItems;
    for (const auto &item : requestJson["feedbackItems"])
    {
        if (!item.isObject())
        {
            throw std::invalid_argument("feedbackItems 数组中的元素必须是对象。");
        }

        if (!item.isMember("code") || !item["code"].isString())
        {
            throw std::invalid_argument("feedbackItems.code 必须是字符串。");
        }

        if (!item.isMember("completionStatus") ||
            !item["completionStatus"].isString())
        {
            throw std::invalid_argument(
                "feedbackItems.completionStatus 必须是字符串。");
        }

        if (!item.isMember("selfRatedMastery") ||
            !item["selfRatedMastery"].isNumeric())
        {
            throw std::invalid_argument(
                "feedbackItems.selfRatedMastery 必须是数值。");
        }

        feedbackItems.push_back(algorithm::adjuster::FeedbackItem{
            item["code"].asString(),
            item["completionStatus"].asString(),
            item["selfRatedMastery"].asDouble()});
    }

    if (feedbackItems.empty())
    {
        throw std::invalid_argument("feedbackItems 不能为空。");
    }

    return feedbackItems;
}

Json::Value buildReasonTracePayload(
    const algorithm::planner::PathReasonTrace &reasonTrace)
{
    Json::Value payload;
    payload["knowledgePointCode"] = reasonTrace.knowledgePointCode;
    payload["isTarget"] = reasonTrace.isTarget;
    payload["isPrerequisite"] = reasonTrace.isPrerequisite;
    payload["masteryScore"] = reasonTrace.masteryScore;
    payload["masteryGap"] = reasonTrace.masteryGap;
    payload["relevanceScore"] = reasonTrace.relevanceScore;
    payload["importanceScore"] = reasonTrace.importanceScore;
    payload["timeCostPenalty"] = reasonTrace.timeCostPenalty;

    Json::Value triggers(Json::arrayValue);
    for (const auto &reason : reasonTrace.triggerReasons)
    {
        triggers.append(reason);
    }
    payload["triggerReasons"] = triggers;
    return payload;
}

Json::Value buildExplanationPayload(
    const algorithm::explainer::PathExplanation &explanation)
{
    Json::Value payload;
    payload["summary"] = explanation.summary;

    Json::Value bullets(Json::arrayValue);
    for (const auto &bullet : explanation.bullets)
    {
        bullets.append(bullet);
    }
    payload["bullets"] = bullets;

    Json::Value labels(Json::arrayValue);
    for (const auto &label : explanation.labels)
    {
        labels.append(label);
    }
    payload["labels"] = labels;

    return payload;
}

Json::Value buildPlanningPayload(
    const algorithm::graph::KnowledgeGraph &graph,
    const algorithm::planner::PlanningRequest &request,
    const algorithm::planner::PlanningResult &planningResult,
    const std::string &message)
{
    Json::Value path(Json::arrayValue);
    for (const auto &item : planningResult.path)
    {
        const auto explanation =
            algorithm::explainer::explainLearningPathItem(item);

        Json::Value pathItem;
        pathItem["order"] = item.order;
        pathItem["code"] = item.point.code;
        pathItem["name"] = item.point.name;
        pathItem["chapterNo"] = item.point.chapterNo;
        pathItem["chapterName"] = item.point.chapterName;
        pathItem["estimatedMinutes"] = item.point.estimatedMinutes;
        pathItem["difficultyLevel"] = item.point.difficultyLevel;
        pathItem["nodeType"] = item.point.nodeType;
        pathItem["status"] = item.status;
        pathItem["scheduledNow"] = item.scheduledNow;
        pathItem["isTarget"] = item.isTarget;
        pathItem["isPrerequisite"] = item.isPrerequisite;
        pathItem["masteryScore"] = item.masteryScore;
        pathItem["masteryPercent"] =
            static_cast<int>(std::round(item.masteryScore * 100.0));
        pathItem["description"] = item.point.description;
        pathItem["reasonTrace"] = buildReasonTracePayload(item.reasonTrace);
        pathItem["explanation"] = buildExplanationPayload(explanation);
        pathItem["learningResources"] =
            services::LearningResourceService::buildResourcesForLearningPathItem(
                item);
        path.append(pathItem);
    }

    Json::Value payload;
    payload["status"] = "ok";
    payload["message"] = message;
    payload["course"]["code"] = graph.courseCode;
    payload["course"]["name"] = graph.courseName;
    payload["targetCodes"] = Json::Value(Json::arrayValue);
    for (const auto &targetCode : request.targetCodes)
    {
        payload["targetCodes"].append(targetCode);
    }
    payload["summary"]["availableMinutes"] = planningResult.availableMinutes;
    payload["summary"]["scheduledMinutes"] = planningResult.scheduledMinutes;
    payload["summary"]["totalRequiredMinutes"] =
        planningResult.totalRequiredMinutes;
    payload["summary"]["scheduledCount"] = planningResult.scheduledCount;
    payload["summary"]["deferredCount"] = planningResult.deferredCount;
    payload["summary"]["masteredCount"] = planningResult.masteredCount;
    payload["summary"]["requiredKnowledgePointCount"] =
        static_cast<int>(planningResult.path.size());
    payload["summary"]["targetReachableWithinBudget"] =
        planningResult.targetReachableWithinBudget;
    payload["path"] = path;
    payload["resourceRecommendations"] =
        services::LearningResourceService::buildResourceRecommendations(
            planningResult.path);

    return payload;
}

algorithm::graph::KnowledgeGraph buildScopedGraphFromPayload(
    const Json::Value &scopePayload)
{
    if (!scopePayload.isObject() || !scopePayload.isMember("view") ||
        scopePayload["view"]["isRoot"].asBool())
    {
        throw std::invalid_argument("scopeCode 未指向有效的细化图谱范围。");
    }

    algorithm::graph::KnowledgeGraph graph;
    graph.courseId = scopePayload["course"]["id"].asInt();
    graph.courseCode = scopePayload["course"]["code"].asString();
    graph.courseName = scopePayload["course"]["name"].asString();
    graph.courseDescription = scopePayload["course"]["description"].asString();
    graph.targetAudience = scopePayload["course"]["targetAudience"].asString();

    if (!scopePayload.isMember("nodes") || !scopePayload["nodes"].isArray())
    {
        throw std::runtime_error("细化图谱缺少 nodes 数组。");
    }

    if (!scopePayload.isMember("edges") || !scopePayload["edges"].isArray())
    {
        throw std::runtime_error("细化图谱缺少 edges 数组。");
    }

    for (const auto &nodeJson : scopePayload["nodes"])
    {
        graph.points.push_back(algorithm::graph::KnowledgePoint{
            nodeJson["dbId"].asInt(),
            nodeJson["code"].asString(),
            nodeJson["label"].asString(),
            nodeJson["chapterNo"].asInt(),
            nodeJson["chapterName"].asString(),
            nodeJson["nodeType"].asString(),
            nodeJson["difficultyLevel"].asInt(),
            nodeJson["estimatedMinutes"].asInt(),
            nodeJson["displayOrder"].asInt(),
            nodeJson["isCore"].asBool(),
            nodeJson["description"].asString()});
    }

    int syntheticEdgeId = 1;
    for (const auto &edgeJson : scopePayload["edges"])
    {
        graph.dependencies.push_back(algorithm::graph::KnowledgeDependency{
            syntheticEdgeId++,
            edgeJson["source"].asString(),
            edgeJson["target"].asString(),
            edgeJson["relationType"].asString(),
            edgeJson["description"].asString()});
    }

    return graph;
}

std::vector<std::string> collectDefaultTargetCodes(
    const algorithm::graph::KnowledgeGraph &graph)
{
    std::unordered_map<std::string, int> outdegree;
    outdegree.reserve(graph.points.size());

    for (const auto &point : graph.points)
    {
        outdegree[point.code] = 0;
    }

    for (const auto &dependency : graph.dependencies)
    {
        ++outdegree[dependency.sourceCode];
    }

    std::vector<algorithm::graph::KnowledgePoint> sinkPoints;
    for (const auto &point : graph.points)
    {
        if (outdegree[point.code] == 0)
        {
            sinkPoints.push_back(point);
        }
    }

    auto sortPoints = [](std::vector<algorithm::graph::KnowledgePoint> &points) {
        std::sort(points.begin(), points.end(),
                  [](const auto &left, const auto &right) {
                      if (left.displayOrder != right.displayOrder)
                      {
                          return left.displayOrder < right.displayOrder;
                      }

                      return left.code < right.code;
                  });
    };

    if (sinkPoints.empty())
    {
        sinkPoints = graph.points;
    }

    sortPoints(sinkPoints);

    std::vector<std::string> targetCodes;
    targetCodes.reserve(sinkPoints.size());
    for (const auto &point : sinkPoints)
    {
        targetCodes.push_back(point.code);
    }

    return targetCodes;
}
}

namespace services
{
Json::Value PathPlanningService::buildPathPayload(const Json::Value &requestJson)
{
    const auto request = parsePlanningRequest(requestJson);
    const auto graph = KnowledgeGraphService::loadKnowledgeGraph();
    const auto planningResult =
        algorithm::planner::generateLearningPath(graph, request);

    return buildPlanningPayload(
        graph, request, planningResult, "已生成最小版个性化学习路径。");
}

Json::Value PathPlanningService::buildAdjustedPathPayload(
    const Json::Value &requestJson)
{
    const auto baseRequest = parsePlanningRequest(requestJson);
    const auto feedbackItems = parseFeedbackItems(requestJson);
    const auto graph = KnowledgeGraphService::loadKnowledgeGraph();
    const auto adjustmentResult = algorithm::adjuster::adjustLearningPath(
        graph, baseRequest, feedbackItems);

    auto payload = buildPlanningPayload(graph, adjustmentResult.updatedRequest,
                                        adjustmentResult.adjustedPlan,
                                        "已根据学习反馈调整推荐路径。");

    Json::Value updatedMasteryByCode;
    for (const auto &[code, mastery] : adjustmentResult.updatedRequest.masteryByCode)
    {
        updatedMasteryByCode[code] = mastery;
    }

    Json::Value adjustmentDetails(Json::arrayValue);
    for (const auto &detail : adjustmentResult.adjustmentDetails)
    {
        Json::Value item;
        item["code"] = detail.code;
        item["completionStatus"] = detail.completionStatus;
        item["previousMastery"] = detail.previousMastery;
        item["updatedMastery"] = detail.updatedMastery;
        item["ruleApplied"] = detail.ruleApplied;

        Json::Value reasons(Json::arrayValue);
        for (const auto &reason : detail.adjustmentReasons)
        {
            reasons.append(reason);
        }
        item["adjustmentReasons"] = reasons;
        adjustmentDetails.append(item);
    }

    payload["feedbackSummary"]["feedbackItemCount"] =
        static_cast<int>(adjustmentResult.adjustmentDetails.size());
    payload["feedbackSummary"]["completedCount"] =
        adjustmentResult.completedCount;
    payload["feedbackSummary"]["partialCount"] = adjustmentResult.partialCount;
    payload["feedbackSummary"]["blockedCount"] = adjustmentResult.blockedCount;
    payload["updatedMasteryByCode"] = updatedMasteryByCode;
    payload["adjustments"] = adjustmentDetails;

    return payload;
}

Json::Value PathPlanningService::buildDetailPathPayload(
    const Json::Value &requestJson)
{
    const auto scopeCode = parseDetailScopeCode(requestJson);
    auto request = parseDetailPlanningRequest(requestJson);
    const auto scopePayload = KnowledgeGraphService::buildGraphPayload(scopeCode);
    const auto scopeGraph = buildScopedGraphFromPayload(scopePayload);

    if (request.targetCodes.empty())
    {
        request.targetCodes = collectDefaultTargetCodes(scopeGraph);
    }

    if (request.targetCodes.empty())
    {
        throw std::invalid_argument("当前细化图谱中没有可用的目标节点。");
    }

    const auto planningResult =
        algorithm::planner::generateLearningPath(scopeGraph, request);
    auto payload = buildPlanningPayload(
        scopeGraph, request, planningResult, "已生成当前细化图谱范围内的学习路径。");

    payload["scope"] = scopePayload["view"];
    payload["breadcrumbs"] = scopePayload["breadcrumbs"];
    payload["meta"] = scopePayload["meta"];

    return payload;
}

Json::Value PathPlanningService::buildAdjustedDetailPathPayload(
    const Json::Value &requestJson)
{
    const auto scopeCode = parseDetailScopeCode(requestJson);
    auto baseRequest = parseDetailPlanningRequest(requestJson);
    const auto feedbackItems = parseFeedbackItems(requestJson);
    const auto scopePayload = KnowledgeGraphService::buildGraphPayload(scopeCode);
    const auto scopeGraph = buildScopedGraphFromPayload(scopePayload);

    if (baseRequest.targetCodes.empty())
    {
        baseRequest.targetCodes = collectDefaultTargetCodes(scopeGraph);
    }

    if (baseRequest.targetCodes.empty())
    {
        throw std::invalid_argument("当前细化图谱中没有可用的目标节点。");
    }

    const auto adjustmentResult = algorithm::adjuster::adjustLearningPath(
        scopeGraph, baseRequest, feedbackItems);

    auto payload = buildPlanningPayload(
        scopeGraph, adjustmentResult.updatedRequest, adjustmentResult.adjustedPlan,
        "已根据本范围学习反馈调整局部学习路径。");

    Json::Value updatedMasteryByCode;
    for (const auto &[code, mastery] : adjustmentResult.updatedRequest.masteryByCode)
    {
        updatedMasteryByCode[code] = mastery;
    }

    Json::Value adjustmentDetails(Json::arrayValue);
    for (const auto &detail : adjustmentResult.adjustmentDetails)
    {
        Json::Value item;
        item["code"] = detail.code;
        item["completionStatus"] = detail.completionStatus;
        item["previousMastery"] = detail.previousMastery;
        item["updatedMastery"] = detail.updatedMastery;
        item["ruleApplied"] = detail.ruleApplied;

        Json::Value reasons(Json::arrayValue);
        for (const auto &reason : detail.adjustmentReasons)
        {
            reasons.append(reason);
        }
        item["adjustmentReasons"] = reasons;
        adjustmentDetails.append(item);
    }

    payload["scope"] = scopePayload["view"];
    payload["breadcrumbs"] = scopePayload["breadcrumbs"];
    payload["meta"] = scopePayload["meta"];
    payload["feedbackSummary"]["feedbackItemCount"] =
        static_cast<int>(adjustmentResult.adjustmentDetails.size());
    payload["feedbackSummary"]["completedCount"] =
        adjustmentResult.completedCount;
    payload["feedbackSummary"]["partialCount"] = adjustmentResult.partialCount;
    payload["feedbackSummary"]["blockedCount"] = adjustmentResult.blockedCount;
    payload["updatedMasteryByCode"] = updatedMasteryByCode;
    payload["adjustments"] = adjustmentDetails;

    return payload;
}
}
