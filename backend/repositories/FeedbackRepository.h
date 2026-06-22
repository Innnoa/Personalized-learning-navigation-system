#pragma once

#include <optional>
#include <string>
#include <vector>

namespace repositories
{
struct FeedbackRecordWrite
{
    int learnerId = 0;
    int knowledgePointId = 0;
    std::string completionStatus;
    double selfRatedMastery = 0.0;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    std::string ruleApplied;
    std::string feedbackBatchId;
};

struct FeedbackBatchRecord
{
    int knowledgePointId = 0;
    std::string code;
    std::string name;
    int chapterNo = 0;
    std::string chapterName;
    std::string completionStatus;
    double selfRatedMastery = 0.0;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    std::string ruleApplied;
    std::string feedbackBatchId;
    std::string recordedAt;
};

struct LatestFeedbackBatchInfo
{
    std::string batchId;
    std::string recordedAt;
    int itemCount = 0;
};

class FeedbackRepository
{
  public:
    static void upsertLearnerMastery(int learnerId, int knowledgePointId,
                                     double masteryScore);
    static void insertFeedbackRecord(const FeedbackRecordWrite &record);
    static std::vector<std::string> listPracticedKnowledgePointCodesByLearnerId(
        int learnerId);
    static std::optional<LatestFeedbackBatchInfo> findLatestFeedbackBatchByLearnerId(
        int learnerId);
    static std::vector<FeedbackBatchRecord> listFeedbackRecordsByBatchId(
        int learnerId, const std::string &feedbackBatchId);
    static void rollbackFeedbackBatch(
        int learnerId, const std::string &feedbackBatchId,
        const std::vector<FeedbackBatchRecord> &records);
};
}
