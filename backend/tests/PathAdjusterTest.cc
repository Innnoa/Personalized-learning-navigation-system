#include <drogon/drogon_test.h>

#include "algorithm/adjuster/LearningPathAdjuster.h"

DROGON_TEST(LearningPathAdjusterPromotesCompletedNode)
{
    algorithm::graph::KnowledgeGraph graph;
    graph.courseCode = "demo";
    graph.courseName = "demo";
    graph.points = {
        {1, "a", "基础", 1, "第一章", "foundation", 1, 20, 10, true, ""},
        {2, "b", "中间节点", 1, "第一章", "structure", 2, 30, 20, true, ""},
        {3, "c", "目标", 1, "第一章", "algorithm", 3, 40, 30, true, ""}};
    graph.dependencies = {
        {1, "a", "b", "prerequisite", ""},
        {2, "b", "c", "prerequisite", ""}};

    algorithm::planner::PlanningRequest request;
    request.targetCodes = {"c"};
    request.availableMinutes = 40;
    request.masteryByCode = {{"a", 0.9}, {"b", 0.2}, {"c", 0.0}};

    std::vector<algorithm::adjuster::FeedbackItem> feedbackItems = {
        {"b", "completed", 0.9}};

    const auto result = algorithm::adjuster::adjustLearningPath(
        graph, request, feedbackItems);

    REQUIRE(result.adjustmentDetails.size() == 1);
    CHECK(result.adjustmentDetails[0].updatedMastery >= 0.85);
    CHECK(result.updatedRequest.masteryByCode.at("b") >= 0.85);
    CHECK(result.adjustedPlan.path.size() == 3);
    CHECK(result.adjustedPlan.path[1].point.code == "b");
    CHECK(result.adjustedPlan.path[1].status == "mastered");
    CHECK(result.adjustedPlan.path[2].point.code == "c");
    CHECK(result.adjustedPlan.path[2].status == "scheduled");
}

DROGON_TEST(FeedbackUpdateRulesReturnUpdatedMasteryAndCounts)
{
    algorithm::graph::KnowledgeGraph graph;
    graph.courseCode = "demo";
    graph.courseName = "demo";
    graph.points = {
        {1, "a", "基础", 1, "第一章", "foundation", 1, 20, 10, true, ""},
        {2, "b", "中间节点", 1, "第一章", "structure", 2, 30, 20, true, ""},
        {3, "c", "目标", 1, "第一章", "algorithm", 3, 40, 30, true, ""}};
    graph.dependencies = {
        {1, "a", "b", "prerequisite", ""},
        {2, "b", "c", "prerequisite", ""}};

    const std::unordered_map<std::string, double> masteryByCode = {
        {"a", 0.9},
        {"b", 0.2},
        {"c", 0.1}};
    std::vector<algorithm::adjuster::FeedbackItem> feedbackItems = {
        {"b", "completed", 0.9},
        {"c", "partial", 0.45}};

    const auto result = algorithm::adjuster::applyFeedbackUpdates(
        graph, masteryByCode, feedbackItems);

    CHECK(result.completedCount == 1);
    CHECK(result.partialCount == 1);
    CHECK(result.blockedCount == 0);
    CHECK(result.updatedMasteryByCode.at("b") >= 0.85);
    CHECK(result.updatedMasteryByCode.at("c") > 0.1);
    CHECK(result.adjustmentDetails.size() == 2);
}

DROGON_TEST(FeedbackUpdateRulesClampSelfRatedMasteryByCompletionStatus)
{
    algorithm::graph::KnowledgeGraph graph;
    graph.courseCode = "demo";
    graph.courseName = "demo";
    graph.points = {
        {1, "a", "基础", 1, "第一章", "foundation", 1, 20, 10, true, ""},
        {2, "b", "中间节点", 1, "第一章", "structure", 2, 30, 20, true, ""},
        {3, "c", "目标", 1, "第一章", "algorithm", 3, 40, 30, true, ""}};
    graph.dependencies = {
        {1, "a", "b", "prerequisite", ""},
        {2, "b", "c", "prerequisite", ""}};

    const std::unordered_map<std::string, double> masteryByCode = {
        {"a", 0.2},
        {"b", 0.6},
        {"c", 0.5}};
    std::vector<algorithm::adjuster::FeedbackItem> feedbackItems = {
        {"a", "completed", 0.1},
        {"b", "partial", 0.95},
        {"c", "blocked", 0.9}};

    const auto result = algorithm::adjuster::applyFeedbackUpdates(
        graph, masteryByCode, feedbackItems);

    CHECK(result.updatedMasteryByCode.at("a") >= 0.85);
    CHECK(result.updatedMasteryByCode.at("b") == 0.8);
    CHECK(result.updatedMasteryByCode.at("c") == 0.35);
}

DROGON_TEST(FeedbackUpdateRulesAllowPartialToSlightlyLowerMastery)
{
    algorithm::graph::KnowledgeGraph graph;
    graph.courseCode = "demo";
    graph.courseName = "demo";
    graph.points = {
        {1, "a", "基础", 1, "第一章", "foundation", 1, 20, 10, true, ""},
        {2, "b", "中间节点", 1, "第一章", "structure", 2, 30, 20, true, ""},
        {3, "c", "目标", 1, "第一章", "algorithm", 3, 40, 30, true, ""}};
    graph.dependencies = {
        {1, "a", "b", "prerequisite", ""},
        {2, "b", "c", "prerequisite", ""}};

    const std::unordered_map<std::string, double> masteryByCode = {
        {"a", 0.6},
        {"b", 0.7},
        {"c", 0.5}};
    std::vector<algorithm::adjuster::FeedbackItem> feedbackItems = {
        {"a", "partial", 0.5},
        {"b", "partial", 0.42},
        {"c", "partial", 0.75}};

    const auto result = algorithm::adjuster::applyFeedbackUpdates(
        graph, masteryByCode, feedbackItems);

    CHECK(result.updatedMasteryByCode.at("a") == 0.5);
    CHECK(result.updatedMasteryByCode.at("b") == 0.55);
    CHECK(result.updatedMasteryByCode.at("c") == 0.75);
}
