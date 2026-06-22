# Teacher Resource and Question Database Phase 1 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace teacher-side JSON-backed resource/question management with SQLite-backed tables, automatic JSON migration, and table-based teacher UI while keeping the current course-centered product structure intact.

**Architecture:** Add three new SQLite tables (`learning_resources`, `question_banks`, `questions`) plus backend services/controllers that expose standard CRUD endpoints and migrate existing JSON data on demand. Refactor the teacher resource/questions pages from whole-document editing into filterable table views with create/edit dialogs, while preserving the existing `/teacher/courses/:courseCode/*` navigation shell and keeping student-side consumers compatible through backend response shaping.

**Tech Stack:** Drogon (C++17/20), SQLite, JsonCpp, Vue 3, Vue Router, Vitest, existing backend Drogon test suite

---

## Scope

This plan covers the first fully usable database-backed release of teacher resource/question management:

1. SQLite schema expansion for resources, banks, and questions
2. Automatic migration from existing JSON files into the new tables
3. Backend CRUD endpoints for teacher resource/bank/question management
4. Teacher resource page refactor to a table-based CRUD UI
5. Teacher question page refactor to bank list + question table CRUD UI
6. Compatibility updates so backend resource/question readers use database data

This plan intentionally does not introduce advanced paper generation, grading engines, or a separate admin content platform.

## File Map

### Backend files to modify

- `database/init/001_schema.sql`
  Responsibility: add the new `learning_resources`, `question_banks`, and `questions` tables plus indexes.

- `backend/services/TeacherCourseEditService.h`
  Responsibility: replace whole-document resource read/write API with resource CRUD service signatures and list payloads.

- `backend/services/TeacherCourseEditService.cc`
  Responsibility: implement DB-backed teacher resource CRUD, database reads, course/knowledge-point validation, and automatic resource migration from JSON.

- `backend/controllers/TeacherCourseEditController.h`
  Responsibility: register resource CRUD routes (`GET/POST/PUT/DELETE`) instead of only `GET/PUT`.

- `backend/controllers/TeacherCourseEditController.cc`
  Responsibility: parse request bodies/route params and call the new resource CRUD service methods.

- `backend/services/TeacherQuestionBankService.h`
  Responsibility: add question-bank CRUD and question CRUD service signatures plus migration helper.

- `backend/services/TeacherQuestionBankService.cc`
  Responsibility: implement DB-backed bank/question CRUD, JSON migration, type normalization, and list payload shaping.

- `backend/controllers/TeacherQuestionBankController.h`
  Responsibility: register bank CRUD routes and nested question CRUD routes.

- `backend/controllers/TeacherQuestionBankController.cc`
  Responsibility: parse route params/body and dispatch bank/question CRUD methods.

- `backend/services/LearningResourceService.cc`
  Responsibility: switch resource catalog reads from JSON files to the new database tables while keeping existing student-facing payload shape.

- `frontend/src/api/teacher.js`
  Responsibility: replace “save whole payload” helpers with resource CRUD + bank/question CRUD helpers.

- `backend/tests/TeacherCourseEditServiceTest.cc`
  Responsibility: cover new teacher resource CRUD HTTP/service behavior and confirm JSON migration path.

- `backend/tests/PathPlanningServiceTest.cc`
  Responsibility: confirm student-facing resource recommendation still returns resources after DB migration.

- `backend/tests/CMakeLists.txt`
  Responsibility: include new test files for question-bank CRUD if split out.

### Backend files to create

- `backend/tests/TeacherQuestionBankServiceTest.cc`
  Responsibility: cover bank CRUD, question CRUD, multi-type payloads, and JSON migration.

### Frontend files to modify

- `frontend/src/views/TeacherResourcesView.vue`
  Responsibility: convert to table-style CRUD page with filters, list reloads, and dialog/drawer editing.

- `frontend/src/views/TeacherQuestionsView.vue`
  Responsibility: convert to bank list + question table CRUD UI with dynamic forms by question type.

- `frontend/src/views/TeacherResourcesView.test.js`
  Responsibility: verify table rendering, filtering, resource creation/edit/delete flow.

- `frontend/src/views/TeacherQuestionsView.test.js`
  Responsibility: verify bank rendering, question type switching, question CRUD flow.

### Docs files to modify

- `README.md`
  Responsibility: update implementation notes to describe database-backed resources/questions and new teacher management behavior.

## Task 1: Expand SQLite schema and verify it is loadable

**Files:**
- Modify: `database/init/001_schema.sql`
- Test: `backend/tests/TestSupport.cc` (implicit schema load), `backend/tests/TeacherCourseEditServiceTest.cc`

- [ ] **Step 1: Add failing schema assertions for the new tables**

Extend `backend/tests/TeacherCourseEditServiceTest.cc` with a test that queries SQLite metadata:

```cpp
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
}
```

- [ ] **Step 2: Run the focused schema test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R TeacherContentSchemaIncludesResourceAndQuestionTables
```

Expected: FAIL because the new tables do not exist yet.

- [ ] **Step 3: Add the new tables and indexes**

Update `database/init/001_schema.sql` with the DDL from the design spec:

```sql
CREATE TABLE IF NOT EXISTS learning_resources (...);
CREATE INDEX IF NOT EXISTS idx_learning_resources_course_point
    ON learning_resources(course_id, knowledge_point_id, status, display_order);

CREATE TABLE IF NOT EXISTS question_banks (...);
CREATE INDEX IF NOT EXISTS idx_question_banks_course_point
    ON question_banks(course_id, knowledge_point_id, status);

CREATE TABLE IF NOT EXISTS questions (...);
CREATE INDEX IF NOT EXISTS idx_questions_bank_status_order
    ON questions(bank_id, status, display_order);
```

Use the exact field list and constraints from `docs/superpowers/specs/2026-06-20-teacher-resource-question-database-design.md`.

- [ ] **Step 4: Re-run the focused schema test**

Run from `backend/build`:

```bash
ctest --output-on-failure -R TeacherContentSchemaIncludesResourceAndQuestionTables
```

Expected: PASS.

## Task 2: Implement DB-backed teacher resource CRUD with automatic JSON migration

**Files:**
- Modify: `backend/services/TeacherCourseEditService.h`
- Modify: `backend/services/TeacherCourseEditService.cc`
- Modify: `backend/controllers/TeacherCourseEditController.h`
- Modify: `backend/controllers/TeacherCourseEditController.cc`
- Modify: `backend/tests/TeacherCourseEditServiceTest.cc`

- [ ] **Step 1: Add failing resource CRUD service tests**

Add tests to `backend/tests/TeacherCourseEditServiceTest.cc`:

```cpp
DROGON_TEST(TeacherCourseEditServiceCreatesListsUpdatesAndDeletesResource)
{
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

    const auto created =
        services::TeacherCourseEditService::createResource(
            "teacher_demo", "data-structures", createBody);

    REQUIRE(created["created"].asBool() == true);
    const auto resourceId = created["resource"]["id"].asInt();

    const auto listed =
        services::TeacherCourseEditService::listResources(
            "teacher_demo", "data-structures", Json::Value(Json::objectValue));
    REQUIRE(listed["resources"].isArray());

    Json::Value updateBody;
    updateBody["title"] = "资源 CRUD 测试（已更新）";
    updateBody["recommendedPhase"] = "review";

    const auto updated =
        services::TeacherCourseEditService::updateResource(
            "teacher_demo", "data-structures", resourceId, updateBody);
    CHECK(updated["updated"].asBool() == true);

    const auto removed =
        services::TeacherCourseEditService::deleteResource(
            "teacher_demo", "data-structures", resourceId);
    CHECK(removed["deleted"].asBool() == true);
}
```

- [ ] **Step 2: Add failing HTTP tests for the resource CRUD routes**

Add tests that call:

```text
POST   /api/teacher/courses/data-structures/resources
GET    /api/teacher/courses/data-structures/resources
PUT    /api/teacher/courses/data-structures/resources/{id}
DELETE /api/teacher/courses/data-structures/resources/{id}
```

Use the existing `sendRequest(...)` helper and expect 201/200 statuses.

- [ ] **Step 3: Replace old resource service signatures with CRUD signatures**

In `backend/services/TeacherCourseEditService.h`, define:

```cpp
static Json::Value listResources(const std::string &username,
                                 const std::string &courseCode,
                                 const Json::Value &query);
static Json::Value createResource(const std::string &username,
                                  const std::string &courseCode,
                                  const Json::Value &body);
static Json::Value updateResource(const std::string &username,
                                  const std::string &courseCode,
                                  int resourceId,
                                  const Json::Value &body);
static Json::Value deleteResource(const std::string &username,
                                  const std::string &courseCode,
                                  int resourceId);
```

Remove the old `readResources` / `writeResources` declarations after all callers are updated.

- [ ] **Step 4: Implement automatic resource migration in the service**

Inside `backend/services/TeacherCourseEditService.cc`, add a helper like:

```cpp
void ensureResourceMigrationForCourse(int courseId, const std::string &courseCode)
{
    const auto countResult = getClient()->execSqlSync(
        "select count(*) as count from learning_resources where course_id = ?",
        std::to_string(courseId));
    if (!countResult.empty() && countResult.front()["count"].as<long long>() > 0)
    {
        return;
    }

    const auto root = readResourceFile(courseCode);
    const auto &groups = root["knowledgePointResources"];
    if (!groups.isArray())
    {
        return;
    }

    int displayOrder = 0;
    for (const auto &group : groups)
    {
        const auto knowledgePointCode = group["knowledgePointCode"].asString();
        const auto pointId = findKnowledgePointIdByCode(courseId, knowledgePointCode);
        for (const auto &resource : group["resources"])
        {
            getClient()->execSqlSync(
                "insert into learning_resources "
                "(course_id, knowledge_point_id, title, resource_type, source, url, "
                "description, recommended_usage, recommended_phase, focus_tags_json, "
                "display_order, status) "
                "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 'active')",
                std::to_string(courseId),
                pointId.has_value() ? std::to_string(*pointId) : nullptr,
                resource["title"].asString(),
                resource["type"].asString(),
                resource["source"].asString(),
                resource["url"].asString(),
                resource["description"].asString(),
                resource["recommendedUsage"].asString(),
                resource["recommendedPhase"].asString(),
                Json::writeString(Json::StreamWriterBuilder{}, resource["focusTags"]),
                std::to_string(displayOrder++));
        }
    }
}
```

Implement `findKnowledgePointIdByCode(...)` safely without throwing for missing codes so course-level resources remain possible.

- [ ] **Step 5: Implement resource list/create/update/delete against SQLite**

In `backend/services/TeacherCourseEditService.cc`:

- `listResources(...)` should:
  - verify teacher/course assignment
  - find the course id
  - call migration helper
  - query `learning_resources` left-joining `knowledge_points`
  - apply optional filters (`knowledgePointCode`, `resourceType`, `recommendedPhase`, `status`, `keyword`)
  - return payload:

```json
{
  "courseCode": "data-structures",
  "resources": [
    {
      "id": 1,
      "knowledgePointCode": "ds-intro",
      "knowledgePointName": "数据结构基本概念",
      "title": "资源标题",
      "resourceType": "article",
      "source": "平台",
      "url": "https://...",
      "description": "...",
      "recommendedUsage": "...",
      "recommendedPhase": "learn",
      "focusTags": [],
      "status": "active"
    }
  ]
}
```

- `createResource(...)` inserts a row and returns the inserted resource payload.
- `updateResource(...)` updates only the provided fields and returns updated payload.
- `deleteResource(...)` hard-deletes the row and returns `{ "deleted": true }`.

- [ ] **Step 6: Replace controller routes with CRUD routes**

In `backend/controllers/TeacherCourseEditController.h`, replace:

```cpp
ADD_METHOD_TO(...readResources..., "/api/teacher/courses/{1}/resources", drogon::Get);
ADD_METHOD_TO(...writeResources..., "/api/teacher/courses/{1}/resources", drogon::Put);
```

with:

```cpp
ADD_METHOD_TO(...listResources..., "/api/teacher/courses/{1}/resources", drogon::Get);
ADD_METHOD_TO(...createResource..., "/api/teacher/courses/{1}/resources", drogon::Post);
ADD_METHOD_TO(...updateResource..., "/api/teacher/courses/{1}/resources/{2}", drogon::Put);
ADD_METHOD_TO(...deleteResource..., "/api/teacher/courses/{1}/resources/{2}", drogon::Delete);
```

Parse `{2}` as `int resourceId`.

- [ ] **Step 7: Re-run the focused resource tests**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "TeacherCourseEditService.*Resource|TeacherCourseEditHttp.*Resources|TeacherContentSchemaIncludesResourceAndQuestionTables"
```

Expected: PASS.

## Task 3: Implement DB-backed question-bank + question CRUD with automatic JSON migration

**Files:**
- Modify: `backend/services/TeacherQuestionBankService.h`
- Modify: `backend/services/TeacherQuestionBankService.cc`
- Modify: `backend/controllers/TeacherQuestionBankController.h`
- Modify: `backend/controllers/TeacherQuestionBankController.cc`
- Create: `backend/tests/TeacherQuestionBankServiceTest.cc`
- Modify: `backend/tests/CMakeLists.txt`

- [ ] **Step 1: Create failing tests for bank and question CRUD**

Create `backend/tests/TeacherQuestionBankServiceTest.cc` with service tests like:

```cpp
DROGON_TEST(TeacherQuestionBankServiceCreatesBankAndMultiTypeQuestions)
{
    Json::Value bankBody;
    bankBody["knowledgePointCode"] = "ds-intro";
    bankBody["bankCode"] = "ds-intro-crud-bank";
    bankBody["bankName"] = "绪论测试题库";
    bankBody["description"] = "用于题库 CRUD 测试。";

    const auto createdBank =
        services::TeacherQuestionBankService::createQuestionBank(
            "teacher_demo", "data-structures", bankBody);
    REQUIRE(createdBank["created"].asBool() == true);
    const auto bankId = createdBank["questionBank"]["id"].asInt();

    Json::Value questionBody;
    questionBody["questionCode"] = "intro-multi-1";
    questionBody["questionType"] = "multiple_choice";
    questionBody["difficultyLevel"] = 3;
    questionBody["prompt"] = "以下哪些属于数据结构研究内容？";
    questionBody["options"] = Json::arrayValue;
    questionBody["options"].append("逻辑结构");
    questionBody["options"].append("存储结构");
    questionBody["options"].append("天气变化");
    questionBody["options"].append("运算");
    Json::Value answer;
    Json::Value indexes(Json::arrayValue);
    indexes.append(0);
    indexes.append(1);
    indexes.append(3);
    answer["correctIndexes"] = indexes;
    questionBody["answer"] = answer;
    questionBody["explanation"] = "数据结构研究逻辑结构、存储结构和运算。";
    questionBody["estimatedSeconds"] = 90;

    const auto createdQuestion =
        services::TeacherQuestionBankService::createQuestion(
            "teacher_demo", "data-structures", bankId, questionBody);
    REQUIRE(createdQuestion["created"].asBool() == true);

    const auto questions =
        services::TeacherQuestionBankService::listQuestions(
            "teacher_demo", "data-structures", bankId, Json::Value(Json::objectValue));
    REQUIRE(questions["questions"].isArray());
    REQUIRE(questions["questions"].size() >= 1U);
}
```

Also add tests for:

- `single_choice`
- `true_false`
- `short_answer`
- delete bank cascades question deletion
- unauthorized teacher rejection
- HTTP CRUD endpoints

- [ ] **Step 2: Wire the new test file into CMake and confirm failure**

Add `TeacherQuestionBankServiceTest.cc` to `backend/tests/CMakeLists.txt`, then run:

```bash
cd backend/build && cmake .. && cmake --build . && ctest --output-on-failure -R TeacherQuestionBankService
```

Expected: FAIL because service/controller methods do not exist yet.

- [ ] **Step 3: Replace whole-document API with bank/question CRUD signatures**

In `backend/services/TeacherQuestionBankService.h`, define:

```cpp
static Json::Value listQuestionBanks(const std::string &username,
                                     const std::string &courseCode,
                                     const Json::Value &query);
static Json::Value createQuestionBank(const std::string &username,
                                      const std::string &courseCode,
                                      const Json::Value &body);
static Json::Value updateQuestionBank(const std::string &username,
                                      const std::string &courseCode,
                                      int bankId,
                                      const Json::Value &body);
static Json::Value deleteQuestionBank(const std::string &username,
                                      const std::string &courseCode,
                                      int bankId);
static Json::Value listQuestions(const std::string &username,
                                 const std::string &courseCode,
                                 int bankId,
                                 const Json::Value &query);
static Json::Value createQuestion(const std::string &username,
                                  const std::string &courseCode,
                                  int bankId,
                                  const Json::Value &body);
static Json::Value updateQuestion(const std::string &username,
                                  const std::string &courseCode,
                                  int bankId,
                                  int questionId,
                                  const Json::Value &body);
static Json::Value deleteQuestion(const std::string &username,
                                  const std::string &courseCode,
                                  int bankId,
                                  int questionId);
```

- [ ] **Step 4: Implement automatic bank/question migration**

In `backend/services/TeacherQuestionBankService.cc`:

- if no banks exist for a course, read `question_bank_<courseCode>.json`
- for each `knowledgePointCode`, create a default bank
- map difficulty:

```cpp
int mapLegacyDifficulty(const std::string &difficulty)
{
    if (difficulty == "easy") return 1;
    if (difficulty == "medium") return 3;
    if (difficulty == "hard") return 5;
    return 2;
}
```

- normalize question type:
  - if legacy `type` missing, default to `single_choice`
  - if `type == "single-choice"`, store `single_choice`
- serialize old `options` to `options_json`
- convert old answer fields into `answer_json`

- [ ] **Step 5: Implement question-bank CRUD**

`listQuestionBanks(...)` returns:

```json
{
  "courseCode": "data-structures",
  "questionBanks": [
    {
      "id": 1,
      "knowledgePointCode": "ds-intro",
      "knowledgePointName": "数据结构基本概念",
      "bankCode": "ds-intro-default",
      "bankName": "数据结构基本概念题库",
      "description": "",
      "status": "active",
      "questionCount": 12
    }
  ]
}
```

Implement create/update/delete accordingly.

- [ ] **Step 6: Implement question CRUD**

`listQuestions(...)` should:

- verify teacher/course/bank consistency
- optionally filter by `questionType`, `difficultyLevel`, `status`, `keyword`
- deserialize `options_json` and `answer_json`
- return payload with frontend-ready field names:

```json
{
  "questionBank": { "id": 1, "bankCode": "ds-intro-default", "bankName": "..." },
  "questions": [
    {
      "id": 1,
      "questionCode": "intro-1",
      "questionType": "single_choice",
      "difficultyLevel": 1,
      "prompt": "题干",
      "options": ["A", "B", "C", "D"],
      "answer": { "correctIndexes": [1] },
      "explanation": "解析",
      "estimatedSeconds": 60,
      "status": "active"
    }
  ]
}
```

Create/update/delete should round-trip these same field names.

- [ ] **Step 7: Replace question controller routes**

In `backend/controllers/TeacherQuestionBankController.h`, register:

```cpp
GET    /api/teacher/courses/{1}/question-banks
POST   /api/teacher/courses/{1}/question-banks
PUT    /api/teacher/courses/{1}/question-banks/{2}
DELETE /api/teacher/courses/{1}/question-banks/{2}
GET    /api/teacher/courses/{1}/question-banks/{2}/questions
POST   /api/teacher/courses/{1}/question-banks/{2}/questions
PUT    /api/teacher/courses/{1}/question-banks/{2}/questions/{3}
DELETE /api/teacher/courses/{1}/question-banks/{2}/questions/{3}
```

- [ ] **Step 8: Re-run the focused question-bank tests**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "TeacherQuestionBankService|TeacherQuestionBankHttp"
```

Expected: PASS.

## Task 4: Switch student-facing resource reads to the new database

**Files:**
- Modify: `backend/services/LearningResourceService.cc`
- Modify: `backend/tests/PathPlanningServiceTest.cc`

- [ ] **Step 1: Add a failing compatibility test**

Extend `backend/tests/PathPlanningServiceTest.cc` to assert that path responses still contain resources after the new DB schema is active, without relying on JSON file shape assumptions.

Example:

```cpp
CHECK(item["resources"].isArray());
CHECK(item["resources"].size() >= 1);
CHECK(item["resources"][0]["title"].asString().empty() == false);
CHECK(item["resources"][0]["resourceCoverageType"].asString().empty() == false);
```

- [ ] **Step 2: Replace JSON catalog loading with database loading**

In `backend/services/LearningResourceService.cc`:

- keep current response composition logic as much as possible
- replace raw file-backed resource catalog loading with SQL reads from `learning_resources`
- preserve field names already expected by the student-facing resource recommendation/tests

Do not redesign recommendation logic here. Only swap the persistence source and keep behavior stable.

- [ ] **Step 3: Run the focused resource recommendation test slice**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "PathPlanningService|LearningResource"
```

Expected: PASS.

## Task 5: Refactor teacher resource page into DB-backed table CRUD UI

**Files:**
- Modify: `frontend/src/api/teacher.js`
- Modify: `frontend/src/views/TeacherResourcesView.vue`
- Modify: `frontend/src/views/TeacherResourcesView.test.js`

- [ ] **Step 1: Add failing frontend tests for table-based resource CRUD**

In `frontend/src/views/TeacherResourcesView.test.js`, add tests that verify:

- resources render as table rows, not cards
- knowledge-point filter narrows rows
- clicking “新增资源” opens form
- submitting create calls new POST API helper
- editing calls PUT helper
- deleting calls DELETE helper

Mock the API with:

```js
vi.mock("../api/teacher", () => ({
  fetchCourseResources: vi.fn(),
  createCourseResource: vi.fn(),
  updateCourseResource: vi.fn(),
  deleteCourseResource: vi.fn(),
}));
```

- [ ] **Step 2: Update teacher API helpers**

In `frontend/src/api/teacher.js`, replace the whole-payload saver with:

```js
export async function fetchCourseResources(courseCode, params = {}) { ... }
export async function createCourseResource(courseCode, body, params = {}) { ... }
export async function updateCourseResource(courseCode, resourceId, body, params = {}) { ... }
export async function deleteCourseResource(courseCode, resourceId, params = {}) { ... }
```

- [ ] **Step 3: Rewrite `TeacherResourcesView.vue` to a table layout**

Implement:

- toolbar filters: knowledge point / type / phase / keyword
- `table` with columns:
  - 标题
  - 知识点
  - 类型
  - 阶段
  - 来源
  - 状态
  - 操作
- create/edit dialog state
- per-row edit/delete actions
- immediate reload after CRUD success

Prefer an internal modal panel instead of browser `prompt`.

- [ ] **Step 4: Run the focused frontend resource tests**

Run from `frontend`:

```bash
npm run test -- --run TeacherResourcesView.test.js
```

Expected: PASS.

## Task 6: Refactor teacher question page into bank list + question table CRUD UI

**Files:**
- Modify: `frontend/src/api/teacher.js`
- Modify: `frontend/src/views/TeacherQuestionsView.vue`
- Modify: `frontend/src/views/TeacherQuestionsView.test.js`

- [ ] **Step 1: Add failing frontend tests for bank/question CRUD**

Mock new APIs:

```js
vi.mock("../api/teacher", () => ({
  fetchQuestionBanks: vi.fn(),
  createQuestionBank: vi.fn(),
  updateQuestionBank: vi.fn(),
  deleteQuestionBank: vi.fn(),
  fetchBankQuestions: vi.fn(),
  createBankQuestion: vi.fn(),
  updateBankQuestion: vi.fn(),
  deleteBankQuestion: vi.fn(),
}));
```

Cover:

- bank list renders on the left
- selecting a bank loads questions
- question table renders on the right
- create single-choice question works
- switching type to `true_false` hides generic option editing and shows boolean answer
- switching type to `short_answer` hides options and shows reference answer/keywords

- [ ] **Step 2: Add frontend API helpers for bank/question CRUD**

In `frontend/src/api/teacher.js`, add:

```js
export async function fetchQuestionBanks(courseCode, params = {}) { ... }
export async function createQuestionBank(courseCode, body, params = {}) { ... }
export async function updateQuestionBank(courseCode, bankId, body, params = {}) { ... }
export async function deleteQuestionBank(courseCode, bankId, params = {}) { ... }
export async function fetchBankQuestions(courseCode, bankId, params = {}) { ... }
export async function createBankQuestion(courseCode, bankId, body, params = {}) { ... }
export async function updateBankQuestion(courseCode, bankId, questionId, body, params = {}) { ... }
export async function deleteBankQuestion(courseCode, bankId, questionId, params = {}) { ... }
```

- [ ] **Step 3: Rewrite `TeacherQuestionsView.vue`**

Implement:

- left bank sidebar with create/edit/delete bank controls
- right question table with filters
- create/edit question dialog
- dynamic form sections by `questionType`
- mapping between frontend form model and backend `options`/`answer` payload

Use clear Chinese labels for the four types:

- 单选题
- 多选题
- 判断题
- 简答题

- [ ] **Step 4: Run the focused frontend question tests**

Run from `frontend`:

```bash
npm run test -- --run TeacherQuestionsView.test.js
```

Expected: PASS.

## Task 7: Final regression verification and documentation

**Files:**
- Modify: `README.md`

- [ ] **Step 1: Update README capability description**

Document that:

- teacher resources are now SQLite-backed
- teacher question banks/questions are SQLite-backed
- supported question types include single choice, multiple choice, true/false, short answer

- [ ] **Step 2: Run backend content-management regression tests**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "TeacherCourseEdit|TeacherQuestionBank|PathPlanningService|KnowledgeGraphService"
```

Expected: PASS.

- [ ] **Step 3: Run frontend teacher-page regression tests**

Run from `frontend`:

```bash
npm run test -- --run TeacherResourcesView.test.js TeacherQuestionsView.test.js
```

Expected: PASS.

- [ ] **Step 4: Run a concise manual smoke checklist**

Verify in the running app:

1. Teacher enters a course resource page and sees a table instead of cards.
2. Teacher can create, edit, and delete a resource.
3. Teacher enters a course questions page and sees a bank list + question table.
4. Teacher can create at least one `判断题` and one `简答题`.
5. Student-side resource recommendation still shows resources for a planned node.

- [ ] **Step 5: Commit**

```bash
git add database/init/001_schema.sql backend frontend README.md docs/superpowers/specs/2026-06-20-teacher-resource-question-database-design.md docs/superpowers/plans/2026-06-20-teacher-resource-question-database-phase1.md
git commit -m "feat: migrate teacher resources and questions to sqlite"
```
