#include "repositories/ResourceViewRepository.h"

#include <drogon/drogon.h>

#include <unordered_set>
#include <stdexcept>
#include <string>

namespace
{
void ensureInteractionTypeColumn(
    const drogon::orm::DbClientPtr &client)
{
    const auto columns =
        client->execSqlSync("pragma table_info(learning_resource_view_records)");
    bool hasInteractionType = false;

    for (const auto &column : columns)
    {
        if (column["name"].as<std::string>() == "interaction_type")
        {
            hasInteractionType = true;
            break;
        }
    }

    if (!hasInteractionType)
    {
        client->execSqlSync(
            "alter table learning_resource_view_records "
            "add column interaction_type text not null default 'opened'");
    }

    client->execSqlSync(
        "update learning_resource_view_records "
        "set interaction_type = 'opened' "
        "where coalesce(interaction_type, '') = ''");
}

auto getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 数据库连接。");
    }

    client->execSqlSync(
        "create table if not exists learning_resource_view_records ("
        "id integer primary key autoincrement, "
        "learner_id integer not null, "
        "knowledge_point_id integer not null, "
        "resource_title text not null, "
        "resource_url text not null, "
        "resource_type text not null default '', "
        "resource_source text not null default '', "
        "resource_layer text not null default '', "
        "recommended_phase text not null default '', "
        "source_context text not null default '', "
        "scope_code text not null default '', "
        "linked_reason_summary text not null default '', "
        "interaction_type text not null default 'opened' "
        "check (interaction_type in ('opened', 'viewed', 'completed', 'save_for_later')), "
        "recorded_at text not null default CURRENT_TIMESTAMP, "
        "foreign key (learner_id) references learners(id) on delete cascade, "
        "foreign key (knowledge_point_id) references knowledge_points(id) on delete cascade)");
    ensureInteractionTypeColumn(client);
    client->execSqlSync(
        "create index if not exists idx_learning_resource_view_records_learner "
        "on learning_resource_view_records(learner_id, recorded_at desc)");
    client->execSqlSync(
        "create index if not exists idx_learning_resource_view_records_learner_point "
        "on learning_resource_view_records(learner_id, knowledge_point_id, recorded_at desc)");

    return client;
}
}

namespace repositories
{
void ResourceViewRepository::insertResourceViewRecord(
    const ResourceViewRecordWrite &record)
{
    getClient()->execSqlSync(
        "insert into learning_resource_view_records ("
        "learner_id, knowledge_point_id, resource_title, resource_url, "
        "resource_type, resource_source, resource_layer, recommended_phase, "
        "source_context, scope_code, linked_reason_summary, interaction_type) "
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
        record.learnerId,
        record.knowledgePointId,
        record.resourceTitle,
        record.resourceUrl,
        record.resourceType,
        record.resourceSource,
        record.resourceLayer,
        record.recommendedPhase,
        record.sourceContext,
        record.scopeCode,
        record.linkedReasonSummary,
        record.interactionType);
}

int ResourceViewRepository::countResourceViewRecordsByLearnerId(int learnerId)
{
    const auto result = getClient()->execSqlSync(
        "select count(1) as total "
        "from learning_resource_view_records "
        "where learner_id = ?",
        learnerId);

    if (result.empty())
    {
        return 0;
    }

    return result.front()["total"].as<int>();
}

std::vector<ResourceViewRecord>
ResourceViewRepository::listRecentResourceViewRecordsByLearnerId(
    int learnerId, std::size_t limit)
{
    const auto result = getClient()->execSqlSync(
        "select r.knowledge_point_id, kp.code, kp.name, kp.chapter_no, "
        "kp.chapter_name, r.resource_title, r.resource_url, r.resource_type, "
        "r.resource_source, r.resource_layer, r.recommended_phase, "
        "r.source_context, r.scope_code, r.linked_reason_summary, "
        "r.interaction_type, r.recorded_at "
        "from learning_resource_view_records r "
        "join knowledge_points kp on kp.id = r.knowledge_point_id "
        "where r.learner_id = ? "
        "order by r.recorded_at desc, r.id desc "
        "limit ?",
        learnerId,
        static_cast<long long>(limit));

    std::vector<ResourceViewRecord> records;
    records.reserve(result.size());

    for (const auto &row : result)
    {
        records.push_back(ResourceViewRecord{
            row["knowledge_point_id"].as<int>(),
            row["code"].as<std::string>(),
            row["name"].as<std::string>(),
            row["chapter_no"].as<int>(),
            row["chapter_name"].as<std::string>(),
            row["resource_title"].as<std::string>(),
            row["resource_url"].as<std::string>(),
            row["resource_type"].as<std::string>(),
            row["resource_source"].as<std::string>(),
            row["resource_layer"].as<std::string>(),
            row["recommended_phase"].as<std::string>(),
            row["source_context"].as<std::string>(),
            row["scope_code"].as<std::string>(),
            row["linked_reason_summary"].as<std::string>(),
            row["interaction_type"].as<std::string>(),
            row["recorded_at"].as<std::string>()});
    }

    return records;
}

std::vector<ResourceLatestInteractionRecord>
ResourceViewRepository::listLatestResourceInteractionsByLearnerAndKnowledgePointId(
    int learnerId, int knowledgePointId)
{
    const auto result = getClient()->execSqlSync(
        "select resource_url, interaction_type "
        "from learning_resource_view_records "
        "where learner_id = ? and knowledge_point_id = ? "
        "order by recorded_at desc, id desc",
        learnerId,
        knowledgePointId);

    std::unordered_set<std::string> visitedUrls;
    std::vector<ResourceLatestInteractionRecord> records;
    records.reserve(result.size());

    for (const auto &row : result)
    {
        const auto resourceUrl = row["resource_url"].as<std::string>();
        if (resourceUrl.empty() ||
            visitedUrls.find(resourceUrl) != visitedUrls.end())
        {
            continue;
        }

        visitedUrls.insert(resourceUrl);
        records.push_back(ResourceLatestInteractionRecord{
            resourceUrl,
            row["interaction_type"].as<std::string>()});
    }

    return records;
}
}
