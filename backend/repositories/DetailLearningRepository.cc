#include "repositories/DetailLearningRepository.h"

#include <drogon/drogon.h>

#include <stdexcept>

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
}

namespace repositories
{
std::vector<LearnerDetailMasteryRecord>
DetailLearningRepository::listDetailMasteryByLearnerId(int learnerId)
{
    const auto result = getClient()->execSqlSync(
        "select scope_code, node_code, mastery_score, updated_at "
        "from learner_detail_mastery "
        "where learner_id = ? "
        "order by scope_code asc, node_code asc",
        learnerId);

    std::vector<LearnerDetailMasteryRecord> records;
    records.reserve(result.size());

    for (const auto &row : result)
    {
        records.push_back(LearnerDetailMasteryRecord{
            row["scope_code"].as<std::string>(),
            row["node_code"].as<std::string>(),
            row["mastery_score"].as<double>(),
            row["updated_at"].as<std::string>()});
    }

    return records;
}

std::vector<std::string>
DetailLearningRepository::listPracticedNodeCodesByLearnerAndScope(
    int learnerId, const std::string &scopeCode)
{
    const auto result = getClient()->execSqlSync(
        "select distinct node_code "
        "from detail_learning_feedback_records "
        "where learner_id = ? and scope_code = ? "
        "order by node_code asc",
        learnerId,
        scopeCode);

    std::vector<std::string> practicedNodeCodes;
    practicedNodeCodes.reserve(result.size());

    for (const auto &row : result)
    {
        practicedNodeCodes.push_back(row["node_code"].as<std::string>());
    }

    return practicedNodeCodes;
}

void DetailLearningRepository::upsertDetailMastery(int learnerId,
                                                   const std::string &scopeCode,
                                                   const std::string &nodeCode,
                                                   double masteryScore)
{
    getClient()->execSqlSync(
        "insert into learner_detail_mastery "
        "(learner_id, scope_code, node_code, mastery_score) "
        "values (?, ?, ?, ?) "
        "on conflict(learner_id, scope_code, node_code) do update set "
        "mastery_score = excluded.mastery_score, "
        "updated_at = CURRENT_TIMESTAMP",
        learnerId,
        scopeCode,
        nodeCode,
        masteryScore);
}

void DetailLearningRepository::insertDetailFeedbackRecord(
    const DetailFeedbackRecordWrite &record)
{
    getClient()->execSqlSync(
        "insert into detail_learning_feedback_records "
        "(learner_id, scope_code, node_code, feedback_batch_id, completion_status, "
        "self_rated_mastery, previous_mastery, updated_mastery, rule_applied) "
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?)",
        record.learnerId,
        record.scopeCode,
        record.nodeCode,
        record.feedbackBatchId,
        record.completionStatus,
        record.selfRatedMastery,
        record.previousMastery,
        record.updatedMastery,
        record.ruleApplied);
}
}
