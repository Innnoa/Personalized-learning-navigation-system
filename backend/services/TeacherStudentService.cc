#include "services/TeacherStudentService.h"

#include <drogon/drogon.h>

#include <set>
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

void verifyTeacherCourseAssignment(const std::string &username,
                                   const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select 1 "
        "from teacher_course_assignments tca "
        "join users u on u.id = tca.user_id "
        "join courses c on c.id = tca.course_id "
        "where u.username = ? and c.code = ? limit 1",
        username,
        courseCode);

    if (result.empty())
    {
        throw std::invalid_argument("教师未分配到指定课程。");
    }
}

int findCourseId(const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select id from courses where code = ? limit 1", courseCode);

    if (result.empty())
    {
        throw std::invalid_argument("未找到指定课程。");
    }

    return result.front()["id"].as<int>();
}
}

namespace services
{
Json::Value TeacherStudentService::listStudents(const std::string &username,
                                                const std::string &courseCode,
                                                const std::string &major,
                                                const std::string &gradeLabel)
{
    verifyTeacherCourseAssignment(username, courseCode);

    const auto result = getClient()->execSqlSync(
        "select l.code as learner_code, l.name as learner_name, "
        "l.major, l.grade_label, "
        "coalesce(avg(lm.mastery_score), 0.0) as overall_mastery, "
        "max(lfr.recorded_at) as last_activity_at "
        "from learners l "
        "join courses c on c.id = l.target_course_id "
        "left join learner_mastery lm on lm.learner_id = l.id "
        "left join learning_feedback_records lfr on lfr.learner_id = l.id "
        "where c.code = ? "
        + (major.empty() ? std::string() : std::string(" and l.major = '") + major + "' ")
        + (gradeLabel.empty() ? std::string() : std::string(" and l.grade_label = '") + gradeLabel + "' ")
        + "group by l.id, l.code, l.name, l.major, l.grade_label "
          "order by l.name asc",
        courseCode);

    Json::Value payload;
    payload["courseCode"] = courseCode;
    Json::Value students(Json::arrayValue);

    for (const auto &row : result)
    {
        Json::Value item;
        item["learnerCode"] = row["learner_code"].as<std::string>();
        item["learnerName"] = row["learner_name"].as<std::string>();
        item["major"] = row["major"].as<std::string>();
        item["gradeLabel"] = row["grade_label"].as<std::string>();
        item["overallMastery"] = row["overall_mastery"].as<double>();
        item["lastActivityAt"] = row["last_activity_at"].as<std::string>();
        students.append(item);
    }

    
    payload["students"] = students;
    return payload;
}

Json::Value TeacherStudentService::buildCourseStats(const std::string &username,
                                                    const std::string &courseCode)
{
    verifyTeacherCourseAssignment(username, courseCode);

    const auto result = getClient()->execSqlSync(
        "select count(distinct l.id) as student_count, "
        "coalesce(avg(lm.mastery_score), 0.0) as avg_mastery "
        "from learners l "
        "join courses c on c.id = l.target_course_id "
        "left join learner_mastery lm on lm.learner_id = l.id "
        "where c.code = ?",
        courseCode);

    const auto &row = result.front();
    const unsigned int studentCount = row["student_count"].as<unsigned int>();
    const double avgMastery = row["avg_mastery"].as<double>();

    const auto distribution = getClient()->execSqlSync(
        "select "
        "sum(case when coalesce(avg_score, 0.0) >= 0.0 and coalesce(avg_score, 0.0) < 0.35 "
        "then 1 else 0 end) as low_count, "
        "sum(case when coalesce(avg_score, 0.0) >= 0.35 and coalesce(avg_score, 0.0) < 0.7 "
        "then 1 else 0 end) as mid_count, "
        "sum(case when coalesce(avg_score, 0.0) >= 0.7 and coalesce(avg_score, 0.0) <= 1.0 "
        "then 1 else 0 end) as high_count "
        "from ( "
        "select l.id, avg(lm.mastery_score) as avg_score "
        "from learners l "
        "join courses c on c.id = l.target_course_id "
        "left join learner_mastery lm on lm.learner_id = l.id "
        "where c.code = ? "
        "group by l.id "
        ") sub",
        courseCode);

    const auto &dist = distribution.front();

    Json::Value payload;
    payload["courseCode"] = courseCode;
    payload["studentCount"] = studentCount;
    payload["avgMastery"] = avgMastery;

    Json::Value distObj;
    Json::Value lowObj;
    lowObj["min"] = 0.0;
    lowObj["max"] = 0.35;
    lowObj["count"] = dist["low_count"].as<int>();
    distObj["low"] = lowObj;

    Json::Value midObj;
    midObj["min"] = 0.35;
    midObj["max"] = 0.7;
    midObj["count"] = dist["mid_count"].as<int>();
    distObj["mid"] = midObj;

    Json::Value highObj;
    highObj["min"] = 0.7;
    highObj["max"] = 1.0;
    highObj["count"] = dist["high_count"].as<int>();
    distObj["high"] = highObj;

    payload["masteryDistribution"] = distObj;
    return payload;
}

Json::Value TeacherStudentService::buildKnowledgeStats(const std::string &username,
                                                       const std::string &courseCode)
{
    verifyTeacherCourseAssignment(username, courseCode);

    const auto result = getClient()->execSqlSync(
        "select kp.code, kp.name, kp.chapter_no, kp.chapter_name, "
        "kp.difficulty_level, kp.display_order, "
        "coalesce(avg(lm.mastery_score), 0.0) as avg_mastery, "
        "count(distinct lm.learner_id) as student_count "
        "from knowledge_points kp "
        "join courses c on c.id = kp.course_id "
        "left join learner_mastery lm on lm.knowledge_point_id = kp.id "
        "where c.code = ? "
        "group by kp.id, kp.code, kp.name, kp.chapter_no, kp.chapter_name, "
        "kp.difficulty_level, kp.display_order "
        "order by kp.chapter_no asc, kp.display_order asc",
        courseCode);

    Json::Value payload;
    payload["courseCode"] = courseCode;
    Json::Value points(Json::arrayValue);

    for (const auto &row : result)
    {
        Json::Value item;
        item["code"] = row["code"].as<std::string>();
        item["name"] = row["name"].as<std::string>();
        item["chapterNo"] = row["chapter_no"].as<int>();
        item["chapterName"] = row["chapter_name"].as<std::string>();
        item["difficultyLevel"] = row["difficulty_level"].as<int>();
        item["avgMastery"] = row["avg_mastery"].as<double>();
        item["studentCount"] = row["student_count"].as<unsigned int>();
        points.append(item);
    }

    payload["knowledgePoints"] = points;
    return payload;
}

Json::Value TeacherStudentService::buildStudentProgress(const std::string &username,
                                                        const std::string &courseCode,
                                                        const std::string &learnerCode)
{
    verifyTeacherCourseAssignment(username, courseCode);

    const auto learnerResult = getClient()->execSqlSync(
        "select l.code, l.name, l.major, l.grade_label, "
        "c.code as course_code, c.name as course_name "
        "from learners l "
        "join courses c on c.id = l.target_course_id "
        "where l.code = ? and c.code = ? limit 1",
        learnerCode,
        courseCode);

    if (learnerResult.empty())
    {
        throw std::invalid_argument("未找到指定学生。");
    }

    const auto &lr = learnerResult.front();

    Json::Value payload;
    payload["learner"]["code"] = lr["code"].as<std::string>();
    payload["learner"]["name"] = lr["name"].as<std::string>();
    payload["learner"]["major"] = lr["major"].as<std::string>();
    payload["learner"]["gradeLabel"] = lr["grade_label"].as<std::string>();
    payload["learner"]["courseCode"] = lr["course_code"].as<std::string>();
    payload["learner"]["courseName"] = lr["course_name"].as<std::string>();

    const auto masteryResult = getClient()->execSqlSync(
        "select kp.code, lm.mastery_score "
        "from learner_mastery lm "
        "join knowledge_points kp on kp.id = lm.knowledge_point_id "
        "join learners l on l.id = lm.learner_id "
        "where l.code = ? and kp.course_id = "
        "(select id from courses where code = ?) "
        "order by kp.chapter_no asc, kp.display_order asc",
        learnerCode,
        courseCode);

    Json::Value masteryByCode;
    for (const auto &row : masteryResult)
    {
        masteryByCode[row["code"].as<std::string>()] = row["mastery_score"].as<double>();
    }

    payload["masteryByCode"] = masteryByCode;

    const auto feedbackResult = getClient()->execSqlSync(
        "select kp.code as node_code, kp.name as node_name, "
        "lfr.completion_status, lfr.previous_mastery, lfr.updated_mastery, lfr.recorded_at "
        "from learning_feedback_records lfr "
        "join knowledge_points kp on kp.id = lfr.knowledge_point_id "
        "join learners l on l.id = lfr.learner_id "
        "where l.code = ? and kp.course_id = "
        "(select id from courses where code = ?) "
        "order by lfr.recorded_at desc limit 10",
        learnerCode,
        courseCode);

    Json::Value feedbackItems(Json::arrayValue);
    for (const auto &row : feedbackResult)
    {
        Json::Value item;
        item["nodeCode"] = row["node_code"].as<std::string>();
        item["nodeName"] = row["node_name"].as<std::string>();
        item["completionStatus"] = row["completion_status"].as<std::string>();
        item["previousMastery"] = row["previous_mastery"].as<double>();
        item["updatedMastery"] = row["updated_mastery"].as<double>();
        item["recordedAt"] = row["recorded_at"].as<std::string>();
        feedbackItems.append(item);
    }

    payload["recentFeedback"] = feedbackItems;
    payload["recentPaths"] = Json::arrayValue;
    return payload;
}

Json::Value TeacherStudentService::addStudent(const std::string &username,
                                              const std::string &courseCode,
                                              const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const std::string learnerCode = body.get("learnerCode", "").asString();
    const std::string learnerName = body.get("learnerName", "").asString();
    const std::string major = body.get("major", "").asString();
    const std::string gradeLabel = body.get("gradeLabel", "").asString();

    if (learnerCode.empty())
    {
        throw std::invalid_argument("学生代码不能为空。");
    }

    // Check if learner already exists
    const auto existing = getClient()->execSqlSync(
        "select id, target_course_id from learners where code = ? limit 1", learnerCode);

    if (!existing.empty())
    {
        const int currentCourseId = existing.front()["target_course_id"].as<int>();

        if (currentCourseId == courseId)
        {
            Json::Value payload;
            payload["learnerCode"] = learnerCode;
            payload["alreadyAssigned"] = true;
            return payload;
        }

        // Different course — create or reuse learner with code {learnerCode}-{courseCode}
        const std::string newCode = learnerCode + "-" + courseCode;
        const auto newLearner = getClient()->execSqlSync(
            "select id from learners where code = ? limit 1", newCode);

        int lid = 0;
        if (newLearner.empty())
        {
            const auto userResult = getClient()->execSqlSync(
                "select display_name from users where username = ? limit 1", learnerCode);
            const std::string name = learnerName.empty()
                ? (userResult.empty() ? learnerCode : userResult.front()["display_name"].as<std::string>())
                : learnerName;
            getClient()->execSqlSync(
                "insert into learners (code, name, major, grade_label, target_course_id) "
                "values (?, ?, ?, ?, ?)",
                newCode, name, major, gradeLabel, std::to_string(courseId));
            lid = getClient()->execSqlSync(
                "select id from learners where code = ? limit 1", newCode)
                .front()["id"].as<int>();
        }
        else
        {
            lid = newLearner.front()["id"].as<int>();
            // Update existing learner's course
            getClient()->execSqlSync(
                "update learners set target_course_id = ? where code = ?",
                std::to_string(courseId), newCode);
        }

        // Ensure user_learner_link exists — find user via existing learner's link
        auto uidResult = getClient()->execSqlSync(
            "select user_id from user_learner_links "
            "where learner_id in (select id from learners where code = ? limit 1) limit 1",
            learnerCode);
        if (uidResult.empty()) {
            uidResult = getClient()->execSqlSync(
                "select id from users where username = ? limit 1", learnerCode);
        }
        if (!uidResult.empty() && lid > 0)
        {
            int uid = uidResult.front()["user_id"].as<int>();
            getClient()->execSqlSync(
                "insert or ignore into user_learner_links (user_id, learner_id) "
                "values (?, ?)",
                std::to_string(uid),
                std::to_string(lid));
        }

        Json::Value payload;
        payload["learnerCode"] = newCode;
        payload["created"] = true;
        return payload;
    }

    // Check if user exists with student role but no learner record yet
    const auto userResult = getClient()->execSqlSync(
        "select u.username, u.display_name "
        "from users u "
        "join user_roles r on r.user_id = u.id "
        "where u.username = ? and r.role_code = 'student' limit 1",
        learnerCode);

    if (!userResult.empty())
    {
        const std::string name = learnerName.empty()
            ? userResult.front()["display_name"].as<std::string>()
            : learnerName;
        getClient()->execSqlSync(
            "insert into learners (code, name, major, grade_label, target_course_id) "
            "values (?, ?, ?, ?, ?)",
            learnerCode, name, major, gradeLabel, std::to_string(courseId));

        // Create user_learner_link
        const auto uidResult = getClient()->execSqlSync(
            "select id from users where username = ? limit 1", learnerCode);
        const auto lidResult = getClient()->execSqlSync(
            "select id from learners where code = ? limit 1", learnerCode);
        if (!uidResult.empty() && !lidResult.empty())
        {
            getClient()->execSqlSync(
                "insert or ignore into user_learner_links (user_id, learner_id) "
                "values (?, ?)",
                std::to_string(uidResult.front()["id"].as<int>()),
                std::to_string(lidResult.front()["id"].as<int>()));
        }

        Json::Value payload;
        payload["learnerCode"] = learnerCode;
        payload["created"] = true;
        return payload;
    }

    // New student (no existing user with this code)
    getClient()->execSqlSync(
        "insert into learners (code, name, major, grade_label, target_course_id) "
        "values (?, ?, ?, ?, ?)",
        learnerCode, learnerName, major, gradeLabel, std::to_string(courseId));

    Json::Value payload;
    payload["learnerCode"] = learnerCode;
    payload["created"] = true;
    return payload;
}

Json::Value TeacherStudentService::listAvailableStudents(
    const std::string &username,
    const std::string &courseCode,
    const std::string &major,
    const std::string &gradeLabel)
{
    verifyTeacherCourseAssignment(username, courseCode);

    // 1. Existing learners NOT already in target course, and
    //    whose linked user doesn't already have another learner in target course
    const auto learnerResult = getClient()->execSqlSync(
        "select l.code as learner_code, l.name as learner_name, "
        "l.major, l.grade_label, c.code as course_code, c.name as course_name, "
        "ull.user_id "
        "from learners l "
        "join courses c on c.id = l.target_course_id "
        "left join user_learner_links ull on ull.learner_id = l.id "
        "where c.code != ?"
        + (major.empty() ? std::string() : " and l.major = '" + major + "'")
        + (gradeLabel.empty() ? std::string() : " and l.grade_label = '" + gradeLabel + "'")
        + " order by l.name asc",
        courseCode);

    // Get learner IDs in target course for filtering
    const auto enrolledIds = getClient()->execSqlSync(
        "select ull.user_id from user_learner_links ull "
        "join learners l on l.id = ull.learner_id "
        "join courses c on c.id = l.target_course_id "
        "where c.code = ?", courseCode);

    std::set<int> enrolledUserIds;
    for (const auto &r : enrolledIds)
        if (!r["user_id"].isNull())
            enrolledUserIds.insert(r["user_id"].as<int>());

    Json::Value students(Json::arrayValue);
    for (const auto &row : learnerResult)
    {
        if (!row["user_id"].isNull())
        {
            int uid = row["user_id"].as<int>();
            if (enrolledUserIds.count(uid)) continue; // user already in this course
        }
        Json::Value item;
        item["learnerCode"] = row["learner_code"].as<std::string>();
        item["learnerName"] = row["learner_name"].as<std::string>();
        item["major"] = row["major"].as<std::string>();
        item["gradeLabel"] = row["grade_label"].as<std::string>();
        item["currentCourseCode"] = row["course_code"].as<std::string>();
        item["currentCourseName"] = row["course_name"].as<std::string>();
        item["type"] = "existing";
        students.append(item);
    }

    // 2. Unassigned student users (have student role but no learner record)
    const auto unassignedResult = getClient()->execSqlSync(
        "select u.username, u.display_name "
        "from users u "
        "join user_roles r on r.user_id = u.id "
        "where r.role_code = 'student' "
        "and u.status = 'active' "
        "and not exists (select 1 from user_learner_links ull where ull.user_id = u.id) "
        "order by u.username asc");

    for (const auto &row : unassignedResult)
    {
        Json::Value item;
        item["learnerCode"] = row["username"].as<std::string>();
        item["learnerName"] = row["display_name"].as<std::string>();
        item["major"] = "";
        item["gradeLabel"] = "";
        item["currentCourseCode"] = "";
        item["currentCourseName"] = "未分配";
        item["type"] = "unassigned";
        students.append(item);
    }
    Json::Value payload;

    payload["students"] = students;
    return payload;
}
}
