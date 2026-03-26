#include "services/DetailLearningProgressService.h"

#include "repositories/DetailLearningRepository.h"
#include "repositories/FeedbackRepository.h"
#include "repositories/KnowledgeGraphRepository.h"
#include "repositories/LearnerProfileRepository.h"
#include "services/DetailScopeCatalogService.h"

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
    return std::round(clampMastery(value) * 100.0) / 100.0;
}

double buildNodeWeight(const services::DetailScopeNode &node)
{
    const double estimatedMinutes =
        static_cast<double>(std::max(1, node.estimatedMinutes));
    const double difficultyFactor =
        0.7 + 0.3 * static_cast<double>(std::clamp(node.difficultyLevel, 1, 5));

    return estimatedMinutes * difficultyFactor;
}
}

namespace services
{
DetailLearningProgressSnapshot DetailLearningProgressService::buildProgressSnapshot(
    int learnerId, int courseId,
    const std::unordered_map<std::string, double> &mainMasteryByCode)
{
    const auto detailRecords =
        repositories::DetailLearningRepository::listDetailMasteryByLearnerId(
            learnerId);
    const auto &catalog = DetailScopeCatalogService::getCatalog();

    DetailLearningProgressSnapshot snapshot;
    snapshot.graphMasteryByCode = mainMasteryByCode;

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
        persistedScopeMasteryByCode;
    for (const auto &record : detailRecords)
    {
        const auto masteryScore = roundToTwoDecimals(record.masteryScore);
        persistedScopeMasteryByCode[record.scopeCode][record.nodeCode] =
            masteryScore;
        snapshot.persistedDetailMasteryByCode[record.nodeCode] = masteryScore;
        snapshot.graphMasteryByCode[record.nodeCode] = masteryScore;
    }

    for (const auto &[scopeCode, scope] : catalog.scopesByCode)
    {
        if (scope.parentScopeCode != "root" || scope.parentNodeCode.empty())
        {
            continue;
        }

        const double parentMastery = mainMasteryByCode.count(scope.parentNodeCode) > 0
                                         ? roundToTwoDecimals(
                                               mainMasteryByCode.at(scope.parentNodeCode))
                                         : 0.0;
        double weightedMasterySum = 0.0;
        double totalWeight = 0.0;

        for (const auto &node : scope.nodes)
        {
            const double resolvedMastery =
                persistedScopeMasteryByCode[scopeCode].count(node.code) > 0
                    ? persistedScopeMasteryByCode[scopeCode].at(node.code)
                    : parentMastery;
            const double nodeWeight = buildNodeWeight(node);
            weightedMasterySum += resolvedMastery * nodeWeight;
            totalWeight += nodeWeight;
            snapshot.graphMasteryByCode[node.code] = resolvedMastery;
        }

        if (totalWeight <= 0.0)
        {
            continue;
        }

        snapshot.aggregatedParentMasteryByCode[scope.parentNodeCode] =
            roundToTwoDecimals(weightedMasterySum / totalWeight);
    }

    return snapshot;
}

void DetailLearningProgressService::persistScopeMasteryAndSyncParents(
    int learnerId, int courseId, const std::string &scopeCode,
    const std::unordered_map<std::string, double> &scopeMasteryByCode)
{
    const auto &catalog = DetailScopeCatalogService::getCatalog();
    const auto scopeIt = catalog.scopesByCode.find(scopeCode);
    if (scopeIt == catalog.scopesByCode.end())
    {
        throw std::invalid_argument("未找到指定的细化图谱范围。");
    }

    std::unordered_set<std::string> allowedCodes;
    allowedCodes.reserve(scopeIt->second.nodes.size());
    for (const auto &node : scopeIt->second.nodes)
    {
        allowedCodes.insert(node.code);
    }

    for (const auto &[nodeCode, masteryScore] : scopeMasteryByCode)
    {
        if (allowedCodes.count(nodeCode) == 0)
        {
            continue;
        }

        repositories::DetailLearningRepository::upsertDetailMastery(
            learnerId, scopeCode, nodeCode, roundToTwoDecimals(masteryScore));
    }

    std::unordered_map<std::string, int> pointIdByCode;
    std::unordered_map<std::string, double> mainMasteryByCode;
    const auto masteryRecords =
        repositories::LearnerProfileRepository::listMasteryByLearnerAndCourseId(
            learnerId, courseId);
    mainMasteryByCode.reserve(masteryRecords.size());
    for (const auto &record : masteryRecords)
    {
        mainMasteryByCode[record.code] = roundToTwoDecimals(record.masteryScore);
    }

    const auto pointRecords =
        repositories::KnowledgeGraphRepository::listKnowledgePointsByCourseId(
            courseId);
    pointIdByCode.reserve(pointRecords.size());
    for (const auto &point : pointRecords)
    {
        pointIdByCode[point.code] = point.id;
    }

    const auto snapshot =
        buildProgressSnapshot(learnerId, courseId, mainMasteryByCode);
    for (const auto &[parentCode, masteryScore] :
         snapshot.aggregatedParentMasteryByCode)
    {
        if (pointIdByCode.count(parentCode) == 0)
        {
            continue;
        }

        repositories::FeedbackRepository::upsertLearnerMastery(
            learnerId, pointIdByCode[parentCode], masteryScore);
    }
}
}
