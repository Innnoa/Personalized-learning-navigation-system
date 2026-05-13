#include "services/AdminDashboardService.h"

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
Json::Value AdminDashboardService::listUsers()
{
    const auto result = getClient()->execSqlSync(
        "select u.username, u.display_name, u.status, r.role_code "
        "from users u "
        "join user_roles r on r.user_id = u.id "
        "order by u.username asc, r.role_code asc");

    Json::Value payload;
    Json::Value users(Json::arrayValue);
    std::string currentUsername;
    Json::Value currentUser;

    for (const auto &row : result)
    {
        const auto username = row["username"].as<std::string>();
        if (currentUsername != username)
        {
            if (!currentUsername.empty())
            {
                users.append(currentUser);
            }

            currentUsername = username;
            currentUser = Json::Value(Json::objectValue);
            currentUser["username"] = username;
            currentUser["displayName"] = row["display_name"].as<std::string>();
            currentUser["status"] = row["status"].as<std::string>();
            currentUser["roles"] = Json::Value(Json::arrayValue);
        }

        currentUser["roles"].append(row["role_code"].as<std::string>());
    }

    if (!currentUsername.empty())
    {
        users.append(currentUser);
    }

    payload["users"] = users;
    return payload;
}

Json::Value AdminDashboardService::listCourses()
{
    const auto result = getClient()->execSqlSync(
        "select c.code, c.name, c.target_audience, count(kp.id) as knowledge_point_count "
        "from courses c "
        "left join knowledge_points kp on kp.course_id = c.id "
        "group by c.id, c.code, c.name, c.target_audience "
        "order by c.code asc");

    Json::Value payload;
    Json::Value courses(Json::arrayValue);

    for (const auto &row : result)
    {
        Json::Value item;
        item["courseCode"] = row["code"].as<std::string>();
        item["courseName"] = row["name"].as<std::string>();
        item["targetAudience"] = row["target_audience"].as<std::string>();
        item["knowledgePointCount"] = row["knowledge_point_count"].as<unsigned int>();
        courses.append(item);
    }

    payload["courses"] = courses;
    return payload;
}

Json::Value AdminDashboardService::listCourseAssignments()
{
    const auto result = getClient()->execSqlSync(
        "select u.username, c.code, c.name "
        "from teacher_course_assignments tca "
        "join users u on u.id = tca.user_id "
        "join courses c on c.id = tca.course_id "
        "order by u.username asc, c.code asc");

    Json::Value payload;
    Json::Value assignments(Json::arrayValue);

    for (const auto &row : result)
    {
        Json::Value item;
        item["teacherUsername"] = row["username"].as<std::string>();
        item["courseCode"] = row["code"].as<std::string>();
        item["courseName"] = row["name"].as<std::string>();
        assignments.append(item);
    }

    payload["assignments"] = assignments;
    return payload;
}
}
