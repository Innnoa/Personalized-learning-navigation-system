# AIREAD

最后更新：2026-03-17

## 1. 项目定位

项目名称暂定为：**基于知识点依赖图的个性化学习导航系统**

本项目以“**本科毕业设计可落地、可答辩、可演示**”为最高目标，优先选择：

- 结构清晰
- 实现可控
- 便于阶段性演示
- 适合逐步开发与论文撰写

禁止为了“看起来高级”而引入明显超出本科毕设必要范围的复杂技术方案。

## 2. 固定技术栈

- 前端：Vue
- 后端：Drogon（C++）
- 数据库：SQLite
- 核心算法：C++
- 图谱展示：Cytoscape.js

## 3. 核心目标

1. 构建课程知识点依赖图
2. 根据学习者目标、掌握度、可用时间等信息生成个性化学习路径
3. 学习后根据反馈动态调整推荐路径
4. 在前端可视化展示知识图谱和推荐路径
5. 后续支持“推荐理由解释”功能

## 4. 设计约束

后续所有方案设计、模块划分、接口定义、开发排期与演示规划，必须始终围绕以下原则：

- 适合本科毕业设计答辩
- 适合本地开发与单机部署
- 适合小规模真实演示
- 算法与系统逻辑能够清楚讲解
- 开发难度与时间成本可控

原则上**不要引入**以下复杂技术作为核心方案：

- 微服务
- Redis
- Docker 集群
- 大模型核心决策
- 图神经网络
- 强化学习
- 其他明显超出本科毕设必要性的重型架构

## 5. 方案输出要求

后续讨论与实现时，优先提供以下内容：

- 明确的模块划分
- 适合 C++ / Drogon 的工程结构
- 可执行的开发顺序
- 贴近本科毕设场景的设计建议

如存在多个可选方案，优先选择：

- 更容易实现的
- 更容易讲清楚原理的
- 更方便演示结果的
- 更方便写论文与答辩展示的

## 6. 当前默认开发方向

系统可按以下主线逐步建设：

1. 知识点与依赖关系建模
2. 学习者信息建模
3. 个性化路径生成算法
4. 学习反馈与路径动态调整
5. 前端图谱展示与路径展示
6. 推荐理由解释

## 7. 文档维护规则

从本次开始，**所有系统改动 / 架构改动**都必须同步记录到本文件。

记录范围包括但不限于：

- 系统模块新增、删除、重构
- 前后端接口设计调整
- 数据库表结构或字段语义调整
- 核心算法流程调整
- 推荐策略变更
- 图谱建模方式变更
- 部署方式或工程结构调整

## 8. 变更记录格式

后续新增记录时，统一追加到本文件末尾，建议使用以下格式：

### [YYYY-MM-DD] 变更标题

- 变更类型：系统改动 / 架构改动
- 变更内容：
- 变更原因：
- 影响范围：
- 是否影响既有接口/数据库/算法：
- 验证方式：

## 9. 本文件用途

本文件作为本项目的长期开发约束与架构留痕文档使用。后续如出现系统级或架构级调整，应优先更新此文件，再继续实现。

### [2026-03-17] 第一版项目初始化目录结构

- 变更类型：架构改动
- 变更内容：
  确定项目采用“前后端分离 + 单体后端 + 内嵌算法模块 + SQLite 本地数据库”的第一版目录结构。
  建议根目录按 `frontend/`、`backend/`、`database/`、`docs/` 四个主区域组织。
  其中后端使用 Drogon，内部按 `controllers/`、`services/`、`repositories/`、`models/`、`algorithm/`、`utils/`、`config/` 分层。
  算法模块不单独拆服务，直接作为后端中的 C++ 业务模块，由 Service 层统一调用。
  SQLite 数据库文件与 SQL 初始化脚本分离存放，数据库实例建议放在 `database/data/`，建表脚本放在 `database/init/`，样例数据放在 `database/seed/`。
  前后端联调时采用 `Controller -> Service -> Repository / Algorithm` 的单向调用链，避免控制器直接访问数据库。
  “推荐理由解释”作为可扩展能力预留在算法输出结构和 Service 响应对象中，不单独增加复杂中间层。
- 变更原因：
  需要在项目初始化阶段先固定一版适合本科毕设的工程骨架，保证后续实现、答辩展示和论文撰写都有统一依据。
- 影响范围：
  项目根目录组织方式、Drogon 后端模块分层、算法模块放置方式、SQLite 文件布局、前后端接口联调方式。
- 是否影响既有接口/数据库/算法：
  当前为初始化阶段，属于第一版基线设计，不涉及兼容性破坏。
- 验证方式：
  后续创建项目骨架时检查目录是否与本方案一致；接口开发时检查是否遵循既定分层；数据库初始化时检查脚本与数据文件是否分离存放。

### [2026-03-17] 第一版项目初始化目录结构修订

- 变更类型：架构改动
- 变更内容：
  当前项目初始化目录结构以本修订版为准。

  建议目录结构调整为：

  ```text
  Personalized-learning-navigation-system/
  ├── AIREAD.md
  ├── README.md
  ├── docs/
  │   ├── architecture/
  │   ├── api/
  │   ├── thesis/
  │   ├── course-scope.md
  │   └── development-plan.md
  ├── frontend/
  │   └── src/
  │       ├── api/
  │       ├── assets/
  │       ├── components/
  │       ├── views/
  │       ├── router/
  │       ├── stores/
  │       ├── graph/
  │       ├── types/
  │       └── utils/
  ├── backend/
  │   ├── CMakeLists.txt
  │   ├── main.cc
  │   ├── config/
  │   │   └── config.json
  │   ├── controllers/
  │   ├── services/
  │   ├── repositories/
  │   ├── models/
  │   ├── dto/
  │   ├── algorithm/
  │   │   ├── graph/
  │   │   ├── planner/
  │   │   ├── adjuster/
  │   │   └── explainer/
  │   ├── utils/
  │   └── tests/
  ├── database/
  │   ├── init/
  │   │   └── 001_schema.sql
  │   ├── seed/
  │   │   ├── course_xxx.sql
  │   │   ├── demo_user.sql
  │   │   └── demo_learning_records.sql
  │   ├── data/
  │   └── docs/
  │       └── er-diagram.md
  └── scripts/
  ```

  本次修订明确以下约束：

  1. `database/init/` 只负责数据库结构初始化，不再使用 `002_seed_data.sql` 这类与 `seed/` 重叠的命名。
  2. `database/seed/` 只负责演示数据和样例课程数据，职责与 `init/` 分离。
  3. `database/data/*.db` 与 `database/data/*.sqlite` 必须加入 `.gitignore`；如未来保留 `demo.db`，也只能视为演示快照，不能当作数据库结构的真实来源。
  4. `docs/course-scope.md` 用于明确当前毕设演示的课程边界、知识点范围与演示场景。
  5. `docs/development-plan.md` 用于固化可执行开发顺序，避免目录存在但没有阶段目标。
  6. 前端 `stores/` 只放跨页面共享状态，例如学习者画像、当前推荐路径、图谱筛选条件；弹窗开关、表单输入、临时面板状态保留在组件内部。
  7. 后端分层继续保持 `Controller -> Service -> Repository / Algorithm`，但必须显式补上图数据装配责任，推荐链路为 `Repository -> GraphBuilder -> Planner`。
  8. `GraphBuilder` 可位于 `backend/algorithm/graph/`，或由 `services/` 统一协调，但职责必须清晰：Repository 读取原始数据，GraphBuilder 组装 `KnowledgeGraph`，Planner 接收 `KnowledgeGraph` 做路径规划。
  9. `planner` 与 `explainer` 的输入输出边界必须在设计上固定：
     - `planner` 输入：`KnowledgeGraph`、`LearnerProfile`、`PlanningTarget`、`TimeBudget`
     - `planner` 输出：`LearningPath`、`PathReasonTrace`
     - `explainer` 输入：`LearningPath`、`PathReasonTrace`
     - `explainer` 输出：`ExplanationDto` 或等价结构
  10. “推荐理由解释”禁止凭空生成，必须消费规划阶段输出的 `PathReasonTrace`。解释模块的职责是把依据转换为人类可读说明，而不是重新猜测推荐原因。
  11. 接口命名统一偏向“路径规划/学习导航”语义，而不是泛化的 recommend 语义。当前建议接口为：
      - `GET /api/health`
      - `GET /api/knowledge/graph`
      - `POST /api/path/generate`
      - `POST /api/path/adjust`
      - `GET /api/learner/profile`
      - `POST /api/feedback/submit`
  12. `backend/tests/` 的优先测试重点不是 HTTP 层，而是算法关键用例：
      - 图构建是否正确
      - 拓扑排序是否正确
      - 路径规划是否满足依赖关系
      - 掌握度更新是否符合规则
      - 路径动态调整是否产生合理变化

  推荐理由依据建议至少包含知识点、前置关系、掌握度差距、相关性分值、重要性分值、时间成本惩罚与触发原因列表等字段，供后续解释模块消费。
- 变更原因：
  根据外部审查意见，需要把原有方案中容易职责漂移或命名混乱的部分收紧，特别是数据库脚本边界、推荐理由来源、图构建责任和接口命名一致性。
- 影响范围：
  文档结构、数据库初始化约定、后端算法分层、推荐理由解释实现方式、接口命名、前端状态管理边界、测试优先级。
- 是否影响既有接口/数据库/算法：
  当前仍处于初始化阶段，没有已发布实现；本次修订用于避免后续架构走偏。
- 验证方式：
  后续初始化项目时检查目录与命名是否遵循本修订版；设计 `planner` / `explainer` 接口时检查是否包含 `PathReasonTrace`；数据库初始化时检查 `init/` 与 `seed/` 是否职责分离；测试阶段优先覆盖算法关键用例。
