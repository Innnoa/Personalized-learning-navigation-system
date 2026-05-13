#include "services/TeacherDashboardService.h"

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

namespace services
{
Json::Value TeacherDashboardService::listCourses(const std::string &username)
{
    const auto result = getClient()->execSqlSync(
        "select c.code, c.name, c.target_audience "
        "from teacher_course_assignments tca "
        "join users u on u.id = tca.user_id "
        "join courses c on c.id = tca.course_id "
        "where u.username = ? "
        "order by c.code asc",
        username);

    Json::Value payload;
    payload["teacherUsername"] = username;
    Json::Value courses(Json::arrayValue);

    for (const auto &row : result)
    {
        Json::Value item;
        item["courseCode"] = row["code"].as<std::string>();
        item["courseName"] = row["name"].as<std::string>();
        item["targetAudience"] = row["target_audience"].as<std::string>();
        courses.append(item);
    }

    payload["courses"] = courses;
    return payload;
}

Json::Value TeacherDashboardService::buildCourseOverview(const std::string &username,
                                                         const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select c.code, c.name, c.description, c.target_audience, "
        "count(kp.id) as knowledge_point_count "
        "from courses c "
        "join teacher_course_assignments tca on tca.course_id = c.id "
        "join users u on u.id = tca.user_id "
        "left join knowledge_points kp on kp.course_id = c.id "
        "where u.username = ? and c.code = ? "
        "group by c.id, c.code, c.name, c.description, c.target_audience "
        "limit 1",
        username,
        courseCode);

    if (result.empty())
    {
        throw std::invalid_argument("未找到指定课程。");
    }

    const auto &row = result.front();
    Json::Value payload;
    payload["course"]["courseCode"] = row["code"].as<std::string>();
    payload["course"]["courseName"] = row["name"].as<std::string>();
    payload["course"]["description"] = row["description"].as<std::string>();
    payload["course"]["targetAudience"] =
        row["target_audience"].as<std::string>();
    payload["course"]["knowledgePointCount"] =
        row["knowledge_point_count"].as<unsigned int>();
    return payload;
}
}
