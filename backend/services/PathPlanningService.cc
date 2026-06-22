#include "services/PathPlanningService.h"

#include "algorithm/adjuster/LearningPathAdjuster.h"
#include "algorithm/explainer/LearningPathExplainer.h"
#include "algorithm/planner/LearningPathPlanner.h"
#include "repositories/DetailLearningRepository.h"
#include "repositories/FeedbackRepository.h"
#include "repositories/LearnerProfileRepository.h"
#include "services/DetailLearningProgressService.h"
#include "services/KnowledgeGraphService.h"
#include "services/LearningResourceService.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>

#include <cmath>
#include <algorithm>
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

std::string readDefaultLearnerCode()
{
    const auto &config = drogon::app().getCustomConfig();
    if (config.isMember("default_learner_code") &&
        config["default_learner_code"].isString())
    {
        const auto learnerCode = config["default_learner_code"].asString();
        if (!learnerCode.empty())
        {
            return learnerCode;
        }
    }

    return "demo-learner";
}

std::string resolvePlanningLearnerCode(const Json::Value &requestJson)
{
    if (!requestJson.isMember("learnerCode"))
    {
        return readDefaultLearnerCode();
    }

    if (!requestJson["learnerCode"].isString())
    {
        throw std::invalid_argument("learnerCode 必须是字符串。");
    }

    const auto learnerCode = requestJson["learnerCode"].asString();
    return learnerCode.empty() ? readDefaultLearnerCode() : learnerCode;
}

std::string resolvePlanningCourseCode(const Json::Value &requestJson)
{
    if (!requestJson.isMember("courseCode"))
    {
        return "";
    }

    if (!requestJson["courseCode"].isString())
    {
        throw std::invalid_argument("courseCode 必须是字符串。");
    }

    return requestJson["courseCode"].asString();
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

std::string joinValues(const std::vector<std::string> &values,
                       std::size_t limit,
                       const std::string &overflowUnit)
{
    if (values.empty())
    {
        return "";
    }

    std::ostringstream builder;
    const auto visibleCount = std::min(limit, values.size());
    for (std::size_t index = 0; index < visibleCount; ++index)
    {
        if (index > 0)
        {
            builder << "、";
        }
        builder << values[index];
    }

    if (values.size() > limit)
    {
        builder << "等" << values.size() << overflowUnit;
    }

    return builder.str();
}

std::string resolveKnowledgePointName(
    const algorithm::graph::KnowledgeGraph &graph,
    const std::string &code)
{
    const auto pointIt =
        std::find_if(graph.points.begin(), graph.points.end(),
                     [&code](const auto &point) { return point.code == code; });
    if (pointIt != graph.points.end())
    {
        return pointIt->name;
    }

    return code;
}

Json::Value buildOverallExplanationPayload(
    const algorithm::graph::KnowledgeGraph &graph,
    const algorithm::planner::PlanningRequest &request,
    const algorithm::planner::PlanningResult &planningResult)
{
    std::vector<std::string> targetNames;
    std::vector<std::string> scheduledNames;
    std::vector<std::string> deferredNames;
    std::vector<std::string> prerequisiteNames;
    std::vector<std::string> lowMasteryNames;
    std::vector<std::string> triggerReasons;
    std::vector<std::string> labels;

    targetNames.reserve(request.targetCodes.size());
    for (const auto &code : request.targetCodes)
    {
        appendUnique(targetNames, resolveKnowledgePointName(graph, code));
    }

    for (const auto &item : planningResult.path)
    {
        if (item.status == "scheduled")
        {
            appendUnique(scheduledNames, item.point.name);
        }
        else if (item.status == "deferred")
        {
            appendUnique(deferredNames, item.point.name);
        }

        if (item.isPrerequisite)
        {
            appendUnique(prerequisiteNames, item.point.name);
        }

        if (item.status != "mastered" && item.reasonTrace.masteryGap >= 0.5)
        {
            appendUnique(lowMasteryNames, item.point.name);
        }

        for (const auto &reason : item.reasonTrace.triggerReasons)
        {
            appendUnique(triggerReasons, reason);
        }
    }

    if (!prerequisiteNames.empty())
    {
        appendUnique(labels, "前置依赖链路");
    }
    if (!lowMasteryNames.empty())
    {
        appendUnique(labels, "掌握度待提升");
    }
    if (!scheduledNames.empty())
    {
        appendUnique(labels, "本轮优先安排");
    }
    if (!deferredNames.empty())
    {
        appendUnique(labels, planningResult.targetReachableWithinBudget
                                 ? "下一轮建议"
                                 : "时间预算受限");
    }

    std::ostringstream summary;
    summary << "当前目标为" << joinValues(targetNames, 2, "个目标") << "。";

    std::vector<std::string> reasonFragments;
    if (!prerequisiteNames.empty())
    {
        reasonFragments.push_back("目标链路依赖" +
                                  joinValues(prerequisiteNames, 2, "个前置节点"));
    }
    if (!lowMasteryNames.empty())
    {
        reasonFragments.push_back(joinValues(lowMasteryNames, 2, "个节点") +
                                  "当前掌握度仍偏低");
    }

    if (!reasonFragments.empty())
    {
        summary << "由于";
        for (std::size_t index = 0; index < reasonFragments.size(); ++index)
        {
            if (index > 0)
            {
                summary << "，且";
            }
            summary << reasonFragments[index];
        }
        summary << "，";
    }
    else
    {
        summary << "结合当前掌握度与时间预算，";
    }

    if (!scheduledNames.empty())
    {
        summary << "本轮优先安排" << joinValues(scheduledNames, 3, "个节点");
    }
    else if (!deferredNames.empty())
    {
        summary << "当前暂无可直接纳入本轮的新节点";
    }
    else
    {
        summary << "当前链路已基本满足，无需新增学习节点";
    }

    if (!deferredNames.empty())
    {
        summary << "；" << joinValues(deferredNames, 2, "个节点");
        if (planningResult.targetReachableWithinBudget)
        {
            summary << "保留为下一轮建议";
        }
        else
        {
            summary << "因时间预算限制顺延到下一轮";
        }
    }
    summary << "。";

    Json::Value payload;
    payload["summary"] = summary.str();

    Json::Value bullets(Json::arrayValue);
    {
        std::ostringstream bullet;
        bullet << "当前目标：" << joinValues(targetNames, 3, "个目标");
        bullets.append(bullet.str());
    }
    if (!prerequisiteNames.empty())
    {
        bullets.append("前置依赖：" +
                       joinValues(prerequisiteNames, 3, "个前置节点"));
    }
    if (!lowMasteryNames.empty())
    {
        bullets.append("待补强节点：" +
                       joinValues(lowMasteryNames, 3, "个节点"));
    }
    if (!scheduledNames.empty())
    {
        std::ostringstream bullet;
        bullet << "本轮安排：" << planningResult.scheduledCount << " 个节点，优先学习 "
               << joinValues(scheduledNames, 3, "个节点");
        bullets.append(bullet.str());
    }
    if (!deferredNames.empty())
    {
        std::ostringstream bullet;
        bullet << "下一轮建议：" << planningResult.deferredCount << " 个节点，"
               << joinValues(deferredNames, 3, "个节点");
        if (!planningResult.targetReachableWithinBudget)
        {
            bullet << " 受当前时间预算限制顺延";
        }
        bullets.append(bullet.str());
    }
    if (!triggerReasons.empty())
    {
        bullets.append("关键依据：" + joinValues(triggerReasons, 2, "条依据"));
    }
    {
        std::ostringstream bullet;
        bullet << "预算判断：当前时间预算为 " << planningResult.availableMinutes
               << " 分钟，";
        bullet << (planningResult.targetReachableWithinBudget
                       ? "可以覆盖目标链路。"
                       : "暂时无法完整覆盖目标链路。");
        bullets.append(bullet.str());
    }
    payload["bullets"] = bullets;

    Json::Value jsonLabels(Json::arrayValue);
    for (const auto &label : labels)
    {
        jsonLabels.append(label);
    }
    payload["labels"] = jsonLabels;

    return payload;
}

Json::Value buildPlanningPayload(
    const algorithm::graph::KnowledgeGraph &graph,
    const algorithm::planner::PlanningRequest &request,
    const algorithm::planner::PlanningResult &planningResult,
    const std::string &message,
    const std::string &learnerCode)
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
                item, learnerCode);
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
    payload["overallExplanation"] =
        buildOverallExplanationPayload(graph, request, planningResult);
    payload["resourceRecommendations"] =
        services::LearningResourceService::buildResourceRecommendations(
            planningResult.path, learnerCode);

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

Json::Value buildPracticedDetailNodeCodesPayload(
    int learnerId, const std::string &scopeCode)
{
    Json::Value practicedNodeCodes(Json::arrayValue);
    const auto practicedCodes =
        repositories::DetailLearningRepository::listPracticedNodeCodesByLearnerAndScope(
            learnerId, scopeCode);

    for (const auto &code : practicedCodes)
    {
        practicedNodeCodes.append(code);
    }

    return practicedNodeCodes;
}

Json::Value buildPracticedKnowledgePointCodesPayload(int learnerId)
{
    Json::Value practicedCodes(Json::arrayValue);
    const auto codeList =
        repositories::FeedbackRepository::listPracticedKnowledgePointCodesByLearnerId(
            learnerId);

    for (const auto &code : codeList)
    {
        practicedCodes.append(code);
    }

    return practicedCodes;
}
}

namespace services
{
Json::Value PathPlanningService::buildPathPayload(const Json::Value &requestJson)
{
    const auto request = parsePlanningRequest(requestJson);
    const auto learnerCode = resolvePlanningLearnerCode(requestJson);
    const auto courseCode = resolvePlanningCourseCode(requestJson);
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            learnerCode.empty() ? readDefaultLearnerCode() : learnerCode);
    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法生成学习路径。");
    }
    const auto graph = KnowledgeGraphService::loadKnowledgeGraph(courseCode);
    const auto planningResult =
        algorithm::planner::generateLearningPath(graph, request);

    auto payload = buildPlanningPayload(
        graph,
        request,
        planningResult,
        "已生成最小版个性化学习路径。",
        learnerCode);
    payload["practiceStatusByCode"] =
        buildPracticedKnowledgePointCodesPayload(learner->id);
    return payload;
}

Json::Value PathPlanningService::buildAdjustedPathPayload(
    const Json::Value &requestJson)
{
    const auto baseRequest = parsePlanningRequest(requestJson);
    const auto learnerCode = resolvePlanningLearnerCode(requestJson);
    const auto courseCode = resolvePlanningCourseCode(requestJson);
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            learnerCode.empty() ? readDefaultLearnerCode() : learnerCode);
    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法调整学习路径。");
    }
    const auto feedbackItems = parseFeedbackItems(requestJson);
    const auto graph = KnowledgeGraphService::loadKnowledgeGraph(courseCode);
    const auto adjustmentResult = algorithm::adjuster::adjustLearningPath(
        graph, baseRequest, feedbackItems);

    auto payload = buildPlanningPayload(graph, adjustmentResult.updatedRequest,
                                        adjustmentResult.adjustedPlan,
                                        "已根据学习反馈调整推荐路径。",
                                        learnerCode);

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
    payload["practiceStatusByCode"] =
        buildPracticedKnowledgePointCodesPayload(learner->id);

    return payload;
}

Json::Value PathPlanningService::buildDetailPathPayload(
    const Json::Value &requestJson)
{
    const auto scopeCode = parseDetailScopeCode(requestJson);
    auto request = parseDetailPlanningRequest(requestJson);
    const auto learnerCode = resolvePlanningLearnerCode(requestJson);
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            learnerCode.empty() ? readDefaultLearnerCode() : learnerCode);
    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法生成细化学习路径。");
    }
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
        scopeGraph,
        request,
        planningResult,
        "已生成当前细化图谱范围内的学习路径。",
        learnerCode);

    payload["scope"] = scopePayload["view"];
    payload["breadcrumbs"] = scopePayload["breadcrumbs"];
    payload["meta"] = scopePayload["meta"];
    payload["practiceStatusByCode"] =
        buildPracticedDetailNodeCodesPayload(learner->id, scopeCode);

    return payload;
}

Json::Value PathPlanningService::buildAdjustedDetailPathPayload(
    const Json::Value &requestJson)
{
    const auto scopeCode = parseDetailScopeCode(requestJson);
    auto baseRequest = parseDetailPlanningRequest(requestJson);
    const auto learnerCode = resolvePlanningLearnerCode(requestJson);
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            learnerCode.empty() ? readDefaultLearnerCode() : learnerCode);
    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法保存细化学习反馈。");
    }

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
    const auto feedbackBatchId = drogon::utils::getUuid();
    services::DetailLearningProgressService::persistScopeMasteryAndSyncParents(
        learner->id,
        learner->targetCourseId,
        scopeCode,
        adjustmentResult.updatedRequest.masteryByCode);

    std::unordered_map<std::string, double> selfRatedMasteryByCode;
    selfRatedMasteryByCode.reserve(feedbackItems.size());
    for (const auto &item : feedbackItems)
    {
        selfRatedMasteryByCode[item.code] = item.selfRatedMastery;
    }
    for (const auto &detail : adjustmentResult.adjustmentDetails)
    {
        repositories::DetailLearningRepository::insertDetailFeedbackRecord(
            repositories::DetailFeedbackRecordWrite{
                learner->id,
                scopeCode,
                detail.code,
                detail.completionStatus,
                selfRatedMasteryByCode[detail.code],
                detail.previousMastery,
                detail.updatedMastery,
                detail.ruleApplied,
                feedbackBatchId});
    }

    auto payload = buildPlanningPayload(
        scopeGraph, adjustmentResult.updatedRequest, adjustmentResult.adjustedPlan,
        "已根据本范围学习反馈调整局部学习路径。",
        learnerCode);

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
    payload["feedbackSummary"]["persistedToLearnerProfile"] = true;
    payload["feedbackBatchId"] = feedbackBatchId;
    payload["updatedMasteryByCode"] = updatedMasteryByCode;
    payload["adjustments"] = adjustmentDetails;
    payload["practiceStatusByCode"] =
        buildPracticedDetailNodeCodesPayload(learner->id, scopeCode);

    return payload;
}
}
