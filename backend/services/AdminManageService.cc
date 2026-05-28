#include "services/AdminManageService.h"

#include <drogon/drogon.h>

#include <chrono>
#include <ctime>
#include <fstream>
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

void validateUserCode(const std::string &s)
{
    if (s.empty() || s.size() < 3 || s.size() > 50) return;
    for (char ch : s)
    {
        if (!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_'))
        {
            throw std::invalid_argument("用户名只能包含小写字母、数字和下划线。");
        }
    }
}
}

namespace services
{

// ---------- Users ----------

Json::Value AdminManageService::createUser(const Json::Value &body)
{
    const std::string username = body.get("username", "").asString();
    const std::string displayName = body.get("displayName", username).asString();
    const std::string password = body.get("password", "").asString();

    if (username.empty() || password.empty())
        throw std::invalid_argument("用户名和密码不能为空。");

    validateUserCode(username);

    const auto existing = getClient()->execSqlSync(
        "select 1 from users where username = ? limit 1", username);
    if (!existing.empty())
        throw std::invalid_argument("用户名 " + username + " 已存在。");

    getClient()->execSqlSync(
        "insert into users (username, password_hash, display_name, status) "
        "values (?, ?, ?, 'active')", username, password, displayName);

    // Assign roles
    if (body.isMember("roles") && body["roles"].isArray())
    {
        const auto userIdResult = getClient()->execSqlSync(
            "select id from users where username = ? limit 1", username);
        if (!userIdResult.empty())
        {
            int userId = userIdResult.front()["id"].as<int>();
            for (const auto &role : body["roles"])
            {
                getClient()->execSqlSync(
                    "insert or ignore into user_roles (user_id, role_code) values (?, ?)",
                    std::to_string(userId), role.asString());
            }
        }
    }

    Json::Value payload;
    payload["username"] = username;
    payload["created"] = true;
    return payload;
}

Json::Value AdminManageService::updateUser(const std::string &username,
                                           const Json::Value &body)
{
    if (body.isMember("displayName"))
    {
        getClient()->execSqlSync(
            "update users set display_name = ? where username = ?",
            body["displayName"].asString(), username);
    }
    if (body.isMember("password"))
    {
        getClient()->execSqlSync(
            "update users set password_hash = ? where username = ?",
            body["password"].asString(), username);
    }
    Json::Value payload;
    payload["updated"] = true;
    return payload;
}

Json::Value AdminManageService::toggleUserStatus(const std::string &username)
{
    const auto result = getClient()->execSqlSync(
        "select status from users where username = ? limit 1", username);
    if (result.empty())
        throw std::invalid_argument("用户不存在。");

    const auto status = result.front()["status"].as<std::string>();
    const std::string newStatus = (status == "active") ? "disabled" : "active";

    getClient()->execSqlSync(
        "update users set status = ? where username = ?", newStatus, username);

    Json::Value payload;
    payload["username"] = username;
    payload["status"] = newStatus;
    return payload;
}

Json::Value AdminManageService::setUserRoles(const std::string &username,
                                             const Json::Value &body)
{
    const auto userResult = getClient()->execSqlSync(
        "select id from users where username = ? limit 1", username);
    if (userResult.empty())
        throw std::invalid_argument("用户不存在。");

    const int userId = userResult.front()["id"].as<int>();

    getClient()->execSqlSync("delete from user_roles where user_id = ?",
                             std::to_string(userId));

    if (body.isMember("roles") && body["roles"].isArray())
    {
        for (const auto &role : body["roles"])
        {
            getClient()->execSqlSync(
                "insert or ignore into user_roles (user_id, role_code) values (?, ?)",
                std::to_string(userId), role.asString());
        }
    }

    Json::Value payload;
    payload["updated"] = true;
    return payload;
}

// ---------- Courses ----------

Json::Value AdminManageService::updateCourse(const std::string &courseCode,
                                             const Json::Value &body)
{
    if (body.isMember("name"))
    {
        getClient()->execSqlSync(
            "update courses set name = ? where code = ?",
            body["name"].asString(), courseCode);
    }
    if (body.isMember("description"))
    {
        getClient()->execSqlSync(
            "update courses set description = ? where code = ?",
            body["description"].asString(), courseCode);
    }
    if (body.isMember("targetAudience"))
    {
        getClient()->execSqlSync(
            "update courses set target_audience = ? where code = ?",
            body["targetAudience"].asString(), courseCode);
    }
    Json::Value payload;
    payload["updated"] = true;
    return payload;
}

Json::Value AdminManageService::deleteCourse(const std::string &courseCode)
{
    getClient()->execSqlSync("delete from courses where code = ?", courseCode);
    Json::Value payload;
    payload["deleted"] = true;
    return payload;
}

// ---------- Assignments ----------

Json::Value AdminManageService::assignTeacher(const Json::Value &body)
{
    const std::string username = body.get("username", "").asString();
    const std::string courseCode = body.get("courseCode", "").asString();

    if (username.empty() || courseCode.empty())
        throw std::invalid_argument("教师用户名和课程代码不能为空。");

    const auto userResult = getClient()->execSqlSync(
        "select id from users where username = ? limit 1", username);
    const auto courseResult = getClient()->execSqlSync(
        "select id from courses where code = ? limit 1", courseCode);

    if (userResult.empty()) throw std::invalid_argument("用户不存在。");
    if (courseResult.empty()) throw std::invalid_argument("课程不存在。");

    getClient()->execSqlSync(
        "insert or ignore into teacher_course_assignments (user_id, course_id) "
        "values (?, ?)",
        std::to_string(userResult.front()["id"].as<int>()),
        std::to_string(courseResult.front()["id"].as<int>()));

    Json::Value payload;
    payload["assigned"] = true;
    return payload;
}

Json::Value AdminManageService::unassignTeacher(const std::string &username,
                                                const std::string &courseCode)
{
    getClient()->execSqlSync(
        "delete from teacher_course_assignments "
        "where user_id = (select id from users where username = ? limit 1) "
        "and course_id = (select id from courses where code = ? limit 1)",
        username, courseCode);

    Json::Value payload;
    payload["unassigned"] = true;
    return payload;
}

// ---------- Learners ----------

Json::Value AdminManageService::listAllLearners()
{
    Json::Value payload;
    Json::Value learners(Json::arrayValue);

    // Group by user: show users linked to learners, with all their courses
    const auto userResult = getClient()->execSqlSync(
        "select u.username, u.display_name, l.code, l.name, l.major, l.grade_label, "
        "c.code as course_code, c.name as course_name "
        "from users u "
        "join user_roles r on r.user_id = u.id "
        "join user_learner_links ull on ull.user_id = u.id "
        "join learners l on l.id = ull.learner_id "
        "join courses c on c.id = l.target_course_id "
        "where r.role_code = 'student' and u.status = 'active' "
        "order by u.username asc, c.code asc");

    std::string currentUser;
    Json::Value currentItem;
    Json::Value courses(Json::arrayValue);
    std::string firstName, firstMajor, firstGrade;

    auto flush = [&]() {
        if (currentUser.empty()) return;
        currentItem["learnerCode"] = currentUser;
        currentItem["learnerName"] = firstName;
        currentItem["major"] = firstMajor;
        currentItem["gradeLabel"] = firstGrade;
        currentItem["courses"] = courses;
        learners.append(currentItem);
    };

    for (const auto &row : userResult)
    {
        const auto uname = row["username"].as<std::string>();
        if (uname != currentUser)
        {
            flush();
            currentUser = uname;
            firstName = row["name"].as<std::string>();
            firstMajor = row["major"].as<std::string>();
            firstGrade = row["grade_label"].as<std::string>();
            currentItem = Json::Value(Json::objectValue);
            courses = Json::Value(Json::arrayValue);
        }
        Json::Value c;
        c["courseCode"] = row["course_code"].as<std::string>();
        c["courseName"] = row["course_name"].as<std::string>();
        courses.append(c);
    }
    flush();

    // Unassigned student users
    const auto unassigned = getClient()->execSqlSync(
        "select u.username, u.display_name "
        "from users u "
        "join user_roles r on r.user_id = u.id "
        "where r.role_code = 'student' and u.status = 'active' "
        "and not exists (select 1 from user_learner_links ull where ull.user_id = u.id) "
        "order by u.username asc");

    for (const auto &row : unassigned)
    {
        Json::Value item;
        item["learnerCode"] = row["username"].as<std::string>();
        item["learnerName"] = row["display_name"].as<std::string>();
        item["major"] = "";
        item["gradeLabel"] = "";
        item["courses"] = Json::arrayValue;
        item["status"] = "unassigned";
        learners.append(item);
    }

    payload["learners"] = learners;
    return payload;
}

Json::Value AdminManageService::updateLearner(const std::string &learnerCode,
                                              const Json::Value &body)
{
    if (body.isMember("name"))
        getClient()->execSqlSync("update learners set name = ? where code = ?",
                                 body["name"].asString(), learnerCode);
    if (body.isMember("major"))
        getClient()->execSqlSync("update learners set major = ? where code = ?",
                                 body["major"].asString(), learnerCode);
    if (body.isMember("gradeLabel"))
        getClient()->execSqlSync("update learners set grade_label = ? where code = ?",
                                 body["gradeLabel"].asString(), learnerCode);
    Json::Value p;
    p["updated"] = true;
    return p;
}

Json::Value AdminManageService::deleteLearner(const std::string &learnerCode)
{
    getClient()->execSqlSync("delete from learners where code = ?", learnerCode);
    Json::Value p;
    p["deleted"] = true;
    return p;
}

Json::Value AdminManageService::reassignLearnerCourse(const std::string &learnerCode,
                                                       const Json::Value &body)
{
    const std::string newCourseCode = body.get("courseCode", "").asString();
    if (newCourseCode.empty())
        throw std::invalid_argument("课程代码不能为空。");
    const auto cRes = getClient()->execSqlSync(
        "select id from courses where code = ? limit 1", newCourseCode);
    if (cRes.empty())
        throw std::invalid_argument("课程不存在。");
    getClient()->execSqlSync(
        "update learners set target_course_id = ? where code = ?",
        std::to_string(cRes.front()["id"].as<int>()), learnerCode);
    Json::Value p;
    p["reassigned"] = true;
    return p;
}

// ---------- Logging ----------

void AdminManageService::logOperation(const std::string &username,
                                      const std::string &action,
                                      const std::string &target,
                                      const std::string &detail)
{
    Json::Value logs(Json::arrayValue);
    const std::string path = "../config/operation_logs.json";
    std::ifstream in(path);
    if (in.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
        in.close();
        Json::CharReaderBuilder b;
        Json::Value existing;
        std::string errs;
        std::istringstream s(content);
        if (Json::parseFromStream(b, s, &existing, &errs) && existing.isArray())
            logs = existing;
    }

    Json::Value entry;
    entry["timestamp"] = drogon::app().getDbClient("sqlite_client")
                             ? std::string()
                             : "";
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        char buf[24];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        entry["timestamp"] = buf;
    }
    entry["username"] = username;
    entry["action"] = action;
    entry["target"] = target;
    entry["detail"] = detail;
    logs.append(entry);

    std::ofstream out(path);
    if (out.is_open())
    {
        Json::StreamWriterBuilder sw;
        sw["indentation"] = "  ";
        out << Json::writeString(sw, logs);
        out.close();
    }
}

Json::Value AdminManageService::listLogs(int limit)
{
    const std::string path = "../config/operation_logs.json";
    std::ifstream file(path);
    if (!file.is_open())
    {
        Json::Value p;
        p["logs"] = Json::arrayValue;
        return p;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    Json::CharReaderBuilder b;
    Json::Value logs;
    std::string errs;
    std::istringstream s(content);
    if (!Json::parseFromStream(b, s, &logs, &errs))
    {
        Json::Value p;
        p["logs"] = Json::arrayValue;
        return p;
    }

    if (limit <= 0) limit = 100;
    Json::Value filtered(Json::arrayValue);
    int start = std::max(0, static_cast<int>(logs.size()) - limit);
    for (int i = start; i < static_cast<int>(logs.size()); ++i)
        filtered.append(logs[i]);

    Json::Value payload;
    payload["logs"] = filtered;
    return payload;
}

}
