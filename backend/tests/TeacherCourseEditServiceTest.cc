#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/TeacherCourseEditService.h"

#include <stdexcept>
#include <string>

namespace
{
constexpr uint16_t kTeacherCourseEditTestPort = 18991;

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

Json::Value makeKpBody(const std::string &code)
{
    Json::Value body;
    body["code"] = code;
    body["name"] = "Test KP";
    body["chapterNo"] = 1;
    body["chapterName"] = "Test";
    body["nodeType"] = "concept";
    body["difficultyLevel"] = 2;
    body["estimatedMinutes"] = 30;
    body["displayOrder"] = 99;
    return body;
}

void cleanupKp(const std::string &code)
{
    try
    {
        services::TeacherCourseEditService::deleteKnowledgePoint(
            "teacher_demo", "data-structures", code);
    }
    catch (...)
    {
        // ignore if already cleaned up
    }
}
}

DROGON_TEST(TeacherCourseEditServiceCreatesKnowledgePoint)
{
    const auto payload =
        services::TeacherCourseEditService::createKnowledgePoint(
            "teacher_demo", "data-structures", makeKpBody("test-kp-b1"));

    CHECK(payload["code"].asString() == "test-kp-b1");
    CHECK(payload["created"].asBool() == true);

    cleanupKp("test-kp-b1");
}

DROGON_TEST(TeacherCourseEditServiceRejectsUnassignedTeacherForEdit)
{
    Json::Value body;
    body["code"] = "test-fail";
    body["name"] = "Fail";

    CHECK_THROWS_AS(
        services::TeacherCourseEditService::createKnowledgePoint(
            "unknown_teacher", "data-structures", body),
        std::invalid_argument);
}

DROGON_TEST(TeacherCourseEditServiceUpdatesKnowledgePoint)
{
    // Create first
    services::TeacherCourseEditService::createKnowledgePoint(
        "teacher_demo", "data-structures", makeKpBody("test-kp-upd"));

    Json::Value body;
    body["name"] = "更新后的名称";
    body["difficultyLevel"] = 4;

    const auto payload =
        services::TeacherCourseEditService::updateKnowledgePoint(
            "teacher_demo", "data-structures", "test-kp-upd", body);

    CHECK(payload["updated"].asBool() == true);

    cleanupKp("test-kp-upd");
}

DROGON_TEST(TeacherCourseEditServiceDeletesKnowledgePoint)
{
    services::TeacherCourseEditService::createKnowledgePoint(
        "teacher_demo", "data-structures", makeKpBody("test-kp-del"));

    const auto payload =
        services::TeacherCourseEditService::deleteKnowledgePoint(
            "teacher_demo", "data-structures", "test-kp-del");

    CHECK(payload["deleted"].asBool() == true);
}

DROGON_TEST(TeacherCourseEditServiceUpdatesDependencies)
{
    services::TeacherCourseEditService::createKnowledgePoint(
        "teacher_demo", "data-structures", makeKpBody("test-dep-kp"));

    Json::Value body;
    Json::Value deps(Json::arrayValue);
    Json::Value dep1;
    dep1["knowledgePointCode"] = "test-dep-kp";
    dep1["prerequisiteCode"] = "ds-intro";
    deps.append(dep1);
    body["dependencies"] = deps;

    const auto payload =
        services::TeacherCourseEditService::updateDependencies(
            "teacher_demo", "data-structures", body);

    CHECK(payload["updated"].asBool() == true);
    CHECK(payload["count"].asInt() >= 1);

    cleanupKp("test-dep-kp");
}

DROGON_TEST(TeacherCourseEditServiceReadsResources)
{
    const auto payload =
        services::TeacherCourseEditService::readResources(
            "teacher_demo", "data-structures");

    CHECK(payload.isMember("knowledgePointResources"));
}

DROGON_TEST(TeacherCourseEditServiceRejectsInvalidKnowledgePointCode)
{
    Json::Value body;
    body["code"] = "INVALID";
    body["name"] = "Invalid Code";

    CHECK_THROWS_AS(
        services::TeacherCourseEditService::createKnowledgePoint(
            "teacher_demo", "data-structures", body),
        std::invalid_argument);
}

DROGON_TEST(TeacherCourseEditHttpCreatePointReturns201)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherCourseEditTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Post);
    request->setPath("/api/teacher/courses/data-structures/knowledge-points?username=teacher_demo");
    request->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    request->setBody("{\"code\":\"http-test-kp2\",\"name\":\"HTTP Test\",\"chapterNo\":1,\"chapterName\":\"Ch1\",\"nodeType\":\"concept\",\"difficultyLevel\":1,\"estimatedMinutes\":10,\"displayOrder\":97}");

    const auto [result, response] = sendRequest(client, request);
    REQUIRE(result == drogon::ReqResult::Ok);
    CHECK(response->getStatusCode() == drogon::k201Created);

    cleanupKp("http-test-kp2");
}

DROGON_TEST(TeacherCourseEditHttpReadsResources)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherCourseEditTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses/data-structures/resources?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    CHECK(response->getStatusCode() == drogon::k200OK);

    const auto payload = parseJsonResponse(response);
    CHECK(payload.isMember("knowledgePointResources"));
}
