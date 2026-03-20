#pragma once

#include "algorithm/graph/KnowledgeGraph.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace algorithm::planner
{
struct PlanningRequest
{
    std::vector<std::string> targetCodes;
    int availableMinutes = 120;
    std::unordered_map<std::string, double> masteryByCode;
};

struct PathReasonTrace
{
    std::string knowledgePointCode;
    bool isTarget = false;
    bool isPrerequisite = false;
    double masteryScore = 0.0;
    double masteryGap = 1.0;
    double relevanceScore = 0.0;
    double importanceScore = 0.0;
    double timeCostPenalty = 0.0;
    std::vector<std::string> triggerReasons;
};

struct LearningPathItem
{
    int order = 0;
    graph::KnowledgePoint point;
    std::string status;
    bool scheduledNow = false;
    bool isTarget = false;
    bool isPrerequisite = false;
    double masteryScore = 0.0;
    PathReasonTrace reasonTrace;
};

struct PlanningResult
{
    std::vector<LearningPathItem> path;
    int totalRequiredMinutes = 0;
    int scheduledMinutes = 0;
    int availableMinutes = 0;
    int scheduledCount = 0;
    int deferredCount = 0;
    int masteredCount = 0;
    bool targetReachableWithinBudget = false;
};

PlanningResult generateLearningPath(const graph::KnowledgeGraph &graph,
                                    const PlanningRequest &request);
}
