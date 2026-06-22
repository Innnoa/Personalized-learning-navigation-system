#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include <stdexcept>
#include <string>

namespace
{
constexpr uint16_t kDashboardTestPort = 18991;

drogon::orm::DbClientPtr getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 测试数据库连接。");
    }

    return client;
}

void insertUser(const std::string &username,
                const std::string &password,
                const std::string &displayName,
                const std::string &status)
{
    getClient()->execSqlSync(
        "insert into users (username, password_hash, display_name, status) values (?, ?, ?, ?)",
        username,
        password,
        displayName,
        status);
}

int findUserId(const std::string &username)
{
    const auto result = getClient()->execSqlSync(
        "select id from users where username = ? limit 1", username);
    if (result.empty())
    {
        throw std::runtime_error("未找到测试账号。");
    }

    return result.front()["id"].as<int>();
}

void insertRole(int userId, const std::string &roleCode)
{
    getClient()->execSqlSync(
        "insert into user_roles (user_id, role_code) values (?, ?)", userId, roleCode);
}

Json::Value parseJsonResponse(const drogon::HttpResponsePtr &response)
{
    const auto json = response->getJsonObject();
    if (!json)
    {
        Json::Value payload;
        payload["rawBody"] =
            response != nullptr ? std::string(response->getBody()) : std::string();
        return payload;
    }

    return *json;
}

std::pair<drogon::ReqResult, drogon::HttpResponsePtr> sendRequest(
    const drogon::HttpClientPtr &client,
    const drogon::HttpRequestPtr &request)
{
    return client->sendRequest(request, 5.0);
}
}

DROGON_TEST(TeacherCoursesEndpointReturnsTeacherDemoAssignedCourse)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kDashboardTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["courses"].isArray());
    REQUIRE(payload["courses"].size() == 1U);
    CHECK(payload["courses"][0]["courseCode"].asString() == "data-structures");
}

DROGON_TEST(TeacherCourseOverviewEndpointReturnsMetadataAndKnowledgePointCount)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kDashboardTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses/data-structures/overview?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    CHECK(payload["course"]["courseCode"].asString() == "data-structures");
    CHECK(payload["course"]["courseName"].asString() == "数据结构");
    CHECK(payload["course"]["knowledgePointCount"].asUInt() == 14U);
    CHECK(payload["course"].isMember("dependencyCount"));
    CHECK(payload["course"].isMember("resourceCount"));
    CHECK(payload["course"].isMember("questionBankCount"));
    CHECK(payload["course"].isMember("questionCount"));
    CHECK(payload["course"].isMember("resourceCoveredPointCount"));
    CHECK(payload["course"].isMember("questionCoveredPointCount"));
    CHECK(payload["course"]["dependencyCount"].asUInt() >= 0U);
    CHECK(payload["course"]["resourceCount"].asUInt() >= 0U);
    CHECK(payload["course"]["questionBankCount"].asUInt() >= 0U);
    CHECK(payload["course"]["questionCount"].asUInt() >= 0U);
    CHECK(payload["course"]["resourceCoveredPointCount"].asUInt() >= 0U);
    CHECK(payload["course"]["questionCoveredPointCount"].asUInt() >= 0U);
    CHECK(payload.isMember("chapterStats"));
    REQUIRE(payload["chapterStats"].isArray());
    CHECK(payload["chapterStats"].size() >= 1U);
}

DROGON_TEST(TeacherCourseOverviewEndpointRejectsUnassignedTeacherCourse)
{
    insertUser("teacher_unassigned", "demo123", "未分配教师", "active");
    insertRole(findUserId("teacher_unassigned"), "teacher");

    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kDashboardTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath(
        "/api/teacher/courses/data-structures/overview?username=teacher_unassigned");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k400BadRequest);

    const auto payload = parseJsonResponse(response);
    CHECK(payload["status"].asString() == "error");
}

DROGON_TEST(AdminUsersEndpointReturnsSeededDemoAccounts)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kDashboardTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/admin/users");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["users"].isArray());
    REQUIRE(payload["users"].size() == 3U);
    CHECK(payload["users"][0]["username"].asString() == "admin_demo");
    CHECK(payload["users"][1]["username"].asString() == "student_demo");
    CHECK(payload["users"][2]["username"].asString() == "teacher_demo");
    REQUIRE(payload["users"][0]["roles"].isArray());
    CHECK(payload["users"][0]["roles"][0].asString() == "admin");
}

DROGON_TEST(AdminUsersEndpointAggregatesMultiRoleUserWithoutDuplicateRows)
{
    insertUser("multi_role_dashboard", "demo123", "多角色看板账号", "active");
    const auto userId = findUserId("multi_role_dashboard");
    insertRole(userId, "teacher");
    insertRole(userId, "student");

    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kDashboardTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/admin/users");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["users"].isArray());

    Json::Value matchedUser;
    unsigned int matchedCount = 0;
    for (const auto &user : payload["users"])
    {
        if (user["username"].asString() == "multi_role_dashboard")
        {
            matchedUser = user;
            ++matchedCount;
        }
    }

    CHECK(matchedCount == 1U);
    REQUIRE(matchedUser.isObject());
    REQUIRE(matchedUser["roles"].isArray());
    REQUIRE(matchedUser["roles"].size() == 2U);
    CHECK(matchedUser["roles"][0].asString() == "student");
    CHECK(matchedUser["roles"][1].asString() == "teacher");
}

DROGON_TEST(AdminCourseAssignmentsEndpointReturnsTeacherToCourseMapping)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kDashboardTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/admin/course-assignments");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["assignments"].isArray());
    REQUIRE(payload["assignments"].size() == 1U);
    CHECK(payload["assignments"][0]["teacherUsername"].asString() == "teacher_demo");
    CHECK(payload["assignments"][0]["courseCode"].asString() == "data-structures");
}
