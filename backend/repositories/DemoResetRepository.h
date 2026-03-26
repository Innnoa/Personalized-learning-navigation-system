#pragma once

#include <vector>

namespace repositories
{
struct DemoBaselineMasteryWrite
{
    int knowledgePointId = 0;
    double masteryScore = 0.0;
};

class DemoResetRepository
{
  public:
    static void resetLearnerState(
        int learnerId,
        const std::vector<DemoBaselineMasteryWrite> &baselineMastery);
};
}
