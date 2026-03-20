#include "algorithm/planner/LearningPathPlanner.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <stdexcept>
#include <unordered_set>

namespace
{
double clampMastery(double mastery)
{
    if (mastery < 0.0)
    {
        return 0.0;
    }

    if (mastery > 1.0)
    {
        return 1.0;
    }

    return mastery;
}

double roundToTwoDecimals(double value)
{
    return std::round(value * 100.0) / 100.0;
}

double computeRelevanceScore(int distanceToTarget)
{
    switch (distanceToTarget)
    {
        case 0:
            return 1.0;
        case 1:
            return 0.85;
        case 2:
            return 0.72;
        case 3:
            return 0.6;
        default:
            return 0.5;
    }
}
}

namespace algorithm::planner
{
PlanningResult generateLearningPath(const graph::KnowledgeGraph &graph,
                                    const PlanningRequest &request)
{
    if (request.targetCodes.empty())
    {
        throw std::invalid_argument("至少需要指定一个目标知识点。");
    }

    std::unordered_map<std::string, const graph::KnowledgePoint *> pointByCode;
    pointByCode.reserve(graph.points.size());

    for (const auto &point : graph.points)
    {
        pointByCode[point.code] = &point;
    }

    std::unordered_map<std::string, std::vector<std::string>> prerequisitesByCode;
    std::unordered_map<std::string, std::vector<std::string>> dependentsByCode;
    for (const auto &dependency : graph.dependencies)
    {
        prerequisitesByCode[dependency.targetCode].push_back(dependency.sourceCode);
        dependentsByCode[dependency.sourceCode].push_back(dependency.targetCode);
    }

    std::unordered_set<std::string> targetSet;
    for (const auto &targetCode : request.targetCodes)
    {
        if (pointByCode.find(targetCode) == pointByCode.end())
        {
            throw std::invalid_argument("目标知识点不存在: " + targetCode);
        }

        targetSet.insert(targetCode);
    }

    std::unordered_set<std::string> requiredSet;
    std::vector<std::string> stack(targetSet.begin(), targetSet.end());
    while (!stack.empty())
    {
        const auto currentCode = stack.back();
        stack.pop_back();

        if (!requiredSet.insert(currentCode).second)
        {
            continue;
        }

        for (const auto &prerequisiteCode : prerequisitesByCode[currentCode])
        {
            stack.push_back(prerequisiteCode);
        }
    }

    std::unordered_map<std::string, int> indegree;
    indegree.reserve(requiredSet.size());
    for (const auto &code : requiredSet)
    {
        indegree[code] = 0;
    }

    for (const auto &dependency : graph.dependencies)
    {
        if (requiredSet.count(dependency.sourceCode) == 0 ||
            requiredSet.count(dependency.targetCode) == 0)
        {
            continue;
        }

        ++indegree[dependency.targetCode];
    }

    std::vector<std::string> zeroIndegree;
    zeroIndegree.reserve(requiredSet.size());
    for (const auto &[code, degree] : indegree)
    {
        if (degree == 0)
        {
            zeroIndegree.push_back(code);
        }
    }

    auto sortByDisplayOrder = [&pointByCode](std::vector<std::string> &codes) {
        std::sort(codes.begin(), codes.end(), [&pointByCode](const auto &left,
                                                            const auto &right) {
            const auto *leftPoint = pointByCode.at(left);
            const auto *rightPoint = pointByCode.at(right);
            if (leftPoint->displayOrder != rightPoint->displayOrder)
            {
                return leftPoint->displayOrder < rightPoint->displayOrder;
            }

            return left < right;
        });
    };

    sortByDisplayOrder(zeroIndegree);

    std::vector<std::string> topoOrder;
    topoOrder.reserve(requiredSet.size());

    while (!zeroIndegree.empty())
    {
        const auto currentCode = zeroIndegree.front();
        zeroIndegree.erase(zeroIndegree.begin());
        topoOrder.push_back(currentCode);

        for (const auto &dependentCode : dependentsByCode[currentCode])
        {
            if (requiredSet.count(dependentCode) == 0)
            {
                continue;
            }

            auto &degree = indegree[dependentCode];
            --degree;
            if (degree == 0)
            {
                zeroIndegree.push_back(dependentCode);
                sortByDisplayOrder(zeroIndegree);
            }
        }
    }

    if (topoOrder.size() != requiredSet.size())
    {
        throw std::runtime_error("知识点依赖图存在环，无法生成学习路径。");
    }

    std::unordered_map<std::string, int> distanceToTarget;
    std::queue<std::string> bfsQueue;
    for (const auto &targetCode : targetSet)
    {
        distanceToTarget[targetCode] = 0;
        bfsQueue.push(targetCode);
    }

    while (!bfsQueue.empty())
    {
        const auto currentCode = bfsQueue.front();
        bfsQueue.pop();

        const auto currentDistance = distanceToTarget[currentCode];
        for (const auto &prerequisiteCode : prerequisitesByCode[currentCode])
        {
            if (requiredSet.count(prerequisiteCode) == 0 ||
                distanceToTarget.find(prerequisiteCode) != distanceToTarget.end())
            {
                continue;
            }

            distanceToTarget[prerequisiteCode] = currentDistance + 1;
            bfsQueue.push(prerequisiteCode);
        }
    }

    PlanningResult result;
    result.availableMinutes = std::max(request.availableMinutes, 0);

    int order = 1;
    for (const auto &code : topoOrder)
    {
        const auto *point = pointByCode.at(code);
        const auto masteryIter = request.masteryByCode.find(code);
        const double masteryScore =
            masteryIter == request.masteryByCode.end()
                ? 0.0
                : clampMastery(masteryIter->second);
        const bool isTarget = targetSet.count(code) > 0;
        const bool isPrerequisite = !isTarget;
        const bool mastered = masteryScore >= 0.85;
        const int distance = distanceToTarget.count(code) > 0
                                 ? distanceToTarget[code]
                                 : 3;
        const double relevanceScore = computeRelevanceScore(distance);
        double importanceScore =
            (isTarget ? 1.0 : 0.62) + (point->isCore ? 0.08 : 0.0);
        importanceScore = std::min(1.0, importanceScore);
        const double masteryGap = roundToTwoDecimals(1.0 - masteryScore);
        const double timeCostPenalty =
            result.availableMinutes == 0
                ? 1.0
                : roundToTwoDecimals(std::min(
                      1.0, static_cast<double>(point->estimatedMinutes) /
                               static_cast<double>(result.availableMinutes)));

        std::vector<std::string> triggerReasons;
        if (isTarget)
        {
            triggerReasons.push_back("该知识点是当前学习目标。");
        }
        if (isPrerequisite)
        {
            triggerReasons.push_back("该知识点是目标知识点的前置依赖。");
        }
        if (masteryScore < 0.5)
        {
            triggerReasons.push_back("当前掌握度较低，建议优先学习。");
        }
        else if (!mastered)
        {
            triggerReasons.push_back("当前掌握度尚未稳定，需要继续巩固。");
        }
        else
        {
            triggerReasons.push_back("当前掌握度较高，可作为已掌握节点保留。");
        }

        std::string status;
        bool scheduledNow = false;
        if (mastered)
        {
            status = "mastered";
            ++result.masteredCount;
        }
        else if (result.scheduledMinutes + point->estimatedMinutes <=
                 result.availableMinutes)
        {
            status = "scheduled";
            scheduledNow = true;
            result.scheduledMinutes += point->estimatedMinutes;
            ++result.scheduledCount;
            triggerReasons.push_back("已纳入当前时间预算内的学习路径。");
        }
        else
        {
            status = "deferred";
            ++result.deferredCount;
            triggerReasons.push_back("受当前时间预算限制，建议放入下一轮学习。");
        }

        if (!mastered)
        {
            result.totalRequiredMinutes += point->estimatedMinutes;
        }

        LearningPathItem item;
        item.order = order++;
        item.point = *point;
        item.status = status;
        item.scheduledNow = scheduledNow;
        item.isTarget = isTarget;
        item.isPrerequisite = isPrerequisite;
        item.masteryScore = roundToTwoDecimals(masteryScore);
        item.reasonTrace = PathReasonTrace{
            point->code,
            isTarget,
            isPrerequisite,
            roundToTwoDecimals(masteryScore),
            masteryGap,
            relevanceScore,
            roundToTwoDecimals(importanceScore),
            timeCostPenalty,
            triggerReasons};

        result.path.push_back(item);
    }

    result.targetReachableWithinBudget = result.deferredCount == 0;
    return result;
}
}
