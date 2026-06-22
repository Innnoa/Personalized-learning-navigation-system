#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/TeacherCourseEditService.h"

#include <stdexcept>
#include <string>

namespace
{
bool approxEqual(double left, double right, double epsilon = 1e-9)
{
    return std::abs(left - right) <= epsilon;
}
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

void cleanupResourceByTitle(const std::string &title)
{
    getClient()->execSqlSync(
        "delete from learning_resources where title = ?",
        title);
}

bool hasColumn(const std::string &tableName, const std::string &columnName)
{
    const auto result = getClient()->execSqlSync(
        "pragma table_info(" + tableName + ")");
    for (const auto &row : result)
    {
        if (row["name"].as<std::string>() == columnName)
        {
            return true;
        }
    }

    return false;
}
}

DROGON_TEST(TeacherContentSchemaIncludesResourceAndQuestionTables)
{
    const auto client = getClient();

    const auto resourcesResult = client->execSqlSync(
        "select name from sqlite_master where type = 'table' and name = 'learning_resources'");
    const auto banksResult = client->execSqlSync(
        "select name from sqlite_master where type = 'table' and name = 'question_banks'");
    const auto questionsResult = client->execSqlSync(
        "select name from sqlite_master where type = 'table' and name = 'questions'");

    REQUIRE(resourcesResult.size() == 1);
    REQUIRE(banksResult.size() == 1);
    REQUIRE(questionsResult.size() == 1);
    CHECK(hasColumn("learning_resources", "importance_weight"));
    CHECK(hasColumn("learning_resources", "estimated_minutes"));
    CHECK(hasColumn("learning_resources", "min_mastery"));
    CHECK(hasColumn("learning_resources", "max_mastery"));
    CHECK(hasColumn("questions", "importance_weight"));
    CHECK(hasColumn("questions", "min_mastery"));
    CHECK(hasColumn("questions", "max_mastery"));
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
    getClient()->execSqlSync(
        "delete from learning_resources where course_id = (select id from courses where code = ?)",
        "data-structures");

    const auto payload =
        services::TeacherCourseEditService::readResources(
            "teacher_demo", "data-structures");

    CHECK(payload.isMember("knowledgePointResources"));
    CHECK(payload["resources"].isArray());
    CHECK(payload["resources"].size() >= 1U);
}

DROGON_TEST(TeacherCourseEditServiceCreatesListsUpdatesAndDeletesResource)
{
    cleanupResourceByTitle("资源 CRUD 测试");
    cleanupResourceByTitle("资源 CRUD 测试（已更新）");

    Json::Value createBody;
    createBody["knowledgePointCode"] = "ds-intro";
    createBody["title"] = "资源 CRUD 测试";
    createBody["resourceType"] = "article";
    createBody["source"] = "unit-test";
    createBody["url"] = "https://example.com/resource-crud";
    createBody["description"] = "用于测试数据库资源 CRUD。";
    createBody["recommendedUsage"] = "先阅读后练习。";
    createBody["recommendedPhase"] = "learn";
    createBody["focusTags"] = Json::arrayValue;
    createBody["importanceWeight"] = 1.8;
    createBody["estimatedMinutes"] = 18;
    createBody["minMastery"] = 0.15;
    createBody["maxMastery"] = 0.72;

    const auto created =
        services::TeacherCourseEditService::createResource(
            "teacher_demo", "data-structures", createBody);

    REQUIRE(created["created"].asBool() == true);
    const auto resourceId = created["resource"]["id"].asInt();
    CHECK(approxEqual(created["resource"]["importanceWeight"].asDouble(), 1.8));
    CHECK(created["resource"]["estimatedMinutes"].asInt() == 18);
    CHECK(approxEqual(created["resource"]["minMastery"].asDouble(), 0.15));
    CHECK(approxEqual(created["resource"]["maxMastery"].asDouble(), 0.72));

    const auto listed =
        services::TeacherCourseEditService::listResources(
            "teacher_demo", "data-structures", Json::Value(Json::objectValue));
    REQUIRE(listed["resources"].isArray());
    CHECK(listed["resources"].size() >= 1U);
    CHECK(approxEqual(
        listed["resources"][listed["resources"].size() - 1]["importanceWeight"]
            .asDouble(),
        1.8));

    Json::Value updateBody;
    updateBody["title"] = "资源 CRUD 测试（已更新）";
    updateBody["recommendedPhase"] = "review";
    updateBody["importanceWeight"] = 2.4;
    updateBody["estimatedMinutes"] = 12;
    updateBody["minMastery"] = 0.30;
    updateBody["maxMastery"] = 0.95;

    const auto updated =
        services::TeacherCourseEditService::updateResource(
            "teacher_demo", "data-structures", resourceId, updateBody);
    CHECK(updated["updated"].asBool() == true);
    CHECK(approxEqual(updated["resource"]["importanceWeight"].asDouble(), 2.4));
    CHECK(updated["resource"]["estimatedMinutes"].asInt() == 12);
    CHECK(approxEqual(updated["resource"]["minMastery"].asDouble(), 0.30));
    CHECK(approxEqual(updated["resource"]["maxMastery"].asDouble(), 0.95));

    const auto removed =
        services::TeacherCourseEditService::deleteResource(
            "teacher_demo", "data-structures", resourceId);
    CHECK(removed["deleted"].asBool() == true);
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
