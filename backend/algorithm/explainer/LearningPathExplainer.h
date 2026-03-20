#pragma once

#include "algorithm/planner/LearningPathPlanner.h"

#include <string>
#include <vector>

namespace algorithm::explainer
{
struct PathExplanation
{
    std::string summary;
    std::vector<std::string> bullets;
    std::vector<std::string> labels;
};

PathExplanation explainLearningPathItem(
    const planner::LearningPathItem &item);
}
