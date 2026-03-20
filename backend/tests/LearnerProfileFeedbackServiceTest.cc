#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/FeedbackService.h"
#include "services/LearnerProfileService.h"

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
    CHECK(payload["summary"]["averageMasteryPercent"].asInt() == 41);
    CHECK(std::abs(payload["masteryByCode"]["topological-sort"].asDouble() - 0.0) <
          1e-9);
    CHECK(payload["masteryItems"].size() == 14U);
    CHECK(payload["recentFeedbackItems"].size() == 0U);
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
