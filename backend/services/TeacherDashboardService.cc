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
        "select c.code, c.name, c.description, c.target_audience, u.display_name as teacher_display_name, "
        "count(kp.id) as knowledge_point_count "
        "from courses c "
        "join teacher_course_assignments tca on tca.course_id = c.id "
        "join users u on u.id = tca.user_id "
        "left join knowledge_points kp on kp.course_id = c.id "
        "where u.username = ? and c.code = ? "
        "group by c.id, c.code, c.name, c.description, c.target_audience, u.display_name "
        "limit 1",
        username,
        courseCode);

    if (result.empty())
    {
        throw std::invalid_argument("未找到指定课程。");
    }

    const auto &row = result.front();
    const auto courseIdResult = getClient()->execSqlSync(
        "select c.id from courses c "
        "join teacher_course_assignments tca on tca.course_id = c.id "
        "join users u on u.id = tca.user_id "
        "where u.username = ? and c.code = ? limit 1",
        username,
        courseCode);
    const auto courseId = courseIdResult.front()["id"].as<int>();

    const auto dependencyCountResult = getClient()->execSqlSync(
        "select count(*) as count from knowledge_dependencies where course_id = ?",
        std::to_string(courseId));
    const auto resourceCountResult = getClient()->execSqlSync(
        "select count(*) as count from learning_resources where course_id = ?",
        std::to_string(courseId));
    const auto questionBankCountResult = getClient()->execSqlSync(
        "select count(*) as count from question_banks where course_id = ?",
        std::to_string(courseId));
    const auto questionCountResult = getClient()->execSqlSync(
        "select count(*) as count from questions where course_id = ?",
        std::to_string(courseId));
    const auto resourceCoveredPointCountResult = getClient()->execSqlSync(
        "select count(distinct knowledge_point_id) as count "
        "from learning_resources where course_id = ? and knowledge_point_id is not null",
        std::to_string(courseId));
    const auto questionCoveredPointCountResult = getClient()->execSqlSync(
        "select count(distinct knowledge_point_id) as count "
        "from questions where course_id = ? and knowledge_point_id is not null",
        std::to_string(courseId));

    const auto chapterStatsResult = getClient()->execSqlSync(
        "select chapter_no, chapter_name, count(*) as knowledge_point_count "
        "from knowledge_points where course_id = ? "
        "group by chapter_no, chapter_name order by chapter_no asc",
        std::to_string(courseId));
    const auto uncoveredPointsResult = getClient()->execSqlSync(
        "select kp.code, kp.name, "
        "case when lr.knowledge_point_id is null then 0 else 1 end as has_resource, "
        "case when q.knowledge_point_id is null then 0 else 1 end as has_question "
        "from knowledge_points kp "
        "left join (select distinct knowledge_point_id from learning_resources where course_id = ?) lr "
        "  on lr.knowledge_point_id = kp.id "
        "left join (select distinct knowledge_point_id from questions where course_id = ?) q "
        "  on q.knowledge_point_id = kp.id "
        "where kp.course_id = ? and "
        "(lr.knowledge_point_id is null or q.knowledge_point_id is null) "
        "order by kp.chapter_no asc, kp.display_order asc, kp.id asc",
        std::to_string(courseId),
        std::to_string(courseId),
        std::to_string(courseId));

    Json::Value payload;
    payload["course"]["courseCode"] = row["code"].as<std::string>();
    payload["course"]["courseName"] = row["name"].as<std::string>();
    payload["course"]["description"] = row["description"].as<std::string>();
    payload["course"]["targetAudience"] =
        row["target_audience"].as<std::string>();
    payload["course"]["teacherDisplayName"] =
        row["teacher_display_name"].as<std::string>();
    payload["course"]["teacherUsername"] = username;
    payload["course"]["knowledgePointCount"] =
        row["knowledge_point_count"].as<unsigned int>();
    payload["course"]["dependencyCount"] =
        dependencyCountResult.front()["count"].as<unsigned int>();
    payload["course"]["resourceCount"] =
        resourceCountResult.front()["count"].as<unsigned int>();
    payload["course"]["questionBankCount"] =
        questionBankCountResult.front()["count"].as<unsigned int>();
    payload["course"]["questionCount"] =
        questionCountResult.front()["count"].as<unsigned int>();
    payload["course"]["resourceCoveredPointCount"] =
        resourceCoveredPointCountResult.front()["count"].as<unsigned int>();
    payload["course"]["questionCoveredPointCount"] =
        questionCoveredPointCountResult.front()["count"].as<unsigned int>();
    payload["course"]["uncoveredPointCount"] =
        std::max(
            0,
            static_cast<int>(row["knowledge_point_count"].as<unsigned int>()) -
                static_cast<int>(questionCoveredPointCountResult.front()["count"].as<unsigned int>()));
    if (payload["course"]["resourceCoveredPointCount"].asUInt() <
        payload["course"]["knowledgePointCount"].asUInt())
    {
        payload["course"]["maintenanceStatus"] = "待补资源";
    }
    else if (payload["course"]["questionCoveredPointCount"].asUInt() <
             payload["course"]["knowledgePointCount"].asUInt())
    {
        payload["course"]["maintenanceStatus"] = "待补题库";
    }
    else if (payload["course"]["dependencyCount"].asUInt() == 0U)
    {
        payload["course"]["maintenanceStatus"] = "待补依赖";
    }
    else
    {
        payload["course"]["maintenanceStatus"] = "建设完整";
    }

    Json::Value chapterStats(Json::arrayValue);
    for (const auto &chapterRow : chapterStatsResult)
    {
        Json::Value chapter;
        chapter["chapterNo"] = chapterRow["chapter_no"].as<int>();
        chapter["chapterName"] = chapterRow["chapter_name"].as<std::string>();
        chapter["knowledgePointCount"] =
            chapterRow["knowledge_point_count"].as<unsigned int>();
        chapterStats.append(chapter);
    }
    payload["chapterStats"] = chapterStats;

    Json::Value uncoveredPoints(Json::arrayValue);
    for (const auto &pointRow : uncoveredPointsResult)
    {
        Json::Value point;
        point["code"] = pointRow["code"].as<std::string>();
        point["name"] = pointRow["name"].as<std::string>();
        Json::Value missingCoverage(Json::arrayValue);
        if (pointRow["has_resource"].as<int>() == 0)
        {
            missingCoverage.append("resource");
        }
        if (pointRow["has_question"].as<int>() == 0)
        {
            missingCoverage.append("question_bank");
        }
        point["missingCoverage"] = missingCoverage;
        uncoveredPoints.append(point);
    }
    payload["uncoveredPoints"] = uncoveredPoints;
    return payload;
}
}
