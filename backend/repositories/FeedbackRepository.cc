#include "repositories/FeedbackRepository.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>

#include <algorithm>
#include <cctype>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

namespace
{
auto getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 数据库连接。");
    }

    return client;
}

std::string sanitizeBatchSuffix(const std::string &value)
{
    std::string sanitized;
    sanitized.reserve(value.size());

    for (const auto ch : value)
    {
        if (std::isalnum(static_cast<unsigned char>(ch)) != 0)
        {
            sanitized.push_back(static_cast<char>(ch));
        }
    }

    return sanitized.empty() ? "legacy" : sanitized;
}

void ensureFeedbackRollbackSchema()
{
    static std::once_flag onceFlag;

    std::call_once(onceFlag, []() {
        auto client = getClient();
        const auto columns =
            client->execSqlSync("pragma table_info(learning_feedback_records)");

        bool hasFeedbackBatchId = false;
        for (const auto &row : columns)
        {
            if (row["name"].as<std::string>() == "feedback_batch_id")
            {
                hasFeedbackBatchId = true;
                break;
            }
        }

        if (!hasFeedbackBatchId)
        {
            client->execSqlSync(
                "alter table learning_feedback_records "
                "add column feedback_batch_id text not null default ''");
        }

        client->execSqlSync(
            "create index if not exists idx_learning_feedback_records_learner_batch "
            "on learning_feedback_records(learner_id, feedback_batch_id, recorded_at desc)");

        const auto legacyRows = client->execSqlSync(
            "select id, recorded_at "
            "from learning_feedback_records "
            "where coalesce(feedback_batch_id, '') = '' "
            "order by recorded_at asc, id asc");

        std::unordered_map<std::string, std::string> batchIdByRecordedAt;
        int legacyBatchCounter = 1;
        for (const auto &row : legacyRows)
        {
            const auto recordedAt = row["recorded_at"].as<std::string>();
            auto it = batchIdByRecordedAt.find(recordedAt);
            if (it == batchIdByRecordedAt.end())
            {
                const auto batchId =
                    "legacy-" + std::to_string(legacyBatchCounter++) + "-" +
                    sanitizeBatchSuffix(recordedAt);
                it = batchIdByRecordedAt.emplace(recordedAt, batchId).first;
            }

            client->execSqlSync(
                "update learning_feedback_records "
                "set feedback_batch_id = ? "
                "where id = ?",
                it->second,
                row["id"].as<int>());
        }
    });
}

template <typename DbClientLike>
void upsertLearnerMasteryWithClient(const DbClientLike &client,
                                    int learnerId,
                                    int knowledgePointId,
                                    double masteryScore)
{
    client->execSqlSync(
        "insert into learner_mastery "
        "(learner_id, knowledge_point_id, mastery_score) "
        "values (?, ?, ?) "
        "on conflict(learner_id, knowledge_point_id) do update set "
        "mastery_score = excluded.mastery_score, "
        "updated_at = CURRENT_TIMESTAMP",
        learnerId,
        knowledgePointId,
        masteryScore);
}
}

namespace repositories
{
void FeedbackRepository::upsertLearnerMastery(int learnerId, int knowledgePointId,
                                              double masteryScore)
{
    upsertLearnerMasteryWithClient(getClient(), learnerId, knowledgePointId,
                                   masteryScore);
}

void FeedbackRepository::insertFeedbackRecord(const FeedbackRecordWrite &record)
{
    ensureFeedbackRollbackSchema();

    getClient()->execSqlSync(
        "insert into learning_feedback_records "
        "(learner_id, knowledge_point_id, feedback_batch_id, completion_status, "
        "self_rated_mastery, previous_mastery, updated_mastery, rule_applied) "
        "values (?, ?, ?, ?, ?, ?, ?, ?)",
        record.learnerId, record.knowledgePointId, record.feedbackBatchId,
        record.completionStatus, record.selfRatedMastery, record.previousMastery,
        record.updatedMastery, record.ruleApplied);
}

std::vector<std::string>
FeedbackRepository::listPracticedKnowledgePointCodesByLearnerId(int learnerId)
{
    ensureFeedbackRollbackSchema();

    const auto result = getClient()->execSqlSync(
        "select distinct kp.code "
        "from learning_feedback_records r "
        "join knowledge_points kp on kp.id = r.knowledge_point_id "
        "where r.learner_id = ? "
        "order by kp.code asc",
        learnerId);

    std::vector<std::string> practicedCodes;
    practicedCodes.reserve(result.size());

    for (const auto &row : result)
    {
        practicedCodes.push_back(row["code"].as<std::string>());
    }

    return practicedCodes;
}

std::optional<LatestFeedbackBatchInfo>
FeedbackRepository::findLatestFeedbackBatchByLearnerId(int learnerId)
{
    ensureFeedbackRollbackSchema();

    const auto result = getClient()->execSqlSync(
        "select feedback_batch_id, max(recorded_at) as recorded_at, "
        "count(1) as item_count, max(id) as latest_id "
        "from learning_feedback_records "
        "where learner_id = ? and coalesce(feedback_batch_id, '') <> '' "
        "group by feedback_batch_id "
        "order by max(recorded_at) desc, max(id) desc "
        "limit 1",
        learnerId);

    if (result.empty())
    {
        return std::nullopt;
    }

    const auto &row = result.front();
    return LatestFeedbackBatchInfo{
        row["feedback_batch_id"].as<std::string>(),
        row["recorded_at"].as<std::string>(),
        row["item_count"].as<int>()};
}

std::vector<FeedbackBatchRecord> FeedbackRepository::listFeedbackRecordsByBatchId(
    int learnerId, const std::string &feedbackBatchId)
{
    ensureFeedbackRollbackSchema();

    const auto result = getClient()->execSqlSync(
        "select r.knowledge_point_id, kp.code, kp.name, kp.chapter_no, "
        "kp.chapter_name, r.completion_status, r.self_rated_mastery, "
        "r.previous_mastery, r.updated_mastery, r.rule_applied, "
        "r.feedback_batch_id, r.recorded_at "
        "from learning_feedback_records r "
        "join knowledge_points kp on kp.id = r.knowledge_point_id "
        "where r.learner_id = ? and r.feedback_batch_id = ? "
        "order by r.id desc",
        learnerId,
        feedbackBatchId);

    std::vector<FeedbackBatchRecord> records;
    records.reserve(result.size());

    for (const auto &row : result)
    {
        records.push_back(FeedbackBatchRecord{
            row["knowledge_point_id"].as<int>(),
            row["code"].as<std::string>(),
            row["name"].as<std::string>(),
            row["chapter_no"].as<int>(),
            row["chapter_name"].as<std::string>(),
            row["completion_status"].as<std::string>(),
            row["self_rated_mastery"].as<double>(),
            row["previous_mastery"].as<double>(),
            row["updated_mastery"].as<double>(),
            row["rule_applied"].as<std::string>(),
            row["feedback_batch_id"].as<std::string>(),
            row["recorded_at"].as<std::string>()});
    }

    return records;
}

void FeedbackRepository::rollbackFeedbackBatch(
    int learnerId, const std::string &feedbackBatchId,
    const std::vector<FeedbackBatchRecord> &records)
{
    ensureFeedbackRollbackSchema();

    auto transaction = getClient()->newTransaction();
    try
    {
        for (const auto &record : records)
        {
            upsertLearnerMasteryWithClient(
                transaction, learnerId, record.knowledgePointId,
                record.previousMastery);
        }

        transaction->execSqlSync(
            "delete from learning_feedback_records "
            "where learner_id = ? and feedback_batch_id = ?",
            learnerId,
            feedbackBatchId);
    }
    catch (...)
    {
        transaction->rollback();
        throw;
    }
}
}
