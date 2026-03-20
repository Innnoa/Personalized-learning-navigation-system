#pragma once

#include "algorithm/planner/LearningPathPlanner.h"

#include <json/json.h>
#include <string>
#include <vector>

namespace services
{
class LearningResourceService
{
  public:
    static Json::Value buildResourcesForKnowledgePoint(
        const std::string &knowledgePointCode);
    static Json::Value buildResourcesForLearningPathItem(
        const algorithm::planner::LearningPathItem &item);
    static Json::Value buildResourceRecommendations(
        const std::vector<algorithm::planner::LearningPathItem> &pathItems);
};
}
