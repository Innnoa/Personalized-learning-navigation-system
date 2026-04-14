#include "algorithm/adjuster/LearningPathAdjuster.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_set>

namespace
{
double clampMastery(double mastery)
{
    return std::clamp(mastery, 0.0, 1.0);
}

double roundToTwoDecimals(double value)
{
    return std::round(value * 100.0) / 100.0;
}

std::pair<double, double> resolveMasteryRangeByStatus(
    const std::string &completionStatus)
{
    if (completionStatus == "completed")
    {
        return {0.8, 1.0};
    }

    if (completionStatus == "partial")
    {
        return {0.4, 0.8};
    }

    return {0.0, 0.35};
}

double clampSelfRatedMasteryByStatus(const std::string &completionStatus,
                                     double selfRatedMastery)
{
    const auto [minMastery, maxMastery] =
        resolveMasteryRangeByStatus(completionStatus);
    return std::clamp(clampMastery(selfRatedMastery), minMastery, maxMastery);
}

double resolvePartialUpdatedMastery(double previousMastery,
                                    double selfRatedMastery)
{
    constexpr double kPartialMaxMastery = 0.8;
    constexpr double kPartialProgressFloor = 0.1;
    constexpr double kPartialRegressionCap = 0.15;

    if (selfRatedMastery >= previousMastery)
    {
        const double upwardCandidate =
            std::max(selfRatedMastery, previousMastery + kPartialProgressFloor);
        return std::min(kPartialMaxMastery, upwardCandidate);
    }

    const double lowerBound = std::max(0.4, previousMastery - kPartialRegressionCap);
    return std::clamp(selfRatedMastery, lowerBound, previousMastery);
}
}

namespace algorithm::adjuster
{
FeedbackUpdateResult applyFeedbackUpdates(
    const graph::KnowledgeGraph &graph,
    const std::unordered_map<std::string, double> &baseMasteryByCode,
    const std::vector<FeedbackItem> &feedbackItems)
{
    if (feedbackItems.empty())
    {
        throw std::invalid_argument("至少需要提供一条学习反馈。");
    }

    std::unordered_set<std::string> validCodes;
    validCodes.reserve(graph.points.size());
    for (const auto &point : graph.points)
    {
        validCodes.insert(point.code);
    }

    FeedbackUpdateResult result;
    result.updatedMasteryByCode = baseMasteryByCode;

    for (const auto &feedback : feedbackItems)
    {
        if (feedback.code.empty())
        {
            throw std::invalid_argument("反馈中的知识点编码不能为空。");
        }

        if (validCodes.count(feedback.code) == 0)
        {
            throw std::invalid_argument("反馈中的知识点不存在: " + feedback.code);
        }

        if (feedback.completionStatus != "completed" &&
            feedback.completionStatus != "partial" &&
            feedback.completionStatus != "blocked")
        {
            throw std::invalid_argument("completionStatus 仅支持 completed / partial / blocked。");
        }

        const double previousMastery =
            result.updatedMasteryByCode.count(feedback.code) > 0
                ? clampMastery(result.updatedMasteryByCode[feedback.code])
                : 0.0;
        const double selfRatedMastery =
            clampSelfRatedMasteryByStatus(feedback.completionStatus,
                                          feedback.selfRatedMastery);

        double updatedMastery = previousMastery;
        std::string ruleApplied;
        std::vector<std::string> adjustmentReasons;

        if (feedback.completionStatus == "completed")
        {
            ++result.completedCount;
            updatedMastery = std::max({previousMastery, selfRatedMastery, 0.85});
            ruleApplied = "completed_promote_to_mastered";
            adjustmentReasons.push_back("该节点已完成学习，掌握度提升到已掌握阈值附近。");
            adjustmentReasons.push_back("后续规划时可降低其优先级，转向未掌握的后继节点。");
        }
        else if (feedback.completionStatus == "partial")
        {
            ++result.partialCount;
            updatedMastery =
                resolvePartialUpdatedMastery(previousMastery, selfRatedMastery);
            ruleApplied = "partial_keep_in_progress";
            adjustmentReasons.push_back("该节点仅部分完成，掌握度保留在巩固区间内小幅调整。");
            adjustmentReasons.push_back("系统会在后续规划中继续保留相关复习或补强需求。");
        }
        else
        {
            ++result.blockedCount;
            updatedMastery = std::min({0.35, previousMastery, selfRatedMastery});
            ruleApplied = "blocked_lower_priority_chain";
            adjustmentReasons.push_back("该节点学习受阻，掌握度回落到较低区间。");
            adjustmentReasons.push_back("系统会优先保留该节点及其相关前置内容，避免直接推进后继目标。");
        }

        updatedMastery = roundToTwoDecimals(clampMastery(updatedMastery));
        result.updatedMasteryByCode[feedback.code] = updatedMastery;

        result.adjustmentDetails.push_back(AdjustmentDetail{
            feedback.code,
            feedback.completionStatus,
            roundToTwoDecimals(previousMastery),
            updatedMastery,
            ruleApplied,
            adjustmentReasons});
    }

    return result;
}

AdjustmentResult adjustLearningPath(const graph::KnowledgeGraph &graph,
                                    const planner::PlanningRequest &baseRequest,
                                    const std::vector<FeedbackItem> &feedbackItems)
{
    AdjustmentResult result;
    result.updatedRequest = baseRequest;

    const auto feedbackUpdate = applyFeedbackUpdates(
        graph, baseRequest.masteryByCode, feedbackItems);
    result.updatedRequest.masteryByCode = feedbackUpdate.updatedMasteryByCode;
    result.adjustmentDetails = feedbackUpdate.adjustmentDetails;
    result.completedCount = feedbackUpdate.completedCount;
    result.partialCount = feedbackUpdate.partialCount;
    result.blockedCount = feedbackUpdate.blockedCount;
    result.adjustedPlan =
        planner::generateLearningPath(graph, result.updatedRequest);
    return result;
}
}
