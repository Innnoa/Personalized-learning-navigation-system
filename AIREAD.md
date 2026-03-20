# AIREAD

最后更新：2026-03-19

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
  13. 推荐理由依据建议至少包含知识点、前置关系、掌握度差距、相关性分值、重要性分值、时间成本惩罚与触发原因列表等字段，供后续解释模块消费。
- 变更原因：
  根据外部审查意见，需要把原有方案中容易职责漂移或命名混乱的部分收紧，特别是数据库脚本边界、推荐理由来源、图构建责任和接口命名一致性。
- 影响范围：
  文档结构、数据库初始化约定、后端算法分层、推荐理由解释实现方式、接口命名、前端状态管理边界、测试优先级。
- 是否影响既有接口/数据库/算法：
  当前仍处于初始化阶段，没有已发布实现；本次修订用于避免后续架构走偏。
- 验证方式：
  后续初始化项目时检查目录与命名是否遵循本修订版；设计 `planner` / `explainer` 接口时检查是否包含 `PathReasonTrace`；数据库初始化时检查 `init/` 与 `seed/` 是否职责分离；测试阶段优先覆盖算法关键用例。

### [2026-03-18] WSL 前端依赖安装脚本调整

- 变更类型：系统改动
- 变更内容：
  前端依赖安装脚本不再无条件清空代理环境变量，而是按“当前代理可用性探测 -> WSL 宿主机代理地址改写 -> 直连回退”的顺序自动选择联网模式。
  脚本会在安装前显式输出 Node 版本、npm 版本、registry 地址和当前实际采用的 network mode。
  安装命令统一增加 `--no-progress`、`--audit=false`、`--fund=false`、`--fetch-timeout`、`--fetch-retries`，降低 WSL 下首次安装时看似卡住和额外等待的问题。
  前端启动脚本新增依赖存在性检查，避免在 `node_modules` 未安装完成时直接运行 `vite`。
- 变更原因：
  当前开发环境为 WSL Arch，已存在代理环境变量。原脚本直接删除代理配置，且安装阶段缺少可观测输出，容易让前端依赖安装表现为“长时间卡住但无明确信息”。
- 影响范围：
  `scripts/install_frontend_deps.sh`、`scripts/run_frontend.sh`、`README.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库和算法，仅影响本地开发环境脚本行为与可观测性。
- 验证方式：
  执行 `./scripts/install_frontend_deps.sh`，确认脚本会输出 network mode 并完成依赖安装；执行 `./scripts/run_frontend.sh`，确认依赖缺失时会给出明确提示，依赖存在时可正常启动前端服务。

### [2026-03-18] 演示课程确定为数据结构

- 变更类型：系统改动
- 变更内容：
  当前毕设演示课程正式确定为“数据结构”。
  `docs/course-scope.md` 已同步更新为以“数据结构”为当前唯一演示课程。
  当前已纳入参考的课程资料包括：
  - `docs/Table_of_Contents_for_Data_Structures.doc`
  - `docs/Data_Structures_Question_Bank.xls`
  这两份资料将作为后续整理课程章节、知识点范围、题目覆盖分布与知识图谱初稿的基础来源。
- 变更原因：
  需要尽快固定课程边界，避免系统设计和数据库建模继续悬空；同时“数据结构”具备章节递进清晰、依赖关系明显、适合知识图谱建模与路径规划展示的特点，适合作为本科毕设演示课程。
- 影响范围：
  课程范围文档、数据库样例数据设计、知识点依赖图构建、题库到知识点映射、推荐路径演示场景。
- 是否影响既有接口/数据库/算法：
  会影响后续 `database/seed/` 中课程数据命名、知识点集合选择和路径规划测试样例，但当前尚未产生破坏性兼容问题。
- 验证方式：
  后续创建课程种子数据时检查是否围绕“数据结构”展开；建立知识点清单时检查是否与章节目录和题库内容一致；路径规划示例是否能对应“数据结构”中的典型学习目标。

### [2026-03-18] 初始化前后端最小工程骨架

- 变更类型：系统改动
- 变更内容：
  已创建 `frontend/` 与 `backend/` 两个顶层工程目录，作为前后端分离架构的第一版最小可运行骨架。
  后端使用 Drogon 初始化，并完成以下整理：
  - 配置文件移动到 `backend/config/config.json`
  - 创建 `controllers/`、`services/`、`repositories/`、`dto/`、`algorithm/`、`utils/`、`tests/`
  - 提供最小接口 `GET /api/health`
  - 增加 `SystemStatusService` 与 `TimeUtils` 作为第一版服务层与工具层示例
  前端使用手工脚手架初始化 Vue + Vite 工程结构，并建立：
  - `src/api/`
  - `src/router/`
  - `src/stores/`
  - `src/graph/`
  - `src/components/`
  - `src/views/`
  当前首页已预留课程概览、健康检查卡片与知识图谱演示区域。
  另外已新增：
  - `scripts/run_backend.sh`
  - `scripts/install_frontend_deps.sh`
  - `scripts/run_frontend.sh`
  用于本地启动与依赖安装。
  当前已识别到 WSL 环境存在 `HTTP_PROXY/HTTPS_PROXY/ALL_PROXY=http://127.0.0.1:7890`，该代理变量会影响 Node 包管理器访问外网；前端安装与运行脚本已显式取消这些代理环境变量。
- 变更原因：
  需要尽快从“仅有方案文档”进入“已有可执行骨架”的阶段，确保后续数据库、知识图谱与路径规划逻辑可以直接落位；同时需要处理 WSL 环境下前端包管理器受代理变量干扰的问题。
- 影响范围：
  项目目录结构、后端构建方式、前端开发方式、本地运行脚本、WSL 环境下的前端依赖安装流程。
- 是否影响既有接口/数据库/算法：
  当前仅新增 `GET /api/health` 作为初始化接口，不影响后续正式业务接口设计；数据库和算法主逻辑仍待继续实现。
- 验证方式：
  后端通过本地 CMake 构建验证；前端通过目录结构与入口文件检查骨架完整性；后续在 WSL 中执行 `scripts/install_frontend_deps.sh` 时确认代理变量已被取消。

### [2026-03-18] 第一版数据结构课程知识图谱落地

- 变更类型：系统改动
- 变更内容：
  已在 `database/init/001_schema.sql` 中建立第一版知识图谱数据库基线，当前采用三张核心表：
  - `courses`
  - `knowledge_points`
  - `knowledge_dependencies`
  已在 `database/seed/course_data_structures.sql` 中写入“数据结构”课程演示数据，当前包含 1 门课程、14 个核心知识点、15 条前置依赖边。
  已新增 `scripts/init_database.sh` 用于重建 `database/data/app.db`。
  后端已新增 `GET /api/knowledge/graph`，调用链为 `KnowledgeGraphController -> KnowledgeGraphService -> KnowledgeGraphRepository`，由 Repository 读取 SQLite，由 Service 组装图谱 JSON。
  前端 `KnowledgeGraphPreview` 已从本地 demo 数据切换为调用真实接口，并展示 loading、error 和图谱统计信息。
  `docs/course-scope.md` 已补充当前第一版章节范围、核心知识点与典型依赖关系。
- 变更原因：
  需要尽快把项目从“仅有骨架”推进到“已有真实课程图谱可展示”的状态，确保后续学习路径规划、动态调整和推荐理由解释都能基于真实数据继续开发。
- 影响范围：
  `database/` SQL 结构与种子数据、`scripts/init_database.sh`、后端知识图谱查询链路、前端图谱展示组件、README 运行说明、课程范围文档。
- 是否影响既有接口/数据库/算法：
  新增 `GET /api/knowledge/graph` 与数据库表结构，属于第一版正式业务数据基线；当前尚未接入路径规划算法，但已为后续 `PathReasonTrace` 和学习路径生成预留图数据基础。
- 验证方式：
  执行 `./scripts/init_database.sh` 生成数据库；使用 `sqlite3` 确认课程、知识点和依赖边数量；构建 backend 并请求 `/api/knowledge/graph` 返回 200 JSON；启动 frontend 并确认 `/api/knowledge/graph` 可通过 Vite 代理访问。

### [2026-03-18] 第一版最小路径规划能力接入

- 变更类型：系统改动
- 变更内容：
  已在 `backend/algorithm/planner/` 中新增最小版路径规划算法，输入包含目标知识点、可用时间和掌握度映射，输出包含学习路径、节点状态和 `PathReasonTrace`。
  已新增 `POST /api/path/generate`，调用链为 `PathPlanningController -> PathPlanningService -> LearningPathPlanner`。
  当前路径规划规则保持简单可讲解：
  - 先计算目标知识点的前置依赖闭包
  - 再按依赖拓扑顺序生成路径
  - 根据掌握度将节点分为 `mastered / scheduled / deferred`
  - 根据时间预算决定哪些节点纳入当前轮次学习
  前端首页已新增路径规划面板，支持设置目标知识点、可用时间和掌握度预设，并展示“本轮推荐学习 / 下一轮建议”。
  当前 `PathReasonTrace` 已显式输出目标性、前置依赖性、掌握度差距、相关度、重要度、时间成本惩罚和触发原因列表，后续 explainer 必须消费该结构生成可读解释。
  已新增一个最小算法测试用例，验证路径规划会先满足前置依赖再输出目标节点。
- 变更原因：
  需要把项目从“只有知识图谱展示”推进到“已经具备最小个性化学习导航能力”，让答辩时能真实演示“目标 + 掌握度 + 时间预算 -> 学习路径”的核心闭环。
- 影响范围：
  `backend/algorithm/planner/`、`backend/controllers/PathPlanningController.*`、`backend/services/PathPlanningService.*`、前端路径规划组件、README 运行与测试说明、课程范围文档中的演示场景。
- 是否影响既有接口/数据库/算法：
  新增 `POST /api/path/generate`；不修改既有数据库表结构，但开始正式消费知识图谱数据作为路径规划输入；为后续 `POST /api/path/adjust` 和 explainer 奠定输入输出边界。
- 验证方式：
  构建 backend 与 frontend；执行 `ctest --test-dir backend/build/tests --output-on-failure` 通过算法测试；使用 `curl` 请求 `/api/path/generate` 返回 200 JSON；启动前端后确认路径规划面板可生成结果并通过 Vite 代理访问后端接口。

### [2026-03-18] 第一版学习反馈驱动的路径动态调整能力接入

- 变更类型：系统改动
- 变更内容：
  已在 `backend/algorithm/adjuster/` 中新增最小版路径调整模块，当前通过 `LearningPathAdjuster` 消费学习反馈并更新掌握度，再复用 planner 重新生成学习路径。
  已新增 `POST /api/path/adjust`，调用链保持为 `PathPlanningController -> PathPlanningService -> LearningPathAdjuster -> LearningPathPlanner`。
  当前接口输入由两部分组成：
  - 基础规划请求：`targetCodes`、`availableMinutes`、`masteryByCode`
  - 学习反馈：`feedbackItems[{ code, completionStatus, selfRatedMastery }]`
  当前 `completionStatus` 仅支持三种规则，便于本科毕设阶段讲清楚：
  - `completed`：将节点掌握度提升到已掌握阈值附近
  - `partial`：适度提升掌握度，但仍保留在需要巩固的区间
  - `blocked`：将掌握度压回低区间，避免系统直接推进后继节点
  当前接口输出除重新规划后的 `path` 与 `summary` 外，还新增：
  - `feedbackSummary`
  - `updatedMasteryByCode`
  - `adjustments`
  其中 `adjustments` 会记录每个反馈节点的旧掌握度、新掌握度、采用规则和调整原因，保证“动态调整”过程可以被展示和解释。
  前端路径规划面板已新增学习反馈区域，只对当前轮次 `scheduled` 节点收集反馈；提交后会同步展示最新路径、反馈统计与最近一次调整说明。
- 变更原因：
  需要把系统从“只会生成第一版路径”推进到“能够根据学习结果做下一轮调整”的状态，形成更完整的学习导航闭环；同时要确保后续“推荐理由解释”建立在已有规划依据和调整依据之上，而不是事后拼接说明。
- 影响范围：
  `backend/algorithm/adjuster/`、`backend/controllers/PathPlanningController.*`、`backend/services/PathPlanningService.*`、前端路径规划组件、README 手动测试说明、课程范围文档中的演示场景。
- 是否影响既有接口/数据库/算法：
  新增 `POST /api/path/adjust`；不修改既有数据库表结构；算法层新增“反馈更新掌握度 -> 重新规划”的闭环，但仍保持单机、可讲解、规则可控的实现方式。
- 验证方式：
  执行 `ctest --test-dir backend/build/tests --output-on-failure` 验证路径调整算法测试通过；使用 `curl` 请求 `/api/path/adjust` 返回 200 JSON；在前端路径规划面板提交反馈后，确认页面会刷新学习路径、显示反馈统计并输出最近一次调整说明。

### [2026-03-18] 第一版推荐理由解释层接入

- 变更类型：系统改动
- 变更内容：
  已在 `backend/algorithm/explainer/` 中新增最小版 `LearningPathExplainer`，作为独立解释层消费 `LearningPathItem.reasonTrace` 与节点基础信息，输出轻量 explanation 结构：
  - `summary`
  - `labels`
  - `bullets`
  当前 explanation 严格基于 planner 已输出的 `PathReasonTrace`、节点状态和节点元信息生成，不新增新的推荐决策分值，也不改变 planner / adjuster 的路径排序与掌握度更新规则。
  `PathPlanningService` 已统一为 `POST /api/path/generate` 与 `POST /api/path/adjust` 的 `path[]` 节点注入 `explanation` 字段，并保留原始 `reasonTrace` 作为底层依据，方便后续答辩时同时展示“结构化依据”和“人类可读解释”。
  前端路径规划面板已从直接显示 `reasonTrace.triggerReasons` 调整为优先显示 `explanation.summary`，并补充解释标签，使“本轮推荐学习 / 下一轮建议”更接近答辩展示语言。
  后端测试已新增 explainer 用例，验证解释文本确实来自目标性、前置依赖性和掌握度差距等已有依据。
- 变更原因：
  需要把“推荐理由解释”从预留设计推进为真实可演示能力，同时保持解释层和规划层职责分离，避免后续出现“解释凭空生成、与规划依据脱节”的问题。
- 影响范围：
  `backend/algorithm/explainer/`、`backend/services/PathPlanningService.cc`、`frontend/src/components/PathPlannerPanel.vue`、`backend/tests/`、README 手动测试说明。
- 是否影响既有接口/数据库/算法：
  不修改数据库结构和规划算法规则；在既有 `path[]` 节点响应上新增 `explanation` 字段，属于向后兼容增强。
- 验证方式：
  重新构建 backend；执行 `ctest --test-dir backend/build/tests --output-on-failure` 通过 4 个测试；执行 `npm run build` 通过前端构建；启动临时后端实例并请求 `/api/path/generate`，确认返回的 `path[]` 中包含 `explanation.summary`、`explanation.labels` 和 `explanation.bullets`。

### [2026-03-18] 第一版学习者画像与反馈持久化接入

- 变更类型：系统改动
- 变更内容：
  已在 `database/init/001_schema.sql` 中新增三张学习者相关表：
  - `learners`
  - `learner_mastery`
  - `learning_feedback_records`
  已在 `database/seed/course_data_structures.sql` 中写入默认演示学习者 `demo-learner` 及 14 条初始掌握度记录。
  `backend/config/config.json` 已增加 `default_learner_code`，用于在单学习者演示模式下读取默认画像。
  后端已新增：
  - `GET /api/learner/profile`
  - `POST /api/feedback/submit`
  新接口继续沿用 `Controller -> Service -> Repository` 分层，其中 `feedback/submit` 会复用 adjuster 的反馈更新规则，把反馈结果写入 `learning_feedback_records`，并同步 upsert 到 `learner_mastery`。
  前端首页已新增学习者画像卡片，路径规划面板会优先读取数据库中的画像掌握度作为默认输入；提交反馈时，首页主流程改为“保存反馈 -> 更新画像 -> 重新生成路径”，而 `POST /api/path/adjust` 保留为独立的动态调整演示接口。
- 变更原因：
  需要把系统从“仅有即时演示逻辑”推进到“已有学习者状态留痕”的阶段，让项目具备更完整的系统闭环，也更适合论文中的数据库设计、功能模块设计和答辩演示。
- 影响范围：
  `database/init/001_schema.sql`、`database/seed/course_data_structures.sql`、`backend/config/config.json`、新增 learner/feedback 后端链路、前端首页与路径规划面板联动、README 与课程范围文档。
- 是否影响既有接口/数据库/算法：
  扩展了数据库结构；新增 `GET /api/learner/profile` 与 `POST /api/feedback/submit`；不改变现有路径规划与推荐解释算法规则，但把反馈更新规则抽取为可复用逻辑，以保证内存规划与数据库持久化一致。
- 验证方式：
  执行 `./scripts/init_database.sh` 重建数据库；使用 `sqlite3` 确认 learner 相关表存在且已写入默认学习者与初始掌握度；重新构建 backend；执行 `ctest --test-dir backend/build/tests --output-on-failure` 通过 5 个测试；执行 `npm run build` 通过前端构建；启动临时后端实例并通过 `curl` 验证 `GET /api/learner/profile` 和 `POST /api/feedback/submit` 可正常返回，并检查数据库中 `learning_feedback_records` 已写入新记录。

### [2026-03-18] 第一版图谱详情面板与解释展开区接入

- 变更类型：系统改动
- 变更内容：
  前端 `KnowledgeGraphPreview` 已新增图谱节点详情面板。当前点击知识点节点后，会在同一张卡片内显示：
  - 节点章节位置
  - 节点类型
  - 难度等级
  - 预计学习时间
  - 节点说明
  - 直接前置依赖
  - 直接后继节点
  该面板完全复用现有 `GET /api/knowledge/graph` 返回的 nodes/edges 数据，不新增后端接口。
  前端 `PathPlannerPanel` 已为路径卡片新增“展开理由 / 收起理由”交互，展开后会显示：
  - `explanation.bullets`
  - 目标相关度
  - 重要度
  - 时间占比
  这样图谱、学习路径和推荐解释之间的关系在首页中形成了更清晰的演示闭环。
- 变更原因：
  当前系统核心能力已基本齐备，需要进一步提升答辩展示时的可读性和可讲解性，让用户可以从图谱层面查看知识点关系，也能从路径层面展开查看更细的推荐依据。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/PathPlannerPanel.vue`、README 手动测试说明。
- 是否影响既有接口/数据库/算法：
  不影响现有接口、数据库和算法规则；属于前端展示层增强，完全复用已有图谱与 explanation 数据。
- 验证方式：
  执行 `npm run build` 通过前端构建；启动前端后检查点击图谱节点是否能显示详情面板；检查路径卡片点击“展开理由”后是否能看到 explanation 细项与指标。

### [2026-03-18] 首页外层布局改为全窗口分布

- 变更类型：系统改动
- 变更内容：
  前端首页外层 `PageLayout` 已取消固定宽度居中容器，改为基于浏览器窗口宽度的全宽布局，并使用响应式横向留白控制大屏与中屏下的边距。
  首页 `HomeView` 主网格列比例同步调整，使知识图谱与路径规划区域在桌面端能获得更大的横向展示空间，不再长期收缩在约千像素宽的中间列内。
  标题说明文案仍保留可读性行宽限制，避免全文字区域随着页面拉伸而过长。
- 变更原因：
  当前首页已经承载知识图谱、路径规划、学习者画像、健康状态与说明卡片。继续使用固定窄容器会压缩图谱和规划面板，不利于全屏演示与答辩展示。
- 影响范围：
  `frontend/src/components/PageLayout.vue`、`frontend/src/views/HomeView.vue`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库和算法，仅影响前端首页展示布局。
- 验证方式：
  执行 `npm run build` 验证前端构建通过；在浏览器中确认首页主内容区域沿窗口宽度展开，而不是被限制在固定居中窄列中。

### [2026-03-18] 知识图谱边走线改为绕开节点的分层连线

- 变更类型：系统改动
- 变更内容：
  前端 `KnowledgeGraphPreview` 中的 Cytoscape 边样式已从普通 `bezier` 曲线调整为更适合 DAG 展示的 `round-taxi` 分层连线，并显式设置向下走线、转折比例和最小转折距离。
  同时提高了 `breadthfirst` 布局的层间距、边界留白与节点避让参数，并拉高图谱画布最小高度，减少边线从中间节点下方穿过或被节点遮挡的情况。
- 变更原因：
  当前数据结构知识图谱存在部分内部依赖边被中间节点覆盖的问题，影响路线可读性和演示效果。分层依赖图更适合使用正交绕行连线而不是普通曲线。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法，仅影响知识图谱前端渲染方式。
- 验证方式：
  执行 `npm run build` 验证前端构建通过；在浏览器中确认图谱内部连线会绕开中间节点，内部依赖路径的可见性优于原始曲线方案。

### [2026-03-18] 图谱节点选中时高亮上下游依赖链

- 变更类型：系统改动
- 变更内容：
  前端 `KnowledgeGraphPreview` 已新增“选中节点上下文高亮”逻辑。
  当前点击某个知识点后，会同时高亮该节点、其上下游相关节点以及对应依赖边，其余无关节点和边会降透明度，帮助用户聚焦当前知识点所在的学习链路。
- 变更原因：
  即使边的走线已经改为分层绕行，复杂依赖图中仍可能存在视觉干扰。通过聚焦当前节点的上下游链路，可以进一步提升图谱可读性和答辩演示时的讲解效率。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法，仅影响图谱前端交互展示逻辑。
- 验证方式：
  执行 `npm run build` 验证前端构建通过；在浏览器中点击任意节点，确认其相关依赖链被高亮，其余元素会降透明度。

### [2026-03-18] 后端补充学习者画像与反馈持久化定向测试

- 变更类型：系统改动
- 变更内容：
  `backend/tests` 已新增学习者画像与反馈持久化链路的定向测试，并补充独立测试数据库初始化支持。
  当前测试启动时会自动创建隔离的 SQLite 测试库，避免直接读写开发演示用的 `database/data/app.db`。
  新增测试重点覆盖：
  - 默认学习者画像读取是否正确
  - `POST /api/feedback/submit` 对应的服务层反馈写库是否会同步更新 `learner_mastery`
  - 反馈记录数量、掌握度结果和返回画像摘要是否保持一致
  同时测试目标已补充 service、repository 与 utils 相关源码链接，确保 learner/profile 与 feedback/persistence 链路可以在测试中直接验证。
- 变更原因：
  当前项目主功能已经基本跑通，下一步更适合先补强关键链路测试，提升系统可信度，便于论文中说明数据库更新闭环与答辩时展示“结果可验证”。
- 影响范围：
  `backend/tests/CMakeLists.txt`、`backend/tests/test_main.cc`、`backend/tests/TestSupport.h`、`backend/tests/TestSupport.cc`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、README 测试说明。
- 是否影响既有接口/数据库/算法：
  不影响既有接口语义、数据库结构和算法规则；仅增强后端测试体系，并为测试运行引入隔离数据库。
- 验证方式：
  执行 `cmake -S backend -B backend/build`、`cmake --build backend/build -j$(nproc)` 和 `ctest --test-dir backend/build/tests --output-on-failure`，确认 6 个测试全部通过。

### [2026-03-18] 接入最近学习记录展示与路径前后对比

- 变更类型：系统改动
- 变更内容：
  后端 `GET /api/learner/profile` 对应的画像 payload 已扩展 `recentFeedbackItems`，用于返回最近学习反馈记录，包括知识点、完成情况、掌握度前后变化、自评掌握度、规则说明和记录时间。
  前端 `LearnerProfileCard` 已新增最近学习记录展示区，用于在画像卡片中直接回顾最近反馈历史。
  前端 `PathPlannerPanel` 已新增“调整前后路径对比”展示区。当前在提交反馈后，会保留调整前路径快照，并与重新生成后的路径结果做对比，展示：
  - 调整前本轮安排
  - 调整后本轮安排
  - 转为已掌握 / 提前到本轮 / 移出本轮 / 顺序调整 等关键变化
- 变更原因：
  需要把“反馈闭环”进一步做成老师可直观看懂的演示流程，让系统不仅能更新路径，还能展示学习记录和路径变化依据，提升答辩讲解效果。
- 影响范围：
  `backend/repositories/LearnerProfileRepository.h`、`backend/repositories/LearnerProfileRepository.cc`、`backend/services/LearnerProfileService.cc`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、`frontend/src/components/LearnerProfileCard.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  扩展了 learner/profile 响应结构，但不改变既有字段语义；不影响数据库结构与核心算法，仅增强展示层和画像响应内容。
- 验证方式：
  执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure` 通过后端验证；执行 `npm run build` 通过前端构建；在页面提交学习反馈后，确认学习者画像卡片显示最近记录，路径规划面板显示调整前后路径对比。

### [2026-03-18] 知识图谱接入多层下钻与第一个细化节点

- 变更类型：架构改动
- 变更内容：
  当前知识图谱已从“单层课程图谱”扩展为“课程主图谱 + 可选细化 scope 图谱”的展示结构。
  后端 `GET /api/knowledge/graph` 已支持通过 `scopeCode` 参数加载指定层级图谱；课程主图谱节点会携带 `hasDetailGraph` 与 `detailScopeCode`，用于标记哪些节点可以继续下钻。
  前端 `KnowledgeGraphPreview` 已新增：
  - 面包屑路径导航
  - 双击节点进入细化图谱
  - 在详情面板中直接跳转到细化图谱
  当前第一个已落地的细化节点为 `ds-intro / 数据结构基本概念`，材料依据为《数据结构（2025秋）》第一章 1.2 基本概念和术语。
  细化图谱数据暂存于 `backend/config/knowledge_graph_details.json`，用于支持后续按单元节点逐个扩展多层图谱，而不影响当前课程级路径规划算法。
- 变更原因：
  当前课程级节点粒度过粗，难以支撑更细的知识点展示与后续分层扩展。多层下钻是必须能力，且复杂度可控，适合作为后续所有章节逐步细化的统一方案。
- 影响范围：
  `backend/services/KnowledgeGraphService.h`、`backend/services/KnowledgeGraphService.cc`、`backend/controllers/KnowledgeGraphController.cc`、`backend/config/config.json`、`backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`backend/tests/test_main.cc`、`frontend/src/api/knowledgeGraph.js`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/views/HomeView.vue`、`docs/architecture/ds-intro-detail-graph.md`、`README.md`
- 是否影响既有接口/数据库/算法：
  扩展了 `GET /api/knowledge/graph` 的查询参数与响应字段；不改变现有数据库结构和路径规划算法，只增强知识图谱展示层与图谱数据组织方式。
- 验证方式：
  执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认新增图谱 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“数据结构基本概念”节点，确认能进入细化图谱并通过面包屑返回上层。

### [2026-03-18] 知识图谱新增第二个细化节点“算法复杂度分析”

- 变更类型：架构改动
- 变更内容：
  在既有“课程主图谱 + 可选细化 scope 图谱”方案上，继续追加第二个已落地的细化节点 `algorithm-analysis / 算法复杂度分析`。
  当前该节点对应《数据结构（2025秋）》第一章 1.3 算法和算法分析，已整理出 10 个细化节点，覆盖：
  - 算法定义
  - 输入与输出
  - 有穷性 / 确定性 / 可行性
  - 评价标准
  - 语句频度
  - 时间复杂度 / 空间复杂度
  - 复杂度案例解析
  同时已补充对应测试与材料说明文档，确保后端接口、前端展示和答辩材料同步一致。
- 变更原因：
  如果只有第一个细化节点，容易让“多层图谱下钻”看起来像一次性的演示技巧。继续落第二个节点后，系统才真正具备“按课程单元逐个扩展”的可持续结构，也更利于答辩时说明该机制是通用能力而非单点样例。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/algorithm-analysis-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `algorithm-analysis` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“算法复杂度分析”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第三个细化节点“线性表”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第三个已落地的细化节点 `linear-list / 线性表`。
  当前该节点依据《数据结构（2025秋）》第二章相关目录，整理出 9 个细化节点，覆盖：
  - 线性表定义
  - 线性表特点
  - 基本操作
  - 抽象数据类型
  - 合并问题与合并算法设计
  - 顺序表示与链式表示
  - 应用实例
  该 scope 的设计重点是把“线性表”保持在概念与表示方向这一层，不直接吞并课程主图中“顺序表”“链表”两个独立节点的职责。
- 变更原因：
  仅有前两个细化节点还不足以证明系统可以沿整门课程主线持续扩展。补上“线性表”后，课程主图中已经有跨章节的三个细化样例，更能体现该下钻机制是通用的章节级能力，同时也方便后续顺着“线性表 -> 顺序表 / 链表”继续做更细层扩展。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/linear-list-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `linear-list` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“线性表”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第四个细化节点“顺序表”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第四个已落地的细化节点 `sequence-list / 顺序表`。
  当前该节点依据《数据结构（2025秋）》第二章 2.2 线性表的顺序表示和实现，整理出 9 个细化节点，覆盖：
  - 顺序存储表示
  - 存储结构定义
  - 随机访问特点
  - 容量约束
  - 初始化
  - 插入 / 删除 / 合并操作
  - 适用场景
  该 scope 的设计重点是把“顺序表”继续拆到接近程序实现的层级，但仍保持在“概念 + 典型操作 + 适用判断”的范围内，不直接扩展到更细的代码级函数实现。
- 变更原因：
  “线性表”节点更多承担抽象层作用，而“顺序表”是课程中非常适合展示实现细节的典型结构。补上这一层后，知识图谱已经能从“课程主线 -> 抽象结构 -> 具体实现”形成连续下钻链路，更利于答辩时展示系统的层级表达能力。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/sequence-list-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `sequence-list` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“顺序表”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第五个细化节点“链表”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第五个已落地的细化节点 `linked-list / 链表`。
  当前该节点依据《数据结构（2025秋）》第二章 2.3 线性表的链式表示和实现，整理出 10 个细化节点，覆盖：
  - 结点结构
  - 相关概念
  - 查找 / 插入 / 删除操作
  - 头插法 / 尾插法建表
  - 基于单链表的有序表合并
  - 与顺序表比较
  - 其他链表形式
  该 scope 的设计重点是把“链表”拆到接近实现逻辑但仍适合课堂讲解的层级，并形成与“顺序表”相对照的一条展示链路。
- 变更原因：
  当前主图中“顺序表”和“链表”是线性表部分最核心的一对实现分支。补上“链表”后，系统已经能够围绕“线性表 -> 顺序表 / 链表”形成完整的对照式下钻结构，这对答辩时解释不同存储实现的适用性和操作代价非常有帮助。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/linked-list-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `linked-list` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“链表”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第六个细化节点“栈”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第六个已落地的细化节点 `stack / 栈`。
  当前该节点依据《数据结构（2025秋）》第三章 3.1 栈、3.2 栈与递归、3.4 栈和队列的应用相关目录，整理出 9 个细化节点，覆盖：
  - 定义与特点
  - 抽象数据类型
  - 顺序栈表示与初始化
  - 入栈 / 出栈与取栈顶
  - 链栈
  - 递归工作栈
  - 典型应用
  该 scope 的设计重点是把“栈”从结构定义延伸到程序递归机制和典型应用场景，形成第三章的第一条稳定下钻链。
- 变更原因：
  目前图谱细化主要集中在线性结构部分。补上“栈”后，系统开始从第二章自然过渡到第三章，证明多层下钻能力不是局限在同一章节内部，而是可以沿整门课程主线继续扩展。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/stack-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `stack` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“栈”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第七个细化节点“队列”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第七个已落地的细化节点 `queue / 队列`。
  当前该节点依据《数据结构（2025秋）》第三章 3.3 队列、3.4 栈和队列的应用相关目录，整理出 9 个细化节点，覆盖：
  - 基本概念
  - 抽象数据类型
  - 顺序队列
  - 循环队列
  - 队列初始化
  - 入队 / 出队操作
  - 链队列
  - 典型应用
  该 scope 的设计重点是把“队列”与“栈”并列展开，同时突出顺序队列到循环队列的实现改进关系，以及队列在银行排队系统这类调度场景中的自然应用。
- 变更原因：
  补上“队列”后，第三章“栈和队列”形成了完整的双主线下钻结构，这有助于答辩时系统性展示不同受限线性结构的定义、实现和应用，而不是只展示其中一侧。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/queue-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `queue` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“队列”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第八个细化节点“串”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第八个已落地的细化节点 `string / 串`。
  当前该节点依据《数据结构（2025秋）》第四章 4.1 串的定义与操作、4.2 串的表示与实现、4.3 串的模式匹配相关目录，整理出 8 个细化节点，覆盖：
  - 定义与基本概念
  - 基本操作
  - 抽象数据类型
  - 定长顺序存储
  - 堆分配存储
  - 链式存储
  - 模式匹配问题
  - 简单模式匹配
  该 scope 的设计重点是把“串”从基础表示过渡到模式匹配问题，同时把 `kmp` 保留为后续单独继续下钻的算法节点。
- 变更原因：
  补上“串”后，知识图谱开始从第三章稳定过渡到第四章，且为后续 `kmp` 节点的更深入细化打下清晰前置基础。这比直接进入 KMP 更适合本科毕设的分层展示节奏。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/string-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `string` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“串”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第九个细化节点“KMP 模式匹配”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第九个已落地的细化节点 `kmp / KMP 模式匹配`。
  当前该节点依据《数据结构（2025秋）》第四章 4.3 串的模式匹配中的 KMP 算法，整理出 8 个细化节点，覆盖：
  - 模式匹配回顾
  - 朴素匹配局限
  - 部分匹配思想
  - next 数组含义
  - next 求法
  - 失配回退机制
  - KMP 主过程
  - 复杂度优势
  该 scope 的设计重点是把 KMP 拆到“思想 -> 辅助结构 -> 主过程 -> 优势”这一层，适合毕设答辩时做算法机制说明。
- 变更原因：
  “串”节点更多承担基础表示与问题引入作用，而 KMP 是第四章最值得单独展开的代表性算法。补上这一层后，第四章已经形成“串 -> 简单模式匹配 -> KMP”这条完整的算法展示链。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/kmp-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `kmp` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“KMP 模式匹配”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第十个细化节点“树与二叉树基础”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第十个已落地的细化节点 `tree-basic / 树与二叉树基础`。
  当前该节点依据《数据结构（2025秋）》第五章 5.1 树、5.2 二叉树相关目录，整理出 8 个细化节点，覆盖：
  - 树的定义与术语
  - 树的抽象数据类型
  - 二叉树定义与特点
  - 二叉树抽象数据类型
  - 二叉树性质
  - 二叉树存储结构
  - 树与二叉树关系
  - 应用入口
  该 scope 的设计重点是把第五章的结构基础层先铺稳，为后续“二叉树遍历”“哈夫曼树与编码”等节点继续下钻提供共同前置图谱。
- 变更原因：
  在第四章算法节点之后，课程主线需要回到第五章的结构基础层。补上“树与二叉树基础”后，系统能从线性结构、串匹配平滑过渡到层次结构，为后续树相关算法和编码内容建立统一前提。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/tree-basic-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `tree-basic` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“树与二叉树基础”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第十一个细化节点“二叉树遍历”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第十一个已落地的细化节点 `binary-tree-traversal / 二叉树遍历`。
  当前该节点依据《数据结构（2025秋）》第五章 5.3 遍历二叉树相关目录，整理出 8 个细化节点，覆盖：
  - 遍历定义与策略
  - 递归遍历思想
  - 先序遍历
  - 中序遍历
  - 后序遍历
  - 层序遍历
  - 非递归遍历思想
  - 应用入口
  该 scope 的设计重点是把“二叉树遍历”保持在“访问策略 + 典型遍历 + 实现思想”这一层，不直接下沉到具体代码模板，从而保持本科毕设可展示、可解释、可继续扩展的粒度。
- 变更原因：
  “树与二叉树基础”已经解决了结构层前置知识，但还缺少一个能代表第五章算法过程的展示节点。补上“二叉树遍历”后，系统能清晰呈现从结构概念到典型树算法的过渡，也为后续继续扩展“哈夫曼树与编码”等节点留出更稳定的章节节奏。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/binary-tree-traversal-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `binary-tree-traversal` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“二叉树遍历”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第十二个细化节点“哈夫曼树与编码”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第十二个已落地的细化节点 `huffman-tree / 哈夫曼树与编码`。
  当前该节点依据《数据结构（2025秋）》第五章 5.6 哈夫曼树与哈夫曼编码相关目录，整理出 8 个细化节点，覆盖：
  - 带权路径长度
  - 最优二叉树
  - 哈夫曼树定义
  - 构造思想
  - 构造过程
  - 前缀编码约束
  - 哈夫曼编码与译码
  - 应用入口
  该 scope 的设计重点是把“哈夫曼树与编码”保持在“评价指标 + 最优结构 + 构造 + 编码应用”这一层，不直接下沉到复杂实现细节，从而保持本科毕设答辩时可解释、可演示的层次。
- 变更原因：
  “二叉树遍历”已经补齐了第五章中的代表性遍历算法，但还缺少一个能体现“树结构走向实际应用”的典型节点。补上“哈夫曼树与编码”后，第五章就能同时覆盖结构基础、遍历算法和编码应用三类展示重点。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/huffman-tree-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `huffman-tree` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“哈夫曼树与编码”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第十三个细化节点“图的存储与遍历”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第十三个已落地的细化节点 `graph-basic / 图的存储与遍历`。
  当前该节点依据《数据结构（2025秋）》第六章 6.1 图的概述、6.2 图的存储结构、6.3 图的遍历相关目录，整理出 8 个细化节点，覆盖：
  - 图定义与基本术语
  - 图的分类与连通性
  - 图的抽象数据类型
  - 邻接矩阵存储
  - 邻接表存储
  - 深度优先遍历
  - 广度优先遍历
  - 应用入口
  该 scope 的设计重点是把“图的存储与遍历”保持在“概念 + 表示 + 两类基础遍历”这一层，不直接扩展到更复杂的图算法细节，从而为后续 `topological-sort` 留出更清晰的下一层展开空间。
- 变更原因：
  完成第五章后，课程主线需要自然进入第六章图结构部分。补上“图的存储与遍历”后，系统可以展示从树结构到图结构的过渡，并为“拓扑排序与关键路径”这类更具体的图算法建立稳定前置基础。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/graph-basic-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `graph-basic` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“图的存储与遍历”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 知识图谱新增第十四个细化节点“拓扑排序与关键路径”

- 变更类型：架构改动
- 变更内容：
  在既有多层图谱下钻结构上，继续追加第十四个已落地的细化节点 `topological-sort / 拓扑排序与关键路径`。
  当前该节点依据《数据结构（2025秋）》第六章 6.6 拓扑排序与关键路径相关目录，整理出 8 个细化节点，覆盖：
  - 有向无环图
  - AOV 网
  - 拓扑排序定义
  - 拓扑排序算法
  - AOE 网
  - 关键路径概念
  - 关键路径求解
  - 应用入口
  该 scope 的设计重点是把“拓扑排序与关键路径”保持在“约束结构 + 两类建模 + 排序与工期分析”这一层，不与 `graph-basic` 的表示和遍历内容重复，从而让第六章形成清晰的前后分层。
- 变更原因：
  “图的存储与遍历”已经补齐了图结构的基础表示与搜索能力，但用户当前路径规划与演示目标长期围绕“拓扑排序与关键路径”展开。补上这一层后，第六章的展示链就完整闭合，也更贴合当前系统默认演示场景。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/topological-sort-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与验证增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认 `topological-sort` 新 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中双击“拓扑排序与关键路径”节点，确认能进入对应细化图谱并通过面包屑返回上层。

### [2026-03-19] 首批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在既有“课程主图 -> 二级细化 scope”结构上，首次正式落地第三层图谱。当前优先选择了 4 个最值得继续下钻的子节点：
  - `stack-recursion / 递归工作栈`
  - `kmp-next-definition / next 数组含义`
  - `graph-basic-dfs / 深度优先遍历`
  - `topological-sort-critical-path-solve / 关键路径求解`
  这些节点分别代表：
  - 跨章节机制桥梁
  - 经典算法核心抽象
  - 图搜索过程
  - 工程调度分析过程
  当前第三层图谱通过在二级 scope 节点上显式写入 `detailScopeCode` 来标记继续下钻入口，并使用 `parentScopeCode` 把第三层挂接到对应二级 scope 下。
- 变更原因：
  根节点 14 个主知识点的二级图谱已经全部补齐，继续新增 root 级节点的收益已经不高。下一阶段更适合围绕“最难讲清、最值得答辩演示、最能体现系统分层表达能力”的高价值子节点继续下钻。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/stack-recursion-detail-graph.md`、`docs/architecture/kmp-next-definition-detail-graph.md`、`docs/architecture/graph-basic-dfs-detail-graph.md`、`docs/architecture/topological-sort-critical-path-solve-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次属于知识图谱展示层从“二级下钻”扩展到“首批三级下钻”的数据与交互增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入二级图谱后双击“递归工作栈”“next 数组含义”“深度优先遍历”“关键路径求解”节点，确认能继续进入第三层，并通过面包屑逐级返回上层。

### [2026-03-19] 第二批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在首批第三层图谱已经稳定运行的基础上，继续补齐第二批 4 个高价值下钻节点：
  - `binary-tree-traversal-recursive / 递归遍历思想`
  - `huffman-tree-construction-process / 构造过程`
  - `graph-basic-bfs / 广度优先遍历`
  - `topological-sort-algorithm / 拓扑排序算法`
  本批次完成后，系统当前合计已支持 8 个第三层 scope。实现方式继续沿用既有规则：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端面包屑与双击交互复用既有多层导航能力
  这次新增节点分别覆盖：
  - 树遍历中的递归机制
  - 哈夫曼树的构造步骤
  - BFS 的层次搜索过程
  - 拓扑排序的入度消解过程
- 变更原因：
  首批第三层图谱已经证明“二级 scope 继续下钻”这条路线可行，下一步更适合把第五章和第六章里最值得答辩展示的过程性节点补齐。这样既能增强图谱层次感，也能让系统更贴近“依赖约束 -> 排序 -> 导航”这一毕业设计主线。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/binary-tree-traversal-recursive-detail-graph.md`、`docs/architecture/huffman-tree-construction-process-detail-graph.md`、`docs/architecture/graph-basic-bfs-detail-graph.md`、`docs/architecture/topological-sort-algorithm-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第二批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”“拓扑排序与关键路径”二级图谱后，继续双击“递归遍历思想”“构造过程”“广度优先遍历”“拓扑排序算法”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第三批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前两批第三层图谱稳定运行的基础上，继续补齐第三批 4 个高价值下钻节点：
  - `binary-tree-traversal-iterative / 非递归遍历思想`
  - `huffman-tree-code-decode / 哈夫曼编码与译码`
  - `graph-basic-adjacency-list / 邻接表存储`
  - `topological-sort-aoe / AOE 网`
  本批次完成后，系统当前合计已支持 12 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 二叉树遍历从递归走向非递归实现
  - 哈夫曼树从结构构造走向编码与译码应用
  - 图遍历背后的邻接表表示基础
  - 关键路径求解前的 AOE 网建模入口
- 变更原因：
  第二批第三层图谱已经把第五章和第六章中的部分过程性节点展开出来，但目前仍偏重“算法过程”，对“表示层”和“建模层”的展示还不够完整。补上这一批后，系统在答辩中就能更完整地讲清“结构表示 -> 算法过程 -> 应用建模”的知识链。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/binary-tree-traversal-iterative-detail-graph.md`、`docs/architecture/huffman-tree-code-decode-detail-graph.md`、`docs/architecture/graph-basic-adjacency-list-detail-graph.md`、`docs/architecture/topological-sort-aoe-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第三批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”“拓扑排序与关键路径”二级图谱后，继续双击“非递归遍历思想”“哈夫曼编码与译码”“邻接表存储”“AOE 网”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第四批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前三批第三层图谱已经稳定运行的基础上，继续补齐第四批 4 个高价值下钻节点：
  - `binary-tree-traversal-level-order / 层序遍历`
  - `huffman-tree-prefix-code / 前缀编码约束`
  - `graph-basic-adjacency-matrix / 邻接矩阵存储`
  - `topological-sort-critical-path-concept / 关键路径概念`
  本批次完成后，系统当前合计已支持 16 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 二叉树遍历中的按层访问过程
  - 哈夫曼编码正确性的前缀约束来源
  - 图表示层中的邻接矩阵方案
  - 关键路径方法中的概念层基础
- 变更原因：
  第三批第三层图谱已经把“表示层”和“建模层”补进来一部分，但目前仍缺少几个最典型、最适合答辩单独解释的基础节点。补上这一批后，树、哈夫曼、图、关键路径这四条主线都拥有更完整的“概念 -> 结构/表示 -> 算法/过程 -> 应用”展示链。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/binary-tree-traversal-level-order-detail-graph.md`、`docs/architecture/huffman-tree-prefix-code-detail-graph.md`、`docs/architecture/graph-basic-adjacency-matrix-detail-graph.md`、`docs/architecture/topological-sort-critical-path-concept-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第四批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”“拓扑排序与关键路径”二级图谱后，继续双击“层序遍历”“前缀编码约束”“邻接矩阵存储”“关键路径概念”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第五批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前四批第三层图谱已经稳定运行的基础上，继续补齐第五批 4 个高价值下钻节点：
  - `queue-circular / 循环队列`
  - `string-naive-match / 简单模式匹配`
  - `kmp-main-process / KMP 主过程`
  - `tree-basic-binary-storage / 二叉树存储结构`
  本批次完成后，系统当前合计已支持 20 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 队列实现中的循环复用机制
  - 串匹配中的朴素基线过程
  - KMP 算法从 next 数组走向主过程运行逻辑
  - 树基础章节中的二叉树具体存储方式
- 变更原因：
  第四批第三层图谱已经把树、哈夫曼、图、关键路径四条主线继续补齐，但课程前半部分的“队列 / 串 / KMP / 树基础”仍然缺少同等层级的展开。补上这一批后，整门课的第三层图谱分布会更均衡，也更适合答辩时展示“课程整体覆盖，而不是只深挖后半章”。 
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/queue-circular-detail-graph.md`、`docs/architecture/string-naive-match-detail-graph.md`、`docs/architecture/kmp-main-process-detail-graph.md`、`docs/architecture/tree-basic-binary-storage-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 重新配置测试注册，再执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第五批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“队列”“串”“KMP 模式匹配”“树与二叉树基础”二级图谱后，继续双击“循环队列”“简单模式匹配”“KMP 主过程”“二叉树存储结构”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第六批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前五批第三层图谱已经稳定运行的基础上，继续补齐第六批 4 个高价值下钻节点：
  - `algorithm-analysis-time-complexity / 时间复杂度`
  - `sequence-list-insert / 插入操作`
  - `linked-list-head-insert / 头插法建表`
  - `stack-applications / 典型应用`
  本批次完成后，系统当前合计已支持 24 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 复杂度分析中的大 O 记号与增长比较
  - 顺序表插入中的后移与代价理解
  - 链表头插法中的指针重连过程
  - 栈在典型应用场景中的建模入口
- 变更原因：
  第五批第三层图谱已经让课程后半段和部分中段内容具备了多层展示能力，但课程前半部分仍然缺少“理论基础 -> 具体操作 -> 典型应用”的均衡展开。补上这一批后，第一章到第三章的关键主线也拥有可下钻、可答辩、可演示的第三层结构。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/algorithm-analysis-time-complexity-detail-graph.md`、`docs/architecture/sequence-list-insert-detail-graph.md`、`docs/architecture/linked-list-head-insert-detail-graph.md`、`docs/architecture/stack-applications-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强。
- 验证方式：
  执行 `python -m json.tool backend/config/knowledge_graph_details.json` 校验配置格式，再执行 `cmake -S backend -B backend/build`、`cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第六批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“算法复杂度分析”“顺序表”“链表”“栈”二级图谱后，继续双击“时间复杂度”“插入操作”“头插法建表”“典型应用”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第七批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前六批第三层图谱已经稳定运行的基础上，继续补齐第七批 4 个高价值下钻节点：
  - `algorithm-analysis-case-study / 复杂度案例解析`
  - `sequence-list-delete / 删除操作`
  - `linked-list-tail-insert / 尾插法建表`
  - `queue-applications / 队列典型应用`
  本批次完成后，系统当前合计已支持 28 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 复杂度分析中的案例推导过程
  - 顺序表删除中的前移与代价理解
  - 链表尾插法中的尾指针维护
  - 队列在层序处理、调度与缓冲场景中的应用入口
- 变更原因：
  第六批第三层图谱已经把第一章到第三章中的部分理论基础、实现操作和栈应用补齐，但“案例分析 / 删除过程 / 尾插建表 / 队列应用”这几条同样重要的教学链路仍未展开。补上这一批后，课程前半部分的图谱层次会更完整，也更适合在答辩时展示“同一章节内多个关键过程都可以逐层下钻”。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/algorithm-analysis-case-study-detail-graph.md`、`docs/architecture/sequence-list-delete-detail-graph.md`、`docs/architecture/linked-list-tail-insert-detail-graph.md`、`docs/architecture/queue-applications-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强。
- 验证方式：
  执行 `python -m json.tool backend/config/knowledge_graph_details.json` 校验配置格式，再执行 `cmake -S backend -B backend/build`、`cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第七批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“算法复杂度分析”“顺序表”“链表”“队列”二级图谱后，继续双击“复杂度案例解析”“删除操作”“尾插法建表”“典型应用”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第八批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前七批第三层图谱已经稳定运行的基础上，继续补齐第八批 4 个高价值下钻节点：
  - `ds-intro-logical-structure / 逻辑结构`
  - `ds-intro-storage-structure / 存储结构`
  - `linear-list-sequential-representation / 顺序表示（线性表）`
  - `linear-list-linked-representation / 链式表示（线性表）`
  本批次完成后，系统当前合计已支持 32 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 第一章中的逻辑结构分类
  - 第一章中的存储结构类型
  - 线性表顺序表示的优势与代价
  - 线性表链式表示的结构与访问方式
- 变更原因：
  前七批第三层图谱已经让课程中后段和前半部分的若干操作性节点具备了多层展示能力，但“数据结构基本概念”和“线性表”这两个较早进入课程主图的二级节点仍然没有第三层入口。补上这一批后，系统在答辩中就能更完整地展示“从基础概念层，到实现方向层，再到具体结构层”的连续导航链。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/ds-intro-detail-graph.md`、`docs/architecture/linear-list-detail-graph.md`、`docs/architecture/ds-intro-logical-structure-detail-graph.md`、`docs/architecture/ds-intro-storage-structure-detail-graph.md`、`docs/architecture/linear-list-sequential-representation-detail-graph.md`、`docs/architecture/linear-list-linked-representation-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强，并继续保持最大深度为三层。
- 验证方式：
  执行 `python -m json.tool backend/config/knowledge_graph_details.json` 校验配置格式，再执行 `cmake -S backend -B backend/build`、`cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第八批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“数据结构基本概念”“线性表”二级图谱后，继续双击“逻辑结构”“存储结构”“顺序表示”“链式表示”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 第九批第三层图谱落地

- 变更类型：架构改动
- 变更内容：
  在前八批第三层图谱已经稳定运行的基础上，继续补齐第九批 4 个高价值下钻节点：
  - `sequence-list-merge / 合并操作（顺序表）`
  - `linked-list-merge / 有序表合并（链表）`
  - `queue-enqueue / 入队操作（队列）`
  - `string-matching-problem / 模式匹配问题`
  本批次完成后，系统当前合计已支持 36 个第三层 scope。实现方式继续保持不变：
  - 在二级 scope 节点上显式写入 `detailScopeCode`
  - 通过 `parentScopeCode` 将第三层 scope 挂接到对应二级 scope 下
  - 由前端既有面包屑和双击交互直接复用多层导航能力
  这次新增节点分别覆盖：
  - 顺序表中的有序合并过程
  - 链表中的有序表合并与指针重连优势
  - 队列基本操作中的入队过程
  - 串算法中的模式匹配公共问题语义
- 变更原因：
  第八批第三层图谱已经把“基础概念层”和“线性表实现方向层”补齐，但顺序表、链表、队列和串这四个重要章节仍然缺少各自的一块关键收尾内容。补上这一批后，系统在三层深度约束内，已经能较完整地展示“概念 -> 实现方向 -> 具体操作 / 问题入口”的课程导航链。
- 影响范围：
  `backend/config/knowledge_graph_details.json`、`backend/tests/KnowledgeGraphServiceTest.cc`、`docs/architecture/sequence-list-merge-detail-graph.md`、`docs/architecture/linked-list-merge-detail-graph.md`、`docs/architecture/queue-enqueue-detail-graph.md`、`docs/architecture/string-matching-problem-detail-graph.md`、`frontend/src/views/HomeView.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变课程级路径规划算法；本次仍属于知识图谱展示层的数据扩展与测试增强，并继续保持最大深度为三层。
- 验证方式：
  执行 `python -m json.tool backend/config/knowledge_graph_details.json` 校验配置格式，再执行 `cmake -S backend -B backend/build`、`cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure`，确认第九批第三层 scope 测试通过；执行 `npm run build` 通过前端构建；在页面中进入“顺序表”“链表”“队列”“串”二级图谱后，继续双击“合并操作”“有序表合并”“入队操作”“模式匹配问题”节点，确认能进入第三层并通过面包屑逐级返回。

### [2026-03-19] 学习导航模块接入第一版资源建议

- 变更类型：架构改动
- 变更内容：
  在既有学习导航模块中新增“资源建议”能力，资源不作为规划算法输入，而是作为路径结果的补充信息随接口一并返回。第一版实现采用静态资源目录驱动：
  - 新增 `backend/config/learning_resources.json`，按知识点 `code` 维护视频教程、文本教程等资源
  - 新增 `LearningResourceService`，统一负责资源目录读取、按知识点匹配和按当前路径聚合推荐
  - `POST /api/path/generate` 返回的每个 `path` 节点新增 `learningResources`
  - 路径结果顶层新增 `resourceRecommendations`，按“本轮推荐学习 / 下一轮建议”聚合当前导航结果中的资源
  - 前端 `PathPlannerPanel` 新增“当前学习导航配套资源”展示区，并在节点详情展开区显示该节点的配套资源
  当前首批资源优先覆盖答辩主链路中的 `queue`、`graph-basic`、`topological-sort`，便于把“学习路径推荐”和“资源支持建议”串成同一条可演示流程。
- 变更原因：
  之前系统只能告诉学习者“学什么、为什么学、学完后如何调整”，但还不能继续回答“学这个节点时具体看什么资源”。把资源建议并入学习导航结果后，系统就能在不引入复杂推荐算法的前提下，形成“路径 + 理由 + 资源”的更完整闭环，更贴近本科毕设答辩中的可落地展示。
- 影响范围：
  `backend/config/learning_resources.json`、`backend/config/config.json`、`backend/services/LearningResourceService.h`、`backend/services/LearningResourceService.cc`、`backend/services/PathPlanningService.cc`、`backend/tests/test_main.cc`、`backend/tests/PathPlanningServiceTest.cc`、`backend/tests/CMakeLists.txt`、`frontend/src/components/PathPlannerPanel.vue`、`README.md`
- 是否影响既有接口/数据库/算法：
  不修改数据库结构，不改变 `LearningPathPlanner` 与 `LearningPathAdjuster` 的决策规则；扩展了路径规划接口响应结构，新增 `path[].learningResources` 与顶层 `resourceRecommendations` 字段，属于兼容式接口增强。
- 验证方式：
  执行 `python -m json.tool backend/config/learning_resources.json` 校验资源配置格式；执行 `cmake -S backend -B backend/build`、`cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure` 验证后端资源服务、路径响应与既有测试通过；执行 `npm run build` 通过前端构建；在页面生成学习路径后，确认“当前学习导航配套资源”卡片出现，且展开 `queue`、`graph-basic`、`topological-sort` 节点时能看到对应资源链接与使用建议。

### [2026-03-19] 学习导航资源目录扩展到课程主图全覆盖

- 变更类型：架构改动
- 变更内容：
  在第一版资源建议能力已经稳定接入的基础上，继续扩展 `backend/config/learning_resources.json`，把资源覆盖范围从答辩主链路的 3 个节点，扩展到课程主图全部 14 个顶层知识点：
  - `ds-intro`
  - `algorithm-analysis`
  - `linear-list`
  - `sequence-list`
  - `linked-list`
  - `stack`
  - `queue`
  - `string`
  - `kmp`
  - `tree-basic`
  - `binary-tree-traversal`
  - `huffman-tree`
  - `graph-basic`
  - `topological-sort`
  每个顶层节点当前至少提供 1 条视频资源和 1 条文本资源，并继续保持静态目录驱动方式，不引入新的接口结构和推荐规则。
- 变更原因：
  第一版资源建议已经能展示“路径 + 理由 + 资源”的闭环，但如果只覆盖 3 个节点，用户切换到其他目标时会出现资源建议稀疏或空白的情况。把资源目录扩展到主图全覆盖后，系统在首页主线展示中就能更稳定地支持不同学习目标下的资源引导，答辩时也更完整。
- 影响范围：
  `backend/config/learning_resources.json`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不修改接口结构，不修改数据库，不改变路径规划、动态调整和推荐解释规则；本次属于静态资源目录扩充与说明更新。
- 验证方式：
  执行 `python -m json.tool backend/config/learning_resources.json` 校验资源配置格式；执行 `ctest --test-dir backend/build/tests --output-on-failure` 确认既有资源服务与路径响应测试继续通过；在页面切换不同顶层目标后，确认“当前学习导航配套资源”卡片能跟随路径返回对应顶层节点资源。

### [2026-03-19] 学习导航资源建议补充阶段标签与第三层定向资源

- 变更类型：架构改动
- 变更内容：
  在课程主图顶层资源已经全覆盖的基础上，继续扩展学习资源的数据结构和展示粒度：
  - `learning_resources.json` 中的资源条目新增 `recommendedPhase`
  - 资源条目新增 `focusTags`
  - 资源条目新增可选 `focusNodeCode / focusNodeLabel`
  - 前端资源卡片新增“适合阶段”“聚焦标签”“对应细化节点”展示
  - 当前已为 3 个高频第三层节点补充定向资源绑定：
    - `queue-circular / 循环队列`
    - `graph-basic-bfs / 广度优先遍历`
    - `topological-sort-algorithm / 拓扑排序算法`
  这样资源建议不再只是“给一个链接”，而是能够说明这份资源更适合预习、主学、巩固还是难点突破，并明确它更贴近哪个细化知识点。
- 变更原因：
  之前的资源建议已经能覆盖顶层节点，但信息粒度仍偏粗，难以直接回答“这份资源该什么时候看、重点看什么、它更贴近哪个子节点”。补上阶段标签和第三层定向资源后，学习导航模块中的资源建议就能和推荐理由解释形成更紧密的配合，也更符合答辩展示时“路径 -> 理由 -> 资源 -> 子知识点”的讲解节奏。
- 影响范围：
  `backend/config/learning_resources.json`、`backend/services/LearningResourceService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不修改数据库，不改变规划与调整算法；扩展了 `learningResources` 与 `resourceRecommendations` 中的资源条目字段，属于兼容式响应增强。
- 验证方式：
  执行 `python -m json.tool backend/config/learning_resources.json` 校验资源目录格式；执行 `cmake --build backend/build -j$(nproc)` 与 `ctest --test-dir backend/build/tests --output-on-failure` 验证资源服务与路径响应测试通过；执行 `npm run build` 通过前端构建；在页面生成学习路径后，确认资源卡片会显示阶段标签、聚焦标签，并在相关资源上显示“对应细化节点”提示。

### [2026-03-19] 学习导航资源建议绑定规划证据并补充新一批第三层定向资源

- 变更类型：架构改动
- 变更内容：
  在既有资源建议能力已经稳定运行的基础上，把资源推荐从“按知识点静态挂载”升级为“按路径节点动态生成”，核心调整如下：
  - `LearningResourceService` 新增 `buildResourcesForLearningPathItem`，按 `LearningPathItem` 生成资源列表
  - 资源条目新增 `selectionReason` 字段
  - `selectionReason` 直接消费 `LearningPathItem` 中的节点角色、状态、掌握度、相关度、时间占比和 `PathReasonTrace.triggerReasons`
  - `PathPlanningService` 在构造 `path[].learningResources` 时改为注入节点感知资源，而不是仅按知识点 code 静态查询
  - 顶层 `resourceRecommendations` 也统一复用节点感知资源生成逻辑，确保资源卡片和路径节点展开区的说明一致
  - 前端 `PathPlannerPanel` 在路径节点资源区和汇总资源区新增“选择原因”展示
  同时继续扩充第三层定向资源映射，本批次新增绑定：
  - `kmp-next-definition / next 数组含义`
  - `binary-tree-traversal-level-order / 层序遍历`
  - `topological-sort-critical-path-solve / 关键路径求解`
  本次实现继续保持“资源建议不参与 planner 决策，只作为路径结果增强信息”的原则，不新增数据库表，也不引入新的推荐算法。
- 变更原因：
  之前的资源建议已经能回答“当前节点看什么资源”，但还不能明确回答“为什么此时推荐这份资源”。把资源选择理由绑定到 planner 已有证据后，系统就能形成“路径 -> 推荐理由 -> 资源理由 -> 细化子节点”的完整说明链，避免解释模块脱离规划依据单独发挥，也更适合答辩演示。
- 影响范围：
  `backend/services/LearningResourceService.h`、`backend/services/LearningResourceService.cc`、`backend/services/PathPlanningService.cc`、`backend/config/learning_resources.json`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不修改数据库结构，不改变 `LearningPathPlanner` 和 `LearningPathAdjuster` 的决策规则；扩展了 `learningResources` 与 `resourceRecommendations` 中资源条目的响应字段，新增 `selectionReason`，属于兼容式接口增强。
- 验证方式：
  执行 `python -m json.tool backend/config/learning_resources.json` 校验资源目录格式；执行 `cmake --build backend/build -j$(nproc)` 完成后端构建；执行 `ctest --test-dir backend/build/tests --output-on-failure` 确认 59 项测试全部通过；执行 `npm run build` 通过前端构建；页面生成路径后，确认资源卡片会显示“选择原因”，并在 `kmp`、`binary-tree-traversal`、`topological-sort` 等节点上看到新的第三层定向提示。

### [2026-03-19] 学习反馈链路新增按批次逐步回退能力

- 变更类型：架构改动
- 变更内容：
  为避免学习者在提交反馈时出现误点或误判后无法恢复，当前系统在既有“反馈保存 -> 画像更新 -> 路径重算”链路上新增了“按反馈批次逐步回退”能力，核心调整如下：
  - `learning_feedback_records` 新增 `feedback_batch_id` 字段，用于把一次提交中的多条反馈记录归入同一批次
  - `FeedbackService::submitFeedback` 在写入反馈记录时自动生成批次 ID，并随响应返回
  - 新增 `POST /api/feedback/rollback`，用于撤销最近一次反馈批次
  - 回退时按该批次记录中的 `previous_mastery` 恢复 `learner_mastery`
  - 回退成功后删除该批次反馈记录，使系统可以继续向前逐步撤销
  - `FeedbackRepository` 增加最新批次查询、批次明细读取和批次回退能力
  - 仓库层对旧数据库做轻量兼容：若历史库缺少 `feedback_batch_id`，会自动补列并为旧记录回填 legacy 批次编号
  - 前端 `PathPlannerPanel` 新增“撤销最近一次反馈”按钮，并在回退后展示“回退前后路径对比”和回退说明
  本次实现继续保持“回退只作用于反馈持久化与画像恢复，不改 planner / adjuster 决策规则”的原则。
- 变更原因：
  之前系统只能不断追加反馈记录和更新掌握度，一旦学习者误选完成状态或高估自评掌握度，就只能手动重新覆盖，演示和答辩时容错性不足。引入按批次逐步回退后，系统就能自然展示“提交反馈 -> 发现误判 -> 撤销上一轮 -> 重新规划”的闭环，明显提升演示稳定性与可解释性。
- 影响范围：
  `database/init/001_schema.sql`、`backend/repositories/FeedbackRepository.h`、`backend/repositories/FeedbackRepository.cc`、`backend/services/FeedbackService.h`、`backend/services/FeedbackService.cc`、`backend/controllers/FeedbackController.h`、`backend/controllers/FeedbackController.cc`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、`frontend/src/api/feedback.js`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  扩展了数据库反馈记录结构并新增 `POST /api/feedback/rollback` 接口；不改变知识图谱、路径规划算法、动态调整规则和资源推荐规则。前端属于兼容式增强，新增回退入口与回退结果展示。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 刷新测试发现列表；执行 `cmake --build backend/build -j$(nproc)` 完成后端构建；执行 `ctest --test-dir backend/build/tests --output-on-failure` 确认 60 项测试全部通过，其中包含“连续两次按批次回退”的新用例；执行 `npm run build` 通过前端构建；页面提交反馈后点击“撤销最近一次反馈”，确认掌握度恢复、画像刷新，且路径规划面板出现“回退前本轮安排 / 回退后本轮安排”的对比结果。

### [2026-03-19] 学习者画像拆分为独立页面并接入顶部导航

- 变更类型：架构改动
- 变更内容：
  为了让首页展示重点更集中，并让“学习者画像建模”更适合单独答辩讲解，当前前端展示结构做了如下调整：
  - 新增独立页面 `frontend/src/views/LearnerProfileView.vue`
  - Vue Router 新增 `/learner-profile` 路由
  - 共享布局 `PageLayout` 增加顶部导航按钮，统一提供“首页 / 学习者画像”切换入口
  - 首页 `HomeView` 不再直接渲染 `LearnerProfileCard`，首页聚焦健康检查、知识图谱与路径规划
  - 学习者画像相关展示集中到独立页面，用于展示掌握度统计、待补强知识点和最近学习记录
  - 首页仍保留 learner profile 数据加载逻辑，用于给路径规划模块提供默认掌握度与反馈记录数量
  本次调整属于前端页面职责重分配，不引入新的后端接口和状态管理抽象。
- 变更原因：
  之前首页同时承载健康状态、学习者画像、知识图谱和路径规划，内容较多，不利于答辩时分别讲清“学习者建模”和“学习导航生成”两条主线。拆分后，首页更聚焦系统主流程，学习者画像页则更适合单独演示画像建模、学习记录和回退能力。
- 影响范围：
  `frontend/src/components/PageLayout.vue`、`frontend/src/router/index.js`、`frontend/src/views/HomeView.vue`、`frontend/src/views/LearnerProfileView.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增后端接口，不修改数据库结构，不改变路径规划、反馈调整和资源推荐算法；属于前端页面结构和导航方式调整。
- 验证方式：
  执行 `npm run build` 通过前端构建；在浏览器中打开首页后，确认顶部可在“首页 / 学习者画像”之间切换；确认首页仍可基于画像默认值生成路径，且进入“学习者画像”页面后可正常查看掌握度摘要、待补强知识点与最近学习记录。

### [2026-03-19] 首页路径规划区调整为“核心区 + 扩展区”分层布局

- 变更类型：架构改动
- 变更内容：
  为了解决首页右侧路径规划模块在资源建议、学习反馈和路径对比内容变长后，显著高于左侧知识图谱预览、从而造成页面下半部分大面积空白的问题，当前前端对路径规划模块的参与布局方式做了调整：
  - `PathPlannerPanel` 改为通过 `display: contents` 直接参与首页父级 grid
  - 路径规划核心卡片保留在首页首屏右侧，仅展示目标设置、掌握度输入、路径摘要、本轮推荐学习和下一轮建议
  - 资源建议卡片改为整宽扩展区，下沉到图谱与核心路径规划下方
  - 学习反馈、逐步回退、路径前后对比也统一下沉到整宽扩展区
  - 首页保留现有两列主网格结构，但通过“核心区在右上、扩展区整宽铺开”的方式，减少左右高度失衡造成的割裂感
  本次调整不改变路径规划状态管理和交互逻辑，只调整模块在首页中的空间层级与视觉重心。
- 变更原因：
  随着资源建议、反馈回退和路径对比能力逐步加入，原先把所有内容都堆叠在右侧单张长卡中的方式会在首屏造成严重的高度失衡。左侧知识图谱预览较短，右侧模块过长，导致图谱下方出现大块空白，不利于演示与浏览。把长内容下沉为整宽扩展区后，首屏会更聚焦“图谱 + 路径”，页面节奏也更自然。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、反馈调整、资源推荐和推荐解释算法；属于前端布局结构调整。
- 验证方式：
  执行 `npm run build` 通过前端构建；在浏览器中打开首页后，确认首屏右侧只保留核心路径规划卡片，资源建议与学习反馈下沉到图谱下方的整宽区域；确认左侧知识图谱下方不再因右侧长卡而出现明显大面积空白。
