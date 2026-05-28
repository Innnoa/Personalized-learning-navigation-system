# 教师完整功能 — 学生进度查看（A 阶段）设计方案

日期：2026-05-26

## 1. 设计背景

当前项目已完成多角色平台骨架（Phase 1），教师端仅具备只读型课程概览能力。本阶段目标是在不推翻现有架构的前提下，为教师添加**学生进度查看**功能，使教师能查看所授课程的学生学习情况。

后续 B 阶段（课程内容管理）和 C 阶段（题库管理）将在此基础上继续扩展。

## 2. 总体设计定位

教师通过 `teacher_course_assignments` 关联课程，可查看课程下所有以该课程为目标课程的学习者（`learners.target_course_id`）。筛选维度复用现有 `learners.major` 和 `learners.grade_label`，不新增班级/学生组实体。

技术方案采用**后端驱动**：新增 `TeacherStudentService` 聚合数据，通过 `TeacherStudentController` 暴露只读端点。

## 3. 后端接口设计

### 3.1 新增文件

| 文件 | 职责 |
|------|------|
| `backend/services/TeacherStudentService.h` | 学生进度数据聚合 |
| `backend/services/TeacherStudentService.cc` | 实现 SQL 查询和数据组装 |
| `backend/controllers/TeacherStudentController.h` | HTTP 端点注册 |
| `backend/controllers/TeacherStudentController.cc` | 端点实现，权限校验（教师 + 课程归属） |

### 3.2 接口列表

#### GET /api/teacher/courses/:courseCode/students

查询参数：`major`（可选）、`grade_label`（可选）

返回：
```json
{
  "courseCode": "data-structures",
  "students": [
    {
      "learnerCode": "demo-learner",
      "learnerName": "演示学生",
      "major": "计算机科学",
      "gradeLabel": "大二",
      "overallMastery": 0.62,
      "lastActivityAt": "2026-05-20 10:30 CST"
    }
  ]
}
```

SQL 逻辑：
- 从 `learners` 按 `target_course_id` 和可选筛选条件查学生
- `overallMastery` = `AVG(learner_mastery.mastery_score)` 按 learner_id 聚合
- `lastActivityAt` = `MAX(learning_feedback_records.recorded_at)`

#### GET /api/teacher/courses/:courseCode/stats

返回：
```json
{
  "courseCode": "data-structures",
  "courseName": "数据结构",
  "studentCount": 3,
  "avgMastery": 0.58,
  "masteryDistribution": {
    "low": { "min": 0, "max": 0.35, "count": 0 },
    "mid": { "min": 0.35, "max": 0.7, "count": 2 },
    "high": { "min": 0.7, "max": 1.0, "count": 1 }
  }
}
```

#### GET /api/teacher/courses/:courseCode/knowledge-stats

返回：
```json
{
  "courseCode": "data-structures",
  "knowledgePoints": [
    {
      "code": "ds-intro",
      "name": "数据结构基本概念",
      "chapterNo": 1,
      "chapterName": "绪论",
      "difficultyLevel": 2,
      "avgMastery": 0.75,
      "studentCount": 3
    }
  ]
}
```

SQL 逻辑：`knowledge_points` JOIN `learner_mastery` GROUP BY 知识点，仅限通过 `learners.target_course_id` 匹配的学生。

#### GET /api/teacher/students/:learnerCode/progress

权限：需验证教师与该学生 target_course 的归属关系。

返回：
```json
{
  "learner": {
    "code": "demo-learner",
    "name": "演示学生",
    "major": "计算机科学",
    "gradeLabel": "大二",
    "courseCode": "data-structures",
    "courseName": "数据结构"
  },
  "masteryByCode": {
    "ds-intro": 0.9,
    "linear-list": 0.85
  },
  "recentFeedback": [
    {
      "nodeCode": "queue",
      "nodeName": "队列",
      "completionStatus": "completed",
      "previousMastery": 0.35,
      "updatedMastery": 0.85,
      "recordedAt": "2026-05-20 10:30 CST"
    }
  ],
  "recentPaths": [
    {
      "targetCodes": ["topological-sort"],
      "availableMinutes": 90,
      "generatedAt": "2026-05-20 09:00 CST",
      "nodeCount": 4
    }
  ]
}
```

### 3.3 权限控制

- 新增 `TeacherStudentService` 辅助方法：验证教师是否分配到指定 courseCode
- `TeacherStudentController` 每个端点先校验 session 角色为 `teacher`，再校验课程归属
- 不满足条件时返回 403（学生进度接口）或 400（课程级别接口）

### 3.4 复用说明

- 不新增数据库表或迁移脚本
- 全部查询基于现有表：`learners`、`learner_mastery`、`learning_feedback_records`、`knowledge_points`、`teacher_course_assignments`、`courses`
- 遵循现有 Repository → Service → Controller 分层

## 4. 前端设计

### 4.1 PageLayout 导航修复

**问题：** 当前 `PageLayout.vue` 导航栏固定显示学生端链接，教师页面复用后显示错误的导航。

**方案：** 给 `PageLayout` 新增 `roleScope` prop（默认 `"student"` 保持向后兼容）。当 `roleScope === "teacher"` 时：

- 导航栏显示教师专属链接：`我的课程`（/teacher）、返回教师首页
- 不显示学生端链接（首页/学习图谱/推荐资源/学习者画像）
- 保留退出按钮

### 4.2 路由结构

```
/teacher                                          → TeacherDashboardView（重构）
/teacher/courses                                  → TeacherCoursesView（保留）
/teacher/courses/:courseCode                      → 带子导航的课程详情容器（新建 TeacherCoursePage.vue）
  /teacher/courses/:courseCode/overview           → 课程信息（重构现有 TeacherCourseOverviewView）
  /teacher/courses/:courseCode/students           → 学生列表（新建 TeacherStudentsView.vue）
  /teacher/courses/:courseCode/stats              → 学情总览（新建 TeacherCourseStatsView.vue）
  /teacher/courses/:courseCode/points             → 知识点分布（新建 TeacherKnowledgeStatsView.vue）
/teacher/courses/:courseCode/students/:learnerCode → 学生详情（新建 TeacherStudentProgressView.vue）
```

路由守卫：所有 `/teacher/*` 路由 meta `{ roleScope: "teacher" }`。

### 4.3 新增前端文件

| 文件 | 职责 |
|------|------|
| `frontend/src/api/teacher.js` | 新增 4 个 API 方法（students, stats, knowledge-stats, student-progress） |
| `frontend/src/views/TeacherCoursePage.vue` | 课程详情容器，含子导航标签 |
| `frontend/src/views/TeacherStudentsView.vue` | 学生列表页，支持 major/grade 筛选 |
| `frontend/src/views/TeacherCourseStatsView.vue` | 学情总览页，掌握度分布 |
| `frontend/src/views/TeacherKnowledgeStatsView.vue` | 知识点分布页，按知识点查看平均掌握度 |
| `frontend/src/views/TeacherStudentProgressView.vue` | 学生个体进度详情页 |

### 4.4 修改前端文件

| 文件 | 变更 |
|------|------|
| `frontend/src/components/PageLayout.vue` | 新增 `roleScope` prop，条件渲染导航栏 |
| `frontend/src/views/TeacherDashboardView.vue` | 传 `roleScope="teacher"`，重命名为"我的课程" |
| `frontend/src/views/TeacherCoursesView.vue` | 传 `roleScope="teacher"` |
| `frontend/src/router/index.js` | 新增上述路由，删除旧的 `teacher-course-detail` 路由，或将其重定向到 `/teacher/courses/:courseCode/overview` |

### 4.5 页面交互设计

#### 课程详情容器（TeacherCoursePage.vue）
- 顶部：课程名称 + 面包屑
- 子导航标签：课程信息 / 学情总览 / 学生列表 / 知识点分布
- `<router-view>` 渲染对应子页面

#### 学生列表（TeacherStudentsView.vue）
- 顶部筛选：专业下拉、年级下拉
- 学生卡片列表：姓名、专业、年级、综合掌握度（进度条）、最近活动时间
- 点击学生卡片进入学生详情页

#### 学情总览（TeacherCourseStatsView.vue）
- 统计卡片行：学生总数、平均掌握度
- 掌握度分布柱状图（低/中/高三档，使用纯 CSS 条形图，不引入额外图表库）
- 复用现有 `ProfileProgressOverview.vue` 或 `profileCharts.js` 的模式

#### 知识点分布（TeacherKnowledgeStatsView.vue）
- 知识点表格：章节 → 知识点名称 → 难度 → 平均掌握度（进度条）
- 按 `chapter_no`, `display_order` 排序

#### 学生详情（TeacherStudentProgressView.vue）
- 学生基本信息卡片
- 掌握度列表（按知识点）
- 最近反馈记录表
- 最近学习路径摘要

### 4.6 测试策略

每个新增/修改的 Vue 文件配 `.test.js`，遵循现有测试模式：
- Pinia mock + 路由 mock
- API 层 mock（`vi.mock`）
- `flushUi()` 异步等待
- `data-testid` 查询断言

## 5. 错误处理

| 场景 | 前端 | 后端 |
|------|------|------|
| 教师未分配课程 | 显示空状态提示 | 返回 200 + 空数组 |
| 访问无权课程 | 显示错误提示 | 返回 403 |
| 学生不存在 | 显示 404 提示 | 返回 404 |
| 后端未启动 | 显示连接失败提示 | — |
| 数据为空 | 显示"暂无数据"空态 | 返回 200 + 空数组/0值 |

## 6. 明确不做

- 不新增数据库表（复用现有 learners、learner_mastery 等表）
- 不新增班级/学生组实体（利用 existing major + grade_label 筛选）
- 不引入额外图表库（使用纯 CSS 条形图 + 数值展示）
- 不做学生增删改（教师端仍保持只读为主，增删改留给 B/C 阶段）
- 不引入前端重型表格组件（使用现有卡片 + CSS Grid 布局）
- 不修改现有学生端页面和行为

## 7. 自审

### 占位符扫描
无 TBD、TODO 或未完成段落。

### 内部一致性
- API 返回字段与数据库字段一致
- 前端路由与后端端点一一对应
- 权限校验在前后端均有覆盖

### 范围检查
聚焦 A 阶段（学生进度查看），不含 B（课程内容管理）和 C（题库管理）。每个接口只做只读聚合查询，不做数据写入。

### 歧义检查
- `overallMastery` 定义为 `AVG(learner_mastery.mastery_score)`，不含 detail_mastery
- `masteryDistribution` 三档边界与现有学生端保持一致（0-0.35 / 0.35-0.7 / 0.7-1.0）
- 筛选参数 `major` / `grade_label` 为精确匹配，不做模糊搜索
