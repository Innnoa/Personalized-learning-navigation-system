#pragma once

#include "algorithm/graph/KnowledgeGraph.h"
#include "algorithm/planner/LearningPathPlanner.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace algorithm::adjuster
{
struct FeedbackItem
{
    std::string code;
    std::string completionStatus;
    double selfRatedMastery = 0.0;
};

struct AdjustmentDetail
{
    std::string code;
    std::string completionStatus;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    std::string ruleApplied;
    std::vector<std::string> adjustmentReasons;
};

struct AdjustmentResult
{
    planner::PlanningRequest updatedRequest;
    planner::PlanningResult adjustedPlan;
    std::vector<AdjustmentDetail> adjustmentDetails;
    int completedCount = 0;
    int partialCount = 0;
    int blockedCount = 0;
};

struct FeedbackUpdateResult
{
    std::unordered_map<std::string, double> updatedMasteryByCode;
    std::vector<AdjustmentDetail> adjustmentDetails;
    int completedCount = 0;
    int partialCount = 0;
    int blockedCount = 0;
};

FeedbackUpdateResult applyFeedbackUpdates(
    const graph::KnowledgeGraph &graph,
    const std::unordered_map<std::string, double> &baseMasteryByCode,
    const std::vector<FeedbackItem> &feedbackItems);

AdjustmentResult adjustLearningPath(const graph::KnowledgeGraph &graph,
                                    const planner::PlanningRequest &baseRequest,
                                    const std::vector<FeedbackItem> &feedbackItems);
}
