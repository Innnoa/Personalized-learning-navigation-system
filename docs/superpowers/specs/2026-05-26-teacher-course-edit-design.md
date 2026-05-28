# 教师完整功能 — 课程内容管理（B 阶段）设计方案

日期：2026-05-26

## 1. 设计背景

A 阶段已完成教师学生进度查看（只读端点 + 4 个学生分析页面）。B 阶段目标是在同架构上新增教师对课程内容的写入能力：
- 知识点管理（增删改 + 依赖关系编辑）
- 学习资源管理（基于既有 JSON 文件的增删改）

## 2. 总体设计定位

不新增数据库表。知识点和依赖关系直接操作现有 `knowledge_points` 和 `knowledge_dependencies` 表。学习资源维持 `learning_resources.json` 文件读写，不迁入数据库。前端在既有 `TeacherCoursePage` 容器中新增 `resources` tab，升级 `points` tab 为可编辑模式。

## 3. 后端接口设计

### 3.1 新增文件

| 文件 | 职责 |
|------|------|
| `backend/services/TeacherCourseEditService.h` | 课程内容编辑业务逻辑 |
| `backend/services/TeacherCourseEditService.cc` | 知识点 CRUD、依赖关系更新、JSON 资源读写 |
| `backend/controllers/TeacherCourseEditController.h` | HTTP 写入端点注册 |
| `backend/controllers/TeacherCourseEditController.cc` | 审核权限 + 调用 service |
| `backend/tests/TeacherCourseEditServiceTest.cc` | 测试 |

### 3.2 接口列表

#### 知识点管理

```
POST /api/teacher/courses/:courseCode/knowledge-points
  Body: { code, name, chapterNo, chapterName, nodeType, difficultyLevel, estimatedMinutes, displayOrder }
  → 201 { code, id }

PUT /api/teacher/courses/:courseCode/knowledge-points/:code
  Body: { name?, chapterNo?, chapterName?, nodeType?, difficultyLevel?, estimatedMinutes?, displayOrder? }
  → 200 { updated: true }

DELETE /api/teacher/courses/:courseCode/knowledge-points/:code
  → 200 { deleted: true }
  (级联删除 learner_mastery 等相关记录)
```

#### 依赖关系管理

```
PUT /api/teacher/courses/:courseCode/knowledge-dependencies
  Body: { dependencies: [{ knowledgePointCode, prerequisiteCode }] }
  → 200 { updated: true, count: N }
```
逻辑：先删除该课程下所有依赖，再批量插入新依赖。保证幂等。

#### 学习资源管理

```
GET /api/teacher/courses/:courseCode/resources
  → 200 { resources: [...], lastModified: "..." }

PUT /api/teacher/courses/:courseCode/resources
  Body: { resources: [...] }
  → 200 { saved: true, count: N }
```
读写路径：`database/seed/course_data_structures.json`（当前仅 data-structures 课程有该文件，后续其他课程同理）。文件写操作使用 Drogon `trantor::asyncFile` 或简单 `ifstream/ofstream`。

### 3.3 权限控制

所有端点校验：
1. session activeRole === "teacher"
2. `verifyTeacherCourseAssignment(username, courseCode)` — 教师必须分配到该课程
3. 知识点 code 必须为字母数字和连字符组合，长度 2-50

## 4. 前端设计

### 4.1 路由结构

在既有嵌套路由中新增 resources：

```
/teacher/courses/:courseCode
  ├── /overview      (课程信息 — 不变)
  ├── /stats         (学情总览 — 不变)
  ├── /students      (学生列表 — 不变)
  ├── /points        (知识点分布 → 升级为可编辑)
  └── /resources     (新增 — 学习资源管理)
```

### 4.2 新增/修改文件

| 文件 | 变更 |
|------|------|
| `frontend/src/api/teacher.js` | 新增 5 个 API 方法 |
| `frontend/src/views/TeacherKnowledgeStatsView.vue` | 修改 — 增加编辑按钮 + 内联表单 |
| `frontend/src/views/TeacherKnowledgeStatsView.test.js` | 修改 — 补编辑测试 |
| `frontend/src/views/TeacherResourcesView.vue` | 新建 — 资源管理页 |
| `frontend/src/views/TeacherResourcesView.test.js` | 新建 — 资源管理测试 |
| `frontend/src/router/index.js` | 新增 resources 子路由 |

### 4.3 知识点编辑交互设计

`TeacherKnowledgeStatsView` 升级：

- 表格每行右侧增加 `编辑` 和 `删除` 按钮
- 点击 `编辑`：该行变为内联编辑模式（字段变为 input/select）
  - code（新增时必填，编辑时只读）、name、chapterNo、chapterName、nodeType、difficultyLevel、estimatedMinutes、displayOrder
- 依赖关系编辑：知识点详情面板/弹窗中列出所有知识点作为可多选的前置依赖
- 表格顶部新增 `+ 新建知识点` 按钮
- 删除前弹出确认框

### 4.4 学习资源管理交互设计

`TeacherResourcesView`：

- 顶部筛选项：按知识点 code 筛选
- 资源卡片列表，每个卡片展示：title、url、type、source、layer、phase
- 每卡片有 `编辑` 和 `删除` 按钮
- 顶部 `+ 新增资源` 按钮，弹出表单
- 保存按钮：全量写入 JSON 文件（PUT 接口）

### 4.5 测试策略

- 后端：6 项 Drogon 测试（知识点增删改 + 依赖更新 + 资源读写 + HTTP 端点）
- 前端：每个页面配测试，覆盖增删改表单渲染、API mock、错误态

## 5. 明确不做

- 不新增数据库表（复用 knowledge_points、knowledge_dependencies）
- 不将学习资源迁入数据库（保持 JSON 文件）
- 不做可视化的依赖图编辑（使用下拉多选简单表单）
- 不做课程元信息编辑（课程名称/描述由 admin 管理）
- 不做批量导入导出

## 6. 自审

### 占位符扫描
无 TBD、TODO 或未完成段落。

### 内部一致性
- API 端点命名与前端路由一致
- knowledge_points 的所有可编辑字段与数据库 schema 一致
- 权限校验前后端均有覆盖

### 范围检查
聚焦 B 阶段（课程内容管理），不含 C（题库管理）。知识点和资源管理互不交叉。

### 歧义检查
- 知识点删除级联影响 `learner_mastery`、`knowledge_dependencies`、`learning_feedback_records`（通过外键 DELETE CASCADE）
- 依赖关系更新为全量替换模式（先删后插，幂等）
- 资源 JSON 文件路径从 course code 推导（`database/seed/course_{courseCode}.json`），读写需文件存在性检查
