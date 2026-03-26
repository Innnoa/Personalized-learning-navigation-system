#include "repositories/DemoResetRepository.h"

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
void DemoResetRepository::resetLearnerState(
    int learnerId,
    const std::vector<DemoBaselineMasteryWrite> &baselineMastery)
{
    auto transaction = getClient()->newTransaction();

    try
    {
        transaction->execSqlSync(
            "delete from learning_feedback_records where learner_id = ?",
            learnerId);
        transaction->execSqlSync(
            "delete from detail_learning_feedback_records where learner_id = ?",
            learnerId);
        transaction->execSqlSync(
            "delete from learning_resource_view_records where learner_id = ?",
            learnerId);
        transaction->execSqlSync(
            "delete from learner_detail_mastery where learner_id = ?",
            learnerId);
        transaction->execSqlSync(
            "delete from learner_mastery where learner_id = ?",
            learnerId);

        for (const auto &item : baselineMastery)
        {
            transaction->execSqlSync(
                "insert into learner_mastery "
                "(learner_id, knowledge_point_id, mastery_score) "
                "values (?, ?, ?)",
                learnerId,
                item.knowledgePointId,
                item.masteryScore);
        }
    }
    catch (...)
    {
        transaction->rollback();
        throw;
    }
}
}
