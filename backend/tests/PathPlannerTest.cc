#include <drogon/drogon_test.h>

#include "algorithm/graph/KnowledgeGraph.h"
#include "algorithm/planner/LearningPathPlanner.h"

DROGON_TEST(LearningPathPlannerOrdersPrerequisitesFirst)
{
    algorithm::graph::KnowledgeGraph graph;
    graph.courseCode = "demo";
    graph.courseName = "demo";
    graph.points = {
        {1, "a", "基础", 1, "第一章", "foundation", 1, 20, 10, true, ""},
        {2, "b", "进阶", 1, "第一章", "algorithm", 2, 30, 20, true, ""},
        {3, "c", "目标", 1, "第一章", "algorithm", 3, 40, 30, true, ""}};
    graph.dependencies = {
        {1, "a", "b", "prerequisite", ""},
        {2, "b", "c", "prerequisite", ""}};

    algorithm::planner::PlanningRequest request;
    request.targetCodes = {"c"};
    request.availableMinutes = 100;
    request.masteryByCode = {{"a", 0.9}, {"b", 0.2}, {"c", 0.0}};

    const auto result =
        algorithm::planner::generateLearningPath(graph, request);

    REQUIRE(result.path.size() == 3);
    CHECK(result.path[0].point.code == "a");
    CHECK(result.path[1].point.code == "b");
    CHECK(result.path[2].point.code == "c");
    CHECK(result.path[0].status == "mastered");
    CHECK(result.path[1].status == "scheduled");
    CHECK(result.path[2].status == "scheduled");
    CHECK(result.targetReachableWithinBudget == true);
}
