#pragma once

#include <string>
#include <vector>

namespace repositories
{
struct DemoBaselineMasteryWrite
{
    int knowledgePointId = 0;
    double masteryScore = 0.0;
};

struct DemoBaselineFeedbackWrite
{
    int knowledgePointId = 0;
    std::string feedbackBatchId;
    std::string completionStatus;
    double selfRatedMastery = 0.0;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    std::string ruleApplied;
    std::string recordedAt;
};

class DemoResetRepository
{
  public:
    static void resetLearnerState(
        int learnerId,
        const std::vector<DemoBaselineMasteryWrite> &baselineMastery,
        const std::vector<DemoBaselineFeedbackWrite> &baselineFeedbackRecords);
};
}
