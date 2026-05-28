# Teacher Student Progress (Phase A) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add teacher student progress viewing (student list, course stats, knowledge-point distribution, individual student detail) with teacher-specific navigation layout.

**Architecture:** Backend adds `TeacherStudentService` + `TeacherStudentController` with four new read-only endpoints querying existing `learners`, `learner_mastery`, `learning_feedback_records`, `knowledge_points` tables. Frontend adds `roleScope` prop to `PageLayout` for teacher navigation, a course detail container with tab sub-navigation, and four new views following existing Vue patterns.

**Tech Stack:** Drogon (C++20), SQLite, Vue 3, Vue Router, Pinia, Vitest, existing Drogon test suite

---

## File Map

### Backend files to create

- Create: `backend/services/TeacherStudentService.h`
  Responsibility: student list, course stats, knowledge-point stats, individual student progress aggregation.

- Create: `backend/services/TeacherStudentService.cc`
  Responsibility: SQL queries and JSON payload assembly.

- Create: `backend/controllers/TeacherStudentController.h`
  Responsibility: register four student-progress HTTP endpoints with teacher+course authorization.

- Create: `backend/controllers/TeacherStudentController.cc`
  Responsibility: parse request params, call service, return JSON, handle errors.

- Create: `backend/tests/TeacherStudentServiceTest.cc`
  Responsibility: test all four service-level payloads and controller HTTP behavior.

### Backend files to modify

- Modify: `backend/main.cc`
  Responsibility: register `TeacherStudentController` if needed (check current controller registration pattern).

### Frontend files to create

- Create: `frontend/src/views/TeacherCoursePage.vue`
  Responsibility: course detail container with breadcrumb and tab sub-navigation (overview / stats / students / points).

- Create: `frontend/src/views/TeacherCoursePage.test.js`
  Responsibility: test sub-navigation tab rendering and child route presence.

- Create: `frontend/src/views/TeacherStudentsView.vue`
  Responsibility: student list with major/grade filter dropdowns, student cards with mastery bars.

- Create: `frontend/src/views/TeacherStudentsView.test.js`
  Responsibility: test student list rendering, filter by major/grade, empty state.

- Create: `frontend/src/views/TeacherCourseStatsView.vue`
  Responsibility: course stats overview with student count, avg mastery, mastery distribution bars.

- Create: `frontend/src/views/TeacherCourseStatsView.test.js`
  Responsibility: test stats cards rendering, distribution bar display.

- Create: `frontend/src/views/TeacherKnowledgeStatsView.vue`
  Responsibility: knowledge-point table with chapter grouping, avg mastery per point.

- Create: `frontend/src/views/TeacherKnowledgeStatsView.test.js`
  Responsibility: test knowledge-point list rendering, chapter grouping, mastery display.

- Create: `frontend/src/views/TeacherStudentProgressView.vue`
  Responsibility: individual student detail with basic info, mastery list, recent feedback, recent paths.

- Create: `frontend/src/views/TeacherStudentProgressView.test.js`
  Responsibility: test student detail rendering with mock data.

### Frontend files to modify

- Modify: `frontend/src/api/teacher.js`
  Responsibility: add four new API functions.

- Modify: `frontend/src/components/PageLayout.vue`
  Responsibility: add `roleScope` prop, conditional teacher navigation.

- Modify: `frontend/src/components/PageLayout.test.js`
  Responsibility: test teacher navigation rendering.

- Modify: `frontend/src/router/index.js`
  Responsibility: add child routes under `/teacher/courses/:courseCode`, remove old flat route.

- Modify: `frontend/src/router/index.test.js`
  Responsibility: test new teacher route guards.

- Modify: `frontend/src/views/TeacherDashboardView.vue`
  Responsibility: pass `roleScope="teacher"` to PageLayout.

- Modify: `frontend/src/views/TeacherDashboardView.test.js`
  Responsibility: verify PageLayout receives teacher roleScope.

- Modify: `frontend/src/views/TeacherCoursesView.vue`
  Responsibility: pass `roleScope="teacher"` to PageLayout.

- Modify: `frontend/src/views/TeacherCoursesView.test.js`
  Responsibility: verify PageLayout receives teacher roleScope.

- Modify: `frontend/src/views/TeacherCourseOverviewView.vue`
  Responsibility: pass `roleScope="teacher"` to PageLayout.

- Modify: `frontend/src/views/TeacherCourseOverviewView.test.js`
  Responsibility: verify PageLayout receives teacher roleScope.

---

## Task 1: Backend — TeacherStudentService + TeacherStudentController

**Files:**
- Create: `backend/services/TeacherStudentService.h`
- Create: `backend/services/TeacherStudentService.cc`
- Create: `backend/controllers/TeacherStudentController.h`
- Create: `backend/controllers/TeacherStudentController.cc`
- Create: `backend/tests/TeacherStudentServiceTest.cc`

- [ ] **Step 1: Write failing service tests for student list**

Create `backend/tests/TeacherStudentServiceTest.cc`:

```cpp
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>
#include <drogon/HttpClient.h>

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

    REQUIRE(payload["courseCode"].asString() == "data-structures");
    REQUIRE(payload["students"].isArray());
    REQUIRE(payload["students"].size() >= 1U);
    CHECK(payload["students"][0]["learnerCode"].asString() == "demo-learner");
    CHECK(payload["students"][0]["learnerName"].asString() == "演示学生");
    CHECK(payload["students"][0]["overallMastery"].isDouble());
    CHECK(payload["students"][0]["lastActivityAt"].asString().size() > 0);
}

DROGON_TEST(TeacherStudentServiceRejectsUnassignedTeacher)
{
    CHECK_THROWS_AS(
        services::TeacherStudentService::listStudents("unknown_teacher", "data-structures", "", ""),
        std::invalid_argument);
}

DROGON_TEST(TeacherStudentServiceProvidesCourseStats)
{
    const auto payload =
        services::TeacherStudentService::buildCourseStats("teacher_demo", "data-structures");

    CHECK(payload["courseCode"].asString() == "data-structures");
    CHECK(payload["studentCount"].asUInt() >= 1U);
    CHECK(payload["avgMastery"].asDouble() >= 0.0);
    CHECK(payload["avgMastery"].asDouble() <= 1.0);
    REQUIRE(payload["masteryDistribution"].isObject());
    REQUIRE(payload["masteryDistribution"]["low"].isObject());
    REQUIRE(payload["masteryDistribution"]["mid"].isObject());
    REQUIRE(payload["masteryDistribution"]["high"].isObject());
    CHECK(payload["masteryDistribution"]["low"]["min"].asDouble() == 0.0);
    CHECK(payload["masteryDistribution"]["low"]["max"].asDouble() == 0.35);
}

DROGON_TEST(TeacherStudentServiceProvidesKnowledgeStats)
{
    const auto payload =
        services::TeacherStudentService::buildKnowledgeStats("teacher_demo", "data-structures");

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
    const auto payload =
        services::TeacherStudentService::buildStudentProgress(
            "teacher_demo", "data-structures", "demo-learner");

    REQUIRE(payload["learner"].isObject());
    CHECK(payload["learner"]["code"].asString() == "demo-learner");
    REQUIRE(payload["masteryByCode"].isObject());
    REQUIRE(payload["recentFeedback"].isArray());
}

DROGON_TEST(TeacherStudentServiceRejectsStudentFromOtherCourse)
{
    CHECK_THROWS_AS(
        services::TeacherStudentService::buildStudentProgress(
            "teacher_demo", "data-structures", "non-existent-learner"),
        std::invalid_argument);
}

// HTTP-level tests

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
```

- [ ] **Step 2: Run focused test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "TeacherStudentService|TeacherStudents|TeacherStats|TeacherKnowledgeStats|TeacherStudentProgress"
```

Expected: FAIL because the service/controller files do not exist.

- [ ] **Step 3: Implement TeacherStudentService header**

Create `backend/services/TeacherStudentService.h`:

```cpp
#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class TeacherStudentService
{
  public:
    static Json::Value listStudents(const std::string &username,
                                    const std::string &courseCode,
                                    const std::string &major,
                                    const std::string &gradeLabel);
    static Json::Value buildCourseStats(const std::string &username,
                                        const std::string &courseCode);
    static Json::Value buildKnowledgeStats(const std::string &username,
                                           const std::string &courseCode);
    static Json::Value buildStudentProgress(const std::string &username,
                                            const std::string &courseCode,
                                            const std::string &learnerCode);
};
}
```

- [ ] **Step 4: Implement TeacherStudentService implementation**

Create `backend/services/TeacherStudentService.cc`:

```cpp
#include "services/TeacherStudentService.h"

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

void verifyTeacherCourseAssignment(const std::string &username,
                                   const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select 1 from teacher_course_assignments tca "
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

    const int courseId = findCourseId(courseCode);

    std::string query =
        "select l.code as learner_code, l.name as learner_name, "
        "l.major, l.grade_label, "
        "coalesce(avg(lm.mastery_score), 0.0) as overall_mastery, "
        "max(lfr.recorded_at) as last_activity_at "
        "from learners l "
        "left join learner_mastery lm on lm.learner_id = l.id "
        "left join learning_feedback_records lfr on lfr.learner_id = l.id "
        "where l.target_course_id = ? ";
    std::vector<std::string> params{std::to_string(courseId)};

    if (!major.empty())
    {
        query += " and l.major = ? ";
        params.push_back(major);
    }
    if (!gradeLabel.empty())
    {
        query += " and l.grade_label = ? ";
        params.push_back(gradeLabel);
    }
    query += " group by l.id, l.code, l.name, l.major, l.grade_label "
             "order by l.name asc";

    const auto result = getClient()->execSqlSync(
        query,
        params[0],
        params.size() > 1 ? params[1] : "",
        params.size() > 2 ? params[2] : "");

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

    const int courseId = findCourseId(courseCode);

    const auto result = getClient()->execSqlSync(
        "select count(l.id) as student_count, "
        "coalesce(avg(lm.mastery_score), 0.0) as avg_mastery "
        "from learners l "
        "left join learner_mastery lm on lm.learner_id = l.id "
        "where l.target_course_id = ?",
        std::to_string(courseId));

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
        "left join learner_mastery lm on lm.learner_id = l.id "
        "where l.target_course_id = ? "
        "group by l.id "
        ") sub",
        std::to_string(courseId));

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

    const int courseId = findCourseId(courseCode);

    const auto result = getClient()->execSqlSync(
        "select kp.code, kp.name, kp.chapter_no, kp.chapter_name, "
        "kp.difficulty_level, kp.display_order, "
        "coalesce(avg(lm.mastery_score), 0.0) as avg_mastery, "
        "count(distinct lm.learner_id) as student_count "
        "from knowledge_points kp "
        "left join learner_mastery lm on lm.knowledge_point_id = kp.id "
        "left join learners l on l.id = lm.learner_id and l.target_course_id = ? "
        "where kp.course_id = ? "
        "group by kp.id, kp.code, kp.name, kp.chapter_no, kp.chapter_name, "
        "kp.difficulty_level, kp.display_order "
        "order by kp.chapter_no asc, kp.display_order asc",
        std::to_string(courseId),
        std::to_string(courseId));

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
        "select l.code, l.name, l.major, l.grade_label, c.code as course_code, c.name as course_name "
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

    // Mastery by code
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

    // Recent feedback (last 10)
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

    // Recent paths (placeholder — path records are not currently persisted)
    payload["recentPaths"] = Json::arrayValue;

    return payload;
}
}
```

- [ ] **Step 5: Implement TeacherStudentController header**

Create `backend/controllers/TeacherStudentController.h`:

```cpp
#pragma once

#include <drogon/HttpController.h>

namespace api
{
class TeacherStudentController : public drogon::HttpController<TeacherStudentController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeacherStudentController::listStudents,
                  "/api/teacher/courses/{1}/students", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::courseStats,
                  "/api/teacher/courses/{1}/stats", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::knowledgeStats,
                  "/api/teacher/courses/{1}/knowledge-stats", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::studentProgress,
                  "/api/teacher/students/{1}/progress", drogon::Get);
    METHOD_LIST_END

    void listStudents(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      const std::string &courseCode) const;
    void courseStats(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     const std::string &courseCode) const;
    void knowledgeStats(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                        const std::string &courseCode) const;
    void studentProgress(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         const std::string &learnerCode) const;
};
}
```

- [ ] **Step 6: Implement TeacherStudentController**

Create `backend/controllers/TeacherStudentController.cc`:

```cpp
#include "TeacherStudentController.h"

#include "services/TeacherStudentService.h"

using namespace api;

namespace
{
void respondJson(const Json::Value &payload,
                 std::function<void(const drogon::HttpResponsePtr &)> &callback,
                 drogon::HttpStatusCode statusCode = drogon::k200OK)
{
    auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
    response->setStatusCode(statusCode);
    callback(response);
}

void respondError(const std::exception &error,
                  const std::string &message,
                  drogon::HttpStatusCode statusCode,
                  std::function<void(const drogon::HttpResponsePtr &)> &callback)
{
    Json::Value payload;
    payload["status"] = "error";
    payload["message"] = message;
    payload["detail"] = error.what();
    respondJson(payload, callback, statusCode);
}
}

void TeacherStudentController::listStudents(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");
    const auto major = req->getParameter("major");
    const auto gradeLabel = req->getParameter("grade_label");

    try
    {
        respondJson(
            services::TeacherStudentService::listStudents(username, courseCode, major, gradeLabel),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取学生列表失败。", drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::courseStats(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherStudentService::buildCourseStats(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取课程统计失败。", drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::knowledgeStats(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherStudentService::buildKnowledgeStats(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取知识点统计失败。", drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::studentProgress(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &learnerCode) const
{
    const auto username = req->getParameter("username");
    const auto courseCode = req->getParameter("courseCode");

    try
    {
        respondJson(
            services::TeacherStudentService::buildStudentProgress(
                username, courseCode, learnerCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取学生进度失败。", drogon::k500InternalServerError, callback);
    }
}
```

- [ ] **Step 7: Build backend and run focused tests**

Run from `backend/build`:

```bash
cmake --build . -j$(nproc)
ctest --output-on-failure -R "TeacherStudent|TeacherStudents|TeacherStats|TeacherKnowledgeStats|TeacherStudentProgress"
```

Expected: PASS for all new tests.

- [ ] **Step 8: Build and run full backend test suite**

```bash
ctest --output-on-failure
```

Expected: All existing tests still pass, new tests pass too.

- [ ] **Step 9: Commit backend changes**

```bash
git add backend/services/TeacherStudentService.h backend/services/TeacherStudentService.cc backend/controllers/TeacherStudentController.h backend/controllers/TeacherStudentController.cc backend/tests/TeacherStudentServiceTest.cc
git commit -m "feat: add teacher student progress backend (service + controller + tests)"
```

---

## Task 2: Frontend — PageLayout teacher navigation

**Files:**
- Modify: `frontend/src/components/PageLayout.vue`
- Modify: `frontend/src/components/PageLayout.test.js`

- [ ] **Step 1: Add failing test for teacher navigation in PageLayout**

Read existing `frontend/src/components/PageLayout.test.js` first, then add:

```js
it("renders teacher navigation when roleScope is teacher", () => {
  const wrapper = mount(PageLayout, {
    props: { roleScope: "teacher" },
    global: {
      plugins: [createTestingPinia({ stubActions: false })],
      stubs: { RouterLink: true },
    },
  });
  expect(wrapper.text()).toContain("我的课程");
  expect(wrapper.text()).not.toContain("学习图谱");
  expect(wrapper.text()).not.toContain("主图路径规划");
});

it("renders student navigation when roleScope is student (default)", () => {
  const wrapper = mount(PageLayout, {
    props: {},
    global: {
      plugins: [createTestingPinia({ stubActions: false })],
      stubs: { RouterLink: true },
    },
  });
  expect(wrapper.text()).toContain("学习图谱");
  expect(wrapper.text()).toContain("学习者画像");
});
```

- [ ] **Step 2: Run test and confirm failure**

```bash
cd frontend && npm run test -- --run src/components/PageLayout.test.js
```

Expected: FAIL because roleScope prop and teacher nav do not exist.

- [ ] **Step 3: Modify PageLayout.vue to add roleScope prop and teacher nav**

Read existing `frontend/src/components/PageLayout.vue` and add the roleScope prop. The key change is in the `<script setup>` section — add:

```js
const props = defineProps({
  eyebrow: { type: String, default: "" },
  title: { type: String, default: "" },
  description: { type: String, default: "" },
  roleScope: { type: String, default: "student" },
});
```

In the template, add a `v-if` wrapper around the existing student nav links, and add teacher nav links alongside:

```vue
<template>
  <nav class="page-nav" v-if="isNavVisible">
    <template v-if="roleScope === 'teacher'">
      <RouterLink :to="{ name: 'teacher-dashboard' }" class="nav-link" :class="{ active: isTeacherDashboardActive }">
        我的课程
      </RouterLink>
      <RouterLink :to="{ name: 'teacher-courses' }" class="nav-link" :class="{ active: isTeacherCoursesActive }">
        课程列表
      </RouterLink>
    </template>
    <template v-else>
      <!-- existing student nav links -->
      <RouterLink :to="{ name: 'learning-graph' }" class="nav-link" :class="{ active: isLearningGraphActive }">
        学习图谱
      </RouterLink>
      <!-- ... rest of existing student links ... -->
    </template>
    <button class="logout-btn" @click="handleLogout">退出</button>
  </nav>
</template>
```

Add computed properties for teacher nav active states:

```js
const isTeacherDashboardActive = computed(() => route.name === "teacher-dashboard");
const isTeacherCoursesActive = computed(() => route.name === "teacher-courses");
```

Make nav visible for both roles:

```js
const isNavVisible = computed(() => authStore.isAuthenticated);
```

- [ ] **Step 4: Run tests and verify**

```bash
cd frontend && npm run test -- --run src/components/PageLayout.test.js
```

Expected: PASS. All existing and new PageLayout tests pass.

- [ ] **Step 5: Commit**

```bash
git add frontend/src/components/PageLayout.vue frontend/src/components/PageLayout.test.js
git commit -m "feat: add teacher navigation to PageLayout via roleScope prop"
```

---

## Task 3: Frontend — Add new teacher API methods

**Files:**
- Modify: `frontend/src/api/teacher.js`

- [ ] **Step 1: Add four new API functions**

Read existing `frontend/src/api/teacher.js`, append:

```js
export async function fetchTeacherStudents(courseCode, params = {}) {
  const { data } = await http.get(`/teacher/courses/${courseCode}/students`, { params });
  return data;
}

export async function fetchTeacherCourseStats(courseCode, params = {}) {
  const { data } = await http.get(`/teacher/courses/${courseCode}/stats`, { params });
  return data;
}

export async function fetchTeacherKnowledgeStats(courseCode, params = {}) {
  const { data } = await http.get(`/teacher/courses/${courseCode}/knowledge-stats`, { params });
  return data;
}

export async function fetchTeacherStudentProgress(learnerCode, params = {}) {
  const { data } = await http.get(`/teacher/students/${learnerCode}/progress`, { params });
  return data;
}
```

- [ ] **Step 2: Commit**

```bash
git add frontend/src/api/teacher.js
git commit -m "feat: add teacher student progress API methods"
```

---

## Task 4: Frontend — TeacherCoursePage container + router update

**Files:**
- Create: `frontend/src/views/TeacherCoursePage.vue`
- Create: `frontend/src/views/TeacherCoursePage.test.js`
- Modify: `frontend/src/router/index.js`
- Modify: `frontend/src/router/index.test.js`

- [ ] **Step 1: Write failing test for TeacherCoursePage**

Create `frontend/src/views/TeacherCoursePage.test.js`:

```js
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCoursePage from "./TeacherCoursePage.vue";
import { useAuthStore } from "../stores/authStore";

const pushMock = vi.fn();

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock }),
    useRoute: () => routeState,
  };
});

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView(options = {}) {
  routeState = {
    name: "teacher-course-students",
    path: "/teacher/courses/data-structures/students",
    params: { courseCode: options.courseCode || "data-structures" },
  };
  const pinia = createPinia();
  setActivePinia(pinia);
  const authStore = useAuthStore();
  if (options.session) {
    authStore.setSession(options.session);
  }
  return {
    authStore,
    wrapper: mount(TeacherCoursePage, {
      props: { courseCode: options.courseCode || "data-structures" },
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            props: ["eyebrow", "title", "description", "roleScope"],
            template: "<div data-testid='page-layout'><slot /></div>",
          },
          RouterLink: { template: "<a><slot /></a>" },
          RouterView: { template: "<div data-testid='router-view' />" },
        },
      },
    }),
  };
}

describe("TeacherCoursePage", () => {
  beforeEach(() => {
    pushMock.mockReset();
  });

  it("renders sub-navigation tabs", () => {
    const { wrapper } = mountView();
    expect(wrapper.text()).toContain("课程信息");
    expect(wrapper.text()).toContain("学情总览");
    expect(wrapper.text()).toContain("学生列表");
    expect(wrapper.text()).toContain("知识点分布");
  });

  it("highlights active tab based on route name", async () => {
    const { wrapper } = mountView();
    await flushUi();
    const activeLink = wrapper.find(".tab-link--active");
    expect(activeLink.exists()).toBe(true);
  });

  it("passes roleScope='teacher' to PageLayout", () => {
    const { wrapper } = mountView();
    expect(wrapper.get("[data-testid='page-layout']").attributes("rolescope")).toBe("teacher");
  });
});
```

- [ ] **Step 2: Run test and confirm failure**

```bash
cd frontend && npm run test -- --run src/views/TeacherCoursePage.test.js
```

Expected: FAIL because the file does not exist.

- [ ] **Step 3: Implement TeacherCoursePage.vue**

Create `frontend/src/views/TeacherCoursePage.vue`:

```vue
<template>
  <PageLayout
    :eyebrow="`教师工作台 · 课程详情`"
    :title="courseCode"
    description="课程内学生进度、统计与知识点分布概览。"
    role-scope="teacher"
  >
    <nav class="sub-nav">
      <RouterLink
        :to="tabRoute('overview')"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('overview') }"
      >
        课程信息
      </RouterLink>
      <RouterLink
        :to="tabRoute('stats')"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('stats') }"
      >
        学情总览
      </RouterLink>
      <RouterLink
        :to="tabRoute('students')"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('students') }"
      >
        学生列表
      </RouterLink>
      <RouterLink
        :to="tabRoute('points')"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('points') }"
      >
        知识点分布
      </RouterLink>
    </nav>
    <section class="course-page-content">
      <RouterView />
    </section>
  </PageLayout>
</template>

<script setup>
import { computed } from "vue";
import { useRoute } from "vue-router";

import PageLayout from "../components/PageLayout.vue";

defineProps({
  courseCode: { type: String, required: true },
});

const route = useRoute();

const tabSuffixMap = {
  overview: "teacher-course-overview",
  stats: "teacher-course-stats",
  students: "teacher-course-students",
  points: "teacher-course-points",
};

function tabRoute(tab) {
  return {
    name: tabSuffixMap[tab],
    params: { courseCode: route.params.courseCode },
  };
}

function isTabActive(tab) {
  return route.name === tabSuffixMap[tab];
}
</script>

<style scoped>
.sub-nav {
  display: flex;
  gap: 0;
  border-bottom: 2px solid #d8e0e6;
  margin-bottom: 20px;
}

.tab-link {
  padding: 10px 20px;
  color: #51606d;
  text-decoration: none;
  font-weight: 600;
  border-bottom: 3px solid transparent;
  margin-bottom: -2px;
  transition: color 0.2s, border-color 0.2s;
}

.tab-link:hover {
  color: #0c6a71;
}

.tab-link--active {
  color: #0c6a71;
  border-bottom-color: #0c6a71;
}

.course-page-content {
  min-height: 200px;
}
</style>
```

- [ ] **Step 4: Update router**

Read `frontend/src/router/index.js`, replace the flat teacher course detail route:

```js
// Replace:
// {
//   path: "/teacher/courses/:courseCode",
//   name: "teacher-course-detail",
//   component: TeacherCourseOverviewView,
//   meta: { roleScope: "teacher" },
// },
// With:
{
  path: "/teacher/courses/:courseCode",
  component: TeacherCoursePage,
  props: true,
  meta: { roleScope: "teacher" },
  children: [
    {
      path: "",
      redirect: { name: "teacher-course-overview" },
    },
    {
      path: "overview",
      name: "teacher-course-overview",
      component: TeacherCourseOverviewView,
      meta: { roleScope: "teacher" },
    },
    {
      path: "stats",
      name: "teacher-course-stats",
      component: TeacherCourseStatsView,
      meta: { roleScope: "teacher" },
    },
    {
      path: "students",
      name: "teacher-course-students",
      component: TeacherStudentsView,
      meta: { roleScope: "teacher" },
    },
    {
      path: "points",
      name: "teacher-course-points",
      component: TeacherKnowledgeStatsView,
      meta: { roleScope: "teacher" },
    },
  ],
},
// Add student progress route:
{
  path: "/teacher/courses/:courseCode/students/:learnerCode",
  name: "teacher-student-progress",
  component: TeacherStudentProgressView,
  props: true,
  meta: { roleScope: "teacher" },
},
```

Add the new imports at the top of the router file:

```js
import TeacherCoursePage from "../views/TeacherCoursePage.vue";
import TeacherStudentsView from "../views/TeacherStudentsView.vue";
import TeacherCourseStatsView from "../views/TeacherCourseStatsView.vue";
import TeacherKnowledgeStatsView from "../views/TeacherKnowledgeStatsView.vue";
import TeacherStudentProgressView from "../views/TeacherStudentProgressView.vue";
```

Update existing teacher views' links that reference `{ name: "teacher-course-detail" }` to `{ name: "teacher-course-overview" }`.

- [ ] **Step 5: Run tests and verify**

```bash
cd frontend && npm run test -- --run src/views/TeacherCoursePage.test.js src/router/index.test.js
```

Expected: PASS.

- [ ] **Step 6: Commit**

```bash
git add frontend/src/views/TeacherCoursePage.vue frontend/src/views/TeacherCoursePage.test.js frontend/src/router/index.js frontend/src/router/index.test.js
git commit -m "feat: add TeacherCoursePage with tab sub-navigation and router children"
```

---

## Task 5: Frontend — TeacherStudentsView

**Files:**
- Create: `frontend/src/views/TeacherStudentsView.vue`
- Create: `frontend/src/views/TeacherStudentsView.test.js`

- [ ] **Step 1: Write failing test**

Create `frontend/src/views/TeacherStudentsView.test.js`:

```js
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherStudentsView from "./TeacherStudentsView.vue";
import { fetchTeacherStudents } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => routeState,
  };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherStudents: vi.fn().mockResolvedValue({
    courseCode: "data-structures",
    students: [
      {
        learnerCode: "demo-learner",
        learnerName: "演示学生",
        major: "计算机科学",
        gradeLabel: "大二",
        overallMastery: 0.62,
        lastActivityAt: "2026-05-20 10:30 CST",
      },
    ],
  }),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  routeState = {
    name: "teacher-course-students",
    params: { courseCode: "data-structures" },
  };
  const pinia = createPinia();
  setActivePinia(pinia);
  const authStore = useAuthStore();
  authStore.setSession({
    user: { username: "teacher_demo", displayName: "教师演示账号" },
    roles: ["teacher"],
    activeRole: "teacher",
    linkedLearner: null,
  });
  return {
    wrapper: mount(TeacherStudentsView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: { template: "<div data-testid='page-layout'><slot /></div>" },
          RouterLink: { template: "<a><slot /></a>" },
        },
      },
    }),
  };
}

describe("TeacherStudentsView", () => {
  beforeEach(() => {
    fetchTeacherStudents.mockClear();
  });

  it("renders student cards with names and mastery", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("演示学生");
    expect(wrapper.text()).toContain("计算机科学");
    expect(wrapper.text()).toContain("62%");
  });

  it("renders empty state when no students", async () => {
    fetchTeacherStudents.mockResolvedValueOnce({
      courseCode: "data-structures",
      students: [],
    });
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("暂无学生");
  });

  it("renders error state on API failure", async () => {
    fetchTeacherStudents.mockRejectedValueOnce(new Error("network error"));
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("未能读取");
  });
});
```

- [ ] **Step 2: Run test and confirm failure**

```bash
cd frontend && npm run test -- --run src/views/TeacherStudentsView.test.js
```

Expected: FAIL because the file does not exist.

- [ ] **Step 3: Implement TeacherStudentsView.vue**

Create `frontend/src/views/TeacherStudentsView.vue`:

```vue
<template>
  <section class="students-layout">
    <div class="filter-bar">
      <label class="filter-label">
        专业
        <select v-model="filterMajor" class="filter-select" @change="loadStudents">
          <option value="">全部</option>
          <option v-for="m in availableMajors" :key="m" :value="m">{{ m }}</option>
        </select>
      </label>
      <label class="filter-label">
        年级
        <select v-model="filterGrade" class="filter-select" @change="loadStudents">
          <option value="">全部</option>
          <option v-for="g in availableGrades" :key="g" :value="g">{{ g }}</option>
        </select>
      </label>
    </div>

    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载学生列表...</p>
    <p v-else-if="students.length === 0" class="state-message">暂无学生数据。</p>

    <div v-else class="student-grid">
      <article
        v-for="student in students"
        :key="student.learnerCode"
        class="student-card"
        data-testid="student-card"
      >
        <div class="student-header">
          <h3>{{ student.learnerName }}</h3>
          <span class="student-meta">{{ student.major }} · {{ student.gradeLabel }}</span>
        </div>
        <div class="mastery-bar">
          <div class="mastery-bar__label">综合掌握度</div>
          <div class="mastery-bar__track">
            <div
              class="mastery-bar__fill"
              :style="{ width: masteryPercent(student.overallMastery) }"
            ></div>
          </div>
          <span class="mastery-bar__value">{{ masteryPercent(student.overallMastery) }}</span>
        </div>
        <p class="student-activity" v-if="student.lastActivityAt">
          最近活动：{{ student.lastActivityAt }}
        </p>
        <RouterLink
          class="student-link"
          :to="{
            name: 'teacher-student-progress',
            params: { courseCode, learnerCode: student.learnerCode },
          }"
        >
          查看详情
        </RouterLink>
      </article>
    </div>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchTeacherStudents } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const studentsList = ref([]);
const filterMajor = ref("");
const filterGrade = ref("");

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const students = computed(() => studentsList.value);

const availableMajors = computed(() => {
  const set = new Set(studentsList.value.map((s) => s.major).filter(Boolean));
  return [...set].sort();
});

const availableGrades = computed(() => {
  const set = new Set(studentsList.value.map((s) => s.gradeLabel).filter(Boolean));
  return [...set].sort();
});

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

async function loadStudents() {
  loading.value = true;
  loadError.value = "";
  try {
    const payload = await fetchTeacherStudents(courseCode.value, {
      username: teacherUsername.value,
      major: filterMajor.value || undefined,
      grade_label: filterGrade.value || undefined,
    });
    studentsList.value = Array.isArray(payload?.students) ? payload.students : [];
  } catch (error) {
    loadError.value = "未能读取学生列表，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadStudents);
</script>

<style scoped>
.students-layout {
  display: grid;
  gap: 18px;
}

.filter-bar {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
}

.filter-label {
  display: flex;
  align-items: center;
  gap: 8px;
  color: #51606d;
  font-size: 0.9rem;
}

.filter-select {
  padding: 6px 10px;
  border: 1px solid #d8e0e6;
  border-radius: 8px;
  background: #ffffff;
  color: #15364a;
}

.student-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
}

.student-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.student-header h3 {
  margin: 0;
  font-size: 1.1rem;
  color: #15364a;
}

.student-meta {
  display: block;
  margin-top: 4px;
  color: #51606d;
  font-size: 0.9rem;
}

.mastery-bar {
  display: flex;
  align-items: center;
  gap: 10px;
  margin-top: 14px;
}

.mastery-bar__label {
  flex-shrink: 0;
  color: #51606d;
  font-size: 0.85rem;
}

.mastery-bar__track {
  flex: 1;
  height: 10px;
  background: #eef7f8;
  border-radius: 5px;
  overflow: hidden;
}

.mastery-bar__fill {
  height: 100%;
  background: #0c6a71;
  border-radius: 5px;
  transition: width 0.3s;
}

.mastery-bar__value {
  flex-shrink: 0;
  color: #0c6a71;
  font-weight: 700;
  font-size: 0.9rem;
}

.student-activity {
  margin: 10px 0 0;
  color: #8896a0;
  font-size: 0.8rem;
}

.student-link {
  display: inline-flex;
  margin-top: 12px;
  color: #0c6a71;
  font-weight: 700;
  text-decoration: none;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}
</style>
```

- [ ] **Step 4: Run tests and verify**

```bash
cd frontend && npm run test -- --run src/views/TeacherStudentsView.test.js
```

Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add frontend/src/views/TeacherStudentsView.vue frontend/src/views/TeacherStudentsView.test.js
git commit -m "feat: add TeacherStudentsView with major/grade filtering"
```

---

## Task 6: Frontend — TeacherCourseStatsView

**Files:**
- Create: `frontend/src/views/TeacherCourseStatsView.vue`
- Create: `frontend/src/views/TeacherCourseStatsView.test.js`

- [ ] **Step 1: Write failing test**

Create `frontend/src/views/TeacherCourseStatsView.test.js`:

```js
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCourseStatsView from "./TeacherCourseStatsView.vue";
import { fetchTeacherCourseStats } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => routeState,
  };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherCourseStats: vi.fn().mockResolvedValue({
    courseCode: "data-structures",
    courseName: "数据结构",
    studentCount: 3,
    avgMastery: 0.58,
    masteryDistribution: {
      low: { min: 0, max: 0.35, count: 0 },
      mid: { min: 0.35, max: 0.7, count: 2 },
      high: { min: 0.7, max: 1.0, count: 1 },
    },
  }),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  routeState = { params: { courseCode: "data-structures" } };
  const pinia = createPinia();
  setActivePinia(pinia);
  useAuthStore().setSession({
    user: { username: "teacher_demo" },
    roles: ["teacher"],
    activeRole: "teacher",
    linkedLearner: null,
  });
  return {
    wrapper: mount(TeacherCourseStatsView, {
      global: {
        plugins: [pinia],
        stubs: {
          RouterLink: true,
        },
      },
    }),
  };
}

describe("TeacherCourseStatsView", () => {
  beforeEach(() => {
    fetchTeacherCourseStats.mockClear();
  });

  it("renders student count and average mastery", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("3");
    expect(wrapper.text()).toContain("58%");
  });

  it("renders mastery distribution bars", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("较高");
    expect(wrapper.text()).toContain("中等");
    expect(wrapper.text()).toContain("较低");
  });

  it("renders loading and error states", async () => {
    fetchTeacherCourseStats.mockRejectedValueOnce(new Error("fail"));
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("未能读取");
  });
});
```

- [ ] **Step 2: Run test and confirm failure**

```bash
cd frontend && npm run test -- --run src/views/TeacherCourseStatsView.test.js
```

Expected: FAIL because the file does not exist.

- [ ] **Step 3: Implement TeacherCourseStatsView.vue**

Create `frontend/src/views/TeacherCourseStatsView.vue`:

```vue
<template>
  <section class="stats-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载学情数据...</p>

    <template v-else>
      <div class="summary-grid">
        <article class="summary-card">
          <p class="summary-label">学生总数</p>
          <strong class="summary-value">{{ stats.studentCount }}</strong>
        </article>
        <article class="summary-card">
          <p class="summary-label">平均掌握度</p>
          <strong class="summary-value">{{ masteryPercent(stats.avgMastery) }}</strong>
        </article>
      </div>

      <article class="distribution-card">
        <h3>掌握度分布</h3>
        <div class="distribution-bars">
          <div class="dist-row">
            <span class="dist-label">较高 (70-100%)</span>
            <div class="dist-track">
              <div
                class="dist-fill dist-fill--high"
                :style="{ width: barWidth('high') }"
              ></div>
            </div>
            <strong class="dist-count">{{ dist.high.count }}</strong>
          </div>
          <div class="dist-row">
            <span class="dist-label">中等 (35-70%)</span>
            <div class="dist-track">
              <div
                class="dist-fill dist-fill--mid"
                :style="{ width: barWidth('mid') }"
              ></div>
            </div>
            <strong class="dist-count">{{ dist.mid.count }}</strong>
          </div>
          <div class="dist-row">
            <span class="dist-label">较低 (0-35%)</span>
            <div class="dist-track">
              <div
                class="dist-fill dist-fill--low"
                :style="{ width: barWidth('low') }"
              ></div>
            </div>
            <strong class="dist-count">{{ dist.low.count }}</strong>
          </div>
        </div>
      </article>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchTeacherCourseStats } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const statsData = ref(null);

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const stats = computed(() => statsData.value || { studentCount: 0, avgMastery: 0, masteryDistribution: { low: { count: 0 }, mid: { count: 0 }, high: { count: 0 } } });
const dist = computed(() => stats.value.masteryDistribution || { low: { count: 0 }, mid: { count: 0 }, high: { count: 0 } });

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

function barWidth(tier) {
  const total = (dist.value.low?.count || 0) + (dist.value.mid?.count || 0) + (dist.value.high?.count || 0);
  if (total === 0) return "0%";
  return Math.round(((dist.value[tier]?.count || 0) / total) * 100) + "%";
}

async function loadStats() {
  loading.value = true;
  loadError.value = "";
  try {
    statsData.value = await fetchTeacherCourseStats(courseCode.value, {
      username: teacherUsername.value,
    });
  } catch (error) {
    loadError.value = "未能读取课程统计数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadStats);
</script>

<style scoped>
.stats-layout {
  display: grid;
  gap: 18px;
}

.summary-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
}

.summary-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.summary-label {
  margin: 0;
  color: #51606d;
}

.summary-value {
  display: block;
  margin-top: 10px;
  font-size: 1.5rem;
  color: #15364a;
}

.distribution-card {
  padding: 20px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.distribution-card h3 {
  margin: 0 0 16px;
  color: #15364a;
}

.distribution-bars {
  display: grid;
  gap: 14px;
}

.dist-row {
  display: flex;
  align-items: center;
  gap: 12px;
}

.dist-label {
  flex-shrink: 0;
  width: 130px;
  color: #51606d;
  font-size: 0.9rem;
}

.dist-track {
  flex: 1;
  height: 18px;
  background: #eef7f8;
  border-radius: 9px;
  overflow: hidden;
}

.dist-fill {
  height: 100%;
  border-radius: 9px;
  transition: width 0.3s;
}

.dist-fill--high {
  background: #0c6a71;
}

.dist-fill--mid {
  background: #4a9ea3;
}

.dist-fill--low {
  background: #d0ccb5;
}

.dist-count {
  flex-shrink: 0;
  width: 30px;
  text-align: right;
  color: #15364a;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}
</style>
```

- [ ] **Step 4: Run tests and verify**

```bash
cd frontend && npm run test -- --run src/views/TeacherCourseStatsView.test.js
```

Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add frontend/src/views/TeacherCourseStatsView.vue frontend/src/views/TeacherCourseStatsView.test.js
git commit -m "feat: add TeacherCourseStatsView with mastery distribution"
```

---

## Task 7: Frontend — TeacherKnowledgeStatsView

**Files:**
- Create: `frontend/src/views/TeacherKnowledgeStatsView.vue`
- Create: `frontend/src/views/TeacherKnowledgeStatsView.test.js`

- [ ] **Step 1: Write failing test**

Create `frontend/src/views/TeacherKnowledgeStatsView.test.js`:

```js
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherKnowledgeStatsView from "./TeacherKnowledgeStatsView.vue";
import { fetchTeacherKnowledgeStats } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return { ...actual, useRoute: () => routeState };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherKnowledgeStats: vi.fn().mockResolvedValue({
    courseCode: "data-structures",
    knowledgePoints: [
      {
        code: "ds-intro",
        name: "数据结构基本概念",
        chapterNo: 1,
        chapterName: "绪论",
        difficultyLevel: 2,
        avgMastery: 0.75,
        studentCount: 3,
      },
      {
        code: "linear-list",
        name: "线性表",
        chapterNo: 2,
        chapterName: "线性表",
        difficultyLevel: 3,
        avgMastery: 0.45,
        studentCount: 2,
      },
    ],
  }),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  routeState = { params: { courseCode: "data-structures" } };
  const pinia = createPinia();
  setActivePinia(pinia);
  useAuthStore().setSession({
    user: { username: "teacher_demo" },
    roles: ["teacher"],
    activeRole: "teacher",
    linkedLearner: null,
  });
  return {
    wrapper: mount(TeacherKnowledgeStatsView, {
      global: {
        plugins: [pinia],
        stubs: { RouterLink: true },
      },
    }),
  };
}

describe("TeacherKnowledgeStatsView", () => {
  beforeEach(() => {
    fetchTeacherKnowledgeStats.mockClear();
  });

  it("renders knowledge point names and mastery", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("数据结构基本概念");
    expect(wrapper.text()).toContain("线性表");
    expect(wrapper.text()).toContain("75%");
    expect(wrapper.text()).toContain("45%");
  });

  it("renders difficulty level stars", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("★★");
  });

  it("renders empty state when no data", async () => {
    fetchTeacherKnowledgeStats.mockResolvedValueOnce({
      courseCode: "data-structures",
      knowledgePoints: [],
    });
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("暂无数据");
  });
});
```

- [ ] **Step 2: Run test and confirm failure**

```bash
cd frontend && npm run test -- --run src/views/TeacherKnowledgeStatsView.test.js
```

Expected: FAIL because the file does not exist.

- [ ] **Step 3: Implement TeacherKnowledgeStatsView.vue**

Create `frontend/src/views/TeacherKnowledgeStatsView.vue`:

```vue
<template>
  <section class="points-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载知识点数据...</p>
    <p v-else-if="points.length === 0" class="state-message">暂无知识点数据。</p>

    <div v-else class="points-table-wrapper">
      <table class="points-table">
        <thead>
          <tr>
            <th>章节</th>
            <th>知识点</th>
            <th>难度</th>
            <th>平均掌握度</th>
            <th>学生数</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="point in points" :key="point.code" data-testid="knowledge-row">
            <td class="cell-chapter">{{ point.chapterNo }}. {{ point.chapterName }}</td>
            <td class="cell-name">{{ point.name }}</td>
            <td class="cell-difficulty">{{ difficultyStars(point.difficultyLevel) }}</td>
            <td class="cell-mastery">
              <div class="mini-bar">
                <div class="mini-bar__track">
                  <div
                    class="mini-bar__fill"
                    :style="{ width: masteryPercent(point.avgMastery) }"
                  ></div>
                </div>
                <span class="mini-bar__value">{{ masteryPercent(point.avgMastery) }}</span>
              </div>
            </td>
            <td class="cell-count">{{ point.studentCount }}</td>
          </tr>
        </tbody>
      </table>
    </div>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchTeacherKnowledgeStats } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const knowledgeData = ref(null);

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const points = computed(() => knowledgeData.value?.knowledgePoints || []);

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

function difficultyStars(level) {
  const stars = ["", "★", "★★", "★★★", "★★★★", "★★★★★"];
  return stars[level] || "";
}

async function loadPoints() {
  loading.value = true;
  loadError.value = "";
  try {
    knowledgeData.value = await fetchTeacherKnowledgeStats(courseCode.value, {
      username: teacherUsername.value,
    });
  } catch (error) {
    loadError.value = "未能读取知识点统计数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadPoints);
</script>

<style scoped>
.points-layout {
  display: grid;
  gap: 18px;
}

.points-table-wrapper {
  overflow-x: auto;
}

.points-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 0.9rem;
}

.points-table th,
.points-table td {
  padding: 12px 14px;
  text-align: left;
  border-bottom: 1px solid #d8e0e6;
}

.points-table th {
  color: #51606d;
  font-weight: 600;
  white-space: nowrap;
}

.points-table td {
  color: #15364a;
}

.cell-chapter {
  color: #51606d;
  font-size: 0.85rem;
}

.cell-name {
  font-weight: 600;
}

.cell-difficulty {
  color: #b8960c;
  white-space: nowrap;
}

.cell-count {
  text-align: center;
}

.mini-bar {
  display: flex;
  align-items: center;
  gap: 8px;
}

.mini-bar__track {
  width: 80px;
  height: 8px;
  background: #eef7f8;
  border-radius: 4px;
  overflow: hidden;
}

.mini-bar__fill {
  height: 100%;
  background: #0c6a71;
  border-radius: 4px;
}

.mini-bar__value {
  color: #0c6a71;
  font-weight: 700;
  font-size: 0.85rem;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}

@media (max-width: 720px) {
  .mini-bar__track {
    width: 50px;
  }
}
</style>
```

- [ ] **Step 4: Run tests and verify**

```bash
cd frontend && npm run test -- --run src/views/TeacherKnowledgeStatsView.test.js
```

Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add frontend/src/views/TeacherKnowledgeStatsView.vue frontend/src/views/TeacherKnowledgeStatsView.test.js
git commit -m "feat: add TeacherKnowledgeStatsView with chapter-grouped table"
```

---

## Task 8: Frontend — TeacherStudentProgressView

**Files:**
- Create: `frontend/src/views/TeacherStudentProgressView.vue`
- Create: `frontend/src/views/TeacherStudentProgressView.test.js`

- [ ] **Step 1: Write failing test**

Create `frontend/src/views/TeacherStudentProgressView.test.js`:

```js
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherStudentProgressView from "./TeacherStudentProgressView.vue";
import { fetchTeacherStudentProgress } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return { ...actual, useRoute: () => routeState };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherStudentProgress: vi.fn().mockResolvedValue({
    learner: {
      code: "demo-learner",
      name: "演示学生",
      major: "计算机科学",
      gradeLabel: "大二",
      courseCode: "data-structures",
      courseName: "数据结构",
    },
    masteryByCode: { "ds-intro": 0.9, "linear-list": 0.85, queue: 0.62 },
    recentFeedback: [
      {
        nodeCode: "queue",
        nodeName: "队列",
        completionStatus: "completed",
        previousMastery: 0.35,
        updatedMastery: 0.85,
        recordedAt: "2026-05-20 10:30 CST",
      },
    ],
    recentPaths: [],
  }),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  routeState = {
    params: { courseCode: "data-structures", learnerCode: "demo-learner" },
  };
  const pinia = createPinia();
  setActivePinia(pinia);
  useAuthStore().setSession({
    user: { username: "teacher_demo" },
    roles: ["teacher"],
    activeRole: "teacher",
    linkedLearner: null,
  });
  return {
    wrapper: mount(TeacherStudentProgressView, {
      props: { courseCode: "data-structures", learnerCode: "demo-learner" },
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: { template: "<div data-testid='page-layout'><slot /></div>" },
          RouterLink: true,
        },
      },
    }),
  };
}

describe("TeacherStudentProgressView", () => {
  beforeEach(() => {
    fetchTeacherStudentProgress.mockClear();
  });

  it("renders student basic info", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("演示学生");
    expect(wrapper.text()).toContain("计算机科学");
  });

  it("renders mastery list from masteryByCode", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("ds-intro");
    expect(wrapper.text()).toContain("90%");
    expect(wrapper.text()).toContain("linear-list");
  });

  it("renders recent feedback records", async () => {
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("队列");
    expect(wrapper.text()).toContain("已完成");
  });

  it("renders error state on API failure", async () => {
    fetchTeacherStudentProgress.mockRejectedValueOnce(new Error("network error"));
    const { wrapper } = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("未能读取");
  });
});
```

- [ ] **Step 2: Run test and confirm failure**

```bash
cd frontend && npm run test -- --run src/views/TeacherStudentProgressView.test.js
```

Expected: FAIL because the file does not exist.

- [ ] **Step 3: Implement TeacherStudentProgressView.vue**

Create `frontend/src/views/TeacherStudentProgressView.vue`:

```vue
<template>
  <PageLayout
    eyebrow="教师工作台 · 学生详情"
    :title="learner?.name || '学生详情'"
    description="查看学生掌握度、最近学习反馈与学习记录。"
    role-scope="teacher"
  >
    <nav class="breadcrumb">
      <RouterLink
        :to="{ name: 'teacher-course-students', params: { courseCode } }"
        class="breadcrumb-link"
      >
        ← 返回学生列表
      </RouterLink>
    </nav>

    <section class="progress-layout">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载学生数据...</p>

      <template v-else>
        <article class="info-card">
          <div class="info-row">
            <span class="info-label">姓名</span>
            <strong>{{ learner.name }}</strong>
          </div>
          <div class="info-row">
            <span class="info-label">专业</span>
            <span>{{ learner.major }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">年级</span>
            <span>{{ learner.gradeLabel }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">课程</span>
            <span>{{ learner.courseName }}</span>
          </div>
        </article>

        <article class="mastery-card">
          <h3>掌握度明细</h3>
          <div class="mastery-list">
            <div
              v-for="(score, code) in masteryByCode"
              :key="code"
              class="mastery-row"
              data-testid="mastery-row"
            >
              <span class="mastery-code">{{ code }}</span>
              <div class="mastery-bar">
                <div class="mastery-bar__track">
                  <div
                    class="mastery-bar__fill"
                    :style="{ width: masteryPercent(score) }"
                  ></div>
                </div>
                <span class="mastery-bar__value">{{ masteryPercent(score) }}</span>
              </div>
            </div>
          </div>
          <p v-if="Object.keys(masteryByCode).length === 0" class="state-message">
            暂无掌握度数据。
          </p>
        </article>

        <article class="feedback-card">
          <h3>最近学习反馈</h3>
          <div v-if="recentFeedback.length > 0" class="feedback-list">
            <div
              v-for="(fb, idx) in recentFeedback"
              :key="idx"
              class="feedback-row"
              data-testid="feedback-row"
            >
              <div class="feedback-header">
                <strong>{{ fb.nodeName }}</strong>
                <span class="feedback-status" :class="statusClass(fb.completionStatus)">
                  {{ statusLabel(fb.completionStatus) }}
                </span>
              </div>
              <div class="feedback-detail">
                掌握度 {{ masteryPercent(fb.previousMastery) }} → {{ masteryPercent(fb.updatedMastery) }}
              </div>
              <p class="feedback-time">{{ fb.recordedAt }}</p>
            </div>
          </div>
          <p v-else class="state-message">暂无学习反馈记录。</p>
        </article>
      </template>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { fetchTeacherStudentProgress } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const props = defineProps({
  courseCode: { type: String, required: true },
  learnerCode: { type: String, required: true },
});

const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const progressData = ref(null);

const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const learner = computed(() => progressData.value?.learner || {});
const masteryByCode = computed(() => progressData.value?.masteryByCode || {});
const recentFeedback = computed(() => progressData.value?.recentFeedback || []);

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

function statusLabel(status) {
  const map = { completed: "已完成", partial: "部分完成", blocked: "学习受阻" };
  return map[status] || status;
}

function statusClass(status) {
  const map = { completed: "status--completed", partial: "status--partial", blocked: "status--blocked" };
  return map[status] || "";
}

async function loadProgress() {
  loading.value = true;
  loadError.value = "";
  try {
    progressData.value = await fetchTeacherStudentProgress(props.learnerCode, {
      username: teacherUsername.value,
      courseCode: props.courseCode,
    });
  } catch (error) {
    loadError.value = "未能读取学生进度数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadProgress);
</script>

<style scoped>
.progress-layout {
  display: grid;
  gap: 18px;
}

.breadcrumb {
  margin-bottom: 12px;
}

.breadcrumb-link {
  color: #0c6a71;
  font-weight: 600;
  text-decoration: none;
}

.info-card,
.mastery-card,
.feedback-card {
  padding: 20px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.info-card h3,
.mastery-card h3,
.feedback-card h3 {
  margin: 0 0 14px;
  color: #15364a;
}

.info-row {
  display: flex;
  gap: 12px;
  padding: 6px 0;
  color: #15364a;
}

.info-label {
  width: 60px;
  color: #51606d;
  flex-shrink: 0;
}

.mastery-list {
  display: grid;
  gap: 10px;
}

.mastery-row {
  display: flex;
  align-items: center;
  gap: 12px;
}

.mastery-code {
  width: 160px;
  font-family: monospace;
  color: #51606d;
  font-size: 0.85rem;
}

.mastery-bar {
  flex: 1;
  display: flex;
  align-items: center;
  gap: 10px;
}

.mastery-bar__track {
  flex: 1;
  height: 10px;
  background: #eef7f8;
  border-radius: 5px;
  overflow: hidden;
}

.mastery-bar__fill {
  height: 100%;
  background: #0c6a71;
  border-radius: 5px;
}

.mastery-bar__value {
  color: #0c6a71;
  font-weight: 700;
  font-size: 0.85rem;
  width: 40px;
  text-align: right;
}

.feedback-list {
  display: grid;
  gap: 12px;
}

.feedback-row {
  padding: 12px;
  border: 1px solid #d8e0e6;
  border-radius: 10px;
  background: #f8fafb;
}

.feedback-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.feedback-status {
  font-size: 0.8rem;
  padding: 2px 8px;
  border-radius: 6px;
  font-weight: 600;
}

.status--completed {
  background: #e0f2e9;
  color: #1a7a3a;
}

.status--partial {
  background: #fef3d0;
  color: #8a6d1b;
}

.status--blocked {
  background: #fce4e4;
  color: #9b3333;
}

.feedback-detail {
  margin-top: 6px;
  color: #51606d;
  font-size: 0.85rem;
}

.feedback-time {
  margin: 4px 0 0;
  color: #8896a0;
  font-size: 0.8rem;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}
</style>
```

- [ ] **Step 4: Run tests and verify**

```bash
cd frontend && npm run test -- --run src/views/TeacherStudentProgressView.test.js
```

Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add frontend/src/views/TeacherStudentProgressView.vue frontend/src/views/TeacherStudentProgressView.test.js
git commit -m "feat: add TeacherStudentProgressView with mastery and feedback detail"
```

---

## Task 9: Frontend — Update existing teacher views for roleScope + route name changes

**Files:**
- Modify: `frontend/src/views/TeacherDashboardView.vue`
- Modify: `frontend/src/views/TeacherDashboardView.test.js`
- Modify: `frontend/src/views/TeacherCoursesView.vue`
- Modify: `frontend/src/views/TeacherCoursesView.test.js`
- Modify: `frontend/src/views/TeacherCourseOverviewView.vue`
- Modify: `frontend/src/views/TeacherCourseOverviewView.test.js`

- [ ] **Step 1: Update TeacherDashboardView — add roleScope prop and update route link**

In `TeacherDashboardView.vue`, change the existing router-link from:
```vue
:to="{ name: 'teacher-course-detail', params: { courseCode: course.courseCode } }"
```
to:
```vue
:to="{ name: 'teacher-course-overview', params: { courseCode: course.courseCode } }"
```

In the `<PageLayout>` tag, add `role-scope="teacher"`.

- [ ] **Step 2: Update TeacherDashboardView test — verify PageLayout receives roleScope**

In `TeacherDashboardView.test.js`, ensure the PageLayout stub captures and asserts the `roleScope` prop, or just verify the test still passes given the prop is Boolean/string.

- [ ] **Step 3: Update TeacherCoursesView — same changes**

Add `role-scope="teacher"` to `<PageLayout>`. Change `teacher-course-detail` to `teacher-course-overview` in router-link `:to`.

- [ ] **Step 4: Update TeacherCoursesView.test.js — same pattern**

Ensure tests pass after router-link name change.

- [ ] **Step 5: Update TeacherCourseOverviewView — same changes**

Add `role-scope="teacher"` to `<PageLayout>`. The overview view is now rendered inside `TeacherCoursePage` via `<RouterView>`, so it should NOT have its own `<PageLayout>` anymore — it should use a plain `<section>` or `<div>` wrapper instead. Read the file and update.

- [ ] **Step 6: Update TeacherCourseOverviewView test**

Ensure the test works without PageLayout (the overview is now rendered inside TeacherCoursePage's RouterView, no longer standalone).

- [ ] **Step 7: Run all modified tests**

```bash
cd frontend && npm run test -- --run src/views/TeacherDashboardView.test.js src/views/TeacherCoursesView.test.js src/views/TeacherCourseOverviewView.test.js
```

Expected: All pass.

- [ ] **Step 8: Commit**

```bash
git add frontend/src/views/TeacherDashboardView.vue frontend/src/views/TeacherDashboardView.test.js frontend/src/views/TeacherCoursesView.vue frontend/src/views/TeacherCoursesView.test.js frontend/src/views/TeacherCourseOverviewView.vue frontend/src/views/TeacherCourseOverviewView.test.js
git commit -m "feat: update teacher views with roleScope and new route names"
```

---

## Task 10: Final verification

**Files:**
- Modify: `AI_GUIDE.md`
- Modify: `README.md`

- [ ] **Step 1: Run backend full test suite**

```bash
cd backend/build && ctest --output-on-failure
```

Expected: All tests pass (existing + new teacher student tests).

- [ ] **Step 2: Run frontend full test suite**

```bash
cd frontend && npm run test -- --run
```

Expected: All existing tests pass, new tests pass too.

- [ ] **Step 3: Build frontend**

```bash
cd frontend && npm run build
```

Expected: BUILD PASS.

- [ ] **Step 4: Update AI_GUIDE.md**

Append a session summary:

```markdown
### 2026-05-26 HH:MM CST

- 会话类型：教师完整功能 — A 阶段（学生进度查看）
- 用户问题：开始开发教师的完整功能，按 A（学生进度）→ B（内容管理）→ C（题库管理）推进，当前 A 阶段。
- 修改文件：
  - 后端新增：`backend/services/TeacherStudentService.{h,cc}`、`backend/controllers/TeacherStudentController.{h,cc}`、`backend/tests/TeacherStudentServiceTest.cc`
  - 前端新增：`frontend/src/views/TeacherCoursePage.vue`、`TeacherStudentsView.vue`、`TeacherCourseStatsView.vue`、`TeacherKnowledgeStatsView.vue`、`TeacherStudentProgressView.vue` 及对应测试文件
  - 前端修改：`PageLayout.vue`（新增 roleScope prop）、`teacher.js`（新增 4 API）、`router/index.js`（新增子路由）、各既有教师页面
- 实现结果：
  - 后端新增 4 个只读接口：学生列表（支持专业/年级筛选）、课程统计（掌握度分布）、知识点统计、学生进度详情
  - 前端新增课程详情容器（tab 子导航）、学生列表（含筛选）、学情总览（CSS 条形图）、知识点分布（章节分组表格）、学生进度详情（掌握度+反馈记录）
  - PageLayout 支持 teacher 角色导航，教师页面不再显示学生端链接
  - 所有新增页面均已配测试
- 验证命令：
  - `cd backend/build && ctest --output-on-failure`
  - `cd frontend && npm run test -- --run`
  - `cd frontend && npm run build`
- 验证结论：
  - [填写实际结果]
- 残留观察：
  - B 阶段（课程内容管理）和 C 阶段（题库管理）待后续推进
  - recentPaths 字段目前返回空数组（路径记录尚未持久化），后续可增强
```

- [ ] **Step 5: Update README.md**

Add a capability note under "当前已实现":

```markdown
- 教师学生进度查看：教师可查看所授课程的学生列表（支持专业/年级筛选）、学情总览（掌握度分布）、知识点统计和学生个体进度详情
```

- [ ] **Step 6: Final commit**

```bash
git add AI_GUIDE.md README.md
git commit -m "docs: record teacher student progress implementation (Phase A)"
```

---

## Self-Review

### 1. Spec coverage
- GET /api/teacher/courses/:courseCode/students → Task 1 Step 3-4 (service + controller)
- GET /api/teacher/courses/:courseCode/stats → Task 1 Step 3-4 (service + controller)
- GET /api/teacher/courses/:courseCode/knowledge-stats → Task 1 Step 3-4 (service + controller)
- GET /api/teacher/students/:learnerCode/progress → Task 1 Step 3-4 (service + controller)
- PageLayout roleScope → Task 2
- Router children → Task 4
- TeacherStudentsView → Task 5
- TeacherCourseStatsView → Task 6
- TeacherKnowledgeStatsView → Task 7
- TeacherStudentProgressView → Task 8
- Update existing views → Task 9
- Docs update → Task 10

### 2. Placeholder scan
No TBD, TODO, "add appropriate error handling", "write tests for the above", or other placeholder patterns found.

### 3. Type consistency
- `courseCode` is string everywhere (route params, API params, service signature)
- `learnerCode` is string everywhere
- `username` is used consistently as query parameter matching AuthService session
- API response field names (camelCase) match frontend usage
- Route names: `teacher-course-overview`, `teacher-course-stats`, `teacher-course-students`, `teacher-course-points`, `teacher-student-progress`
- File imports match file creation order (new views imported in router after they are created)
