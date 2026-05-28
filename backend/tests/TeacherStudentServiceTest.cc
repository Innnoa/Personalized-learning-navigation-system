#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/TeacherStudentService.h"

#include <stdexcept>
#include <string>

namespace
{
constexpr uint16_t kTeacherStudentTestPort = 18991;

drogon::orm::DbClientPtr getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 测试数据库连接。");
    }

    return client;
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

DROGON_TEST(TeacherStudentServiceListsStudentsForAssignedCourse)
{
    const auto payload =
        services::TeacherStudentService::listStudents("teacher_demo", "data-structures", "", "");

    CHECK(payload["courseCode"].asString() == "data-structures");
    REQUIRE(payload["students"].isArray());
    REQUIRE(payload["students"].size() >= 1U);

    const auto &first = payload["students"][0];
    CHECK(first.isMember("learnerCode"));
    CHECK(first.isMember("learnerName"));
    CHECK(first.isMember("overallMastery"));
    CHECK(first.isMember("lastActivityAt"));
}

DROGON_TEST(TeacherStudentServiceRejectsUnassignedTeacher)
{
    CHECK_THROWS_AS(
        services::TeacherStudentService::listStudents(
            "unknown_teacher", "data-structures", "", ""),
        std::invalid_argument);
}

DROGON_TEST(TeacherStudentServiceProvidesCourseStats)
{
    const auto payload = services::TeacherStudentService::buildCourseStats(
        "teacher_demo", "data-structures");

    CHECK(payload["courseCode"].asString() == "data-structures");
    CHECK(payload["studentCount"].asUInt() >= 1U);
    CHECK(payload["avgMastery"].asDouble() >= 0.0);
    CHECK(payload["avgMastery"].asDouble() <= 1.0);
    REQUIRE(payload["masteryDistribution"].isObject());
    REQUIRE(payload["masteryDistribution"]["low"].isObject());
    REQUIRE(payload["masteryDistribution"]["mid"].isObject());
    REQUIRE(payload["masteryDistribution"]["high"].isObject());
}

DROGON_TEST(TeacherStudentServiceProvidesKnowledgeStats)
{
    const auto payload = services::TeacherStudentService::buildKnowledgeStats(
        "teacher_demo", "data-structures");

    CHECK(payload["courseCode"].asString() == "data-structures");
    REQUIRE(payload["knowledgePoints"].isArray());
    REQUIRE(payload["knowledgePoints"].size() >= 1U);

    const auto &first = payload["knowledgePoints"][0];
    CHECK(first["code"].asString().size() > 0);
    CHECK(first["name"].asString().size() > 0);
    CHECK(first["chapterNo"].asInt() >= 1);
    CHECK(first["difficultyLevel"].asInt() >= 1);
    CHECK(first["difficultyLevel"].asInt() <= 5);
    CHECK(first["avgMastery"].asDouble() >= 0.0);
    CHECK(first["avgMastery"].asDouble() <= 1.0);
}

DROGON_TEST(TeacherStudentServiceProvidesStudentProgress)
{
    const auto payload = services::TeacherStudentService::buildStudentProgress(
        "teacher_demo", "data-structures", "demo-learner");

    REQUIRE(payload["learner"].isObject());
    CHECK(payload["learner"]["code"].asString() == "demo-learner");
    REQUIRE(payload["masteryByCode"].isObject());
    REQUIRE(payload["recentFeedback"].isArray());
    CHECK(payload.isMember("recentPaths"));
}

DROGON_TEST(TeacherStudentServiceRejectsStudentFromOtherCourse)
{
    CHECK_THROWS_AS(
        services::TeacherStudentService::buildStudentProgress(
            "teacher_demo", "data-structures", "non-existent-learner"),
        std::invalid_argument);
}

DROGON_TEST(TeacherStudentsHttpEndpointReturnsStudentList)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherStudentTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses/data-structures/students?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["students"].isArray());
    REQUIRE(payload["students"].size() >= 1U);
}

DROGON_TEST(TeacherStatsHttpEndpointReturnsStats)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherStudentTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses/data-structures/stats?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    CHECK(payload["studentCount"].asUInt() >= 1U);
    REQUIRE(payload["masteryDistribution"].isObject());
}

DROGON_TEST(TeacherKnowledgeStatsHttpEndpointReturnsKnowledgePoints)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherStudentTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses/data-structures/knowledge-stats?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["knowledgePoints"].isArray());
}

DROGON_TEST(TeacherStudentProgressHttpEndpointReturnsStudentDetail)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherStudentTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/students/demo-learner/progress?username=teacher_demo&courseCode=data-structures");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    REQUIRE(payload["learner"].isObject());
    CHECK(payload["learner"]["code"].asString() == "demo-learner");
}
