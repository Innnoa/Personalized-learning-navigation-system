#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/DemoResetService.h"
#include "services/FeedbackService.h"
#include "services/LearnerProfileService.h"
#include "services/ResourceViewService.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
drogon::orm::DbClientPtr getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 测试数据库连接。");
    }

    return client;
}

int findLearnerId(const std::string &learnerCode)
{
    const auto result = getClient()->execSqlSync(
        "select id from learners where code = ? limit 1",
        learnerCode);
    if (result.empty())
    {
        throw std::runtime_error("测试学习者不存在: " + learnerCode);
    }

    return result.front()["id"].as<int>();
}

void createIsolatedLearnerFromDemo(const std::string &learnerCode,
                                   const std::string &learnerName)
{
    const auto existing = getClient()->execSqlSync(
        "select id from learners where code = ? limit 1",
        learnerCode);
    if (!existing.empty())
    {
        return;
    }

    getClient()->execSqlSync(
        "insert into learners (code, name, major, grade_label, target_course_id, note) "
        "select ?, ?, major, grade_label, target_course_id, ? "
        "from learners where code = 'demo-learner' limit 1",
        learnerCode,
        learnerName,
        "后端测试专用学习者，用于验证反馈持久化链路。");

    const int learnerId = findLearnerId(learnerCode);
    getClient()->execSqlSync(
        "delete from detail_learning_feedback_records where learner_id = ?",
        learnerId);
    getClient()->execSqlSync(
        "delete from learner_detail_mastery where learner_id = ?",
        learnerId);
    getClient()->execSqlSync(
        "delete from learning_resource_view_records where learner_id = ?",
        learnerId);
    getClient()->execSqlSync(
        "delete from learning_feedback_records where learner_id = ?",
        learnerId);
    getClient()->execSqlSync(
        "delete from learner_mastery where learner_id = ?",
        learnerId);
    getClient()->execSqlSync(
        "insert into learner_mastery (learner_id, knowledge_point_id, mastery_score) "
        "select ?, knowledge_point_id, mastery_score "
        "from learner_mastery "
        "where learner_id = (select id from learners where code = 'demo-learner')",
        learnerId);
}

double queryMasteryScore(const std::string &learnerCode,
                         const std::string &knowledgePointCode)
{
    const auto result = getClient()->execSqlSync(
        "select lm.mastery_score "
        "from learner_mastery lm "
        "join learners l on l.id = lm.learner_id "
        "join knowledge_points kp on kp.id = lm.knowledge_point_id "
        "where l.code = ? and kp.code = ? limit 1",
        learnerCode,
        knowledgePointCode);
    if (result.empty())
    {
        throw std::runtime_error("未找到掌握度记录。");
    }

    return result.front()["mastery_score"].as<double>();
}

int queryFeedbackRecordCount(const std::string &learnerCode)
{
    const auto result = getClient()->execSqlSync(
        "select count(1) as total "
        "from learning_feedback_records r "
        "join learners l on l.id = r.learner_id "
        "where l.code = ?",
        learnerCode);
    return result.front()["total"].as<int>();
}

int queryResourceViewRecordCount(const std::string &learnerCode)
{
    const auto result = getClient()->execSqlSync(
        "select count(1) as total "
        "from learning_resource_view_records r "
        "join learners l on l.id = r.learner_id "
        "where l.code = ?",
        learnerCode);
    return result.front()["total"].as<int>();
}
}

DROGON_TEST(LearnerProfileServiceBuildsDefaultLearnerPayload)
{
    const auto payload = services::LearnerProfileService::buildProfilePayload();

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["learner"]["code"].asString() == "demo-learner");
    CHECK(payload["course"]["code"].asString() == "data-structures");
    CHECK(payload["summary"]["knowledgePointCount"].asInt() == 14);
    CHECK(payload["summary"]["masteredCount"].asInt() == 1);
    CHECK(payload["summary"]["inProgressCount"].asInt() == 12);
    CHECK(payload["summary"]["notStartedCount"].asInt() == 1);
    CHECK(payload["summary"]["feedbackRecordCount"].asInt() == 0);
    CHECK(payload["summary"]["resourceViewRecordCount"].asInt() == 0);
    CHECK(payload["summary"]["averageMasteryPercent"].asInt() == 41);
    CHECK(std::abs(payload["masteryByCode"]["topological-sort"].asDouble() - 0.0) <
          1e-9);
    CHECK(payload["masteryItems"].size() == 14U);
    CHECK(payload["recentFeedbackItems"].size() == 0U);
    CHECK(payload["recentResourceViewItems"].size() == 0U);
}

DROGON_TEST(LearnerProfileServiceBuildsGraphMasteryForDetailNodes)
{
    const auto payload = services::LearnerProfileService::buildProfilePayload();

    CHECK(payload["graphMasteryByCode"].isObject());
    CHECK(payload["detailMasteryByCode"].isObject());
    CHECK(payload["detailMasteryByCode"].empty() == true);
    CHECK(std::abs(payload["graphMasteryByCode"]["queue-definition"].asDouble() -
                   payload["masteryByCode"]["queue"].asDouble()) < 1e-9);
    CHECK(std::abs(payload["graphMasteryByCode"]["queue-circular"].asDouble() -
                   payload["masteryByCode"]["queue"].asDouble()) < 1e-9);
}

DROGON_TEST(ResourceViewServicePersistsRecentViewedResources)
{
    const std::string learnerCode = "resource-view-test-learner";
    createIsolatedLearnerFromDemo(learnerCode, "资源行为测试学习者");

    Json::Value firstRequest(Json::objectValue);
    firstRequest["learnerCode"] = learnerCode;
    firstRequest["knowledgePointCode"] = "queue";
    firstRequest["resourceTitle"] = "Hello 算法 · 队列";
    firstRequest["resourceUrl"] =
        "https://www.hello-algo.com/chapter_stack_and_queue/queue/";
    firstRequest["resourceType"] = "article";
    firstRequest["resourceSource"] = "Hello 算法";
    firstRequest["resourceLayer"] = "图文补充";
    firstRequest["recommendedPhase"] = "巩固";
    firstRequest["sourceContext"] = "main_path_resource_panel";
    firstRequest["scopeCode"] = "root";
    firstRequest["linkedReasonLabelSummary"] =
        "对应推荐理由：本轮学习 / 掌握度待提升";

    const auto firstPayload =
        services::ResourceViewService::recordResourceView(firstRequest);

    CHECK(firstPayload["status"].asString() == "ok");
    CHECK(firstPayload["summary"]["resourceViewRecordCount"].asInt() == 1);
    CHECK(firstPayload["savedResourceViewCount"].asInt() == 1);
    CHECK(firstPayload["recordedResource"]["knowledgePointCode"].asString() ==
          "queue");
    CHECK(firstPayload["recordedResource"]["interactionType"].asString() ==
          "opened");
    CHECK(firstPayload["recordedResource"]["interactionTypeLabel"].asString() ==
          "已打开");
    REQUIRE(firstPayload["recentResourceViewItems"].size() == 1U);
    CHECK(firstPayload["recentResourceViewItems"][0]["resourceTitle"].asString() ==
          "Hello 算法 · 队列");
    CHECK(firstPayload["recentResourceViewItems"][0]["sourceContextLabel"]
              .asString() == "首页资源建议区");
    CHECK(firstPayload["recentResourceViewItems"][0]["interactionType"].asString() ==
          "opened");

    Json::Value secondRequest(Json::objectValue);
    secondRequest["learnerCode"] = learnerCode;
    secondRequest["knowledgePointCode"] = "topological-sort";
    secondRequest["resourceTitle"] = "拓扑排序与关键路径";
    secondRequest["resourceUrl"] = "https://example.com/topological-sort";
    secondRequest["resourceType"] = "video";
    secondRequest["resourceSource"] = "课程演示资源";
    secondRequest["resourceLayer"] = "课程风格优先";
    secondRequest["recommendedPhase"] = "主学";
    secondRequest["sourceContext"] = "detail_scope_resource_panel";
    secondRequest["scopeCode"] = "topological-sort";
    secondRequest["linkedReasonLabelSummary"] =
        "对应推荐理由：目标节点 / 高目标相关";
    secondRequest["interactionType"] = "completed";

    const auto secondPayload =
        services::ResourceViewService::recordResourceView(secondRequest);

    CHECK(secondPayload["summary"]["resourceViewRecordCount"].asInt() == 2);
    REQUIRE(secondPayload["recentResourceViewItems"].size() == 2U);
    CHECK(secondPayload["recentResourceViewItems"][0]["resourceTitle"].asString() ==
          "拓扑排序与关键路径");
    CHECK(secondPayload["recentResourceViewItems"][0]["sourceContextLabel"]
              .asString() == "细化图谱资源区（topological-sort）");
    CHECK(secondPayload["recentResourceViewItems"][0]["resourceLayer"].asString() ==
          "课程风格优先");
    CHECK(secondPayload["recentResourceViewItems"][0]["interactionType"].asString() ==
          "completed");
    CHECK(secondPayload["recentResourceViewItems"][0]["interactionTypeLabel"]
              .asString() == "已学完");
    CHECK(secondPayload["recentResourceViewItems"][1]["resourceTitle"].asString() ==
          "Hello 算法 · 队列");

    Json::Value thirdRequest(Json::objectValue);
    thirdRequest["learnerCode"] = learnerCode;
    thirdRequest["knowledgePointCode"] = "string";
    thirdRequest["resourceTitle"] = "数据结构课程串章节速查";
    thirdRequest["resourceUrl"] = "https://example.com/string-review";
    thirdRequest["resourceType"] = "document";
    thirdRequest["resourceSource"] = "课程演示讲义";
    thirdRequest["resourceLayer"] = "答辩复盘";
    thirdRequest["recommendedPhase"] = "答辩复盘";
    thirdRequest["sourceContext"] = "resource_recommendation_page";
    thirdRequest["scopeCode"] = "root";
    thirdRequest["linkedReasonLabelSummary"] =
        "对应推荐理由：下一轮建议 / 稍后补充";
    thirdRequest["interactionType"] = "save_for_later";

    const auto thirdPayload =
        services::ResourceViewService::recordResourceView(thirdRequest);

    CHECK(thirdPayload["summary"]["resourceViewRecordCount"].asInt() == 3);
    REQUIRE(thirdPayload["recentResourceViewItems"].size() == 3U);
    CHECK(thirdPayload["recentResourceViewItems"][0]["interactionType"].asString() ==
          "save_for_later");
    CHECK(thirdPayload["recentResourceViewItems"][0]["sourceContextLabel"]
              .asString() == "单节点资源页");
    CHECK(thirdPayload["recentResourceViewItems"][0]["interactionTypeLabel"]
              .asString() == "稍后再看");
    CHECK(queryResourceViewRecordCount(learnerCode) == 3);
}

DROGON_TEST(FeedbackServicePersistsFeedbackAndUpdatesLearnerProfile)
{
    const std::string learnerCode = "feedback-test-learner";
    createIsolatedLearnerFromDemo(learnerCode, "反馈测试学习者");

    Json::Value request(Json::objectValue);
    request["learnerCode"] = learnerCode;

    Json::Value feedbackItems(Json::arrayValue);

    Json::Value completedItem(Json::objectValue);
    completedItem["code"] = "queue";
    completedItem["completionStatus"] = "completed";
    completedItem["selfRatedMastery"] = 0.95;
    feedbackItems.append(completedItem);

    Json::Value blockedItem(Json::objectValue);
    blockedItem["code"] = "graph-basic";
    blockedItem["completionStatus"] = "blocked";
    blockedItem["selfRatedMastery"] = 0.05;
    feedbackItems.append(blockedItem);

    request["feedbackItems"] = feedbackItems;

    const auto payload = services::FeedbackService::submitFeedback(request);

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["learner"]["code"].asString() == learnerCode);
    CHECK(payload["savedRecordCount"].asInt() == 2);
    CHECK(payload["feedbackBatchId"].asString().empty() == false);
    CHECK(payload["summary"]["feedbackRecordCount"].asInt() == 2);
    CHECK(payload["feedbackSummary"]["feedbackItemCount"].asInt() == 2);
    CHECK(payload["feedbackSummary"]["completedCount"].asInt() == 1);
    CHECK(payload["feedbackSummary"]["partialCount"].asInt() == 0);
    CHECK(payload["feedbackSummary"]["blockedCount"].asInt() == 1);
    CHECK(std::abs(payload["masteryByCode"]["queue"].asDouble() - 0.95) < 1e-9);
    CHECK(std::abs(payload["masteryByCode"]["graph-basic"].asDouble() - 0.05) <
          1e-9);
    REQUIRE(payload["adjustments"].size() == 2U);
    REQUIRE(payload["recentFeedbackItems"].size() == 2U);
    CHECK(payload["adjustments"][0]["ruleApplied"].asString() ==
          "completed_promote_to_mastered");
    CHECK(payload["adjustments"][1]["ruleApplied"].asString() ==
          "blocked_lower_priority_chain");
    CHECK(payload["recentFeedbackItems"][0]["code"].asString() == "graph-basic");
    CHECK(payload["recentFeedbackItems"][0]["updatedMasteryPercent"].asInt() == 5);
    CHECK(payload["recentFeedbackItems"][1]["code"].asString() == "queue");
    CHECK(payload["recentFeedbackItems"][1]["updatedMasteryPercent"].asInt() == 95);

    CHECK(std::abs(queryMasteryScore(learnerCode, "queue") - 0.95) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "graph-basic") - 0.05) <
          1e-9);
    CHECK(queryFeedbackRecordCount(learnerCode) == 2);
}

DROGON_TEST(FeedbackServiceRollsBackLatestFeedbackBatchStepByStep)
{
    const std::string learnerCode = "feedback-rollback-learner";
    createIsolatedLearnerFromDemo(learnerCode, "反馈回退测试学习者");

    const double originalQueueMastery =
        queryMasteryScore(learnerCode, "queue");
    const double originalGraphMastery =
        queryMasteryScore(learnerCode, "graph-basic");
    const double originalTopoMastery =
        queryMasteryScore(learnerCode, "topological-sort");

    Json::Value firstRequest(Json::objectValue);
    firstRequest["learnerCode"] = learnerCode;
    Json::Value firstFeedbackItems(Json::arrayValue);

    Json::Value queueItem(Json::objectValue);
    queueItem["code"] = "queue";
    queueItem["completionStatus"] = "completed";
    queueItem["selfRatedMastery"] = 0.9;
    firstFeedbackItems.append(queueItem);

    Json::Value graphItem(Json::objectValue);
    graphItem["code"] = "graph-basic";
    graphItem["completionStatus"] = "blocked";
    graphItem["selfRatedMastery"] = 0.05;
    firstFeedbackItems.append(graphItem);

    firstRequest["feedbackItems"] = firstFeedbackItems;
    const auto firstPayload = services::FeedbackService::submitFeedback(
        firstRequest);

    CHECK(firstPayload["savedRecordCount"].asInt() == 2);
    CHECK(queryFeedbackRecordCount(learnerCode) == 2);

    Json::Value secondRequest(Json::objectValue);
    secondRequest["learnerCode"] = learnerCode;
    Json::Value secondFeedbackItems(Json::arrayValue);

    Json::Value topoItem(Json::objectValue);
    topoItem["code"] = "topological-sort";
    topoItem["completionStatus"] = "partial";
    topoItem["selfRatedMastery"] = 0.45;
    secondFeedbackItems.append(topoItem);

    secondRequest["feedbackItems"] = secondFeedbackItems;
    const auto secondPayload = services::FeedbackService::submitFeedback(
        secondRequest);

    CHECK(secondPayload["savedRecordCount"].asInt() == 1);
    CHECK(queryFeedbackRecordCount(learnerCode) == 3);
    CHECK(std::abs(queryMasteryScore(learnerCode, "topological-sort") - 0.45) <
          1e-9);

    Json::Value rollbackRequest(Json::objectValue);
    rollbackRequest["learnerCode"] = learnerCode;

    const auto firstRollbackPayload =
        services::FeedbackService::rollbackLatestFeedback(rollbackRequest);

    CHECK(firstRollbackPayload["status"].asString() == "ok");
    CHECK(firstRollbackPayload["rolledBackRecordCount"].asInt() == 1);
    CHECK(firstRollbackPayload["summary"]["feedbackRecordCount"].asInt() == 2);
    CHECK(firstRollbackPayload["rollbackSummary"]["feedbackItemCount"].asInt() ==
          1);
    REQUIRE(firstRollbackPayload["rollbackItems"].size() == 1U);
    CHECK(firstRollbackPayload["rollbackItems"][0]["code"].asString() ==
          "topological-sort");
    CHECK(std::abs(queryMasteryScore(learnerCode, "topological-sort") -
                   originalTopoMastery) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "queue") - 0.9) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "graph-basic") - 0.05) <
          1e-9);
    CHECK(queryFeedbackRecordCount(learnerCode) == 2);

    const auto secondRollbackPayload =
        services::FeedbackService::rollbackLatestFeedback(rollbackRequest);

    CHECK(secondRollbackPayload["status"].asString() == "ok");
    CHECK(secondRollbackPayload["rolledBackRecordCount"].asInt() == 2);
    CHECK(secondRollbackPayload["summary"]["feedbackRecordCount"].asInt() == 0);
    CHECK(secondRollbackPayload["rollbackSummary"]["feedbackItemCount"].asInt() ==
          2);
    REQUIRE(secondRollbackPayload["rollbackItems"].size() == 2U);

    bool foundQueueRollback = false;
    bool foundGraphRollback = false;
    for (const auto &item : secondRollbackPayload["rollbackItems"])
    {
      if (item["code"].asString() == "queue")
      {
          foundQueueRollback = true;
      }
      if (item["code"].asString() == "graph-basic")
      {
          foundGraphRollback = true;
      }
    }

    CHECK(foundQueueRollback == true);
    CHECK(foundGraphRollback == true);
    CHECK(std::abs(queryMasteryScore(learnerCode, "queue") -
                   originalQueueMastery) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "graph-basic") -
                   originalGraphMastery) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "topological-sort") -
                   originalTopoMastery) < 1e-9);
    CHECK(queryFeedbackRecordCount(learnerCode) == 0);
}

DROGON_TEST(DemoResetServiceRestoresLearnerToInitialBaseline)
{
    const std::string learnerCode = "demo-reset-test-learner";
    createIsolatedLearnerFromDemo(learnerCode, "演示重置测试学习者");

    Json::Value feedbackRequest(Json::objectValue);
    feedbackRequest["learnerCode"] = learnerCode;

    Json::Value feedbackItems(Json::arrayValue);

    Json::Value queueItem(Json::objectValue);
    queueItem["code"] = "queue";
    queueItem["completionStatus"] = "completed";
    queueItem["selfRatedMastery"] = 0.90;
    feedbackItems.append(queueItem);

    Json::Value graphItem(Json::objectValue);
    graphItem["code"] = "graph-basic";
    graphItem["completionStatus"] = "partial";
    graphItem["selfRatedMastery"] = 0.55;
    feedbackItems.append(graphItem);

    feedbackRequest["feedbackItems"] = feedbackItems;

    const auto feedbackPayload =
        services::FeedbackService::submitFeedback(feedbackRequest);

    CHECK(feedbackPayload["savedRecordCount"].asInt() == 2);
    CHECK(queryFeedbackRecordCount(learnerCode) == 2);

    Json::Value resourceRequest(Json::objectValue);
    resourceRequest["learnerCode"] = learnerCode;
    resourceRequest["knowledgePointCode"] = "queue";
    resourceRequest["resourceTitle"] = "队列演示资源";
    resourceRequest["resourceUrl"] = "https://example.com/demo-reset-queue";
    resourceRequest["resourceType"] = "article";
    resourceRequest["resourceSource"] = "演示资源";
    resourceRequest["resourceLayer"] = "图文补充";
    resourceRequest["recommendedPhase"] = "巩固";
    resourceRequest["sourceContext"] = "resource_recommendation_page";
    resourceRequest["scopeCode"] = "root";
    resourceRequest["linkedReasonLabelSummary"] =
        "对应推荐理由：本轮学习 / 掌握度待提升";

    const auto resourcePayload =
        services::ResourceViewService::recordResourceView(resourceRequest);

    CHECK(resourcePayload["savedResourceViewCount"].asInt() == 1);
    CHECK(queryResourceViewRecordCount(learnerCode) == 1);

    Json::Value resetRequest(Json::objectValue);
    resetRequest["learnerCode"] = learnerCode;

    const auto resetPayload =
        services::DemoResetService::resetDemoState(resetRequest);

    CHECK(resetPayload["status"].asString() == "ok");
    CHECK(resetPayload["learner"]["code"].asString() == learnerCode);
    CHECK(resetPayload["summary"]["feedbackRecordCount"].asInt() == 0);
    CHECK(resetPayload["summary"]["resourceViewRecordCount"].asInt() == 0);
    CHECK(resetPayload["resetSummary"]["clearedFeedbackRecordCount"].asInt() == 2);
    CHECK(resetPayload["resetSummary"]["clearedResourceViewRecordCount"].asInt() == 1);
    CHECK(resetPayload["resetSummary"]["restoredMasteryPointCount"].asInt() == 14);
    CHECK(resetPayload["recentFeedbackItems"].size() == 0U);
    CHECK(resetPayload["recentResourceViewItems"].size() == 0U);
    CHECK(std::abs(queryMasteryScore(learnerCode, "queue") - 0.35) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "graph-basic") - 0.15) < 1e-9);
    CHECK(std::abs(queryMasteryScore(learnerCode, "topological-sort") - 0.0) < 1e-9);
    CHECK(queryFeedbackRecordCount(learnerCode) == 0);
    CHECK(queryResourceViewRecordCount(learnerCode) == 0);
}
