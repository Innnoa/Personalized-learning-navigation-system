#pragma once

#include <string>
#include <vector>

namespace repositories
{
struct LearnerDetailMasteryRecord
{
    std::string scopeCode;
    std::string nodeCode;
    double masteryScore = 0.0;
    std::string updatedAt;
};

struct DetailFeedbackRecordWrite
{
    int learnerId = 0;
    std::string scopeCode;
    std::string nodeCode;
    std::string completionStatus;
    double selfRatedMastery = 0.0;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    std::string ruleApplied;
    std::string feedbackBatchId;
};

class DetailLearningRepository
{
  public:
    static std::vector<LearnerDetailMasteryRecord> listDetailMasteryByLearnerId(
        int learnerId);
    static std::vector<std::string> listPracticedNodeCodesByLearnerAndScope(
        int learnerId, const std::string &scopeCode);
    static void upsertDetailMastery(int learnerId,
                                    const std::string &scopeCode,
                                    const std::string &nodeCode,
                                    double masteryScore);
    static void insertDetailFeedbackRecord(
        const DetailFeedbackRecordWrite &record);
};
}
