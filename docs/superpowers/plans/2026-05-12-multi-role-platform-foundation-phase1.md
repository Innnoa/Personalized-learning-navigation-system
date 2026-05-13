# Multi-Role Platform Foundation Phase 1 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the first deliverable of the multi-role platform foundation: unified user identity, login/session restoration, role-based routing, student-to-learner binding, and minimal teacher/admin dashboards without rewriting the existing student learning workflow.

**Architecture:** Add a thin identity layer on top of the current learner-centric system. Backend changes introduce user/role/link tables plus lightweight auth/session and read-only dashboard endpoints; frontend changes add an auth store, login page, route guards, and role-specific entry pages while keeping existing student pages largely intact by resolving learner context from the logged-in student account.

**Tech Stack:** Drogon (C++20), SQLite, Vue 3, Vue Router, Pinia, Vitest, existing Drogon test suite

---

## Scope and decomposition

This plan intentionally covers only the first practical deliverable of the second-stage spec in `docs/superpowers/specs/2026-05-12-multi-role-platform-foundation-design.md`:

1. Unified user / role database foundation
2. Auth login / logout / session restore endpoints
3. Frontend login page + auth store + route guards
4. Student login wired into existing learner flow
5. Minimal teacher and admin dashboard/read-only list pages

This plan explicitly excludes:

1. User registration
2. Password reset flows
3. Full RBAC engine
4. Teacher-side content editing CRUD
5. Admin-side destructive CRUD
6. Question bank management implementation

Those belong to later plans after the skeleton platform flow is stable.

## File map

### Database and seed files

- Modify: `database/init/001_schema.sql`
  Responsibility: add `users`, `user_roles`, `user_learner_links`, `teacher_course_assignments`.
- Modify: `database/seed/course_data_structures.sql`
  Responsibility: insert demo student/teacher/admin accounts, user-role links, student-learner link, teacher-course assignment.

### Backend files to create

- Create: `backend/controllers/AuthController.h`
- Create: `backend/controllers/AuthController.cc`
  Responsibility: `POST /api/auth/login`, `POST /api/auth/logout`, `GET /api/auth/session`.
- Create: `backend/controllers/TeacherController.h`
- Create: `backend/controllers/TeacherController.cc`
  Responsibility: read-only teacher dashboard/course endpoints.
- Create: `backend/controllers/AdminController.h`
- Create: `backend/controllers/AdminController.cc`
  Responsibility: read-only admin dashboard/list endpoints.
- Create: `backend/services/AuthService.h`
- Create: `backend/services/AuthService.cc`
  Responsibility: user lookup, password verification, session payload assembly.
- Create: `backend/services/TeacherDashboardService.h`
- Create: `backend/services/TeacherDashboardService.cc`
  Responsibility: teacher course list and course overview payloads.
- Create: `backend/services/AdminDashboardService.h`
- Create: `backend/services/AdminDashboardService.cc`
  Responsibility: admin user list, course list, assignment overview payloads.
- Create: `backend/repositories/UserAccountRepository.h`
- Create: `backend/repositories/UserAccountRepository.cc`
  Responsibility: query `users`, `user_roles`, `user_learner_links`, `teacher_course_assignments`.
- Create: `backend/tests/AuthSessionServiceTest.cc`
  Responsibility: test auth/session and student-to-learner mapping behavior.
- Create: `backend/tests/RoleDashboardServiceTest.cc`
  Responsibility: test teacher/admin read-only dashboard payloads.

### Backend files to modify

- Modify: `backend/config/config.json`
  Responsibility: add minimal auth/session-related custom config if required, e.g. demo user defaults or cookie/session flags.
- Modify: `backend/main.cc`
  Responsibility: enable Drogon session support if the chosen auth/session mechanism uses it.
- Modify: `backend/tests/test_main.cc`
  Responsibility: seed or configure test-session defaults and demo account defaults.
- Modify: `backend/repositories/LearnerProfileRepository.h`
- Modify: `backend/repositories/LearnerProfileRepository.cc`
  Responsibility: add a small helper to resolve learner by code/id if needed for user-link lookup integration.

### Frontend files to create

- Create: `frontend/src/stores/authStore.js`
- Create: `frontend/src/stores/authStore.test.js`
  Responsibility: hold session user, role, linked learner, restore/clear behavior.
- Create: `frontend/src/api/auth.js`
  Responsibility: login, logout, session fetch.
- Create: `frontend/src/api/teacher.js`
  Responsibility: teacher read-only dashboard requests.
- Create: `frontend/src/api/admin.js`
  Responsibility: admin read-only dashboard requests.
- Create: `frontend/src/views/LoginView.vue`
- Create: `frontend/src/views/LoginView.test.js`
  Responsibility: username/password login + demo shortcuts.
- Create: `frontend/src/views/TeacherDashboardView.vue`
- Create: `frontend/src/views/TeacherDashboardView.test.js`
  Responsibility: teacher dashboard landing page.
- Create: `frontend/src/views/TeacherCoursesView.vue`
- Create: `frontend/src/views/TeacherCoursesView.test.js`
  Responsibility: teacher course list page.
- Create: `frontend/src/views/TeacherCourseOverviewView.vue`
- Create: `frontend/src/views/TeacherCourseOverviewView.test.js`
  Responsibility: teacher course overview page.
- Create: `frontend/src/views/AdminDashboardView.vue`
- Create: `frontend/src/views/AdminDashboardView.test.js`
  Responsibility: admin dashboard landing page.
- Create: `frontend/src/views/AdminUsersView.vue`
- Create: `frontend/src/views/AdminUsersView.test.js`
  Responsibility: admin user list page.
- Create: `frontend/src/views/AdminCoursesView.vue`
- Create: `frontend/src/views/AdminCoursesView.test.js`
  Responsibility: admin course list page.
- Create: `frontend/src/views/AdminAssignmentsView.vue`
- Create: `frontend/src/views/AdminAssignmentsView.test.js`
  Responsibility: admin teacher-course assignment page.

### Frontend files to modify

- Modify: `frontend/src/router/index.js`
  Responsibility: add login/teacher/admin routes and route guards.
- Modify: `frontend/src/views/HomeView.vue`
  Responsibility: read learner identity from auth store first instead of hard-coded `demo-learner`.
- Modify: `frontend/src/views/LearnerProfileView.vue`
- Modify: `frontend/src/views/LearningGraphView.vue`
- Modify: `frontend/src/views/DetailLearningView.vue`
- Modify: `frontend/src/views/ResourceRecommendationView.vue`
  Responsibility: resolve learner code from logged-in student session first, keep fallback only as migration safety.
- Modify: existing relevant tests for student pages when auth state becomes the new preferred source.

### Docs files to modify

- Modify: `README.md`
  Responsibility: record multi-role login, role routing, and dashboard scope.
- Modify: `AIREAD.md`
  Responsibility: append architecture/system record for unified users and role routing.

## Task 1: Add unified user/role schema and demo seed data

**Files:**
- Modify: `database/init/001_schema.sql`
- Modify: `database/seed/course_data_structures.sql`
- Test: `backend/tests/test_main.cc` (if seed defaults/config need exposure)

- [ ] **Step 1: Write a failing backend schema/seed test for demo users**

Add a new Drogon test in `backend/tests/AuthSessionServiceTest.cc` that will eventually assert the database contains:

1. `student_demo` with role `student`
2. `teacher_demo` with role `teacher`
3. `admin_demo` with role `admin`
4. `student_demo` linked to learner `demo-learner`
5. `teacher_demo` assigned to course `data-structures`

Use a direct DbClient query test like:

```cpp
DROGON_TEST(UserSeedProvidesDefaultDemoAccounts)
{
    auto db = drogon::app().getDbClient("sqlite_client");

    const auto users = db->execSqlSync(
        "select username from users where username in (?, ?, ?) order by username",
        "admin_demo",
        "student_demo",
        "teacher_demo");

    REQUIRE(users.size() == 3);
    CHECK(users[0]["username"].as<std::string>() == "admin_demo");
    CHECK(users[1]["username"].as<std::string>() == "student_demo");
    CHECK(users[2]["username"].as<std::string>() == "teacher_demo");
}
```

- [ ] **Step 2: Run the focused auth-schema test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R UserSeedProvidesDefaultDemoAccounts
```

Expected: FAIL because the `users` table and demo records do not yet exist.

- [ ] **Step 3: Extend the schema with unified identity tables**

Update `database/init/001_schema.sql` by appending:

```sql
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    display_name TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'active' CHECK (status IN ('active', 'disabled')),
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS user_roles (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    role_code TEXT NOT NULL CHECK (role_code IN ('student', 'teacher', 'admin')),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE (user_id, role_code)
);

CREATE TABLE IF NOT EXISTS user_learner_links (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL UNIQUE,
    learner_id INTEGER NOT NULL UNIQUE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (learner_id) REFERENCES learners(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS teacher_course_assignments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    UNIQUE (user_id, course_id)
);
```

Keep the schema additive. Do not rename `learners` or alter the learner-centric business tables in this task.

- [ ] **Step 4: Seed the three demo accounts and links**

Append seed rows to `database/seed/course_data_structures.sql`:

```sql
INSERT INTO users (username, password_hash, display_name, status)
VALUES
    ('student_demo', 'demo123', '学生演示账号', 'active'),
    ('teacher_demo', 'demo123', '教师演示账号', 'active'),
    ('admin_demo', 'demo123', '管理员演示账号', 'active')
ON CONFLICT(username) DO NOTHING;

INSERT INTO user_roles (user_id, role_code)
VALUES
    ((SELECT id FROM users WHERE username = 'student_demo'), 'student'),
    ((SELECT id FROM users WHERE username = 'teacher_demo'), 'teacher'),
    ((SELECT id FROM users WHERE username = 'admin_demo'), 'admin')
ON CONFLICT(user_id, role_code) DO NOTHING;

INSERT INTO user_learner_links (user_id, learner_id)
VALUES (
    (SELECT id FROM users WHERE username = 'student_demo'),
    (SELECT id FROM learners WHERE code = 'demo-learner')
)
ON CONFLICT(user_id) DO NOTHING;

INSERT INTO teacher_course_assignments (user_id, course_id)
VALUES (
    (SELECT id FROM users WHERE username = 'teacher_demo'),
    (SELECT id FROM courses WHERE code = 'data-structures')
)
ON CONFLICT(user_id, course_id) DO NOTHING;
```

For this phase, plain-text `password_hash` is acceptable only as a temporary demo simplification documented in the spec. Do not introduce a full password-hashing library yet unless it already exists in-project.

- [ ] **Step 5: Reinitialize the local test database and run the focused seed test**

Run from repo root:

```bash
./scripts/init_db.sh && cd backend/build && ctest --output-on-failure -R UserSeedProvidesDefaultDemoAccounts
```

Expected: PASS.

## Task 2: Add backend user-account repository and auth/session service

**Files:**
- Create: `backend/repositories/UserAccountRepository.h`
- Create: `backend/repositories/UserAccountRepository.cc`
- Create: `backend/services/AuthService.h`
- Create: `backend/services/AuthService.cc`
- Test: `backend/tests/AuthSessionServiceTest.cc`

- [ ] **Step 1: Add failing service tests for login/session payloads**

Add tests to `backend/tests/AuthSessionServiceTest.cc` covering:

1. login with `student_demo` returns role `student` and linked learner `demo-learner`
2. login with `teacher_demo` returns role `teacher` and no learner link
3. invalid username/password is rejected

Use an interface target like:

```cpp
DROGON_TEST(AuthServiceBuildsStudentSessionPayload)
{
    const auto payload = services::AuthService::login("student_demo", "demo123");

    CHECK(payload["authenticated"].asBool() == true);
    CHECK(payload["user"]["username"].asString() == "student_demo");
    CHECK(payload["activeRole"].asString() == "student");
    CHECK(payload["linkedLearner"]["learnerCode"].asString() == "demo-learner");
}
```

- [ ] **Step 2: Run the focused auth-service test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "AuthServiceBuildsStudentSessionPayload|AuthServiceRejectsInvalidCredentials"
```

Expected: FAIL because the service does not yet exist.

- [ ] **Step 3: Implement the repository query layer**

Create `backend/repositories/UserAccountRepository.h` with structs and methods such as:

```cpp
namespace repositories
{
struct UserRecord
{
    int id{0};
    std::string username;
    std::string displayName;
    std::string passwordHash;
    std::string status;
};

struct UserRoleRecord
{
    int userId{0};
    std::string roleCode;
};

struct UserLearnerLinkRecord
{
    int userId{0};
    int learnerId{0};
    std::string learnerCode;
    std::string learnerName;
    std::string targetCourseCode;
};

std::optional<UserRecord> findUserByUsername(const std::string &username);
std::vector<UserRoleRecord> listRolesForUser(int userId);
std::optional<UserLearnerLinkRecord> findLearnerLinkForUser(int userId);
}
```

Implement the corresponding SQL in `UserAccountRepository.cc` using `sqlite_client`, following the existing repository style.

- [ ] **Step 4: Implement the auth service**

Create `backend/services/AuthService.h/.cc` with a minimal API:

```cpp
namespace services
{
class AuthService
{
  public:
    static Json::Value login(const std::string &username,
                             const std::string &password);
    static Json::Value buildSessionPayload(const std::string &username);
};
}
```

Behavior:

1. Find user by username
2. Reject if missing or disabled
3. Compare `password` with stored `password_hash` literally for this phase
4. Build payload:

```json
{
  "authenticated": true,
  "user": {
    "username": "student_demo",
    "displayName": "学生演示账号"
  },
  "roles": ["student"],
  "activeRole": "student",
  "linkedLearner": {
    "learnerCode": "demo-learner",
    "learnerName": "演示学生",
    "targetCourseCode": "data-structures"
  }
}
```

For teacher/admin payloads, return `linkedLearner: null`.

- [ ] **Step 5: Re-run the focused auth-service tests**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "AuthServiceBuildsStudentSessionPayload|AuthServiceBuildsTeacherSessionPayload|AuthServiceRejectsInvalidCredentials"
```

Expected: PASS.

## Task 3: Expose auth/session endpoints through Drogon controllers

**Files:**
- Create: `backend/controllers/AuthController.h`
- Create: `backend/controllers/AuthController.cc`
- Modify: `backend/main.cc`
- Modify: `backend/config/config.json`
- Test: `backend/tests/AuthSessionServiceTest.cc`

- [ ] **Step 1: Add a failing HTTP-level auth/session test**

Extend `backend/tests/AuthSessionServiceTest.cc` with HTTP-style controller tests that:

1. `POST /api/auth/login` with valid student credentials returns 200 and session payload
2. `GET /api/auth/session` after login returns authenticated payload
3. `POST /api/auth/logout` clears session

Follow Drogon test style already used in controller tests elsewhere in the repo.

- [ ] **Step 2: Run the focused controller test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "AuthController|AuthSession"
```

Expected: FAIL because no auth controller exists.

- [ ] **Step 3: Enable Drogon session support for this phase**

Update `backend/config/config.json`:

```json
"enable_session": true
```

If `main.cc` needs no change once config enables session, leave it untouched. If a code change is required by Drogon session setup, make the smallest working edit only.

- [ ] **Step 4: Implement the auth controller**

Create `backend/controllers/AuthController.h` with three actions:

```cpp
METHOD_LIST_BEGIN
ADD_METHOD_TO(AuthController::login, "/api/auth/login", drogon::Post);
ADD_METHOD_TO(AuthController::logout, "/api/auth/logout", drogon::Post);
ADD_METHOD_TO(AuthController::session, "/api/auth/session", drogon::Get);
METHOD_LIST_END
```

Implement `AuthController.cc` so that:

1. `login` reads JSON `{ username, password }`
2. Calls `AuthService::login(...)`
3. Stores `username` in session on success
4. `session` reads session username and returns authenticated payload or `{ authenticated: false }`
5. `logout` clears the session and returns `{ authenticated: false }`

- [ ] **Step 5: Re-run the focused controller/session tests**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "AuthController|AuthSession"
```

Expected: PASS.

## Task 4: Add teacher/admin read-only dashboard services and endpoints

**Files:**
- Create: `backend/services/TeacherDashboardService.h`
- Create: `backend/services/TeacherDashboardService.cc`
- Create: `backend/services/AdminDashboardService.h`
- Create: `backend/services/AdminDashboardService.cc`
- Create: `backend/controllers/TeacherController.h`
- Create: `backend/controllers/TeacherController.cc`
- Create: `backend/controllers/AdminController.h`
- Create: `backend/controllers/AdminController.cc`
- Test: `backend/tests/RoleDashboardServiceTest.cc`

- [ ] **Step 1: Add failing tests for teacher/admin dashboard payloads**

Create `backend/tests/RoleDashboardServiceTest.cc` with at least these tests:

1. teacher course list for `teacher_demo` returns `data-structures`
2. teacher course overview returns knowledge-point count and course metadata
3. admin user list returns student_demo/teacher_demo/admin_demo
4. admin assignments endpoint returns teacher-course mapping

- [ ] **Step 2: Run the focused dashboard tests and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "TeacherDashboard|AdminDashboard"
```

Expected: FAIL because the services/controllers do not exist.

- [ ] **Step 3: Implement teacher dashboard service and controller**

Teacher service payloads should remain read-only and small. Example payload:

```json
{
  "courses": [
    {
      "courseCode": "data-structures",
      "courseName": "数据结构",
      "knowledgePointCount": 14,
      "resourceSourceCount": 1
    }
  ]
}
```

Teacher controller endpoints:

1. `GET /api/teacher/courses`
2. `GET /api/teacher/courses/:courseCode/overview`

- [ ] **Step 4: Implement admin dashboard service and controller**

Admin endpoints:

1. `GET /api/admin/users`
2. `GET /api/admin/courses`
3. `GET /api/admin/course-assignments`

Return read-only list payloads only. Do not add mutation endpoints in this phase.

- [ ] **Step 5: Re-run the focused dashboard tests**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "TeacherDashboard|AdminDashboard"
```

Expected: PASS.

## Task 5: Add frontend auth API and auth store

**Files:**
- Create: `frontend/src/api/auth.js`
- Create: `frontend/src/stores/authStore.js`
- Create: `frontend/src/stores/authStore.test.js`

- [ ] **Step 1: Add a failing auth-store persistence test**

Create `frontend/src/stores/authStore.test.js` with tests for:

1. storing authenticated student session payload
2. restoring session from storage on next init
3. clearing session on logout/reset

Example:

```js
it("persists authenticated student session", () => {
  setActivePinia(createPinia());
  const store = useAuthStore();

  store.setSession({
    authenticated: true,
    user: { username: "student_demo", displayName: "学生演示账号" },
    roles: ["student"],
    activeRole: "student",
    linkedLearner: { learnerCode: "demo-learner" },
  });

  setActivePinia(createPinia());
  const restored = useAuthStore();
  expect(restored.activeRole).toBe("student");
  expect(restored.linkedLearner?.learnerCode).toBe("demo-learner");
});
```

- [ ] **Step 2: Run the focused auth-store test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/stores/authStore.test.js
```

Expected: FAIL because the store file does not exist.

- [ ] **Step 3: Implement frontend auth API wrapper**

Create `frontend/src/api/auth.js`:

```js
import http from "./http";

export async function login(payload) {
  const { data } = await http.post("/auth/login", payload);
  return data;
}

export async function logout() {
  const { data } = await http.post("/auth/logout");
  return data;
}

export async function fetchSession() {
  const { data } = await http.get("/auth/session");
  return data;
}
```

- [ ] **Step 4: Implement auth store**

Create `frontend/src/stores/authStore.js` with:

1. `currentUser`
2. `currentRoles`
3. `activeRole`
4. `linkedLearner`
5. `isAuthenticated`

Add actions:

```js
setSession(payload)
clearSession()
async restoreSession()
```

Persist to `sessionStorage` using a dedicated key, mirroring the style of `navigationStore`.

- [ ] **Step 5: Re-run the auth-store test**

Run from `frontend`:

```bash
npm run test -- --run src/stores/authStore.test.js
```

Expected: PASS.

## Task 6: Add login page and role-based route guards

**Files:**
- Create: `frontend/src/views/LoginView.vue`
- Create: `frontend/src/views/LoginView.test.js`
- Modify: `frontend/src/router/index.js`

- [ ] **Step 1: Add a failing login-view test**

Create `frontend/src/views/LoginView.test.js` covering:

1. username/password submit calls login API and updates auth store
2. demo shortcut button logs in as student_demo / teacher_demo / admin_demo
3. successful student login routes to `/`
4. successful teacher login routes to `/teacher`

- [ ] **Step 2: Run the focused login-view test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/views/LoginView.test.js
```

Expected: FAIL because the login view does not exist.

- [ ] **Step 3: Implement login view**

Create `frontend/src/views/LoginView.vue` with:

1. username/password form
2. three demo shortcut buttons
3. submit handler that calls `login(...)`
4. auth store update on success
5. redirect by `activeRole`

The page should stay minimal and defense-friendly, not resemble a production SaaS auth screen.

- [ ] **Step 4: Add route definitions and guards**

Update `frontend/src/router/index.js` to add:

1. `/login`
2. `/teacher`
3. `/teacher/courses`
4. `/teacher/courses/:courseCode`
5. `/admin`
6. `/admin/users`
7. `/admin/courses`
8. `/admin/assignments`

Then add a global `beforeEach` guard that:

1. redirects unauthenticated users to `login`
2. allows student routes only for student
3. allows teacher routes only for teacher
4. allows admin routes only for admin

Use route `meta.roleScope` to keep the guard simple.

- [ ] **Step 5: Re-run the login-view/router test**

Run from `frontend`:

```bash
npm run test -- --run src/views/LoginView.test.js
```

Expected: PASS.

## Task 7: Switch student pages to prefer logged-in learner context

**Files:**
- Modify: `frontend/src/views/HomeView.vue`
- Modify: `frontend/src/views/LearnerProfileView.vue`
- Modify: `frontend/src/views/LearningGraphView.vue`
- Modify: `frontend/src/views/DetailLearningView.vue`
- Modify: `frontend/src/views/ResourceRecommendationView.vue`
- Modify tests for these pages as needed

- [ ] **Step 1: Add a failing student-page auth-context test**

Extend `frontend/src/views/HomeView.test.js` so that when auth store contains `linkedLearner.learnerCode = "demo-learner"`, the view uses that learner context and no longer depends on implicit hard-coded fallback as the primary path.

- [ ] **Step 2: Run the focused home/profile tests and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/views/HomeView.test.js src/views/LearnerProfileView.test.js src/views/DetailLearningView.test.js
```

Expected: FAIL until the views use auth-store-first resolution.

- [ ] **Step 3: Implement auth-store-first learner resolution**

Update the student views to use:

```js
const authStore = useAuthStore();
const learnerCode = computed(
  () => authStore.linkedLearner?.learnerCode || learnerProfile.value?.learner?.code || "demo-learner",
);
```

Apply this pattern where learner context is currently read from hard-coded defaults or indirect page-only state.

Do not remove the fallback entirely in this phase; keep it as migration safety.

- [ ] **Step 4: Re-run the student-view test slice**

Run from `frontend`:

```bash
npm run test -- --run src/views/HomeView.test.js src/views/LearnerProfileView.test.js src/views/DetailLearningView.test.js src/views/LearningGraphView.test.js src/views/ResourceRecommendationView.test.js
```

Expected: PASS.

## Task 8: Add teacher dashboard and course pages

**Files:**
- Create: `frontend/src/api/teacher.js`
- Create: `frontend/src/views/TeacherDashboardView.vue`
- Create: `frontend/src/views/TeacherDashboardView.test.js`
- Create: `frontend/src/views/TeacherCoursesView.vue`
- Create: `frontend/src/views/TeacherCoursesView.test.js`
- Create: `frontend/src/views/TeacherCourseOverviewView.vue`
- Create: `frontend/src/views/TeacherCourseOverviewView.test.js`

- [ ] **Step 1: Add failing teacher view tests**

Cover:

1. teacher dashboard loads course summary cards
2. teacher course list displays `data-structures`
3. teacher course overview displays knowledge point count and resource entry hints

- [ ] **Step 2: Run the focused teacher tests and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/views/TeacherDashboardView.test.js src/views/TeacherCoursesView.test.js src/views/TeacherCourseOverviewView.test.js
```

Expected: FAIL because the pages/API wrapper do not exist.

- [ ] **Step 3: Implement teacher API wrapper and pages**

Use lightweight card/list pages. The first phase only needs:

1. course list
2. course overview summary
3. content-maintenance entry cards with placeholder text

Do not build CRUD forms.

- [ ] **Step 4: Re-run the focused teacher view tests**

Run from `frontend`:

```bash
npm run test -- --run src/views/TeacherDashboardView.test.js src/views/TeacherCoursesView.test.js src/views/TeacherCourseOverviewView.test.js
```

Expected: PASS.

## Task 9: Add admin dashboard and list pages

**Files:**
- Create: `frontend/src/api/admin.js`
- Create: `frontend/src/views/AdminDashboardView.vue`
- Create: `frontend/src/views/AdminDashboardView.test.js`
- Create: `frontend/src/views/AdminUsersView.vue`
- Create: `frontend/src/views/AdminUsersView.test.js`
- Create: `frontend/src/views/AdminCoursesView.vue`
- Create: `frontend/src/views/AdminCoursesView.test.js`
- Create: `frontend/src/views/AdminAssignmentsView.vue`
- Create: `frontend/src/views/AdminAssignmentsView.test.js`

- [ ] **Step 1: Add failing admin view tests**

Cover:

1. dashboard summary cards render
2. users list contains student_demo / teacher_demo / admin_demo
3. courses list contains data-structures
4. assignments list shows teacher_demo -> data-structures

- [ ] **Step 2: Run the focused admin tests and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/views/AdminDashboardView.test.js src/views/AdminUsersView.test.js src/views/AdminCoursesView.test.js src/views/AdminAssignmentsView.test.js
```

Expected: FAIL because the pages/API wrapper do not exist.

- [ ] **Step 3: Implement admin API wrapper and pages**

Keep pages read-only and card/list oriented.

- [ ] **Step 4: Re-run the focused admin view tests**

Run from `frontend`:

```bash
npm run test -- --run src/views/AdminDashboardView.test.js src/views/AdminUsersView.test.js src/views/AdminCoursesView.test.js src/views/AdminAssignmentsView.test.js
```

Expected: PASS.

## Task 10: Update docs and run final verification

**Files:**
- Modify: `README.md`
- Modify: `AIREAD.md`

- [ ] **Step 1: Update README for multi-role platform foundation**

Add concise capability notes such as:

```md
- 统一登录入口：系统支持学生 / 教师 / 管理员账号登录，并保留演示快捷入口
- 角色分流：登录后根据角色进入学生学习系统、教师工作台或管理员工作台
- 平台骨架：教师与管理员已具备查看型最小工作台，支持课程、用户和绑定关系概览
```

- [ ] **Step 2: Append an AIREAD architecture record**

Add a dated entry describing:

1. new `users` identity layer
2. `learners` retained as student learning entity
3. role-based routing and dashboard structure

- [ ] **Step 3: Run final backend verification slice**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "Auth|TeacherDashboard|AdminDashboard|LearnerProfileService|PathPlanningService"
```

Expected: PASS.

- [ ] **Step 4: Run final frontend verification slice**

Run from `frontend`:

```bash
npm run test -- --run src/stores/authStore.test.js src/views/LoginView.test.js src/views/HomeView.test.js src/views/LearnerProfileView.test.js src/views/TeacherDashboardView.test.js src/views/TeacherCoursesView.test.js src/views/TeacherCourseOverviewView.test.js src/views/AdminDashboardView.test.js src/views/AdminUsersView.test.js src/views/AdminCoursesView.test.js src/views/AdminAssignmentsView.test.js
```

Expected: PASS.

- [ ] **Step 5: Run final frontend build**

Run from `frontend`:

```bash
npm run build
```

Expected: PASS.

- [ ] **Step 6: Record implementation completion summary**

Summarize:

1. unified login/role foundation
2. student learner binding
3. teacher/admin minimal dashboards
4. explicit exclusions (no registration, no heavy CRUD)

- [ ] **Step 7: Commit in logical units**

Only if the user asks for commits during execution. Recommended order:

```bash
git add database/init/001_schema.sql database/seed/course_data_structures.sql backend/repositories/UserAccountRepository.* backend/services/AuthService.* backend/controllers/AuthController.* backend/tests/AuthSessionServiceTest.cc backend/config/config.json backend/main.cc
git commit -m "feat: add multi-role auth foundation"

git add backend/services/TeacherDashboardService.* backend/services/AdminDashboardService.* backend/controllers/TeacherController.* backend/controllers/AdminController.* backend/tests/RoleDashboardServiceTest.cc frontend/src/api/auth.js frontend/src/stores/authStore.* frontend/src/views/LoginView.* frontend/src/router/index.js
git commit -m "feat: add role-based login and route guards"

git add frontend/src/api/teacher.js frontend/src/api/admin.js frontend/src/views/Teacher*.vue frontend/src/views/Teacher*.test.js frontend/src/views/Admin*.vue frontend/src/views/Admin*.test.js frontend/src/views/HomeView.vue frontend/src/views/LearnerProfileView.vue frontend/src/views/LearningGraphView.vue frontend/src/views/DetailLearningView.vue frontend/src/views/ResourceRecommendationView.vue
git commit -m "feat: add teacher and admin dashboards"

git add README.md AIREAD.md
git commit -m "docs: record multi-role platform foundation"
```

## Self-review summary

Spec coverage check:

1. Unified identity layer: covered by Tasks 1-3.
2. Login/session restoration: covered by Tasks 2, 3, 5, and 6.
3. Student learner binding and auth-store-first migration: covered by Task 7.
4. Teacher/admin minimal workbenches: covered by Tasks 4, 8, and 9.
5. Second-stage minimum deliverable boundaries: reinforced across Scope and Task 10 docs.

Placeholder scan:

1. No `TODO` / `TBD` placeholders remain.
2. Each task has explicit file targets and verification commands.
3. Out-of-scope items are named explicitly instead of deferred vaguely.

Type consistency check:

1. `linkedLearner` is used consistently across backend payloads and frontend auth store.
2. Role codes are fixed consistently as `student`, `teacher`, `admin`.
3. Teacher/admin route domains match the spec and task names.
