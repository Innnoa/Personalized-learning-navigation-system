# Teacher Course Overview Learning Status Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在教师课程“学情总览”页中新增班级学情总览和重点关注学生列表，同时保留现有课程建设信息，并确保学情区局部失败不会拖垮整页。

**Architecture:** 继续以 `TeacherCourseOverviewView.vue` 作为单页承载，不新增后端接口，直接复用现有 `overview / stats / students` 三个教师接口。页面状态拆成“课程概览主状态”和“学情区局部状态”，用计算属性完成掌握度分层展示与重点学生混合排序。

**Tech Stack:** Vue 3 `<script setup>`, Vue Router, Pinia, existing `frontend/src/api/teacher.js`, Vitest, Vue Test Utils

---

## Scope

本计划只覆盖课程概览页的轻量增强：

1. 在现有概览页中接入班级整体学情数据
2. 在同页展示重点关注学生名单
3. 为混合排序、局部失败、空态补测试

本计划明确不做：

- 不新增后端端点
- 不改学生详情页
- 不改其他教师页结构
- 不加入额外图表库

## File Map

### Frontend files to modify

- `frontend/src/views/TeacherCourseOverviewView.vue`
  Responsibility: 并行加载学情接口、计算重点学生排序、渲染“班级学情总览”与“重点关注学生”区块、处理局部错误与空态。

- `frontend/src/views/TeacherCourseOverviewView.test.js`
  Responsibility: 验证三接口调用、班级态势渲染、重点学生混合排序、详情跳转、学情区局部失败隔离。

### Frontend files referenced only

- `frontend/src/api/teacher.js`
  Responsibility: 已存在 `fetchTeacherCourseStats` 与 `fetchTeacherStudents`，本次只复用，不新增 API。

- `frontend/src/router/index.js`
  Responsibility: 已存在 `teacher-student-progress` 路由，本次只复用该跳转目标。

## Task 1: 为班级态势与重点学生列表补失败测试

**Files:**
- Modify: `frontend/src/views/TeacherCourseOverviewView.test.js`
- Reference: `frontend/src/api/teacher.js`

- [ ] **Step 1: 扩展 API mock，先让测试可以描述学情区行为**

把测试文件顶部的 API import 和 mock 扩成三项：

```js
import {
  fetchTeacherCourseOverview,
  fetchTeacherCourseStats,
  fetchTeacherStudents,
} from "../api/teacher";

vi.mock("../api/teacher", () => ({
  fetchTeacherCourseOverview: vi.fn(),
  fetchTeacherCourseStats: vi.fn(),
  fetchTeacherStudents: vi.fn(),
}));
```

并在 `beforeEach()` 中一起 reset：

```js
beforeEach(() => {
  fetchTeacherCourseOverview.mockReset();
  fetchTeacherCourseStats.mockReset();
  fetchTeacherStudents.mockReset();
  pushMock.mockReset();
});
```

- [ ] **Step 2: 写一个 happy path 失败测试，覆盖班级态势、混合排序和详情跳转**

在 `TeacherCourseOverviewView.test.js` 新增测试：

```js
it("renders learning status summary and ranks priority students by mastery then activity", async () => {
  fetchTeacherCourseOverview.mockResolvedValue({
    course: {
      courseCode: "data-structures",
      courseName: "数据结构",
      description: "课程概览",
      targetAudience: "2022级计算机类本科生",
      teacherUsername: "teacher_demo",
      knowledgePointCount: 14,
      dependencyCount: 18,
      resourceCount: 219,
      questionBankCount: 15,
      questionCount: 1421,
      resourceCoveredPointCount: 12,
      questionCoveredPointCount: 10,
      uncoveredPointCount: 4,
      maintenanceStatus: "待补题库",
    },
    chapterStats: [{ chapterNo: 1, chapterName: "绪论", knowledgePointCount: 2 }],
    uncoveredPoints: [],
  });
  fetchTeacherCourseStats.mockResolvedValue({
    studentCount: 8,
    avgMastery: 0.56,
    masteryDistribution: {
      low: { count: 2 },
      mid: { count: 4 },
      high: { count: 2 },
    },
  });
  fetchTeacherStudents.mockResolvedValue({
    students: [
      {
        learnerCode: "risk-silent",
        learnerName: "王芳",
        major: "计算机科学",
        gradeLabel: "2022级",
        overallMastery: 0.18,
        lastActivityAt: "",
      },
      {
        learnerCode: "risk-active",
        learnerName: "李明",
        major: "计算机科学",
        gradeLabel: "2022级",
        overallMastery: 0.18,
        lastActivityAt: "2026-06-10 10:00 CST",
      },
      {
        learnerCode: "mid-risk",
        learnerName: "赵强",
        major: "软件工程",
        gradeLabel: "2022级",
        overallMastery: 0.24,
        lastActivityAt: "2026-06-01 08:00 CST",
      },
    ],
  });

  const wrapper = mountView();
  await flushUi();

  expect(fetchTeacherCourseStats).toHaveBeenCalledWith("data-structures", {
    username: "teacher_demo",
  });
  expect(fetchTeacherStudents).toHaveBeenCalledWith("data-structures", {
    username: "teacher_demo",
  });
  expect(wrapper.text()).toContain("班级学情总览");
  expect(wrapper.text()).toContain("学生总数");
  expect(wrapper.text()).toContain("平均掌握度");
  expect(wrapper.text()).toContain("56%");
  expect(wrapper.text()).toContain("较低 (0-35%)");

  const priorityRows = wrapper.findAll('[data-testid="priority-student"]');
  expect(priorityRows).toHaveLength(3);
  expect(priorityRows[0].text()).toContain("王芳");
  expect(priorityRows[0].text()).toContain("暂无最近活动记录");
  expect(priorityRows[1].text()).toContain("李明");
  expect(priorityRows[2].text()).toContain("赵强");

  await priorityRows[0].find("button").trigger("click");
  expect(pushMock).toHaveBeenCalledWith({
    name: "teacher-student-progress",
    params: { courseCode: "data-structures", learnerCode: "risk-silent" },
  });
});
```

- [ ] **Step 3: 运行单测并确认当前实现尚未满足**

Run:

```bash
cd frontend && npm run test -- src/views/TeacherCourseOverviewView.test.js
```

Expected: FAIL，原因应为页面尚未渲染 `班级学情总览` / `[data-testid="priority-student"]` 或未调用新增学情接口。

## Task 2: 实现学情区数据加载、排序和 UI

**Files:**
- Modify: `frontend/src/views/TeacherCourseOverviewView.vue`
- Reference: `frontend/src/router/index.js`

- [ ] **Step 1: 扩展组件状态，拆分课程概览与学情区状态**

把 API import 改成：

```js
import {
  fetchTeacherCourseOverview,
  fetchTeacherCourseStats,
  fetchTeacherStudents,
} from "../api/teacher";
```

在现有 refs 旁边新增：

```js
const learningStatusLoading = ref(true);
const learningStatusError = ref("");
const statsPayload = ref(null);
const studentsPayload = ref([]);
```

- [ ] **Step 2: 增加学情区计算属性和排序辅助函数**

在 `numberOrZero()` 后新增：

```js
const studentStats = computed(() => statsPayload.value || {
  studentCount: 0,
  avgMastery: 0,
  masteryDistribution: {
    low: { count: 0 },
    mid: { count: 0 },
    high: { count: 0 },
  },
});

const studentDistribution = computed(
  () => studentStats.value.masteryDistribution || {
    low: { count: 0 },
    mid: { count: 0 },
    high: { count: 0 },
  },
);

function masteryPercent(score) {
  return `${Math.round(numberOrZero(score) * 100)}%`;
}

function parseActivityRank(value) {
  if (!value) return Number.NEGATIVE_INFINITY;
  const timestamp = Date.parse(value);
  return Number.isFinite(timestamp) ? timestamp : Number.NEGATIVE_INFINITY;
}

const priorityStudents = computed(() =>
  [...studentsPayload.value]
    .sort((left, right) => {
      const masteryDiff = numberOrZero(left.overallMastery) - numberOrZero(right.overallMastery);
      if (masteryDiff !== 0) return masteryDiff;
      return parseActivityRank(left.lastActivityAt) - parseActivityRank(right.lastActivityAt);
    })
    .slice(0, 5),
);
```

再补一个文案辅助函数：

```js
function lastActivityLabel(student) {
  return student.lastActivityAt || "暂无最近活动记录";
}
```

- [ ] **Step 3: 把主加载流程改成三接口并行，且允许学情区局部失败**

把 `loadCourseOverview()` 改成 `Promise.allSettled(...)` 结构：

```js
async function loadCourseOverview() {
  loading.value = true;
  learningStatusLoading.value = true;
  loadError.value = "";
  learningStatusError.value = "";

  const [overviewResult, statsResult, studentsResult] = await Promise.allSettled([
    fetchTeacherCourseOverview({
      username: teacherUsername.value,
      courseCode: courseCode.value,
    }),
    fetchTeacherCourseStats(courseCode.value, {
      username: teacherUsername.value,
    }),
    fetchTeacherStudents(courseCode.value, {
      username: teacherUsername.value,
    }),
  ]);

  if (overviewResult.status !== "fulfilled") {
    loadError.value = "未能读取课程概览，请稍后重试。";
    loading.value = false;
    learningStatusLoading.value = false;
    return;
  }

  coursePayload.value = overviewResult.value;

  if (statsResult.status === "fulfilled") {
    statsPayload.value = statsResult.value;
  } else {
    learningStatusError.value = "未能读取班级学情，请稍后重试。";
  }

  if (studentsResult.status === "fulfilled") {
    studentsPayload.value = Array.isArray(studentsResult.value?.students)
      ? studentsResult.value.students
      : [];
  } else {
    learningStatusError.value = "未能读取班级学情，请稍后重试。";
  }

  loading.value = false;
  learningStatusLoading.value = false;
}
```

保留现有 `console.error(error)` 风格时，不要把 `allSettled` 的 rejection reason 吞掉；应在 `rejected` 分支输出：

```js
if (statsResult.status === "rejected") console.error(statsResult.reason);
if (studentsResult.status === "rejected") console.error(studentsResult.reason);
```

- [ ] **Step 4: 在模板中插入班级学情区和重点关注学生列表**

在现有 `metric-grid` 后、`detail-grid` 前插入：

```vue
<section class="learning-status-grid">
  <article class="detail-card">
    <div class="section-head">
      <h3>班级学情总览</h3>
      <span class="section-tip">先看整体，再判断是否需要逐个跟进</span>
    </div>
    <p v-if="learningStatusError" class="state-message state-message--error">
      {{ learningStatusError }}
    </p>
    <template v-else>
      <div class="learning-summary-grid">
        <article class="learning-summary-card">
          <p class="metric-label">学生总数</p>
          <strong class="metric-value">{{ studentStats.studentCount }}</strong>
        </article>
        <article class="learning-summary-card">
          <p class="metric-label">平均掌握度</p>
          <strong class="metric-value">{{ masteryPercent(studentStats.avgMastery) }}</strong>
        </article>
      </div>
      <div class="distribution-bars">
        <div class="dist-row">
          <span class="dist-label">较高 (70-100%)</span>
          <strong class="dist-count">{{ studentDistribution.high.count }}</strong>
        </div>
        <div class="dist-row">
          <span class="dist-label">中等 (35-70%)</span>
          <strong class="dist-count">{{ studentDistribution.mid.count }}</strong>
        </div>
        <div class="dist-row">
          <span class="dist-label">较低 (0-35%)</span>
          <strong class="dist-count">{{ studentDistribution.low.count }}</strong>
        </div>
      </div>
    </template>
  </article>

  <article class="detail-card">
    <div class="section-head">
      <h3>重点关注学生</h3>
      <span class="section-tip">低掌握度优先，其次参考最近活动</span>
    </div>
    <p v-if="learningStatusError" class="state-message state-message--error">
      {{ learningStatusError }}
    </p>
    <p v-else-if="priorityStudents.length === 0" class="empty-note">暂无学生学情数据。</p>
    <ul v-else class="priority-list">
      <li
        v-for="student in priorityStudents"
        :key="student.learnerCode"
        class="priority-item"
        data-testid="priority-student"
      >
        <div>
          <strong>{{ student.learnerName }}</strong>
          <p class="priority-meta">{{ student.major }} · {{ student.gradeLabel }}</p>
          <p class="priority-meta">综合掌握度：{{ masteryPercent(student.overallMastery) }}</p>
          <p class="priority-meta">最近活动：{{ lastActivityLabel(student) }}</p>
        </div>
        <button type="button" class="gap-action" @click="goToStudentProgress(student)">
          查看详情
        </button>
      </li>
    </ul>
  </article>
</section>
```

并在脚本里补：

```js
function goToStudentProgress(student) {
  router.push({
    name: "teacher-student-progress",
    params: {
      courseCode: courseCode.value,
      learnerCode: student.learnerCode,
    },
  });
}
```

- [ ] **Step 5: 补齐样式并让窄屏回落到单列**

在 `<style scoped>` 中新增：

```css
.learning-status-grid {
  display: grid;
  gap: 18px;
  grid-template-columns: repeat(2, minmax(0, 1fr));
}

.learning-summary-grid {
  display: grid;
  gap: 12px;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  margin-top: 16px;
}

.learning-summary-card,
.priority-item {
  padding: 14px;
  border: 1px solid #e7edf2;
  border-radius: 12px;
  background: #fbfdfe;
}

.distribution-bars,
.priority-list {
  display: grid;
  gap: 10px;
  margin-top: 16px;
}

.priority-list {
  list-style: none;
  padding: 0;
}

.priority-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
}

.priority-meta {
  margin: 4px 0 0;
  color: #607380;
}

.dist-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  padding: 10px 0;
  border-bottom: 1px solid #eef2f5;
}

.dist-label,
.dist-count {
  color: #15364a;
}
```

并把现有媒体查询扩成：

```css
@media (max-width: 720px) {
  .overview-header {
    flex-direction: column;
  }

  .learning-status-grid,
  .detail-grid,
  .learning-summary-grid {
    grid-template-columns: 1fr;
  }

  .priority-item {
    flex-direction: column;
    align-items: flex-start;
  }
}
```

- [ ] **Step 6: 运行单测，确认 happy path 通过**

Run:

```bash
cd frontend && npm run test -- src/views/TeacherCourseOverviewView.test.js
```

Expected: `renders learning status summary and ranks priority students by mastery then activity` PASS；已有概览测试也继续 PASS。

- [ ] **Step 7: 提交这个可工作的第一版**

Run:

```bash
git add frontend/src/views/TeacherCourseOverviewView.vue frontend/src/views/TeacherCourseOverviewView.test.js
git commit -m "feat: enrich teacher course overview with learning status"
```

Expected: commit 只包含课程概览页与对应测试，不混入其他工作区改动。

## Task 3: 为学情区局部失败与空态补测试并收口实现

**Files:**
- Modify: `frontend/src/views/TeacherCourseOverviewView.test.js`
- Modify: `frontend/src/views/TeacherCourseOverviewView.vue`

- [ ] **Step 1: 写局部失败测试，固定“课程建设信息不受影响”的行为**

在测试文件新增：

```js
it("keeps course overview visible when learning status requests fail", async () => {
  fetchTeacherCourseOverview.mockResolvedValue({
    course: {
      courseCode: "data-structures",
      courseName: "数据结构",
      description: "课程概览",
      targetAudience: "2022级计算机类本科生",
      teacherUsername: "teacher_demo",
      knowledgePointCount: 14,
      dependencyCount: 18,
      resourceCount: 219,
      questionBankCount: 15,
      questionCount: 1421,
      resourceCoveredPointCount: 12,
      questionCoveredPointCount: 10,
      uncoveredPointCount: 4,
      maintenanceStatus: "待补题库",
    },
    chapterStats: [{ chapterNo: 1, chapterName: "绪论", knowledgePointCount: 2 }],
    uncoveredPoints: [{ code: "queue", name: "队列", missingCoverage: ["question_bank"] }],
  });
  fetchTeacherCourseStats.mockRejectedValueOnce(new Error("stats fail"));
  fetchTeacherStudents.mockRejectedValueOnce(new Error("students fail"));

  const wrapper = mountView();
  await flushUi();

  expect(wrapper.text()).toContain("未能读取班级学情，请稍后重试。");
  expect(wrapper.text()).toContain("内容覆盖");
  expect(wrapper.text()).toContain("覆盖缺口");
  expect(wrapper.text()).toContain("章节分布");
  expect(wrapper.text()).toContain("队列");
});
```

- [ ] **Step 2: 再写一个学生空态测试，固定默认文案**

继续新增：

```js
it("shows empty student state when the course has no student learning data", async () => {
  fetchTeacherCourseOverview.mockResolvedValue({
    course: {
      courseCode: "data-structures",
      courseName: "数据结构",
      description: "课程概览",
      targetAudience: "2022级计算机类本科生",
      teacherUsername: "teacher_demo",
      knowledgePointCount: 14,
      dependencyCount: 18,
      resourceCount: 219,
      questionBankCount: 15,
      questionCount: 1421,
      resourceCoveredPointCount: 12,
      questionCoveredPointCount: 10,
      uncoveredPointCount: 0,
      maintenanceStatus: "建设完整",
    },
    chapterStats: [],
    uncoveredPoints: [],
  });
  fetchTeacherCourseStats.mockResolvedValue({
    studentCount: 0,
    avgMastery: 0,
    masteryDistribution: {
      low: { count: 0 },
      mid: { count: 0 },
      high: { count: 0 },
    },
  });
  fetchTeacherStudents.mockResolvedValue({ students: [] });

  const wrapper = mountView();
  await flushUi();

  expect(wrapper.text()).toContain("暂无学生学情数据。");
  expect(wrapper.findAll('[data-testid="priority-student"]')).toHaveLength(0);
});
```

- [ ] **Step 3: 运行单测并确认至少有一项失败**

Run:

```bash
cd frontend && npm run test -- src/views/TeacherCourseOverviewView.test.js
```

Expected: FAIL，如果当前实现已缺失空态或局部失败文案，失败点应直接指向对应断言。

- [ ] **Step 4: 收口实现，确保局部失败、空态和文案都稳定**

如果 Step 3 失败，回到组件中逐项修正以下行为：

```js
if (statsResult.status === "fulfilled") {
  statsPayload.value = statsResult.value;
} else {
  learningStatusError.value = "未能读取班级学情，请稍后重试。";
}

if (studentsResult.status === "fulfilled") {
  studentsPayload.value = Array.isArray(studentsResult.value?.students)
    ? studentsResult.value.students
    : [];
} else {
  learningStatusError.value = "未能读取班级学情，请稍后重试。";
}
```

并确认模板里存在这两个分支：

```vue
<p v-if="learningStatusError" class="state-message state-message--error">
  {{ learningStatusError }}
</p>
<p v-else-if="priorityStudents.length === 0" class="empty-note">暂无学生学情数据。</p>
```

- [ ] **Step 5: 重新运行测试并确认整个文件通过**

Run:

```bash
cd frontend && npm run test -- src/views/TeacherCourseOverviewView.test.js
```

Expected: PASS，包含原有概览测试、新增 happy path、新增局部失败与空态测试。

- [ ] **Step 6: 提交收口改动**

Run:

```bash
git add frontend/src/views/TeacherCourseOverviewView.vue frontend/src/views/TeacherCourseOverviewView.test.js
git commit -m "test: cover teacher overview learning status edge cases"
```

Expected: commit 只包含课程概览页最终增强与测试补全。

## Validation Checklist

- [ ] `cd frontend && npm run test -- src/views/TeacherCourseOverviewView.test.js`
- [ ] 页面能同时展示课程建设信息和班级学情信息
- [ ] 重点学生名单默认最多 5 人
- [ ] 同掌握度学生中，无活动记录者排在更前
- [ ] 学情区失败时，课程建设区不消失

## Self-Review

### Spec coverage

- `班级整体态势`：Task 1 + Task 2 覆盖
- `重点关注学生`：Task 1 + Task 2 覆盖
- `混合排序`：Task 1 固定行为，Task 2 实现
- `局部失败隔离`：Task 3 覆盖
- `空态文案`：Task 3 覆盖

### Placeholder scan

已避免 `TODO / TBD / add appropriate error handling / similar to task N` 这类占位语句；每个测试和实现步骤都给出具体代码与命令。

### Type consistency

- 使用的 API 名称统一为 `fetchTeacherCourseOverview / fetchTeacherCourseStats / fetchTeacherStudents`
- 重点学生数据字段统一为 `learnerCode / learnerName / major / gradeLabel / overallMastery / lastActivityAt`
- 跳转统一复用 `teacher-student-progress` 路由
