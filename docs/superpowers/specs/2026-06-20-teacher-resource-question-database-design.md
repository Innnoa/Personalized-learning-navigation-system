# 教师端资源与题库数据库化设计

日期：2026-06-20

## 1. 设计背景

当前教师端“学习资源管理”和“题库管理”已经是业务功能，而不是只读演示配置：

- 教师可在页面中新增、删除、编辑资源与题目
- 前端通过整包 JSON 的方式读取和保存
- 后端将资源保存到 `backend/config/learning_resources_<courseCode>.json`
- 后端将题库保存到 `backend/config/question_bank_<courseCode>.json`

这套方案适合演示启动，但不适合作为正式业务持久化层，主要问题包括：

- 无法做精细 CRUD，只能整包读写
- 不利于按课程、知识点、题型、状态做查询与统计
- 不利于并发编辑、审计和后续扩展
- 题型扩展能力弱，默认以单选题结构为中心
- 与现有系统以 SQLite 为核心的数据模型不一致

因此，本次设计目标是把学习资源与题库/题目统一迁移到 SQLite，并把教师端页面改造成标准表格化管理界面。

## 2. 设计目标

本次改造需要同时满足以下目标：

1. 学习资源数据库化，不再以课程 JSON 文件作为主存储
2. 题库与题目数据库化，不再以课程 JSON 文件作为主存储
3. 题目支持多题型：单选、多选、判断、简答
4. 教师端页面改为表格化管理，支持筛选、搜索、增删改
5. 设计需兼容当前系统的课程中心与知识点中心结构
6. 保留现有演示数据，通过自动迁移进入数据库
7. 尽量不破坏学生端现有资源推荐与练习流程

## 3. 设计原则

### 3.1 与现有系统一致

当前系统的核心数据关系是：

- `courses`
- `knowledge_points`
- `knowledge_dependencies`
- `learners`
- `learner_mastery`

资源与题目应继续围绕 `course -> knowledge_point` 展开，而不是引入新的独立层级。

### 3.2 轻量但可扩展

本项目是毕业设计系统，不应过度泛化成复杂内容平台。因此本次设计不引入过多中间实体，而是采用：

- 资源单表
- 题库头表
- 题目表

同时使用 JSON 文本字段承载多题型差异结构，避免一开始拆出大量附属表。

### 3.3 教师体验优先

教师端当前采用课程页 + 子标签页的方式组织功能，因此新的资源管理与题库管理应保持：

- 不跳转到额外后台系统
- 不要求复杂的批处理流程
- 保持“筛选栏 + 表格 + 抽屉/弹窗编辑”的轻量后台交互

### 3.4 自动迁移

当前已有 JSON 数据属于真实演示资产，不应要求手工重录。系统应在数据库表为空时自动导入旧文件内容，并在迁移完成后只以数据库为准。

## 4. 数据模型设计

### 4.1 学习资源表 `learning_resources`

该表用于存储课程资源与知识点资源。

```sql
CREATE TABLE IF NOT EXISTS learning_resources (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    knowledge_point_id INTEGER,
    title TEXT NOT NULL,
    resource_type TEXT NOT NULL DEFAULT 'article',
    source TEXT NOT NULL DEFAULT '',
    url TEXT NOT NULL DEFAULT '',
    description TEXT NOT NULL DEFAULT '',
    recommended_usage TEXT NOT NULL DEFAULT '',
    recommended_phase TEXT NOT NULL DEFAULT 'learn',
    focus_tags_json TEXT NOT NULL DEFAULT '[]',
    display_order INTEGER NOT NULL DEFAULT 0,
    status TEXT NOT NULL DEFAULT 'active' CHECK (status IN ('active', 'archived')),
    created_by_user_id INTEGER,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (knowledge_point_id) REFERENCES knowledge_points(id) ON DELETE SET NULL,
    FOREIGN KEY (created_by_user_id) REFERENCES users(id) ON DELETE SET NULL
);
```

说明：

- `course_id` 必填，资源一定属于某门课程
- `knowledge_point_id` 可空，可表示课程公共资源
- `resource_type` 支持 `video/article/slides/book/website/exercise/other`
- `recommended_phase` 支持 `preview/learn/practice/review/extend`
- `focus_tags_json` 使用 JSON 文本保存标签列表
- `status` 用于软归档

### 4.2 题库表 `question_banks`

题库表用于把题目按课程与知识点组织为可管理的分组。

```sql
CREATE TABLE IF NOT EXISTS question_banks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    knowledge_point_id INTEGER NOT NULL,
    bank_code TEXT NOT NULL,
    bank_name TEXT NOT NULL,
    description TEXT NOT NULL DEFAULT '',
    status TEXT NOT NULL DEFAULT 'active' CHECK (status IN ('active', 'archived')),
    created_by_user_id INTEGER,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (knowledge_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE,
    FOREIGN KEY (created_by_user_id) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE (course_id, bank_code)
);
```

说明：

- 题库必须绑定知识点，不做课程级“无知识点题库”
- 一个知识点可以有一个或多个题库
- `bank_code` 用于接口和迁移过程中的稳定识别

### 4.3 题目表 `questions`

题目表用于承载多题型。

```sql
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    bank_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    knowledge_point_id INTEGER NOT NULL,
    question_code TEXT NOT NULL,
    question_type TEXT NOT NULL CHECK (
        question_type IN ('single_choice', 'multiple_choice', 'true_false', 'short_answer')
    ),
    difficulty_level INTEGER NOT NULL DEFAULT 1 CHECK (difficulty_level BETWEEN 1 AND 5),
    prompt TEXT NOT NULL,
    options_json TEXT NOT NULL DEFAULT '[]',
    answer_json TEXT NOT NULL DEFAULT '{}',
    explanation TEXT NOT NULL DEFAULT '',
    estimated_seconds INTEGER NOT NULL DEFAULT 60 CHECK (estimated_seconds > 0),
    display_order INTEGER NOT NULL DEFAULT 0,
    status TEXT NOT NULL DEFAULT 'active' CHECK (status IN ('active', 'archived')),
    created_by_user_id INTEGER,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (bank_id) REFERENCES question_banks(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (knowledge_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE,
    FOREIGN KEY (created_by_user_id) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE (bank_id, question_code)
);
```

说明：

- `question_type` 控制题型
- `options_json` 对单选、多选、判断题提供选项承载
- `answer_json` 统一承载答案，避免一开始拆多个答案附属表
- `difficulty_level` 改为 1-5，与知识点难度一致

### 4.4 题型数据约定

#### 单选题 `single_choice`

- `options_json`
```json
["A 选项", "B 选项", "C 选项", "D 选项"]
```
- `answer_json`
```json
{"correctIndexes":[1]}
```

#### 多选题 `multiple_choice`

- `options_json`
```json
["A 选项", "B 选项", "C 选项", "D 选项"]
```
- `answer_json`
```json
{"correctIndexes":[0,2]}
```

#### 判断题 `true_false`

- `options_json`
```json
["正确", "错误"]
```
- `answer_json`
```json
{"correctBoolean":true}
```

#### 简答题 `short_answer`

- `options_json`
```json
[]
```
- `answer_json`
```json
{"referenceAnswer":"参考答案文本","keywords":["关键词1","关键词2"]}
```

## 5. 自动迁移设计

### 5.1 迁移来源

- `backend/config/learning_resources.json`
- `backend/config/learning_resources_<courseCode>.json`
- `backend/config/question_bank_<courseCode>.json`

### 5.2 触发条件

系统启动时，当以下条件满足时执行迁移：

- `learning_resources` 表为空，或
- `question_banks` 与 `questions` 表为空

### 5.3 迁移策略

#### 资源迁移

- 读取课程资源 JSON
- 根据 `courseCode` 找到课程
- 根据 `knowledgePointCode` 解析到 `knowledge_points.id`
- 逐条插入 `learning_resources`
- 设置 `display_order` 为原顺序
- 设置 `status = active`

#### 题库迁移

- 读取课程题库 JSON
- 每个 `knowledgePointCode` 生成一个默认题库
- `bank_code` 可采用 `courseCode + "-" + knowledgePointCode + "-default"`
- 将原 `questions` 数组逐条迁入 `questions`
- 若旧数据缺少题型，默认按 `single_choice` 处理
- 若旧数据难度为 `easy/medium/hard`，映射为 `1/3/5`

### 5.4 迁移后行为

迁移完成后：

- 教师端 CRUD 只读写数据库
- 学生端资源与练习读取应改为数据库优先
- JSON 文件不再作为主数据源

## 6. 后端接口设计

### 6.1 学习资源接口

- `GET /api/teacher/courses/{courseCode}/resources`
  支持按知识点、类型、阶段、状态查询

- `POST /api/teacher/courses/{courseCode}/resources`
  新建资源

- `PUT /api/teacher/courses/{courseCode}/resources/{resourceId}`
  更新资源

- `DELETE /api/teacher/courses/{courseCode}/resources/{resourceId}`
  删除资源或改为硬删除

### 6.2 题库接口

- `GET /api/teacher/courses/{courseCode}/question-banks`
- `POST /api/teacher/courses/{courseCode}/question-banks`
- `PUT /api/teacher/courses/{courseCode}/question-banks/{bankId}`
- `DELETE /api/teacher/courses/{courseCode}/question-banks/{bankId}`

### 6.3 题目接口

- `GET /api/teacher/courses/{courseCode}/question-banks/{bankId}/questions`
- `POST /api/teacher/courses/{courseCode}/question-banks/{bankId}/questions`
- `PUT /api/teacher/courses/{courseCode}/question-banks/{bankId}/questions/{questionId}`
- `DELETE /api/teacher/courses/{courseCode}/question-banks/{bankId}/questions/{questionId}`

### 6.4 权限复用

继续复用现有教师课程校验逻辑：

- `teacher_course_assignments`
- `users`
- `courses`

教师只能管理自己被分配课程下的资源、题库与题目。

## 7. 前端 UI / UX 设计

## 7.1 学习资源管理页

保留当前教师课程子页面结构，不新增额外页面层级。

页面结构：

- 顶部工具栏
  - `新增资源`
  - 知识点筛选
  - 资源类型筛选
  - 推荐阶段筛选
  - 状态筛选
  - 关键字搜索

- 主体表格
  - 列：`标题 / 知识点 / 类型 / 阶段 / 来源 / 状态 / 操作`

- 编辑方式
  - 使用右侧抽屉
  - 抽屉中编辑标题、绑定知识点、类型、链接、来源、推荐阶段、推荐用法、标签、状态

交互原则：

- 不保留“整包保存全部”
- 每次新增/编辑/删除为即时 CRUD
- 列表页支持快速筛选与搜索

## 7.2 题库管理页

页面结构采用“两栏式”：

- 左侧：题库列表
  - 按知识点展示题库
  - 支持新增题库、重命名、归档

- 右侧：题目管理区
  - 顶部显示当前题库名
  - 支持按题型、难度、状态筛选
  - 表格列：`题目编码 / 题型 / 难度 / 题干摘要 / 状态 / 操作`

- 编辑方式
  - 使用抽屉或大弹窗编辑题目
  - 根据 `question_type` 动态切换表单

### 7.3 题目编辑表单

#### 单选 / 多选

- 题目编码
- 题型
- 难度
- 题干
- 动态选项列表
- 正确答案选择
- 解析
- 建议作答时长

#### 判断题

- 题目编码
- 难度
- 题干
- 正确答案：正确 / 错误
- 解析

#### 简答题

- 题目编码
- 难度
- 题干
- 参考答案
- 关键词列表
- 解析

### 7.4 风格约束

延续当前教师端风格：

- 不引入厚重后台框架
- 使用现有卡片、表格、按钮视觉语言
- 保持课程页子标签结构不变
- 重点提升可管理性，而不是做全新设计系统

## 8. 与现有学生端的兼容

### 8.1 资源推荐

学生端资源推荐服务应改为数据库读取，但外部返回结构尽量保持兼容，避免同时重写学生端资源页。

### 8.2 练习与题库读取

学生端练习题获取逻辑应从数据库加载题目，而不是依赖前端静态题库数据。

为控制改造范围，允许保留现有前端练习渲染形式，但数据源迁移到后端数据库。

## 9. 测试范围

至少覆盖以下内容：

1. 新表建表成功
2. 自动迁移可把旧 JSON 导入数据库
3. 教师资源 CRUD 正常
4. 教师题库 CRUD 正常
5. 教师题目 CRUD 正常
6. 多题型题目读写正常
7. 教师端资源页表格可正常筛选、创建、编辑、删除
8. 教师端题库页可正常新增题库并管理题目
9. 学生端资源推荐仍能读取资源
10. 学生端练习仍能获取题目

## 10. 非目标

本次不处理：

- 主知识图谱结构重构
- 学习路径算法重写
- 学生端整体 UI 改版
- 自动评分系统
- 复杂试卷编排系统

## 11. 预期结果

改造完成后：

- 教师端资源和题库都成为真正的数据库业务数据
- 资源与题目都能以标准表格方式管理
- 题目支持多题型
- 现有演示 JSON 数据可自动进入数据库
- 系统整体架构从“配置文件驱动”进一步收敛到“数据库驱动”
