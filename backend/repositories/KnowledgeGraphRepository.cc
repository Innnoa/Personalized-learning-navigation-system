#include "repositories/KnowledgeGraphRepository.h"

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
std::optional<CourseRecord> KnowledgeGraphRepository::findCourseByName(
    const std::string &courseName)
{
    const auto result = getClient()->execSqlSync(
        "select id, code, name, description, target_audience "
        "from courses where name = ? limit 1",
        courseName);

    if (result.empty())
    {
        return std::nullopt;
    }

    const auto &row = result.front();
    return CourseRecord{
        row["id"].as<int>(),
        row["code"].as<std::string>(),
        row["name"].as<std::string>(),
        row["description"].as<std::string>(),
        row["target_audience"].as<std::string>()};
}

std::vector<KnowledgePointRecord>
KnowledgeGraphRepository::listKnowledgePointsByCourseId(int courseId)
{
    const auto result = getClient()->execSqlSync(
        "select id, code, name, chapter_no, chapter_name, node_type, "
        "difficulty_level, estimated_minutes, display_order, is_core, "
        "description "
        "from knowledge_points "
        "where course_id = ? "
        "order by display_order asc, id asc",
        courseId);

    std::vector<KnowledgePointRecord> points;
    points.reserve(result.size());

    for (const auto &row : result)
    {
        points.push_back(KnowledgePointRecord{
            row["id"].as<int>(),
            row["code"].as<std::string>(),
            row["name"].as<std::string>(),
            row["chapter_no"].as<int>(),
            row["chapter_name"].as<std::string>(),
            row["node_type"].as<std::string>(),
            row["difficulty_level"].as<int>(),
            row["estimated_minutes"].as<int>(),
            row["display_order"].as<int>(),
            row["is_core"].as<int>() == 1,
            row["description"].as<std::string>()});
    }

    return points;
}

std::vector<KnowledgeDependencyRecord>
KnowledgeGraphRepository::listDependenciesByCourseId(int courseId)
{
    const auto result = getClient()->execSqlSync(
        "select id, prerequisite_point_id, target_point_id, relation_type, "
        "description "
        "from knowledge_dependencies "
        "where course_id = ? "
        "order by id asc",
        courseId);

    std::vector<KnowledgeDependencyRecord> dependencies;
    dependencies.reserve(result.size());

    for (const auto &row : result)
    {
        dependencies.push_back(KnowledgeDependencyRecord{
            row["id"].as<int>(),
            row["prerequisite_point_id"].as<int>(),
            row["target_point_id"].as<int>(),
            row["relation_type"].as<std::string>(),
            row["description"].as<std::string>()});
    }

    return dependencies;
}
}
