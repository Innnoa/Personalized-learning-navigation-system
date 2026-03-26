#pragma once

#include <json/json.h>

#include <string>
#include <unordered_map>

namespace services
{
struct DetailLearningProgressSnapshot
{
    std::unordered_map<std::string, double> graphMasteryByCode;
    std::unordered_map<std::string, double> persistedDetailMasteryByCode;
    std::unordered_map<std::string, double> aggregatedParentMasteryByCode;
};

class DetailLearningProgressService
{
  public:
    static DetailLearningProgressSnapshot buildProgressSnapshot(
        int learnerId, int courseId,
        const std::unordered_map<std::string, double> &mainMasteryByCode);
    static void persistScopeMasteryAndSyncParents(
        int learnerId, int courseId, const std::string &scopeCode,
        const std::unordered_map<std::string, double> &scopeMasteryByCode);
};
}
