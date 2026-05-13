#include <drogon/drogon.h>
#include <drogon/drogon_test.h>
#include <drogon/HttpClient.h>

#include "services/AuthService.h"

#include <future>
#include <stdexcept>

namespace
{
constexpr uint16_t kAuthTestPort = 18991;

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

void insertLearnerLink(int userId, const std::string &learnerCode)
{
    getClient()->execSqlSync(
        "insert into user_learner_links (user_id, learner_id) values (?, (select id from learners where code = ?))",
        userId,
        learnerCode);
}

void insertLearner(const std::string &learnerCode,
                   const std::string &learnerName)
{
    getClient()->execSqlSync(
        "insert into learners (code, name, major, grade_label, target_course_id, note) "
        "values (?, ?, ?, ?, (select id from courses where code = ?), ?)",
        learnerCode,
        learnerName,
        "测试专业",
        "测试年级",
        "data-structures",
        "test-only learner");
}

Json::Value parseJsonResponse(const drogon::HttpResponsePtr &response)
{
    const auto json = response->getJsonObject();
    if (!json)
    {
        throw std::runtime_error("响应未返回 JSON 数据。");
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

DROGON_TEST(UserSeedProvidesDefaultDemoAccounts)
{
    const auto users = getClient()->execSqlSync(
        "select username from users where username in (?, ?, ?) order by username asc",
        "admin_demo",
        "student_demo",
        "teacher_demo");

    REQUIRE(users.size() == 3U);
    CHECK(users[0]["username"].as<std::string>() == "admin_demo");
    CHECK(users[1]["username"].as<std::string>() == "student_demo");
    CHECK(users[2]["username"].as<std::string>() == "teacher_demo");

    const auto studentLink = getClient()->execSqlSync(
        "select l.code as learner_code "
        "from user_learner_links ull "
        "join users u on u.id = ull.user_id "
        "join learners l on l.id = ull.learner_id "
        "where u.username = ? limit 1",
        "student_demo");

    REQUIRE(studentLink.size() == 1U);
    CHECK(studentLink[0]["learner_code"].as<std::string>() == "demo-learner");

    const auto teacherAssignment = getClient()->execSqlSync(
        "select c.code as course_code "
        "from teacher_course_assignments tca "
        "join users u on u.id = tca.user_id "
        "join courses c on c.id = tca.course_id "
        "where u.username = ? limit 1",
        "teacher_demo");

    REQUIRE(teacherAssignment.size() == 1U);
    CHECK(teacherAssignment[0]["course_code"].as<std::string>() == "data-structures");
}

DROGON_TEST(AuthServiceAuthenticatesStudentDemoAccount)
{
    const auto payload =
        services::AuthService::login("student_demo", "demo123");

    CHECK(payload["authenticated"].asBool());
    CHECK(payload["user"]["username"].asString() == "student_demo");
    REQUIRE(payload["roles"].isArray());
    REQUIRE(payload["roles"].size() == 1U);
    CHECK(payload["roles"][0].asString() == "student");
    CHECK(payload["activeRole"].asString() == "student");
    REQUIRE(payload["linkedLearner"].isObject());
    CHECK(payload["linkedLearner"]["learnerCode"].asString() == "demo-learner");
}

DROGON_TEST(AuthServiceAuthenticatesTeacherDemoAccountWithoutLearnerLink)
{
    const auto payload =
        services::AuthService::login("teacher_demo", "demo123");

    CHECK(payload["authenticated"].asBool());
    CHECK(payload["activeRole"].asString() == "teacher");
    CHECK(payload["linkedLearner"].isNull());
}

DROGON_TEST(AuthServiceRejectsDisabledUser)
{
    insertUser("disabled_demo", "demo123", "禁用演示账号", "disabled");
    insertRole(findUserId("disabled_demo"), "student");

    CHECK_THROWS_AS(services::AuthService::login("disabled_demo", "demo123"),
                    std::invalid_argument);
}

DROGON_TEST(AuthServiceUsesFirstRoleAsActiveRoleAndKeepsTeacherLinkedLearnerNull)
{
    insertUser("multi_role_demo", "demo123", "多角色演示账号", "active");
    insertLearner("multi-role-learner", "多角色学习者");
    const int userId = findUserId("multi_role_demo");
    insertRole(userId, "teacher");
    insertRole(userId, "student");
    insertLearnerLink(userId, "multi-role-learner");

    const auto payload =
        services::AuthService::login("multi_role_demo", "demo123");

    CHECK(payload["authenticated"].asBool());
    REQUIRE(payload["roles"].isArray());
    REQUIRE(payload["roles"].size() == 2U);
    CHECK(payload["roles"][0].asString() == "teacher");
    CHECK(payload["roles"][1].asString() == "student");
    CHECK(payload["activeRole"].asString() == "teacher");
    CHECK(payload["linkedLearner"].isNull());
}

DROGON_TEST(AuthServiceRejectsInvalidCredentials)
{
    CHECK_THROWS_AS(services::AuthService::login("student_demo", "wrong-password"),
                    std::invalid_argument);
    CHECK_THROWS_AS(services::AuthService::login("missing_user", "demo123"),
                    std::invalid_argument);
}

DROGON_TEST(AuthHttpLoginWithValidStudentCredentialsReturnsAuthenticatedPayload)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kAuthTestPort);
    client->enableCookies();

    Json::Value requestBody;
    requestBody["username"] = "student_demo";
    requestBody["password"] = "demo123";

    auto request = drogon::HttpRequest::newHttpJsonRequest(requestBody);
    request->setMethod(drogon::Post);
    request->setPath("/api/auth/login");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    CHECK(payload["authenticated"].asBool());
    CHECK(payload["user"]["username"].asString() == "student_demo");
    CHECK(payload["activeRole"].asString() == "student");
    CHECK(payload["linkedLearner"]["learnerCode"].asString() == "demo-learner");
}

DROGON_TEST(AuthHttpSessionAfterLoginReturnsAuthenticatedPayload)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kAuthTestPort);
    client->enableCookies();

    Json::Value loginBody;
    loginBody["username"] = "student_demo";
    loginBody["password"] = "demo123";

    auto loginRequest = drogon::HttpRequest::newHttpJsonRequest(loginBody);
    loginRequest->setMethod(drogon::Post);
    loginRequest->setPath("/api/auth/login");

    const auto [loginResult, loginResponse] = sendRequest(client, loginRequest);
    REQUIRE(loginResult == drogon::ReqResult::Ok);
    REQUIRE(loginResponse != nullptr);
    REQUIRE(loginResponse->getStatusCode() == drogon::k200OK);

    auto sessionRequest = drogon::HttpRequest::newHttpRequest();
    sessionRequest->setMethod(drogon::Get);
    sessionRequest->setPath("/api/auth/session");

    const auto [sessionResult, sessionResponse] = sendRequest(client, sessionRequest);

    REQUIRE(sessionResult == drogon::ReqResult::Ok);
    REQUIRE(sessionResponse != nullptr);
    CHECK(sessionResponse->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(sessionResponse);
    CHECK(payload["authenticated"].asBool());
    CHECK(payload["user"]["username"].asString() == "student_demo");
    CHECK(payload["activeRole"].asString() == "student");
}

DROGON_TEST(AuthHttpSessionBeforeLoginReturnsUnauthenticatedFalse)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kAuthTestPort);
    client->enableCookies();

    auto sessionRequest = drogon::HttpRequest::newHttpRequest();
    sessionRequest->setMethod(drogon::Get);
    sessionRequest->setPath("/api/auth/session");

    const auto [sessionResult, sessionResponse] = sendRequest(client, sessionRequest);

    REQUIRE(sessionResult == drogon::ReqResult::Ok);
    REQUIRE(sessionResponse != nullptr);
    CHECK(sessionResponse->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(sessionResponse);
    CHECK(!payload["authenticated"].asBool());
}

DROGON_TEST(AuthHttpLogoutClearsSessionAndSessionEndpointReturnsFalse)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kAuthTestPort);
    client->enableCookies();

    Json::Value loginBody;
    loginBody["username"] = "student_demo";
    loginBody["password"] = "demo123";

    auto loginRequest = drogon::HttpRequest::newHttpJsonRequest(loginBody);
    loginRequest->setMethod(drogon::Post);
    loginRequest->setPath("/api/auth/login");

    const auto [loginResult, loginResponse] = sendRequest(client, loginRequest);
    REQUIRE(loginResult == drogon::ReqResult::Ok);
    REQUIRE(loginResponse != nullptr);
    REQUIRE(loginResponse->getStatusCode() == drogon::k200OK);

    auto logoutRequest = drogon::HttpRequest::newHttpRequest();
    logoutRequest->setMethod(drogon::Post);
    logoutRequest->setPath("/api/auth/logout");

    const auto [logoutResult, logoutResponse] = sendRequest(client, logoutRequest);

    REQUIRE(logoutResult == drogon::ReqResult::Ok);
    REQUIRE(logoutResponse != nullptr);
    CHECK(logoutResponse->getStatusCode() == drogon::k200OK);

    const auto logoutPayload = parseJsonResponse(logoutResponse);
    CHECK(!logoutPayload["authenticated"].asBool());

    auto sessionRequest = drogon::HttpRequest::newHttpRequest();
    sessionRequest->setMethod(drogon::Get);
    sessionRequest->setPath("/api/auth/session");

    const auto [sessionResult, sessionResponse] = sendRequest(client, sessionRequest);

    REQUIRE(sessionResult == drogon::ReqResult::Ok);
    REQUIRE(sessionResponse != nullptr);
    CHECK(sessionResponse->getStatusCode() == drogon::k200OK);

    const auto sessionPayload = parseJsonResponse(sessionResponse);
    CHECK(!sessionPayload["authenticated"].asBool());
}
