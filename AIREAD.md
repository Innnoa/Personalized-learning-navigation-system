# AIREAD

最后更新：2026-03-26

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

### [2026-03-20] 首页路径规划扩展区进一步拆分为分段卡片布局

- 变更类型：架构改动
- 变更内容：
  在“核心区 + 扩展区”分层布局已经落地的基础上，为了进一步减少扩展区像单条长带一样连续堆叠带来的割裂感，当前前端继续对首页路径规划扩展区做了分段化整理，核心调整如下：
  - `PathPlannerPanel` 的扩展区新增独立引导卡片，明确说明该区域承载“资源建议 / 学习反馈 / 操作摘要 / 路径变化”
  - 扩展区内部改为显式分段卡片结构，不再只依赖一串连续内容堆叠
  - 资源建议保持整宽展示，优先利用横向空间承载较长的资源列表
  - 学习反馈与“最近一次反馈/回退摘要”在桌面端采用并列栅格，若当前没有摘要数据，学习反馈卡片自动恢复整宽，避免出现空列
  - 路径变化区保持整宽展示，便于对比“调整前后 / 回退前后”的完整学习安排
  - 样式层新增扩展区引导标签、分段标题和响应式布局规则，使扩展区在桌面端和窄屏端都能保持明确的信息分区
  本次调整仍然只涉及前端页面结构与样式组织，不改动路径规划、反馈回退、资源推荐和推荐解释的业务逻辑。
- 变更原因：
  虽然前一版已经把长内容从首屏右侧抽离到下方整宽区域，但扩展区内部仍可能表现为一条连续的长内容带。对于演示和答辩来说，这会削弱“资源支持”“学习反馈”“路径变化”三类能力的边界感。把扩展区拆成更明确的分段卡片后，用户可以更自然地理解每一块内容的职责，页面视觉节奏也更稳定。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、反馈调整、资源推荐和推荐解释算法；属于首页前端布局与信息分段方式优化。
- 验证方式：
  执行 `npm run build` 通过前端构建；在浏览器中打开首页后，确认扩展区顶部出现引导卡片，资源建议、学习反馈、操作摘要和路径变化以独立卡片分段呈现；确认在没有摘要数据时学习反馈卡片会自动占满整宽，在有摘要数据时与摘要卡片并列展示。

### [2026-03-20] 首页路径规划扩展区调整为“左主内容 + 右辅助摘要”双列结构

- 变更类型：架构改动
- 变更内容：
  为了让扩展区在桌面端进一步形成稳定的信息重心，本次继续对 `PathPlannerPanel` 的扩展区内部布局做了主副列拆分：
  - 扩展区引导卡片下方新增 `planner-extension-columns`，桌面端采用“左侧主内容列 + 右侧辅助摘要列”结构
  - 左侧主内容列集中承载资源建议和学习反馈，保留足够宽度承接较长的资源列表与反馈表单
  - 右侧辅助摘要列新增“当前导航速览”卡片，汇总资源分组数、本轮节点数、反馈记录数和路径变化项数
  - 辅助摘要列会基于当前状态动态给出一句操作提示，并在已有路径变化时展示前 3 条关键变化标签，减少用户在长内容里来回查找
  - 最近一次反馈/回退的“操作摘要”也移动到右侧辅助列，形成“左侧处理内容、右侧看状态”的稳定桌面布局
  - 详细的“路径变化”仍保留为下方整宽卡片，避免前后路径对比区域因列宽过窄而影响阅读
  - 响应式下恢复为单列结构；右侧速览卡片的 `sticky` 行为仅在桌面端启用
  本次改动依然只涉及前端模板与样式组织，不修改任何路径规划、反馈回退、资源推荐与解释生成逻辑。
- 变更原因：
  上一版已经把扩展区拆成独立卡片，但在桌面端仍然更像“多个块顺次往下排”。当资源区较长时，用户需要在长内容中反复滚动查看当前状态与最近操作，不够利于答辩演示。通过引入右侧辅助摘要列，可以把高频查看的信息固定在一侧，让扩展区形成更清楚的“操作区 / 观察区”分工。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、反馈调整、资源推荐和推荐解释算法；属于首页扩展区的前端空间组织优化。
- 验证方式：
  执行 `npm run build` 通过前端构建；在浏览器中打开首页后，确认扩展区在桌面端呈现“左侧资源与反馈、右侧辅助摘要”的双列结构；确认右侧会显示“当前导航速览”和最近一次操作摘要，且下方“路径变化”仍保持整宽展示；在窄屏下确认该布局自动退化为单列。

### [2026-03-20] 细化图谱新增 scope 内局部学习路径规划能力

- 变更类型：架构改动
- 变更内容：
  为了让多层细化图谱不仅能“看结构”，还能真正回答“这个主节点内部先学什么”，当前系统新增了 detailScope 范围内的局部学习路径规划能力，核心调整如下：
  - 新增 `POST /api/path/detail/generate`，用于按当前 `scopeCode` 生成本范围学习路径
  - 后端没有另起一套新算法，而是复用现有 `LearningPathPlanner`、`LearningPathExplainer` 与 `LearningResourceService`
  - 具体实现方式是：先通过 `KnowledgeGraphService::buildGraphPayload(scopeCode)` 读取当前细化 scope，再把该 scope 的 `nodes / edges` 装配为临时 `KnowledgeGraph`，随后直接调用现有 planner 生成局部路径
  - 当请求未显式提供 `targetNodeCode` 时，系统会自动选择当前 scope 中“无后继节点”的末端节点作为默认目标
  - 资源服务增强为：若某细化节点没有独立资源条目，会继续回退匹配 `focusNodeCode`，从而让 `queue-circular`、`kmp-next-definition`、`graph-basic-bfs` 等细化节点也能获得定向资源
  - 前端 `KnowledgeGraphPreview` 在 `currentScopeCode != root` 时新增“本范围学习路径”面板，支持目标节点选择、时间预算设置、细化节点掌握度模拟、本轮/下轮结果展示以及本范围资源建议
  - 细化路径结果会同步高亮到 Cytoscape 图谱中，区分本轮学习、顺延节点、已掌握节点和当前目标节点
  - 当前阶段细化掌握度先按 `scopeCode + nodeCode` 在前端局部维护，不引入新的数据库表，也不把细化层与一级主路径打平成单一大图
  本次实现明确把细化路径定义为“主学习路径中某个主节点的局部执行计划”，而不是新的独立推荐系统。
- 变更原因：
  之前系统虽然已经支持多层节点下钻，但进入细化图谱后仍缺少“当前范围内部的学习顺序建议”，只能靠用户自己阅读依赖边。这样不利于把“主路径规划”延伸成真正的分层学习导航。加入局部路径规划后，系统就能形成“先规划主节点，再规划主节点内部执行顺序”的两层闭环，既更符合个性化学习导航主题，也更适合答辩演示。
- 影响范围：
  `backend/controllers/PathPlanningController.h`、`backend/controllers/PathPlanningController.cc`、`backend/services/PathPlanningService.h`、`backend/services/PathPlanningService.cc`、`backend/services/LearningResourceService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/api/path.js`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  新增 `POST /api/path/detail/generate` 接口；不修改数据库结构；不改变既有主路径 planner、反馈调整规则和解释生成规则，只是把它们复用于 detailScope 子图。细化掌握度当前不持久化，仍属于前端局部状态增强。
- 验证方式：
  执行 `cmake -S backend -B backend/build` 刷新 Drogon 测试发现列表；执行 `cmake --build backend/build -j$(nproc)` 完成后端构建；执行 `ctest --test-dir backend/build --output-on-failure` 确认 62 项测试全部通过，其中包含新增的 detail path 用例；执行 `npm run build` 通过前端构建；在浏览器中进入任一细化 scope 后，确认页面会自动生成“本范围学习路径”，并在图谱中同步高亮计划节点。

### [2026-03-20] 细化图谱局部学习路径支持学习反馈重算与路径前后对比

- 变更类型：架构改动
- 变更内容：
  在上一阶段已经具备 `detailScope` 局部学习路径生成能力的基础上，当前系统继续补齐“学后反馈 -> 即时重算 -> 展示变化”的闭环，核心调整如下：
  - 新增 `POST /api/path/detail/adjust`，用于接收当前细化 scope 内本轮学习节点的反馈，并立即返回重算后的局部路径
  - 后端仍不新起独立算法，而是继续复用现有 `LearningPathAdjuster`、`LearningPathPlanner`、`LearningPathExplainer` 与 `LearningResourceService`
  - `PathPlanningService::buildAdjustedDetailPathPayload(...)` 会先按 `scopeCode` 组装当前 scope 子图，再根据 `feedbackItems` 更新局部掌握度，随后在同一 scope 内重新规划路径
  - 接口响应中补充 `feedbackSummary`、`updatedMasteryByCode`、`adjustments`，并保留 `scope / breadcrumbs / meta / path / resourceRecommendations`，让前端可以直接复用局部路径展示面板
  - 前端 `KnowledgeGraphPreview` 在“本范围学习路径”区域新增学习反馈表单，允许用户对本轮细化节点提交“已完成 / 部分完成 / 学习受阻”和学习后掌握度
  - 前端新增“最近一次局部反馈结果”摘要卡片，用于展示每个反馈节点掌握度从多少提升或回落到多少，以及具体调整原因
  - 前端新增“局部路径前后对比”区块，显式展示调整前后本轮安排、转为已掌握、提前到本轮、移出本轮和顺序调整等变化
  - 局部反馈后的掌握度目前仍只维护在前端当前 scope 的临时状态中，不写入数据库，也不与主路径画像自动合并
  - 局部反馈结果与重算后的节点状态会继续同步高亮到 Cytoscape 图谱中，保证图谱、局部路径和变化摘要三者一致
  本次实现的目标是让细化图谱不仅能规划“范围内先学什么”，还能演示“学完之后为什么变、变成什么样”。
- 变更原因：
  如果细化图谱只有静态的局部路径而没有学习反馈闭环，那么它更像一个局部排序器，而不是完整的学习导航。加入 scope 内反馈重算后，系统就可以在答辩时演示“主路径节点内部也能根据学习结果动态调整”，这会让项目更完整，也能为后续把推荐理由解释绑定到局部变化提供依据。
- 影响范围：
  `backend/controllers/PathPlanningController.h`、`backend/controllers/PathPlanningController.cc`、`backend/services/PathPlanningService.h`、`backend/services/PathPlanningService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/api/path.js`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  新增 `POST /api/path/detail/adjust` 接口；不修改数据库结构；不改变既有主路径反馈规则和推荐解释逻辑，只是把相同的调整规则复用于 detailScope 局部路径。当前局部反馈仍不持久化，因此不会影响主路径画像表与已有反馈回退机制。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `cmake --build backend/build -j$(nproc)` 完成后端构建；执行 `ctest --test-dir backend/build --output-on-failure` 确认 62 项测试全部通过，其中包含新增的 `PathPlanningServiceAdjustsDetailScopePathWithFeedback` 用例；在浏览器中进入任一细化 scope 后，确认可以提交局部反馈、看到掌握度变化摘要、查看本轮路径前后对比，并在图谱中同步看到更新后的高亮状态。

### [2026-03-20] 首页主路径生成后支持在左侧知识图谱中显化二级 / 三级节点

- 变更类型：架构改动
- 变更内容：
  为了让首页左侧图谱在点击“生成学习路径”后不只高亮一级课程节点，而能直接展示更细粒度的学习路线，当前前端新增了“主路径显化”机制，核心调整如下：
  - `HomeView` 新增主路径结果中转状态，接收 `PathPlannerPanel` 产生的最新主路径结果，并传递给 `KnowledgeGraphPreview`
  - `PathPlannerPanel` 新增 `plan-updated` 事件；在首次生成主路径、提交学习反馈后重算主路径、执行反馈回退后重算主路径这三种场景下，都会把最新 `planResult` 向外同步
  - `KnowledgeGraphPreview` 新增 `mainPathPlan` 输入，当当前视图位于根图谱时，会根据主路径中的一级节点自动展开对应的细化 scope
  - 展开逻辑没有新增后端专用接口，而是复用现有 `GET /api/knowledge/graph?scopeCode=...`，按路径节点递归加载二级和三级细化图谱
  - 对于每个被展开的主路径节点，前端会把主节点与细化 scope 的入口节点通过合成边连接起来，使 root 主线和二级 / 三级节点形成一张连续可读的显化图
  - 二级 / 三级节点会根据所属一级主路径节点的状态继承显化样式，例如本轮学习分支、顺延分支、已掌握分支会使用不同的颜色和边框提示
  - 左侧图谱顶部新增显化提示条，说明当前已经展开了多少个一级节点，以及总共显化了多少个二级 / 三级节点
  - 图谱详情面板的节点信息、直接前置依赖和直接后继节点现在基于“当前实际渲染图谱”计算，因此展开出来的二级 / 三级节点也可以直接点击查看详情；但用于把主节点与细化 scope 串起来的合成边不会被当作真实依赖关系显示
  本次改动的目标是把“一级学习路径”与“节点内部细化知识结构”直接连在一起，让首页图谱能在答辩演示时更直观地回答“系统推荐的这条路径具体展开后是什么样”。
- 变更原因：
  之前首页左侧图谱只能显示课程一级主线，而右侧路径规划已经能生成个性化学习路径。两者之间存在信息断层：用户知道“先学哪些一级节点”，但看不到这些节点展开后的更细粒度知识路线。加入显化机制后，主路径结果可以直接映射到图谱中，形成更完整的“从课程主线到单元细化”的视觉闭环。
- 影响范围：
  `frontend/src/views/HomeView.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增数据库表；不修改主路径 planner、detail planner、反馈调整器与推荐解释逻辑；不新增专用后端接口，只复用已有知识图谱 scope 查询接口。此次变更属于首页前端组件通信与图谱渲染结构增强。
- 验证方式：
  执行 `npm run build` 通过前端构建；在首页点击“生成学习路径”后，确认左侧知识图谱会在一级主线节点下自动展开对应的二级 / 三级细化节点，并保留原有主路径高亮；确认点击展开节点可以查看节点详情，且双击带细化能力的展开节点仍可继续进入对应 scope。

### [2026-03-20] 回退首页主路径自动显化二级 / 三级节点的实验改动

- 变更类型：架构改动
- 变更内容：
  按当前需求，已撤销“首页主路径生成后自动在左侧知识图谱中显化二级 / 三级节点”的前端实验方案，系统恢复到上一个稳定版本，具体包括：
  - 移除 `HomeView` 中主路径结果向 `KnowledgeGraphPreview` 的额外传递
  - 移除 `PathPlannerPanel` 新增的 `plan-updated` 事件同步
  - 移除 `KnowledgeGraphPreview` 中基于主路径结果递归加载二级 / 三级 scope 并拼接合成边的逻辑
  - 移除首页根图谱顶部的显化提示条与对应 Cytoscape 显化样式
  - 恢复左侧图谱为上一版行为：主页面只展示当前 scope 的原始图谱；细化节点仍通过双击进入，不在首页主路径生成时自动展开
  本次回退不影响已经完成的多层细化图谱、局部学习路径生成、局部反馈重算、资源建议和主路径反馈回退能力。
- 变更原因：
  当前这版主路径自动显化效果不符合预期，因此按需求回退到上一版本，优先保证首页图谱行为稳定、交互清晰。
- 影响范围：
  `frontend/src/views/HomeView.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变主路径或局部路径算法；仅回退首页前端组件通信与图谱渲染实验改动。
- 验证方式：
  执行 `npm run build` 通过前端构建；在首页点击“生成学习路径”后，确认左侧仍保持上一版的一级主线图谱展示，不再自动展开二级 / 三级节点；确认双击可细化节点后，细化图谱和局部路径功能仍可正常使用。

### [2026-03-20] 主路径卡片中的配套资源入口收敛到独立资源建议区

- 变更类型：架构改动
- 变更内容：
  为了让首页路径规划区域的信息分工更清晰，当前对主路径面板中的资源展示方式做了收敛调整：
  - “本轮推荐学习”和“下一轮建议”卡片中的展开详情不再内嵌显示配套资源列表
  - 节点卡片内部现在只保留推荐理由、解释标签、解释要点和解释指标，不再混入资源链接
  - 配套资源统一只在下方独立的“当前学习导航配套资源”区域展示，作为单一资源入口
  - 同步移除了主路径卡片内部仅用于内嵌资源块的辅助函数和对应样式，避免后续展示入口再次分散
  本次调整不影响资源推荐结果本身，只调整其在首页中的呈现位置。
- 变更原因：
  当资源同时出现在“本轮/下一轮推荐学习”模块和“资源建议”模块时，页面会出现重复信息，用户也不容易判断应该看哪一块。把资源统一收敛到独立资源区后，主路径卡片负责回答“为什么学”和“先学什么”，资源建议区负责回答“去哪里学”，信息边界更清楚。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐和推荐解释算法；属于首页主路径区域的前端展示层调整。
- 验证方式：
  执行 `npm run build` 通过前端构建；在首页生成学习路径后，确认“本轮推荐学习 / 下一轮建议”展开详情中不再显示资源链接，且所有资源仍只在独立的“当前学习导航配套资源”区域出现。

### [2026-03-20] 资源建议区增加分组筛选与折叠控制

- 变更类型：架构改动
- 变更内容：
  为了让首页下方的资源建议区更适合答辩演示和连续浏览，当前对资源展示方式进一步做了前端层优化：
  - 资源建议区顶部新增“全部资源 / 只看本轮 / 只看下一轮”筛选按钮，便于快速切换当前关注的资源分组
  - 每个知识点资源分组新增“展开资源 / 收起资源”控制，避免同一页面一次性展开过多资源卡片
  - 默认策略调整为优先展开“本轮推荐学习”对应资源，“下一轮建议”资源默认收起，降低资源区对当前主路径阅读的干扰
  - 资源分组头部新增状态标签，明确标示该组资源属于“本轮推荐学习”还是“下一轮建议”
  - 当前筛选条件下没有资源时，会显示明确的空状态提示，避免用户误以为资源数据丢失
  本次改动仅涉及首页资源建议区的前端交互，不改变资源推荐内容本身。
- 变更原因：
  在资源被统一收敛到独立区域后，如果多个节点同时带有视频和文本教程，资源区会明显变长，影响页面整体节奏。加入筛选和折叠后，用户可以先聚焦本轮要学的内容，下一轮资源按需展开，首页演示效果更稳定，也更容易讲清楚“当前学什么”和“稍后看什么”。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、反馈调整、资源推荐和推荐解释算法；属于首页资源建议区的前端展示与交互优化。
- 验证方式：
  执行 `npm run build` 通过前端构建；在首页生成学习路径后，确认资源建议区可按“全部 / 本轮 / 下一轮”筛选，且本轮资源默认展开、下一轮资源默认收起，展开收起操作不影响路径规划与反馈功能。

### [2026-03-20] 资源建议排序细化并增加“优先看”标记

- 变更类型：架构改动
- 变更内容：
  为了让学习导航中的资源建议不只是“列出来”，而是能更明确地回答“先看哪条资源”，当前对资源推荐结果增加了统一排序和优先标记：
  - 后端 `LearningResourceService` 新增资源优先级评分逻辑，综合考虑当前节点处于“本轮学习”还是“下一轮建议”、资源阶段定位（预习 / 主学 / 巩固 / 难点突破 / 答辩复盘 / 延伸学习）、资源类型（视频 / 文本 / 资料）以及是否直接对齐当前节点
  - 同一知识点下的资源现在会先在后端完成排序，再输出 `recommendationRank`、`isPrimaryRecommendation` 和 `priorityLabel` 等字段，避免不同前端面板各自定义排序规则
  - 首页路径规划的“资源建议”区会在每组资源头部显示“优先资源”，并在排序后的第一条资源上显示“优先看”标记
  - 细化图谱页面的局部资源建议区同步显示“优先资源”和“优先看”标记，保证根图谱与二级 / 三级图谱的资源阅读方式一致
  - 资源推荐分组对象新增 `primaryResourceTitle` 与 `primaryResourcePriorityLabel`，用于在资源组折叠时仍然快速告知用户本组最值得先看的内容
  本次改动保持现有资源库内容不变，只细化排序逻辑和展示信号。
- 变更原因：
  之前资源区虽然已经独立展示，但同组资源仍主要按配置顺序呈现，用户需要自己判断应该先看视频、还是先读文本、还是先补某个难点材料。加入统一排序和“优先看”标记后，系统在答辩演示时可以更清楚地说明“为什么这条资源排第一”，资源建议从“资料列表”提升为更接近“学习指引”的状态。
- 影响范围：
  `backend/services/LearningResourceService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增数据库表；不改变学习路径主算法与反馈调整算法；仅增强资源推荐结果的后端排序字段与前端显示方式，属于资源建议子模块的可解释性增强。
- 验证方式：
  执行 `cmake --build . -j$(nproc)` 通过后端构建；执行 `ctest --output-on-failure` 时 63/63 项测试全部通过；执行 `npm run build` 通过前端构建。页面上确认同一资源组会显示“优先资源”，且排序后的首条资源带有“优先看”标记。

### [2026-03-20] 资源建议增加“课程风格优先 / 图文补充 / 答辩复盘”分层

- 变更类型：架构改动
- 变更内容：
  为了让资源建议更像“学习导航”，而不是单纯的资料列表，当前在排序基础上继续加入了资源分层能力：
  - 后端 `LearningResourceService` 基于 `recommendedPhase`、资源类型以及是否为首选资源，自动为每条资源计算 `resourceLayer` 与 `resourceLayerHint`
  - 当前资源分为三类：`课程风格优先`、`图文补充`、`答辩复盘`
  - 每个资源推荐分组会额外输出 `resourceLayerSummary`，用于告诉前端该组资源在三类中的数量分布
  - 首页“资源建议”区顶部新增三类资源总览统计，用户可以先判断当前显示的资源整体偏向哪一类
  - 首页每个资源组会显示本组的三类资源分布，每条资源也会显示所属分层及对应说明
  - 细化图谱页的局部资源建议区同步显示分层标签和组内分布，保证主页面与细化页面的资源解释方式一致
  本次改动不要求额外维护新的资源配置字段，而是直接复用现有资源目录中的阶段、类型和排序结果做动态分层。
- 变更原因：
  即使已经加入“优先看”排序，资源建议仍然偏向“先看哪一条”的单点提示，用户对“这条资源在整个学习过程里属于什么角色”仍然不够直观。加入分层后，系统可以更清楚地区分“主线学习材料”“图文补充材料”和“答辩复盘材料”，更符合毕设答辩中对系统可解释性的要求。
- 影响范围：
  `backend/services/LearningResourceService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增数据库表；不改变路径规划、反馈调整和资源内容本身；仅增强资源建议结果的展示语义与前端说明方式，属于资源推荐模块的展示层和可解释性增强。
- 验证方式：
  执行 `cmake --build . -j$(nproc)` 通过后端构建；执行 `ctest --output-on-failure` 验证资源分层字段与原有能力兼容；执行 `npm run build` 通过前端构建。页面上确认首页资源区会显示三类资源统计，每条资源有明确分层标签，细化图谱页也同步显示相同分类语义。

### [2026-03-23] 资源卡片显式挂接推荐理由标签

- 变更类型：架构改动
- 变更内容：
  为了让资源建议与路径推荐理由形成更直接的对应关系，当前进一步增强了资源卡片的解释信息：
  - 后端 `LearningResourceService` 在构建资源推荐时，会基于当前节点的 `PathExplanation.labels` 和资源阶段定位，自动生成 `linkedReasonLabels` 与 `linkedReasonLabelSummary`
  - 这些标签用于说明“这条资源主要对应哪几个推荐理由”，例如 `本轮学习`、`下一轮建议`、`前置依赖`、`掌握度待提升`、`高目标相关`
  - 首页资源区中，每条资源卡片新增“对应推荐理由”标签组，并附一行简短说明，用户无需回到路径卡片对照就能理解资源和推荐理由之间的关系
  - 细化图谱页中的局部资源建议区同步显示相同的“对应推荐理由”标签组，保证根图谱和细化图谱的资源解释方式一致
  - 后端测试补充校验 `linkedReasonLabels` / `linkedReasonLabelSummary` 字段，确保新增解释信息与原有资源排序、分层逻辑兼容
  本次改动不引入新的人工配置字段，而是直接复用现有推荐解释标签与资源阶段信息动态生成。
- 变更原因：
  之前资源卡片虽然已经有“优先看”和分层信息，但用户仍需要自己在资源区和路径区之间来回对照，才能判断“这条资源为什么会推荐给我”。把推荐理由标签直接挂到资源卡片上后，资源建议可以更明确地回答“它对应哪条推荐依据”，演示和答辩时也更容易讲清楚系统的因果链路。
- 影响范围：
  `backend/services/LearningResourceService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增数据库表；不改变路径规划主算法、反馈调整和资源内容本身；仅增强资源推荐结果中的解释字段，并在前端资源卡片中展示，属于资源推荐与推荐解释的联动增强。
- 验证方式：
  执行 `cmake --build . -j$(nproc)` 通过后端构建；执行 `ctest --output-on-failure` 验证新增解释字段与原有 63 项测试兼容；执行 `npm run build` 通过前端构建。页面上确认资源卡片会出现“对应推荐理由”标签组与简短说明。

### [2026-03-23] 资源点击追踪接入 SQLite，并在学习者画像页展示最近查看资源

- 变更类型：架构改动
- 变更内容：
  为了让系统形成更完整的“推荐 -> 点击资源 -> 留下学习行为轨迹”的闭环，当前新增了资源查看记录链路：
  - 数据库新增 `learning_resource_view_records` 表，用于持久化资源点击行为，记录学习者、对应知识点、资源标题、链接、资源类型、资源分层、推荐阶段、来源上下文和记录时间
  - 后端新增 `ResourceViewRepository` 与 `ResourceViewService`，提供资源查看行为的保存、计数和最近记录查询能力
  - 新增接口 `POST /api/resource/view`，前端在点击资源链接时会把该次查看行为上报到后端
  - 为了兼容当前已有数据库文件，`ResourceViewRepository` 在运行时会自动补齐资源查看记录表和索引，避免必须手动重建数据库
  - `LearnerProfileService` 现已把资源浏览记录并入学习者画像，新增 `summary.resourceViewRecordCount` 和 `recentResourceViewItems`
  - 首页主路径资源区与细化图谱资源区都会在点击资源时触发上报，并通过 `profile-updated` 事件把最新画像返回给首页，使资源浏览计数可即时刷新
  - 学习者画像页新增“最近查看资源”区域，展示最近点击过的资源标题、对应知识点、资源分层、来源上下文和记录时间
  本次改动让系统从“只会推荐资源”提升为“能记录学习行为轨迹”，更接近一个可演示的学习导航闭环。
- 变更原因：
  之前系统已经能生成路径、解释推荐原因并给出资源，但资源一旦被点击，系统内部并不会留下痕迹，答辩时难以说明“推荐之后学生是否真的跟着资源去学了”。加入资源查看记录后，系统至少能回答“最近看了哪些资源、这些资源对应哪些知识点、是从首页主路径还是从细化图谱进入的”，形成更完整的行为证据链。
- 影响范围：
  `database/init/001_schema.sql`、`backend/controllers/ResourceViewController.*`、`backend/repositories/ResourceViewRepository.*`、`backend/repositories/KnowledgeGraphRepository.*`、`backend/services/ResourceViewService.*`、`backend/services/LearnerProfileService.cc`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、`frontend/src/api/resource.js`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/LearnerProfileCard.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/views/LearnerProfileView.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  新增一张 SQLite 行为记录表和一个资源行为接口；不改变路径规划主算法、反馈调整算法和资源推荐内容本身；属于学习行为留痕与学习者画像扩展。
- 验证方式：
  执行 `cmake -S .. -B .` 重新生成构建文件后，执行 `cmake --build . -j$(nproc)` 通过后端构建；执行 `ctest --output-on-failure` 时 65/65 项测试全部通过；执行 `npm run build` 通过前端构建。页面上确认点击首页或细化图谱中的资源链接后，学习者画像页会新增最近查看资源记录，首页右侧速览中的资源浏览计数也会同步更新。

### [2026-03-23] 资源行为记录扩展为“已打开 / 已看 / 学完 / 稍后再看”

- 变更类型：架构改动
- 变更内容：
  为了让资源推荐不只停留在“被点击过”，当前把资源记录链路扩展成更完整的资源行为反馈：
  - SQLite 表 `learning_resource_view_records` 新增 `interaction_type` 字段，支持 `opened`、`viewed`、`completed`、`save_for_later` 四类最小行为状态
  - 为兼容已存在的数据库文件，`ResourceViewRepository` 在运行时会先检查 `interaction_type` 是否存在；若旧库缺少该列，则自动执行补列，并把历史空值回填为 `opened`
  - `POST /api/resource/view` 继续沿用原有接口地址，但请求体现在允许传入 `interactionType`，不新增第二套资源反馈接口，避免前后端职责分裂
  - 后端 `ResourceViewService` 会校验行为枚举是否合法，并在返回的学习者画像中附带最近资源行为的 `interactionType` 与中文标签，便于前端直接渲染
  - 首页“路径规划”资源区和细化图谱资源区，除了点击资源标题自动记为“已打开”外，还新增三个轻量按钮：`记为已看`、`记为学完`、`稍后再看`
  - 学习者画像页把“最近查看资源”升级为“最近资源行为”，每条记录会同时展示资源类型和行为状态徽标，便于答辩时说明“推荐之后学生做了什么”
  本次改动保持资源推荐逻辑、学习路径算法和反馈调整算法不变，只把资源使用情况从“点击留痕”推进到“最小资源行为闭环”。
- 变更原因：
  之前系统已经能记录“资源是否被点开”，但仍回答不了“学生只是打开了，还是已经看完了，还是准备稍后再看”。在毕设答辩场景下，这会让资源模块显得偏静态。加入 4 类资源行为后，系统可以更完整地展示“推荐 -> 查看 -> 标记状态 -> 回到画像页观察轨迹”的过程，演示感更强，也更接近真实学习导航系统。
- 影响范围：
  `database/init/001_schema.sql`、`backend/controllers/ResourceViewController.cc`、`backend/repositories/ResourceViewRepository.*`、`backend/services/ResourceViewService.cc`、`backend/services/LearnerProfileService.cc`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/LearnerProfileCard.vue`、`frontend/src/views/LearnerProfileView.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增表，不新增资源反馈子系统，不改变路径规划和反馈调整算法；仅在现有资源行为表与现有 `/api/resource/view` 接口上扩展状态语义，并兼容旧 SQLite 数据库。
- 验证方式：
  执行 `cmake -S .. -B .` 重新生成后端构建文件；执行 `cmake --build . -j$(nproc)` 通过后端构建；执行 `ctest --output-on-failure` 时 65/65 项测试全部通过；执行 `npm run build` 通过前端构建。页面上确认资源标题点击会记录“已打开”，三个轻量按钮能额外记录“已看 / 学完 / 稍后再看”，学习者画像页会同步展示最近资源行为及对应状态徽标。

### [2026-03-23] 资源排序接入学习者最近行为，已学完资源自动后移

- 变更类型：架构改动
- 变更内容：
  为了让资源建议不只是静态排序，而是能对学习者已经做过的操作作出响应，当前把“最近资源行为”进一步接入了资源排序链路：
  - `LearningResourceService` 在构建资源推荐时，会读取指定学习者在该知识点下每条资源的最近一次行为记录，并按资源 URL 做最近状态聚合
  - 资源排序现在在原有“学习阶段 / 资源类型 / 细化节点聚焦”评分基础上，再叠加轻量行为加权：
    - `completed`：显著降权，避免已学完资源长期占据首位
    - `viewed`：适度降权，优先把未接触资源放前面
    - `opened`：轻微降权，减少重复置顶
    - `save_for_later`：当前轮次适度后移；若节点本身属于“下一轮建议”，则继续保留其可见性
  - 资源卡片新增最近行为徽标与“排序调整”说明，例如 `已学完`、`已看过`、`稍后再看`，避免排序变化变得不可解释
  - 路径规划主页面和细化图谱页面在调用路径生成接口时，现在都会显式携带 `learnerCode`，使资源排序能使用对应学习者的真实行为记录
  - 路径规划测试新增两条用例，分别验证“已学完资源会自动后移”和“路径响应中会带出最近资源行为字段”
  本次改动没有引入新的推荐模型，也没有让资源行为反向修改掌握度，只是在现有资源建议上增加一层本科毕设可解释、可演示的个性化排序。
- 变更原因：
  之前即使系统已经记录了“已看 / 已学完 / 稍后再看”，资源区的排序本身仍然是静态的，用户再次生成路径时，看见的资源顺序几乎不变，学习闭环不够明显。接入最近行为后，系统可以更自然地表现出“你已经学完的材料会后移，没看过的材料会被优先展示”，资源推荐因此更像“学习导航”，而不只是“学习资料列表”。
- 影响范围：
  `backend/repositories/ResourceViewRepository.*`、`backend/services/LearningResourceService.*`、`backend/services/PathPlanningService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增数据库表；不修改路径规划主算法、反馈调整算法和学习者画像结构；仅在现有资源推荐服务中引入轻量行为加权，并要求路径生成接口可接收 `learnerCode` 以启用该能力。旧请求不传 `learnerCode` 时仍会回退到默认演示学习者。
- 验证方式：
  执行 `cmake -S .. -B .` 重新生成后端构建文件；执行 `cmake --build . -j$(nproc)` 通过后端构建；执行 `ctest --output-on-failure` 时 67/67 项测试全部通过；执行 `npm run build` 通过前端构建。页面上确认资源卡片会显示最近行为徽标，已学完资源在重新生成路径后会自动后移，排序变化同时附有解释说明。

### [2026-03-23] 首页与画像页滚动性能优化，降低长页面重绘成本

- 变更类型：前端性能优化
- 变更内容：
  为了缓解首页和学习者画像页在长内容滚动时的轻微卡顿，当前对前端展示层做了两类轻量优化：
  - 统一引入全局面板样式变量，提升卡片背景不透明度，移除大面积卡片上的 `backdrop-filter: blur(...)`，并收敛阴影强度，降低滚动时的持续重绘成本
  - 对首页静态说明卡片、学习者画像页说明卡片，以及路径规划/细化图谱中的长列表项和重复卡片块，补充 `contain: paint` 与 `content-visibility: auto` / `contain-intrinsic-size`，让浏览器在滚动时减少不必要的离屏绘制
  - 导航激活态按钮的阴影也同步收敛，避免整页多层阴影叠加
  本次调整不改变页面结构、交互逻辑和功能模块，只优化样式层的渲染成本。
- 变更原因：
  随着知识图谱、路径规划、资源建议、反馈对比和画像记录不断叠加，首页已经成为一个内容较长、卡片较多的单页。此前大量半透明卡片同时使用背景模糊和大阴影，在浏览器滚动时容易触发较高的重绘开销，表现为“略有卡顿”。通过降低玻璃效果成本并为长列表分段渲染，可以在不牺牲答辩展示完整性的前提下改善滚动流畅度。
- 影响范围：
  `frontend/src/style.css`、`frontend/src/components/PageLayout.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/components/HealthStatusCard.vue`、`frontend/src/components/LearnerProfileCard.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/PathPlannerPanel.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库和算法；仅涉及前端视觉样式与浏览器渲染策略优化。
- 验证方式：
  执行 `npm run build` 通过前端构建。页面上应保持原有布局和功能不变，同时在滚动长页面时减少玻璃模糊带来的重绘压力。

### [2026-03-23] 首页资源建议拆为单节点资源页，避免把全部资源堆在同一屏

- 变更类型：架构改动
- 变更内容：
  为了让首页路径规划区域继续聚焦“学什么、为什么学、如何调整”，当前把原本堆叠在首页下方的大块资源列表拆成按节点进入的单独资源页面：
  - 前端新增动态路由 `/resource-recommendation/:code` 与 `ResourceRecommendationView`，每次只展示一个学习节点的完整推荐资源
  - 首页 `PathPlannerPanel` 中的“本轮推荐学习”“下一轮建议”和资源入口卡片，统一新增“查看推荐资源”按钮；点击后进入对应节点的资源页，而不是在首页展开全部资源明细
  - 首页扩展区原“资源建议”主体改为“资源入口”摘要，只保留节点级入口、分层统计与主资源提示，不再渲染每个节点下所有资源卡片
  - 新增 `navigationStore` 中的资源上下文快照能力，使用 Pinia + `sessionStorage` 持久化最近一次路径规划返回的 `resourceRecommendations`、目标标签、时间预算和当前学习者编码，保证页面跳转后仍能按节点读取最近一次资源结果
  - 单节点资源页继续复用现有资源行为上报接口，支持“已打开 / 已看 / 学完 / 稍后再看”四类记录，但这些操作只发生在当前节点页内部，不再占据首页纵向空间
  - 为避免学习者画像页出现错误来源说明，资源页上报行为的 `sourceContext` 现已独立标记为 `resource_recommendation_page`，画像页会显示为“单节点资源页”
  本次改动不新增后端资源查询接口，不引入新的资源存储结构，而是直接复用最近一次路径规划结果中的资源快照，保持实现简单且适合毕设演示。
- 变更原因：
  随着资源建议内容不断补充，首页“路径规划”模块内如果同时铺开多个节点的全部资源卡片，会导致右侧图谱和左侧导航信息被长列表拉散，页面显得割裂，也不利于答辩时按节点演示。拆成单节点资源页后，首页重新回到“路径主线展示”，而资源部分则变成“按需进入的专题页”，结构更清楚，交互也更接近真实学习导航产品。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/stores/navigationStore.js`、`frontend/src/router/index.js`、`backend/services/LearnerProfileService.cc`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增数据库表，不新增资源接口，不改变路径规划、反馈调整与资源排序算法；仅调整前端页面组织方式，并补充资源行为来源语义，属于学习导航展示层与资源页交互边界优化。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `cmake --build backend/build -j$(nproc)` 通过后端构建；执行 `ctest --test-dir backend/build/tests --output-on-failure -R ResourceViewServicePersistsRecentViewedResources` 验证“单节点资源页”来源标签在学习者画像链路中正确显示。页面上确认首页仅显示资源入口摘要，点击“查看推荐资源”后会跳到对应节点资源页，且资源行为在画像页中会显示为“单节点资源页”来源。

### [2026-03-23] README 同步当前资源页结构，并为资源页补齐兜底与前端最小测试

- 变更类型：架构改动
- 变更内容：
  为了让文档、自测流程和当前前端实现保持一致，同时降低资源页在演示时因刷新或旧链接导致的断链风险，当前补做了三类收尾工作：
  - `README.md` 已同步到当前实际结构，明确首页展示的是“按学习节点查看推荐资源”的入口区，而不是旧版的大块资源列表；手动测试说明和测试命令也同步更新
  - `ResourceRecommendationView` 新增资源页兜底逻辑：
    - 若最近一次资源快照不存在，则明确提示需要回首页重新生成路径
    - 若资源快照仍存在，但当前访问的节点已不在最近一次快照中，则显示“当前节点资源快照已失效”提示，并提供“回首页重新生成路径”和“查看最近缓存资源”两个兜底入口
  - `navigationStore.clearResourceRecommendationContext()` 现在会同时重置 `activeLearnerCode`，避免资源快照失效后残留旧学习者上下文
  - 前端新增最小测试基础：
    - 引入 `vitest`、`jsdom`、`@vue/test-utils`
    - 新增 `npm run test`
    - 补充 `navigationStore` 持久化测试
    - 补充单节点资源页“无快照 / 节点失效”兜底测试
  本次改动不改变学习路径、资源排序和反馈算法，只提升当前版本的文档一致性、演示稳定性与前端回归保障。
- 变更原因：
  资源页已经拆分完成，但若 README 仍停留在旧版首页资源结构，后续自测、论文撰写和答辩讲解都会出现描述漂移。与此同时，单节点资源页依赖最近一次路径规划快照，刷新页面或直接打开旧链接时容易出现“资源页空掉”的尴尬场景。补充兜底交互和前端最小测试后，这一块的行为边界会更清晰，演示稳定性也更高。
- 影响范围：
  `README.md`、`frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/stores/navigationStore.js`、`frontend/package.json`、`frontend/vite.config.js`、`frontend/src/test/setup.js`、`frontend/src/stores/navigationStore.test.js`、`frontend/src/views/ResourceRecommendationView.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强前端资源页兜底能力、同步项目文档，并补充前端最小测试基础。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test` 时 2 个测试文件、4 条测试全部通过。页面上确认资源页在“无快照”和“旧节点链接”两种场景下会给出明确兜底提示，并可返回首页或切换到最近缓存资源。

### [2026-03-23] 学习者画像页可直接打开薄弱节点的单节点资源页

- 变更类型：架构改动
- 变更内容：
  为了让“学习者建模 -> 学习资源导航”形成更直接的演示闭环，当前把学习者画像页中的薄弱节点列表与单节点资源页打通：
  - 学习者画像页“当前待补强知识点”中，每个薄弱节点新增“查看推荐资源”按钮
  - 点击后，前端不会新增后端专用资源接口，而是直接复用现有 `POST /api/path/generate`，以该薄弱节点为目标、结合当前画像 `masteryByCode` 生成一版最小路径结果
  - 生成完成后，前端会把这次返回的 `resourceRecommendations` 写入 `navigationStore` 作为资源上下文，再自动跳转到对应的单节点资源页
  - 若该薄弱节点未返回可用资源，则会在画像页内给出明确错误提示，避免直接跳转到空白页面
  - 按钮支持单项 loading 状态，避免连续重复点击
  - 前端新增一条页面级测试，验证“从画像页点击薄弱节点按钮 -> 生成资源上下文 -> 跳转资源页”的主链路
  本次改动保持“资源页展示”和“资源上下文生成”继续复用已有路径规划结果，不引入新的资源接口或额外数据库结构。
- 变更原因：
  之前学习者画像页虽然已经能展示“当前待补强知识点”，但用户看完短板之后仍需要手动切回首页、重新选择目标、再进入资源页，链路偏长。打通后，画像页就能直接回答“这个短板节点现在建议看什么资源”，更适合答辩时演示“发现问题 -> 查看资源 -> 回到首页继续规划”的闭环过程。
- 影响范围：
  `frontend/src/components/LearnerProfileCard.vue`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/views/LearnerProfileView.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐和反馈调整算法；仅在前端画像页增加对现有路径生成接口的复用调用，并补充对应交互与测试。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test` 时 3 个测试文件、5 条测试全部通过。页面上确认在学习者画像页点击某个薄弱节点的“查看推荐资源”后，可直接跳转到对应单节点资源页，并带入该节点的最新资源上下文。

### [2026-03-23] 单节点资源页返回首页时自动回带目标节点并重算路径

- 变更类型：架构改动
- 变更内容：
  为了让“资源页 -> 首页路径规划”的往返操作更顺滑，当前把资源页返回首页的行为从“单纯回首页”升级为“回首页并自动带回当前目标”：
  - 单节点资源页点击“返回首页路径规划”时，现在会把当前知识点 code 作为 `target` 查询参数一并带回首页
  - 首页 `PathPlannerPanel` 新增对路由 `query.target` 的识别逻辑；若该目标存在且能在当前知识点选项中匹配到，就会自动把它设为当前学习目标
  - 在路径规划组件完成画像初始化后，若检测到有效的 `query.target`，会直接按该目标自动生成一版路径，而不是仍停留在默认目标
  - 若 `query.target` 无效或已不在当前课程图谱内，则会回退到原有默认目标初始化逻辑，不会导致首页报错
  - 前端新增两条测试：
    - 资源页返回首页时会带上 `target` 查询参数
    - 路径规划面板在读取到 `query.target` 后，会按该目标完成首轮自动规划
  本次改动继续复用现有首页路径生成逻辑，不增加新的导航状态存储或后端接口。
- 变更原因：
  之前从单节点资源页返回首页时，只能回到首页本身，无法保证首页会停留在刚才查看资源的那个节点，用户仍需要手动重新选择目标，交互上断了一下。补上目标回带后，资源页和首页路径规划之间形成了更自然的来回跳转，答辩演示时也更符合“看完资源 -> 回到该知识点继续看路径”的直觉。
- 影响范围：
  `frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/ResourceRecommendationView.test.js`、`frontend/src/components/PathPlannerPanel.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强前端路由参数使用方式，并在首页初始化阶段消费该参数。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test` 时 4 个测试文件、7 条测试全部通过。页面上确认从单节点资源页返回首页后，首页会自动切到当前节点作为学习目标，并重新生成对应路径。

### [2026-03-23] 首页路径规划目标同步到左侧知识图谱高亮

- 变更类型：架构改动
- 变更内容：
  为了让首页“路径规划”和左侧“知识图谱”之间的联动更直观，当前把路径规划中的当前目标同步到了图谱展示层：
  - `PathPlannerPanel` 现在会在当前学习目标变化时向父层发出 `target-updated` 事件
  - `HomeView` 负责接收当前目标 code，并把它作为 `activeTargetCode` 传给 `KnowledgeGraphPreview`
  - `KnowledgeGraphPreview` 新增 `activeTargetCode` 属性，并在 Cytoscape 渲染状态中为对应节点添加独立的 `is-active-target` 高亮样式
  - 当前目标节点会在图谱中显示红色高亮光晕；若当前 scope 内能找到该节点，则图谱会同步将该节点作为当前选中节点，右侧详情也会跟随切换
  - 若当前处于某个细化 scope 且该目标节点不在本 scope 内，则不会报错，只保留当前 scope 展示状态，等待用户返回主图后再恢复目标高亮
  - 前端新增一条 `HomeView` 组件测试，用于验证“路径规划目标事件 -> 首页状态 -> 图谱预览属性”这条主通信链路
  本次改动不恢复之前已回退的“自动展开二级/三级节点显化”方案，只做目标级联动高亮，保持交互简单、稳定、适合毕设演示。
- 变更原因：
  之前首页右侧的路径规划虽然已经能显示当前目标，但左侧图谱没有直接响应，用户需要自己在图里找对应节点，路径和图谱像是两块分离的信息。把当前目标同步高亮到图谱后，首页就更像一个整体的学习导航界面，答辩时也更容易展示“右边选目标，左边图谱立即响应”的效果。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/views/HomeView.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强首页组件之间的前端通信与图谱样式状态管理。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test` 时 5 个测试文件、8 条测试全部通过。页面上确认切换路径规划目标后，左侧知识图谱会同步高亮当前目标节点；若返回首页时带有 `target` 查询参数，图谱也会跟随切换并保持高亮。

### [2026-03-23] 图谱增加“当前路径目标已同步”提示条

- 变更类型：架构改动
- 变更内容：
  为了让用户更容易理解“为什么左侧图谱节点突然高亮”，当前在图谱说明区补了一层显式提示：
  - `KnowledgeGraphPreview` 在 scope 区域下方新增目标同步提示条
  - 若首页当前路径目标存在且命中当前 scope 内节点，会显示“当前路径目标已同步到图谱，左侧节点高亮可帮助你快速定位”
  - 若当前处于某个细化 scope，且首页目标不在本层图谱范围内，则会显示“当前路径目标不在本层细化图谱范围内，可通过面包屑返回主图查看”的提示
  - 若当前仍在主图 scope，但路径目标暂未命中当前图谱，也会给出轻量提示而不是完全静默
  - 前端新增 `KnowledgeGraphPreview` 组件测试，分别验证“命中当前 scope”与“当前 scope 外”两种提示文案
  本次改动不改变任何路径、资源或反馈结果，只是在首页图谱联动基础上补一层解释提示，降低首次使用时的理解成本。
- 变更原因：
  单纯的高亮联动虽然已经可用，但对第一次使用页面的人来说，仍可能不清楚这是“当前路径目标同步”带来的效果，尤其在从资源页或画像页返回首页时更容易产生疑惑。加上简短提示条后，系统能更明确地说明当前图谱状态的来源，答辩展示时也更利于讲解交互闭环。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/KnowledgeGraphPreview.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强首页图谱的解释层文案与对应前端测试。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test` 时 6 个测试文件、10 条测试全部通过。页面上确认首页图谱会在目标同步后显示对应提示条，并在目标不属于当前细化 scope 时给出返回上层查看的说明。

### [2026-03-23] 主图节点详情支持一键设为当前学习目标

- 变更类型：架构改动
- 变更内容：
  为了让首页形成“图谱 -> 路径规划”的反向联动，当前在主图节点详情区补充了直接驱动右侧路径规划的入口：
  - `KnowledgeGraphPreview` 在主图节点详情区新增“设为当前学习目标”按钮
  - 该按钮只在课程主图层显示，不在细化 scope 内显示，避免把细化节点误当成首页主路径目标
  - 点击后，图谱组件会向 `HomeView` 发出 `set-target` 事件
  - `HomeView` 统一维护当前首页活动目标，并把它同时传给 `KnowledgeGraphPreview` 与 `PathPlannerPanel`
  - `PathPlannerPanel` 新增 `externalTargetCode` 输入；当图谱侧切换目标时，右侧路径规划会自动切换到对应目标，并立即重算一版路径
  - 这条反向链路与已有“路径目标 -> 图谱高亮”链路形成双向联动：首页现在支持“右边选目标，左边图谱响应”以及“左边点节点，右边路径响应”两种操作方式
  - 前端测试补充验证：
    - `HomeView` 测试图谱侧 `set-target` 事件能正确传递到路径规划组件
    - `PathPlannerPanel` 测试 `externalTargetCode` 变化后会自动重算路径
  本次改动继续沿用前端事件通信与现有路径生成接口，不新增任何后端接口或数据库字段。
- 变更原因：
  之前首页虽然已经支持“路径规划目标同步高亮到图谱”，但联动仍然是单向的，用户如果先在图谱里看到某个节点，仍需要手动到右侧再选一次目标。加入“设为当前学习目标”后，图谱就不再只是展示，而是真正参与导航过程，首页整体会更像一个完整的学习导航工作台。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/views/HomeView.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强首页组件之间的前端双向通信逻辑。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test` 时 6 个测试文件、12 条测试全部通过。页面上确认在主图节点详情区点击“设为当前学习目标”后，右侧路径规划会自动切换到该节点并重算路径，同时左侧图谱保持与当前目标一致的高亮状态。

### [2026-03-23] 路径规划结果支持反向定位左侧图谱节点

- 变更类型：架构改动
- 变更内容：
  为了让首页形成“图谱 -> 路径规划 -> 图谱”的完整闭环，当前把右侧路径结果中的每个节点也接入了左侧图谱定位链路：
  - `PathPlannerPanel` 在“本轮推荐学习”和“下一轮建议”的每个路径项上新增“在图谱中定位”按钮
  - 点击后，路径规划组件会向父层发出 `focus-node` 事件，不直接操作图谱，继续保持组件职责清晰
  - `HomeView` 继续作为首页双向联动的中介层，新增 `focusedGraphNodeRequest` 状态，并以 `requestId` 的方式封装每次定位请求，确保重复点击同一节点时也能再次触发图谱侧处理
  - `KnowledgeGraphPreview` 新增 `externalSelectedNodeRequest` 输入；接到定位请求后，若当前 scope 内存在该节点，则直接选中并刷新右侧节点详情
  - 若当前用户停留在某个细化 scope，而请求定位的是主图节点，图谱组件会先自动回到 `root` scope，再在课程主图上完成节点选中，避免用户手动退回上层后再找节点
  - 现有 `activeTargetCode` 高亮逻辑继续保留，因此当前学习目标与“本次临时定位节点”可以同时表达：目标节点维持红色高亮，定位节点则进入选中态与上下文强调态
  - 前端测试补充验证：
    - `PathPlannerPanel` 测试“点击路径项的在图谱中定位按钮 -> 发出 `focus-node` 事件”
    - `HomeView` 测试“路径规划侧定位事件 -> 首页状态 -> 图谱预览属性”这条通信链路
    - `KnowledgeGraphPreview` 测试“接收外部定位请求后切换当前选中节点”
  本次改动继续沿用首页现有父子事件通信与 Cytoscape 状态刷新逻辑，不新增后端接口、数据库字段或额外前端全局 store。
- 变更原因：
  之前首页已经支持“路径目标高亮图谱”和“图谱节点设为路径目标”，但当用户在右侧看到一条具体推荐项时，仍需要回到左侧自己找节点位置，闭环还差最后一步。补上“路径结果反向定位图谱”后，首页两个核心模块就真正串成了一个工作台，更适合答辩时演示“看到推荐 -> 立即回图定位 -> 继续讲依赖关系”的过程。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/views/HomeView.test.js`、`frontend/src/components/KnowledgeGraphPreview.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强首页路径结果与图谱之间的前端反向定位通信。
- 验证方式：
  执行 `npm run build` 通过前端构建；执行 `npm run test -- --run` 时 6 个测试文件、15 条测试全部通过。页面上确认在首页路径结果中点击“在图谱中定位”后，左侧图谱会选中对应节点；若当前正停留在某个细化 scope 中，则会先返回课程主图再完成定位。

### [2026-03-23] 首页移除独立资源入口区，资源统一从路径项直接进入

- 变更类型：架构改动
- 变更内容：
  为了进一步收缩首页纵向长度、减少滚动割裂，当前取消了首页下方独立的“资源入口 / 按学习节点查看推荐资源”整块区域：
  - `PathPlannerPanel` 下半部分不再单独渲染资源入口卡片、资源筛选标签、资源层统计摘要和节点级入口列表
  - 首页资源访问方式统一收敛为路径结果项内的“查看推荐资源”按钮，也就是“本轮推荐学习”和“下一轮建议”中的每个节点直接进入对应单节点资源页
  - 扩展区标题和说明同步调整为“反馈与路径变化”，强调下方区域只负责学习反馈、操作摘要和路径变化，而不是再承担资源入口展示
  - 扩展区右侧“当前导航速览”仍保留资源分组数量与资源行为条数，用于辅助答辩说明当前路径具备多少组可用资源，但不再占用一整块列表空间
  - 为避免后续页面结构回退，前端新增一条测试，验证在存在 `resourceRecommendations` 的情况下：首页不再出现“按学习节点查看推荐资源”标题，同时路径项中的“查看推荐资源”按钮依然保留
  本次改动只收缩首页展示层，不移除 `resourceRecommendations` 数据结构、不影响 `navigationStore` 中的资源快照，也不改变单节点资源页、画像页直达资源和资源行为记录链路。
- 变更原因：
  虽然此前首页已经把资源明细从首页挪到了单节点资源页，但下方仍保留了一个“资源入口摘要区”。在当前界面里，这块区域的作用已经和路径项内的“查看推荐资源”按钮重复，用户为了看资源还要继续往下滚，反而破坏了首页“图谱 + 路径”主线展示的紧凑性。去掉后，首页会更像一个真正的导航工作台，资源则按节点按需进入，体验也更直接。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅调整首页资源入口的展示位置与相关前端文案。
- 验证方式：
  执行 `npm run test -- --run` 时 6 个测试文件、16 条测试全部通过；执行 `npm run build` 通过前端构建。页面上确认首页下方已不再出现独立的“按学习节点查看推荐资源”模块，资源访问统一通过路径项中的“查看推荐资源”按钮进入单节点资源页。

### [2026-03-23] 修复“当前导航速览”滚动时被下方卡片覆盖的问题

- 变更类型：界面行为修复
- 变更内容：
  针对首页右侧扩展区中的“辅助摘要 / 当前导航速览”卡片，当前按侧栏结构重新修复 sticky 逻辑：
  - 问题根因不是单纯层级不够，而是“当前导航速览”和“最近一次反馈或回退”两张卡片处在同一条右侧栏里，却只给第一张卡片单独加了 sticky；这样滚动时两张卡片必然会在同一视觉通道里相遇，结果不是互相遮挡，就是看起来一起滑动
  - `PathPlannerPanel` 现改为让整个 `.planner-extension-side` 侧栏承担 `position: sticky`，并设置 `top`、`max-height` 与 `overflow: auto`
  - 原本单独 sticky 的 `.planner-overview-card` 已恢复为普通卡片，不再自己承担 sticky 逻辑
  - 这样右侧辅助区域会作为一整组内容固定在视口侧边，“当前导航速览”和“最近一次反馈或回退”在同一个 sticky 容器里顺序展示，不再出现两张卡片互相压盖的问题
  - 移动端断点下，右侧辅助栏会恢复为普通文档流，避免在窄屏上出现独立滚动区域
  本次改动修复的是侧栏 sticky 的作用对象与滚动容器划分，不调整首页信息结构，也不影响路径规划、反馈、资源或图谱联动逻辑。
- 变更原因：
  用户实际使用时发现“当前导航速览”在上下滚动过程中既不能稳定固定，又会和下面的“操作摘要”发生遮挡。继续只调 `z-index` 只会把“被遮挡”变成“遮挡别人”，不能解决结构问题。把 sticky 从单张卡片提升到整条右侧辅助栏，才符合这个页面的真实布局逻辑。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅修复 sticky 侧栏卡片的前端样式行为。
- 验证方式：
  执行 `npm run test -- --run` 时 6 个测试文件、16 条测试全部通过；执行 `npm run build` 通过前端构建。页面上确认滚动首页时，右侧辅助栏会作为整体稳定贴在侧边显示，“当前导航速览”不会再单独滑动，也不会和下方“操作摘要”互相遮挡。

### [2026-03-23] 学习者画像页增加“当前学习图谱”，只展示已学习节点结构

- 变更类型：架构改动
- 变更内容：
  为了让学习者画像页不仅展示抽象统计，还能直观演示“当前已经学到哪一段知识结构”，当前新增了画像页专用的已学习节点图谱：
  - 前端新增 `LearnerLearningGraph` 组件，并接入 `LearnerProfileView`
  - 该组件直接复用现有课程主图接口 `GET /api/knowledge/graph`，不新增后端接口
  - 图谱过滤规则显式固定为：学习者画像中 `masteryPercent > 0` 的节点视为“已学习节点”
  - 组件会从课程主图中过滤出这些已学习节点，并仅保留它们之间仍然成立的依赖边，形成一张“当前学习图谱”
  - 图中继续保留最小交互：点击节点后，右侧会显示章节、掌握度、类型、预计学习时间，以及在当前已学习结构中的直接前置 / 后继节点
  - 节点视觉上区分“已掌握”和“学习中”两类状态，方便答辩时直接说明当前画像不仅记录了分数，还能映射到知识结构推进位置
  - `LearnerProfileView` 右侧现改为“当前学习图谱 + 展示建议”的纵向结构，使画像页展示更完整
  - 前端新增 `LearnerLearningGraph.test.js`，验证：
    - 图谱会把课程主图过滤为仅包含已学习节点及其结构边
    - 当所有节点掌握度都为 0% 时，会显示明确空状态说明
  本次改动完全复用现有画像数据与主图接口，不引入新的后端状态、数据库字段或额外规划逻辑。
- 变更原因：
  之前学习者画像页已经能展示掌握度统计、薄弱节点和最近记录，但这些内容更偏“表格式画像”，还不能直观回答“这个学生现在已经学到了课程结构的哪一段”。补上“当前学习图谱”后，画像页就可以直接演示“已学习节点结构”，既更适合答辩，也更容易和首页主知识图谱形成呼应。
- 影响范围：
  `frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/LearnerLearningGraph.test.js`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/views/LearnerProfileView.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅新增一个画像页专用的前端图谱展示组件，并复用现有画像与主图数据。
- 验证方式：
  执行 `npm run test -- --run` 时 7 个测试文件、18 条测试全部通过；执行 `npm run build` 通过前端构建。页面上确认进入学习者画像页后，会出现“当前学习图谱”，且图中只显示掌握度大于 0% 的节点与它们之间的依赖结构。

### [2026-03-23] 学习者画像页“当前学习图谱”升级为全量多层图谱并增加学习状态语义

- 变更类型：架构改动
- 变更内容：
  为了让学习者画像页能够直接演示“学生已经学到哪一层、还有哪些细粒度节点未学、当前节点上下游是什么”，当前把原先仅展示已学习节点的画像图谱升级为全量多层学习图谱：
  - `LearnerLearningGraph` 不再只读取课程主图后做已学习过滤，而是会递归调用现有 `GET /api/knowledge/graph` 接口，自动拉取 `root` 主图、二级细化图和三级细化图，并在前端合并成一张统一图谱
  - 合并过程中保留每个 scope 的原始依赖边，并新增“父节点 -> 子图入口节点”的连接边，用于把上层节点和下钻后的细化结构串起来，便于在画像页一次性展示全量结构
  - 学习状态语义当前固定为：
    - `masteryPercent > 0` 的节点视为“已学习节点”，节点与其对应已学习路径显示为蓝色
    - 未学习节点与未学习路径显示为灰色，但保持可读、可点击
    - 当前选中节点显示为浅粉色
    - 点击节点后，会凸显其上下关联节点与路径；不在当前关联链上的元素只做轻度弱化，避免图谱完全看不清
  - 图谱上方新增颜色图例，明确说明蓝色 / 灰色 / 浅粉色 / 关联高亮的含义，便于答辩演示时直接讲解
  - 右侧详情面板同步升级，除章节、掌握度、类型和预计学习时间外，还会显示图谱层级、所在 scope，以及直接前置 / 后继节点的学习状态
  - 前端测试改为验证：
    - 画像图谱会递归汇总主图、二级细化图和三级细化图
    - 即使学习者当前还没有任何已学节点，也仍然展示完整图谱，而不是退化成空状态
  本次改动仍然完全复用现有知识图谱接口与学习者画像数据，不新增后端接口、数据库字段或算法逻辑，只调整画像页前端图谱装配与表现方式。
- 变更原因：
  原先的“只展示已学习节点结构”虽然能说明学生不是从零开始，但对于当前项目已经有二级 / 三级细化图谱的前提来说，表达力还是不够。老师在答辩时更可能追问“已学到哪个细化节点”“这个节点和上层章节的关系是什么”“没学的部分在全图里位于哪里”。因此需要把画像图谱升级为全量多层图谱，并且直接在颜色与交互上表达学习状态和上下游关系。
- 影响范围：
  `frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/LearnerLearningGraph.test.js`、`frontend/src/views/LearnerProfileView.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅增强学习者画像页中“当前学习图谱”的前端递归装配、着色规则与详情展示。
- 验证方式：
  执行 `npm run test -- --run` 时 7 个测试文件、18 条测试全部通过；执行 `npm run build` 通过前端构建。页面上确认学习者画像页的“当前学习图谱”会同时展示主图、二级和三级节点，已学习节点 / 路径为蓝色，未学习节点 / 路径为灰色，选中节点为浅粉色，并可通过点击节点凸显上下关联结构。

### [2026-03-23] 学习者画像页图谱改回按层浏览，二级 / 三级节点通过双击进入

- 变更类型：架构改动
- 变更内容：
  根据当前交互目标，学习者画像页中的“当前学习图谱”不再直接把二级 / 三级节点全部摊开显示，而是改回与主图一致的逐层浏览方式：
  - 默认仅展示当前 scope 的节点与依赖边，不再把主图、二级图和三级图合并为一张大图
  - 二级 / 三级细化图通过双击上级节点进入，同时在右侧节点详情区保留“进入下一层图谱”按钮，方便答辩演示时显式操作
  - 图谱顶部增加面包屑按钮，可从当前 scope 返回上层 scope
  - 已学习节点 / 路径继续保持蓝色，未学习节点 / 路径继续保持灰色，当前选中节点保持浅粉色；点击节点后仍会凸显当前层内的上下关联结构
  - 图谱统计信息现在按“当前层”计算，而不是按全量合并图计算，避免用户误以为首页会一次性展开所有细化层
  - 前端测试同步改为验证：
    - 初始只展示当前层节点
    - 双击节点可以依次进入二级 / 三级图谱
    - 通过面包屑可以返回上层
    - 即使当前没有已学节点，也仍会显示当前层图谱
  本次调整不改变后端接口、数据库结构和算法，只收敛学习者画像页图谱的展示边界，使其与知识图谱主页面的层级浏览逻辑保持一致。
- 变更原因：
  直接把全部层级节点摊开虽然信息更全，但会削弱“细化节点是逐层进入”的交互语义，也容易让学习者画像页的图谱变得过满。改回按层浏览后，界面逻辑与首页主图统一，用户更容易理解“一级节点 -> 双击进入二级 -> 再双击进入三级”的层级结构。
- 影响范围：
  `frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/LearnerLearningGraph.test.js`、`frontend/src/views/LearnerProfileView.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不新增接口，不修改数据库结构，不改变路径规划、资源推荐、反馈调整与推荐解释算法；仅调整学习者画像页图谱的前端展示方式与交互路径。

### [2026-03-23] 学习者画像页图谱改为全宽展示，节点详情移至下方

- 变更类型：界面布局调整
- 变更内容：
  为了让学习者画像页中的 Cytoscape 图谱拥有更大的可视空间，当前将“当前学习图谱”模块的布局从左右分栏改为上下分区：
  - 图谱画布改为独占当前学习图谱卡片的整条横向空间，不再被右侧节点详情面板压缩
  - 节点详情区整体下移到图谱下方，点击节点后在下方展示掌握度、层级、范围、说明和前置 / 后继节点信息
  - 详情区的基础信息卡改为三列布局，前置 / 后继关系改为双栏并排，充分利用新的横向空间
  - 图谱最小高度同步上调，使当前学习图谱在桌面端更适合展示较宽的层级结构
  本次改动只调整学习者画像页中当前学习图谱模块的前端排版，不改变图谱数据、颜色语义、双击下钻逻辑或节点高亮规则。
- 变更原因：
  之前的左右分栏会明显压缩图谱横向宽度，尤其在当前图谱节点数稍多时，布局更容易显得拥挤。将图谱改为全宽后，节点分布会更自然，也更符合“图谱为主、详情为辅”的展示优先级。
- 影响范围：
  `frontend/src/components/LearnerLearningGraph.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅调整学习者画像页图谱模块的前端布局。

### [2026-03-23] 首页图谱与学习者画像图谱禁用节点拖拽

- 变更类型：界面交互修正
- 变更内容：
  为了避免用户在滚动或拖动画布时误把节点位置拖乱，当前统一在两个 Cytoscape 图谱组件中禁用了节点拖拽：
  - 首页知识图谱 `KnowledgeGraphPreview` 启用 `autoungrabify: true`
  - 学习者画像页图谱 `LearnerLearningGraph` 也启用 `autoungrabify: true`
  - 同时关闭 Cytoscape 默认滚轮缩放，改为只有按住 `Ctrl / Cmd` 再滚轮时才执行图谱缩放；普通滚动会继续交给页面本身，避免触控板或鼠标滚轮误缩放
  - 节点的点击选中、双击进入下一层、上下文高亮、路径高亮和图谱布局计算逻辑保持不变
  本次改动只修正图谱的前端交互方式，不调整节点数据、布局算法或细化图谱导航逻辑。
- 变更原因：
  在当前项目中，节点位置是由布局算法自动计算的，用户手动拖拽节点并没有实际业务价值，反而会在页面滚动、触控板滑动或误触时破坏图谱的稳定展示。同时默认滚轮缩放也容易和页面滚动冲突，因此需要把图谱交互收敛为“节点固定 + `Ctrl / Cmd` 滚轮缩放”。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/LearnerLearningGraph.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅禁用图谱节点的前端拖拽行为。

### [2026-03-23] 图谱节点改为按掌握度粉色梯度显示，并在节点内展示进度

- 变更类型：界面语义增强
- 变更内容：
  为了让知识图谱中的学习进度表达更直接，当前把首页图谱和学习者画像图谱的节点视觉统一为掌握度渐变语义：
  - 首页 `KnowledgeGraphPreview` 与画像页 `LearnerLearningGraph` 的节点底色，都改为根据掌握度从浅粉到较深粉的渐变，不再使用原先的蓝 / 灰或按节点类型分色作为主视觉编码
  - 每个节点内部都会在知识点名称下方增加第二行文案，例如 `进度 40%`，直接显示当前学习进度
  - 首页主图的掌握度来源改为直接消费学习者画像 `masteryByCode`；在细化 scope 中，则使用当前范围内的细化掌握度编辑状态，使节点颜色和进度能随本范围掌握度变化实时更新
  - 选中节点和上下文高亮改为主要依赖描边与阴影强调，避免覆盖节点本身的掌握度颜色
  - 首页节点详情区同步补充掌握度字段，保证图谱内进度展示和右侧详情信息一致
  本次改动只调整图谱展示语义，不修改学习者画像、反馈更新、路径规划或数据库结构。
- 变更原因：
  之前节点颜色更多在表达“节点类型”或“是否已学”，而用户当前更关心“学到了多少”。把颜色语义切换为掌握度梯度，并在节点内部直接标出百分比后，图谱会更像真正的学习进度图，而不是单纯的结构图。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/views/HomeView.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅增强首页图谱和学习者画像图谱的前端掌握度可视化表达。

### [2026-03-23] 进一步增强图谱节点掌握度层次与节点内进度可读性

- 变更类型：界面语义优化
- 变更内容：
  在已有“粉色梯度 + 节点内显示进度”的基础上，当前继续优化两张图谱的掌握度可读性：
  - 掌握度颜色从原先的线性浅粉到深粉，改为更明显的三段层次：
    - `0% - 35%`：更浅的粉色
    - `36% - 70%`：中等粉色
    - `71% - 100%`：较深但不过暗的粉色
  - 节点边框颜色同步按三段层次加深，使不同掌握度节点在同一屏内更容易被快速区分
  - 节点内部第二行文案从“进度 xx%”统一调整为“掌握 xx%”，让语义更直接、更贴近学习者画像的表达方式
  - 学习者画像页图例同步明确标出三段掌握度区间，便于答辩时口头解释颜色含义
  本次优化不改节点结构和交互逻辑，只强化图谱中掌握度的视觉层次与读数效率。
- 变更原因：
  之前虽然已经切到粉色渐变，但相邻掌握度节点之间的差异还不够明显，第二行“进度 xx%”文案也偏中性。进一步拉开颜色层次并改成“掌握 xx%”后，用户能更快从图上看出“谁更弱、谁更熟”，更符合学习导航场景。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/LearnerLearningGraph.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅优化图谱节点掌握度的前端视觉层次与文案表达。

### [2026-03-23] 选中节点描边改为更干净的深玫瑰色

- 变更类型：界面视觉微调
- 变更内容：
  为了让“当前选中节点”和“节点本身的掌握度粉色梯度”区分得更明确，当前对两张图谱的选中态做了小幅视觉收口：
  - 首页图谱与学习者画像图谱中的 `node.is-selected` 边框统一调整为更深、更干净的玫瑰色
  - 选中节点阴影同步收成同色系的柔和玫瑰粉，不再使用偏亮的浅粉光晕
  - 学习者画像页图例中“当前选中节点”的颜色也同步到新的选中态色值
  本次改动只微调选中节点样式，不影响节点掌握度颜色、点击选中、双击下钻或图谱联动逻辑。
- 变更原因：
  在节点已经采用掌握度粉色梯度后，如果选中态仍然使用接近节点底色的浅粉描边，会让“这是被选中了”与“这是掌握度较高”两种语义不够分明。改成更深一点的玫瑰色后，选中态会更清楚，但又不会跳出整套粉色系统。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/LearnerLearningGraph.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅调整图谱中当前选中节点的前端样式。

### [2026-03-23] 首页“图谱预览”改为全宽画布，下置节点详情

- 变更类型：界面布局调整
- 变更内容：
  为了让首页知识图谱拥有更完整的横向展示空间，当前将首页“图谱预览”模块从左右分栏改为“上图下详情”布局：
  - 图谱画布改为占满“图谱预览”卡片的整条横向空间，不再被右侧节点详情面板压缩
  - 节点详情区整体下移到图谱下方，点击节点后在下方展示章节、类型、难度、掌握度、说明、直接前置 / 后继依赖，以及同步到路径规划 / 进入细化图谱等操作
  - 节点详情的基础信息区调整为三列布局，操作区与依赖区改成双栏排布，更适合宽屏下置展示
  - 图谱最小高度同步提升，使首页主图在桌面端展示更舒展
  本次改动只调整首页图谱模块的前端排版，不改变图谱数据、双击下钻、路径联动、节点高亮和局部路径规划逻辑。
- 变更原因：
  之前首页主图和节点详情采用左右分栏，图谱横向空间被明显压缩，在节点较多或需要同时观察多条依赖关系时不够舒展。改成全宽画布后，首页主图会更符合“图谱为主、详情为辅”的展示优先级，也和学习者画像页当前图谱布局保持一致。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅调整首页“图谱预览”模块的前端布局。

### [2026-03-23] 前端图谱统一收敛到独立“学习图谱”页面

- 变更类型：架构改动
- 变更内容：
  为了把“课程结构展示”和“路径规划操作”拆开，当前前端导航调整为“首页 / 学习图谱 / 学习者画像”三页结构：
  - 首页不再嵌入知识图谱，职责收敛为学习目标设置、路径生成、学习反馈、反馈回退和路径对比展示
  - 原先首页图谱与学习者画像页图谱的展示职责，统一收拢到新的 `LearningGraphView` 页面，并以 `LearnerLearningGraph` 组件的样式与交互为基准
  - 学习图谱页保留多层下钻、面包屑、掌握度粉色梯度、上下关联节点和节点详情展示，并新增“设为当前学习目标”按钮
  - 当用户在学习图谱页点击“设为当前学习目标”后，前端会跳回首页，并通过路由参数把对应节点作为路径规划目标，触发首页自动重算学习路径
  - 首页路径项中的图谱联动按钮同步改为跳转到独立“学习图谱”页并定位对应节点，避免图谱移出首页后留下失效交互
  - 学习者画像页同步去除图谱区域，职责收敛为掌握度统计、薄弱知识点、最近学习记录与最近资源行为展示
- 变更原因：
  当前图谱已经支持多层 scope 下钻和较多节点展示，继续把图谱与路径规划堆叠在同一首页中，会导致页面职责混杂、视觉重点分散，也不利于答辩时按模块演示。将图谱独立成导航页后，可以按“学习图谱 -> 首页路径规划 -> 学习者画像”的顺序展示，更清晰也更稳定。
- 影响范围：
  `frontend/src/router/index.js`、`frontend/src/components/PageLayout.vue`、`frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/views/LearningGraphView.vue`、`frontend/src/views/LearnerProfileView.vue`、前端相关测试文件、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构与路径规划算法；仅调整前端页面职责与页面间跳转方式。
- 验证方式：
  访问前端后确认顶部导航已新增“学习图谱”；在学习图谱页点击“设为当前学习目标”后，应自动跳回首页并以该节点生成路径；在首页路径项点击“前往学习图谱”后，应跳到图谱页并定位对应节点；学习者画像页不再展示图谱模块。

### [2026-03-23] 顶部导航补入“推荐资源”入口，便于跨页回访资源页

- 变更类型：架构改动
- 变更内容：
  为了让单节点资源页不再只是“从路径卡片临时跳入”的孤立页面，当前把“推荐资源”正式纳入顶部导航：
  - `PageLayout` 顶部导航扩展为“首页 / 学习图谱 / 推荐资源 / 学习者画像”
  - 当系统已经存在最近一次资源推荐快照时，导航中的“推荐资源”会作为可点击入口，默认跳到当前资源页，或跳到最近缓存资源中的第一个节点
  - 当用户尚未生成任何资源快照时，导航中的“推荐资源”会以禁用态显示，避免跳转到空页面
  - 这样用户在首页、学习图谱页和学习者画像页之间切换时，都可以随时回到最近一次资源页继续查看，不必重新滚回路径卡片寻找入口
- 变更原因：
  单节点资源页已经成为稳定功能页，但如果它不在导航体系内，用户一旦切走到其他主页面，就只能重新从路径项或薄弱节点按钮进入，回访成本偏高。把资源页补入顶部导航后，四个主页面的关系会更完整，也更适合答辩时多页面来回切换展示。
- 影响范围：
  `frontend/src/components/PageLayout.vue`、`frontend/src/components/PageLayout.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅调整前端导航结构与资源页回访入口。
- 验证方式：
  启动前端后，在尚未生成资源快照时确认顶部“推荐资源”为禁用态；任意页面点击一次“查看推荐资源”进入资源页后，再切回其他页面，确认顶部导航中的“推荐资源”已可点击，并能回到最近一次资源页。

### [2026-03-23] 新增一键恢复演示初始状态能力

- 变更类型：架构改动
- 变更内容：
  为了提高答辩和演示时的可重复性，当前新增了“恢复演示初始状态”能力：
  - 后端新增 `POST /api/demo/reset`，用于把默认演示学习者恢复到项目 seed 中定义的初始掌握度基线
  - 重置过程在单事务中完成，会清空该学习者的 `learning_feedback_records`、`learning_resource_view_records` 与当前 `learner_mastery`，再写回默认 14 个顶层知识点的基线掌握度
  - 当前版本由于数据库中没有单独保存“初始掌握度快照”，因此演示重置使用与 seed 数据一致的固定基线映射；目标是保证毕设演示稳定可复现，而不是做通用化多课程回档系统
  - 首页健康检查卡片下方新增“恢复演示初始状态”按钮；点击后前端会调用重置接口、清空资源页快照、移除当前首页目标路由参数，并通过重挂载路径规划面板清空旧的本轮结果
  - 重置成功后会在首页显示本次清空的反馈记录数与资源行为数，便于演示时说明系统已恢复到初始基线
- 变更原因：
  当前系统已经具备反馈、资源行为、路径调整与回退能力，但连续演示几轮后，学习者画像会逐渐偏离最初示例，导致答辩时难以快速回到标准起点。加入一键恢复后，可以在不重建整库的前提下，快速把默认演示学习者恢复到统一的初始状态，明显提升演示效率。
- 影响范围：
  `backend/controllers/DemoController.*`、`backend/services/DemoResetService.*`、`backend/repositories/DemoResetRepository.*`、`backend/tests/LearnerProfileFeedbackServiceTest.cc`、`frontend/src/api/demo.js`、`frontend/src/views/HomeView.vue`、`frontend/src/views/HomeView.test.js`、`frontend/src/components/HealthStatusCard.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不改变路径规划算法、反馈调整算法与资源排序逻辑；新增一个演示辅助接口，并在前端新增一个首页按钮。数据库结构不变，但重置逻辑会覆盖默认演示学习者的当前掌握度与行为记录。
- 验证方式：
  后端执行构建与测试后，调用 `POST /api/demo/reset`，确认返回画像中的 `feedbackRecordCount` 与 `resourceViewRecordCount` 归零，且 `queue`、`graph-basic`、`topological-sort` 等掌握度恢复到 seed 基线。前端执行测试与构建后，在首页点击“恢复演示初始状态”，确认路径规划面板与资源快照同步清空。

### [2026-03-24] 学习图谱页新增跨页面状态记忆

- 变更类型：架构改动
- 变更内容：
  为了让“学习图谱”页在页面切换后仍然保持用户刚才的浏览上下文，当前前端新增了图谱状态记忆能力：
  - `frontend/src/stores/navigationStore.js` 新增 `learningGraphViewState`，并通过 `sessionStorage` 持久化以下字段：
    - `currentScopeCode`：当前浏览到的图谱层级
    - `selectedNodeCode`：当前选中的节点
    - `lastFocusedNodeCode`：最近一次显式定位的节点
    - `lastFocusedScopeCode`：最近一次显式定位节点所在的 scope
  - `frontend/src/components/LearnerLearningGraph.vue` 在以下时机自动写回状态：
    - 首次加载图谱后
    - 切换面包屑或双击进入下一层后
    - 点击节点切换当前选中项后
  - 当用户从首页“前往学习图谱”带着 `focus` 参数跳入图谱页时，系统会优先消费这次显式定位请求；如果该 `focus` 正好就是最近一次定位过的细化节点，则会自动恢复到对应细化 scope，否则回到主图根层定位目标节点
  - 当用户通过顶部导航或其他页面再次回到“学习图谱”页且没有新的 `focus` 参数时，系统会恢复到上次离开时所在的 scope 与选中节点，而不是每次都回到根层重新开始
  - 首页“恢复演示初始状态”现在会同时清空资源快照和图谱浏览状态，避免演示重置后仍停留在旧的深层 scope 中
- 变更原因：
  之前图谱页虽然已经支持三级下钻和节点定位，但一旦切到首页、资源页或学习者画像页，再回到图谱时就会从头开始，答辩演示时需要重复下钻，效率较低。加入状态记忆后，图谱页终于具备“可往返切页、不中断讲解”的稳定体验，更适合作为毕设演示中的结构展示页。
- 影响范围：
  `frontend/src/stores/navigationStore.js`、`frontend/src/stores/navigationStore.test.js`、`frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/LearnerLearningGraph.test.js`、`frontend/src/views/HomeView.vue`、`frontend/src/views/HomeView.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、SQLite 数据库结构与路径规划算法；仅增强前端学习图谱页的状态保持与演示重置后的前端上下文清理逻辑。
- 验证方式：
  前端执行 `npm run test -- --run` 与 `npm run build`，确认通过；手动验证时，在学习图谱页下钻到二级或三级节点后切换到首页或资源页，再回到“学习图谱”页，应恢复到上次 scope 与选中节点；点击首页“恢复演示初始状态”后，再进入图谱页应回到根层默认状态。

### [2026-03-24] 首页路径规划新增“学习路径导出”能力

- 变更类型：架构改动
- 变更内容：
  为了让系统输出结果更容易用于答辩展示和论文附录，当前在首页路径规划结果区新增了“导出本次学习路径”能力：
  - `frontend/src/components/PathPlannerPanel.vue` 在生成路径后新增导出按钮，位置紧跟在路径摘要下方
  - 导出格式采用纯文本 `.txt`，优先保证实现简单、结果稳定、可直接附到论文或答辩演示材料中
  - 导出内容覆盖：
    - 目标节点与目标编码
    - 时间预算
    - 规划摘要（本轮安排、下一轮建议、已掌握数、耗时判断）
    - 本轮推荐学习节点及其推荐理由摘要
    - 下一轮建议节点及其理由摘要
    - 对应资源建议摘要，包括节点资源数、优先资源与前两条代表性资源
  - 导出逻辑被拆到 `frontend/src/utils/learningPathExport.js`，避免把文本拼装细节塞进组件内部；同时方便后续继续扩成 Markdown / 打印页
  - 当浏览器环境不支持导出时，前端会在路径规划卡片中显示明确错误提示，而不是静默失败
- 变更原因：
  当前系统主链路已经可以形成完整学习路径，但如果没有导出能力，系统结果只能停留在页面截图层面，不利于论文附录整理，也不利于答辩时展示“系统可输出可复现结果”。增加文本导出后，系统输出会更像一个可交付的毕设成果，而不是只能现场演示的页面。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/utils/learningPathExport.js`、`frontend/src/utils/learningPathExport.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、SQLite 数据库和路径规划算法；仅增强首页路径规划结果的前端导出能力。
- 验证方式：
  前端执行 `npm run test -- --run` 与 `npm run build`，确认通过；手动验证时，在首页生成路径后点击“导出本次学习路径”，浏览器应下载一个 `.txt` 文件，内容应包含目标、时间预算、本轮路径、理由摘要和资源建议。

### [2026-03-24] 路径规划结果新增“路径总说明”

- 变更类型：架构改动
- 变更内容：
  为了让老师在答辩时更快理解“整条路径为什么这样排”，当前给路径规划结果新增了总说明字段与展示区：
  - 后端 `backend/services/PathPlanningService.cc` 新增 `overallExplanation` 聚合逻辑，直接基于已有规划结果与 `reasonTrace` 汇总整条路径的总体说明
  - 聚合结果包含：
    - `summary`：一段完整自然语言说明
    - `labels`：例如“前置依赖链路”“掌握度待提升”“本轮优先安排”“时间预算受限”
    - `bullets`：将目标、前置依赖、待补强节点、本轮安排、下一轮建议与预算判断拆成多条简洁说明
  - 该字段会自动进入主路径规划与细化 scope 路径规划的响应，不需要前端额外猜测或自行拼接
  - 前端 `frontend/src/components/PathPlannerPanel.vue` 在路径结果摘要下方新增“本次路径为什么这样安排”卡片，优先展示整条路径的逻辑，再向下展开到单节点理由
  - 学习路径导出文本也同步纳入这段总说明，保证答辩材料与页面展示一致
- 变更原因：
  之前系统已经能解释“单个节点为什么被推荐”，但老师更常问的是“为什么整条路径这样排”。如果没有总说明，答辩时需要靠人工把多个节点理由拼起来，叙述成本高。新增 `overallExplanation` 后，系统可以先给出一段整体验证性很强的解释，再展示每个节点的细节。
- 影响范围：
  `backend/services/PathPlanningService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/utils/learningPathExport.js`、`frontend/src/utils/learningPathExport.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响 SQLite 数据库结构；不改变路径规划排序算法，只是在返回结果中增加一个严格基于现有规划依据的聚合解释字段。
- 验证方式：
  后端执行 `cmake --build . -j$(nproc)` 与 `ctest --output-on-failure`，确认通过；前端执行 `npm run test -- --run` 与 `npm run build`，确认通过；手动验证时，在首页生成路径后，应先看到“路径总说明”卡片，再看到具体节点列表，导出的文本文件中也应包含这段总说明。

### [2026-03-24] 学习反馈新增快捷录入预设

- 变更类型：系统改动
- 变更内容：
  为了缩短答辩演示时的反馈录入操作，当前在两处学习反馈入口都新增了统一的快捷预设按钮：
  - 首页 `frontend/src/components/PathPlannerPanel.vue`
  - 细化图谱局部反馈 `frontend/src/components/KnowledgeGraphPreview.vue`

  预设逻辑抽离到 `frontend/src/utils/feedbackQuickPreset.js`，统一提供三种一键录入方案：
  - `已完成并掌握`：自动设置为 `completed`，并把自评掌握度拉到至少 `90%`
  - `看过但不熟`：自动设置为 `partial`，并给出 `55% - 75%` 的中间掌握度区间
  - `未完成`：自动设置为 `blocked`，并给出 `15% - 35%` 的低掌握度区间

  这三种预设只负责快速填充默认反馈值，不会锁死表单；用户点完后仍然可以继续手动修改下拉框和滑块。
  同时新增 `frontend/src/utils/feedbackQuickPreset.test.js`，并补充首页路径规划组件测试，保证快捷录入会同步更新“完成情况 + 掌握度”两个字段。
- 变更原因：
  当前系统已经具备学习反馈、路径重算与回退能力，但答辩演示时如果每个节点都手动选状态、再拖滑块，会让流程显得拖沓。加入统一的快捷录入后，可以更快演示“反馈 -> 重算 -> 对比变化”的主链路，同时保持操作语义和后端调整规则一致。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/utils/feedbackQuickPreset.js`、`frontend/src/utils/feedbackQuickPreset.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、SQLite 数据库结构与路径规划算法；仅增强前端反馈录入交互，并让首页与细化图谱两套反馈表单的默认录入语义保持一致。
- 验证方式：
  前端执行 `npm run test -- --run src/utils/feedbackQuickPreset.test.js src/components/PathPlannerPanel.test.js src/components/KnowledgeGraphPreview.test.js`、`npm run test -- --run` 与 `npm run build`，确认通过；手动验证时，在首页或细化图谱中点击快捷预设按钮后，应立即同步改变对应节点的“完成情况”和“学习后掌握度”，并且仍可继续手动微调后再提交反馈。

### [2026-03-24] 细化节点资源覆盖补齐，并统一资源页来源提示与异常态

- 变更类型：架构改动
- 变更内容：
  为了把“资源覆盖度系统补齐、页面文案继续压缩、异常态统一”这几项收口工作做实，当前对资源推荐链路和前端资源展示做了三类调整：
  - 后端 `LearningResourceService` 现在会读取 `knowledge_graph_details.json` 中的细化 scope 目录，把细化节点与其上层主节点关系装配出来
  - 资源选择策略明确改为三层兜底：
    - 节点自身直配资源优先
    - 若存在 `focusNodeCode` 精确命中的焦点资源，则作为针对该细化节点的补充
    - 若仍不足，则沿细化 scope 的父链向上继承最近上层节点的通用资源
  - 为了让上层资源真正可继承，`learning_resources.json` 中部分原先过度绑定三级焦点节点的资源被重新归类为上层节点通用资源，例如：
    - `queue` 下的 Hello Algo 队列文章不再强绑 `queue-circular`
    - `binary-tree-traversal` 下的遍历文章不再强绑 `binary-tree-traversal-level-order`
    - `topological-sort` 下的部分资源不再强绑 `topological-sort-algorithm`
  - 资源响应新增 `resourceCoverageType`、`inheritedFromCode`、`inheritedFromLabel` 字段，前端在单节点资源页和图谱局部资源区都会显式标出“来自上层：某节点”，避免把继承资源误当成该细化节点的专属资源
  - 首页、学习图谱、推荐资源、学习者画像四个主页面的顶部文案进一步压缩；资源快照缺失、当前节点不在最近快照内、后端未启动、画像读取失败等提示文案同步改为更直接的可回退说明
- 变更原因：
  之前顶层节点资源覆盖已经完整，但二级 / 三级细化节点仍存在“并非每个节点都单独有资源”的现实问题。如果继续手工为所有细化节点各写一套资源，会明显走向过度工程化，也不适合本科毕设周期。改为“直配 + 焦点 + 上层继承”的轻量兜底策略后，既能保证展示覆盖度，又能在答辩时清楚说明资源来源与设计取舍。
- 影响范围：
  `backend/services/LearningResourceService.cc`、`backend/config/learning_resources.json`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/views/LearningGraphView.vue`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/components/PageLayout.vue`、`frontend/src/components/HealthStatusCard.vue`、`frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/PathPlannerPanel.vue`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不改变路径规划排序算法与 SQLite 表结构；资源返回对象增加了来源标记字段，资源装配逻辑从“只看当前节点配置”扩展为“当前节点 + 焦点资源 + 上层继承资源”的组合策略。
- 验证方式：
  后端先执行 `cmake -S .. -B .` 刷新测试注册，再执行 `cmake --build . -j$(nproc)` 与 `ctest --output-on-failure`，确认 `69/69` 通过，并覆盖新增的 `LearningResourceServiceFallsBackToAncestorResourcesForDetailNode`；前端执行 `npm run test -- --run`，确认 `36/36` 通过，再执行 `npm run build` 通过构建；额外用本地自检脚本确认当前已落地的细化节点资源覆盖达到 `409/409`。

### [2026-03-24] 首页删除“扩展区：反馈与路径变化”模块

- 变更类型：系统改动
- 变更内容：
  为了让首页重新聚焦“目标设置 -> 路径生成 -> 结果导出 -> 资源跳转”这条主链，当前把 `PathPlannerPanel` 中整块“扩展区：反馈与路径变化”展示区域移除：
  - 删除首页路径规划面板下方的扩展区引导卡片
  - 删除首页中的学习反馈表单、操作摘要、辅助速览和路径变化对比展示
  - 首页 `HomeView` 不再向 `PathPlannerPanel` 传入反馈记录数与资源行为数这类扩展区专用参数
  - 首页顶部说明同步收紧为“目标设置、路径生成与结果导出”，不再声明首页承担反馈与路径调整展示职责
  - `README.md` 同步删除与首页扩展区相关的演示步骤和手动测试项
- 变更原因：
  当前首页已经承担路径规划主操作，再把反馈、回退、路径变化等长内容放在下方，会重新拉长页面并削弱主线展示。删除后，首页会更像一个简洁的规划工作台，资源查看和画像回看也更容易按导航页切换演示。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/views/HomeView.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端反馈接口、数据库结构与路径规划算法；本次仅移除首页上的相关展示与交互区域。
- 验证方式：
  前端执行 `npm run test -- --run src/components/PathPlannerPanel.test.js src/views/HomeView.test.js` 与 `npm run build`，确认通过；页面上确认首页不再出现“反馈与路径变化”“当前导航速览”“提交反馈并调整路径”等模块与文案。

### [2026-03-24] 撤回首页扩展区删除，恢复“反馈与路径变化”模块

- 变更类型：系统改动
- 变更内容：
  根据用户澄清，本轮撤回上一条“删除首页扩展区”的改动，首页恢复到删除前的结构：
  - `PathPlannerPanel` 重新显示“扩展区：反馈与路径变化”引导卡片
  - 首页重新显示学习反馈表单、快捷录入、反馈回退、辅助速览、操作摘要和路径变化对比
  - `HomeView` 重新向 `PathPlannerPanel` 传入反馈记录数与资源行为数，并恢复 `feedback-saved` 联动
  - `README.md` 恢复与首页反馈、路径变化、演示顺序、手动测试有关的说明
- 变更原因：
  上一轮对需求理解有偏差。用户要处理的是首页中某个“小问题”，并不是要彻底移除首页的反馈与路径变化能力，因此需要立即恢复到原有交互状态。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/views/HomeView.test.js`、`README.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构与路径规划算法；本次是对前端上一轮误删展示的恢复。
- 验证方式：
  前端执行 `npm run test -- --run src/components/PathPlannerPanel.test.js src/views/HomeView.test.js` 与 `npm run build`，确认通过；页面上确认首页重新出现“反馈与路径变化”“当前导航速览”“提交反馈并调整路径”等模块与按钮。

### [2026-03-24] 首页扩展区删除“辅助摘要 / 当前导航速览”卡片

- 变更类型：系统改动
- 变更内容：
  在保留首页“反馈与路径变化”主体功能的前提下，当前从右侧扩展栏中移除了“辅助摘要 / 当前导航速览”卡片：
  - 删除“当前导航速览”标题、统计网格、概览提示文案和变化标签列表
  - 保留右侧“最近一次反馈或回退”操作摘要卡片
  - 保留下方整宽“路径变化”对比区
  - `PathPlannerPanel` 不再接收仅供该卡片使用的 `resourceViewRecordCount`
  - `HomeView` 与对应前端测试同步移除这一路传参
- 变更原因：
  用户明确要求删除的是“辅助摘要”卡片本身，而不是再次调整整个首页扩展区。删除后，右侧区域会更简洁，页面重点继续放在反馈操作和路径变化本身。
- 影响范围：
  `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`、`frontend/src/components/PathPlannerPanel.test.js`、`frontend/src/views/HomeView.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅减少首页扩展区中的一张辅助展示卡片。
- 验证方式：
  前端执行 `npm run test -- --run src/components/PathPlannerPanel.test.js src/views/HomeView.test.js` 与 `npm run build`，确认通过；页面上确认首页不再出现“辅助摘要”“当前导航速览”卡片，但学习反馈、操作摘要和路径变化区仍正常存在。

### [2026-03-24] 新增“细化学习”页面与二级路径规划工作区

- 变更类型：系统改动
- 变更内容：
  为了把“一级路径规划之后，单个主节点内部还可以继续导航”这条毕设主线显式落地，当前前端新增了统一的“细化学习”页面，并把它接入首页路径规划和全站导航：
  - `navigationStore` 新增 `detailLearningContext`，用于跨页面保存最近一次首页路径中所有“可细化”的一级节点、当前选中的二级 `scopeCode`、来源目标和学习者编码
  - 首页 `PathPlannerPanel` 在“本轮推荐学习”节点存在 `detailScopeCode` 时，会显示“细化学习”按钮
  - 点击该按钮后，系统会把当前首页路径中所有可细化节点写入 `detailLearningContext`，再跳转到统一的细化学习页面
  - `PageLayout` 导航栏新增“细化学习”入口；若当前没有细化上下文，则显示为禁用态
  - 前端新增 `DetailLearningView.vue`，用于展示当前路径中的可细化分支，并在这些一级节点之间切换
  - 前端新增 `DetailLearningWorkspace.vue`，结构固定为：
    - 二级静态图谱
    - 二级路径规划表单
    - 二级路径结果
    - 二级局部反馈与路径变化
  - 细化图谱限定为“只读、不可拖动、不可点击、仅展示当前二级范围”，不再继续下钻三级节点
  - 二级路径规划完全复用现有后端 detail scope 接口：
    - `GET /api/knowledge/graph?scopeCode=...`
    - `POST /api/path/detail/generate`
    - `POST /api/path/detail/adjust`
  - 本次没有新增后端接口，也没有修改数据库结构
- 变更原因：
  用户希望保留首页一级路径规划的主职责不动，同时给“本轮推荐学习”中的一级节点增加显式的二级学习入口，让答辩时可以清楚展示“主路径 -> 子路径”的层级导航逻辑。
- 影响范围：
  `frontend/src/stores/navigationStore.js`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/PageLayout.vue`、`frontend/src/router/index.js`、`frontend/src/views/HomeView.vue`、`frontend/src/views/DetailLearningView.vue`、`frontend/src/components/DetailLearningWorkspace.vue`
- 是否影响既有接口/数据库/算法：
  不影响数据库和后端算法；不改既有后端接口，只扩展前端状态组织、导航编排和页面结构。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/stores/navigationStore.test.js src/components/PageLayout.test.js src/components/PathPlannerPanel.test.js src/views/HomeView.test.js src/views/DetailLearningView.test.js`，确认通过；随后执行前端全量测试 `npm run test -- --run` 与生产构建 `npm run build`，确认通过；页面上验证首页可对有 `detailScopeCode` 的本轮节点显示“细化学习”按钮，点击后能进入新页面并切换当前路径中的二级分支。

### [2026-03-24] 细化学习图谱改为“未学习灰色、已学习按进度着色”

- 变更类型：系统改动
- 变更内容：
  调整 `DetailLearningWorkspace` 中二级细化图谱的展示规则，使其更贴近正式学习图谱的阅读方式：
  - 继续展示当前二级范围内的全部细化节点与依赖边，不再只强调路径节点
  - 未学习节点改为灰色背景与灰色描边，文字保持可读
  - 未学习路径改为灰色连线
  - 已学习 / 学习中的节点继续按掌握度使用浅粉到深粉的进度色
  - 当前局部路径的“本轮学习 / 下一轮建议 / 当前目标”描边高亮仍保留
  - 图例同步新增“灰色：未学习节点 / 路径”，便于演示时直接说明颜色语义
- 变更原因：
  用户希望细化学习页中的二级图谱能像正式学习图谱一样，一眼看出“哪些子节点还没学、哪些已经开始学习”，而不是所有节点都落在粉色系里导致未学习状态不够明显。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页的图谱可视语义。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过；页面上验证二级图谱中的未学习节点与未学习路径呈灰色，已学习节点按掌握度保持进度色。

### [2026-03-24] 细化学习图谱语义收紧为“学习状态图 + 当前目标”

- 变更类型：系统改动
- 变更内容：
  在上一轮把未学习节点改为灰色后，进一步收紧了细化学习页图谱的视觉语义：
  - 图谱不再同时叠加“本轮学习 / 下一轮建议 / 已掌握”整条局部路径的描边状态
  - 图谱主语义改为与正式学习图谱一致：节点与边主要表达“已学习 / 未学习”状态
  - 仅保留“当前规划目标”红色描边，用来帮助用户把下方局部路径规划表单与图中目标节点对应起来
  - 局部路径结果继续完整保留在图谱下方的列表、反馈与路径变化区，不再让图上路径高亮覆盖掉学习状态本身
  - 图例同步删除“本轮学习 / 下一轮建议”这类路径描边说明，只保留未学习、掌握度梯度和当前规划目标
- 变更原因：
  问题的根因不是“节点没加载全”，而是细化学习页图谱此前混合了两层语义：
  1. 学习状态图（未学习 / 已学习）
  2. 局部路径结果图（本轮 / 下轮 / 目标）
  页面进入时又会自动生成局部路径，导致路径描边把学习状态的灰色 / 粉色层次盖住，看起来不像正式学习图谱。现在改为“图谱优先表达学习状态，路径结果放在下方列表展示”，视觉逻辑更稳定。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页图谱的展示优先级和高亮规则。
- 验证方式：
  前端执行 `npm run build`，确认通过；页面上验证细化学习图谱中的未学习节点 / 路径为灰色，已学习节点按掌握度着色，且图上只保留当前规划目标的红色描边。

### [2026-03-24] 细化学习图谱开放画布拖动与缩放

- 变更类型：系统改动
- 变更内容：
  将细化学习页中的二级图谱从完全静态视图调整为“节点固定、画布可拖动、滚轮可缩放”的交互方式：
  - 保持节点位置由布局算法决定，节点本身仍不可拖拽
  - 开启 Cytoscape 画布平移能力，便于查看较大的二级细化图谱
  - 开启滚轮缩放，并设置缩放上下限与较缓的缩放灵敏度
  - 页面说明文案同步改为“支持拖动画布与滚轮缩放查看细节”
- 变更原因：
  用户反馈细化学习页当前图谱即使逻辑已调整，仍然不便观察细节；与其继续在固定画布内压样式，不如直接开放画布拖动与缩放，让用户能够主动查看局部区域。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页图谱交互方式。
- 验证方式：
  前端执行 `npm run build`，确认通过；页面上验证细化学习图谱支持拖动画布与滚轮缩放，节点本身仍保持固定不被拖动。

### [2026-03-24] 细化学习图谱首屏自动收拢到整图视口

- 变更类型：系统改动
- 变更内容：
  细化学习页中的二级图谱在创建 Cytoscape 实例后，不再只依赖其隐式初始视口，而是新增一轮显式的视口稳定流程：
  - 等待图谱容器完成首帧布局稳定
  - 先以 `preset` 方式完成实例初始化，避免在容器尺寸未稳定时提前计算正式布局
  - 主动调用 `resize()`
  - 再重新执行当前 scope 的正式布局
  - 布局结束后调用 `fit()` 把当前 scope 的整张图重新收拢到可视区域
  图谱的数据装配、scope 传递、节点与边的过滤逻辑均保持不变。
- 变更原因：
  经排查，`linear-list` 二级 scope 的后端配置本身完整，包含 9 个节点和 11 条依赖边；首页进入细化学习时使用的 `scopeCode` 传递链路也没有证据显示被错误改写。
  真正问题出在细化学习页切换 loading 状态并挂载图谱容器时，正式布局过早执行，导致多个节点在不稳定容器尺寸下重叠到局部区域，视觉上看起来像“只剩一个节点”；单纯 `fit()` 视口不足以解决这个重叠问题，因此需要把正式布局延后到容器稳定后重新执行。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅修正细化学习页图谱首屏呈现稳定性。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习图谱取消节点自动锁定，恢复布局铺开能力

- 变更类型：系统改动
- 变更内容：
  细化学习页 `DetailLearningWorkspace` 的 Cytoscape 配置取消 `autolock: true`，保留 `autoungrabify: true`。
  这样节点仍然不会被用户拖拽，但布局算法可以正常计算并铺开节点坐标，不会再把多个节点锁在初始位置上。
- 变更原因：
  继续排查后确认，细化学习页“看起来只剩一个节点”的更直接根因，是节点在创建时被自动锁定，布局算法无法把二级图谱中的多个节点从默认位置展开，视觉上表现为多个节点重叠在同一点。
  这也解释了为什么其他图谱页正常，而细化学习页异常：其他图谱组件没有使用 `autolock: true`。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅修正细化学习页图谱的前端布局行为。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习图谱收紧为“固定展示 + 点击选目标”

- 变更类型：系统改动
- 变更内容：
  细化学习页图谱的交互进一步收紧为静态展示模式：
  - 关闭画布拖拽
  - 关闭滚轮缩放
  - 保留节点点击
  - 点击节点后，会把该节点同步为下方细化路径规划表单中的“当前范围目标节点”
  同时同步更新图谱说明文案，明确当前图谱仅用于阅读结构和选择学习目标。
- 变更原因：
  用户确认当前细化学习图谱已经能完整显示后，希望交互更稳定，避免误触拖动和缩放影响演示；当前页面真正需要保留的交互只有“点击节点选择本轮细化学习目标”。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页图谱的人机交互方式。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习默认规划目标改为当前图谱最上游节点

- 变更类型：系统改动
- 变更内容：
  `DetailLearningWorkspace` 的默认目标节点判定逻辑从“优先选择最末端节点”调整为“优先选择当前细化图谱中入度为 0 的最上游节点”。
  当图谱中存在多个最上游节点时，继续按 `displayOrder` 和节点标签排序后取最靠前的一个；若图谱不存在明确的最上游节点，则退回到排序后的第一个节点。
- 变更原因：
  用户要求细化图谱进入页面后，默认规划目标应当对应图谱最上边的节点，而不是最下边的终点节点。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页默认目标的前端初始化规则。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习图谱卡片收敛到主学习图谱同款 UI

- 变更类型：系统改动
- 变更内容：
  将 `DetailLearningWorkspace` 上半部分的图谱卡片改为与主学习图谱一致的组织方式与视觉语言：
  - 增加当前范围条与面包屑样式
  - 增加同款统计卡片布局
  - 图例改为主学习图谱同类的色块样式，并增加“当前选中节点 / 上下关联节点”说明
  - 图谱下方新增与主学习图谱一致的节点详情面板
  - 新增直接前置节点 / 直接后继节点关系区
  - 图谱高亮规则改为与主学习图谱一致：当前选中节点强调、上下关联节点与边增强、其它节点变暗
  - 保留细化学习特有的下半部分路径规划、学习反馈与路径变化功能
- 变更原因：
  用户要求将细化学习的 UI 与 style 调整为主学习图谱同一套视觉和结构，减少页面之间的割裂感，同时保留细化学习独有的路径规划能力。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页上半部分图谱卡片的结构、样式与前端高亮语义。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习图谱颜色与描边进一步对齐主学习图谱

- 变更类型：系统改动
- 变更内容：
  细化学习页图谱进一步收敛到主学习图谱的颜色规则：
  - 节点颜色改为浅粉 / 中粉 / 深粉三段式掌握度渐变，不再单独使用灰色未学习底色
  - 图例文案同步改为“浅粉：0% - 35% / 中粉：36% - 70% / 深粉：71% - 100%”
  - 连线默认颜色改为主学习图谱同款粉色系
  - 节点描边统一改为虚线，包括默认边框、当前选中节点描边和上下关联高亮描边
- 变更原因：
  用户要求细化图谱直接使用主学习图谱中的浅粉 / 中粉 / 深粉配色，并将描边统一改为虚线，以减少两个图谱页面之间的视觉差异。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页图谱的前端视觉语义。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习下半部分卡片配色收敛到主学习图谱风格

- 变更类型：系统改动
- 变更内容：
  将细化学习页下半部分的路径规划、学习反馈、操作摘要与路径变化卡片，从原先偏绿的面板语义收敛到主学习图谱偏冷灰蓝的视觉方向：
  - 表单下拉框背景改为冷灰蓝底
  - 路径卡片、反馈卡片、路径变化卡片改为灰蓝面板底色与边框
  - 路径项与反馈项改为与主学习图谱关系卡相近的灰蓝列表项样式
  - 主操作按钮改为与主学习图谱主操作一致的粉色渐变
  - 次级按钮与快捷预设按钮改为偏灰蓝 / 浅蓝语义
- 变更原因：
  在细化学习页上半部分图谱卡片已与主学习图谱统一后，继续将下半部分功能卡片的配色一起收紧，减少整页上下之间的风格割裂。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅调整细化学习页下半部分功能卡片的前端样式。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 清理废弃图谱组件与未使用模板文件

- 变更类型：系统改动
- 变更内容：
  基于当前实际运行链路完成一次低风险代码清理：
  - 删除已退出运行态与测试链路的旧首页图谱组件 `frontend/src/components/KnowledgeGraphPreview.vue`
  - 删除仅服务于该旧组件的测试文件 `frontend/src/components/KnowledgeGraphPreview.test.js`
  - 删除未被任何运行代码引用的本地示例图数据 `frontend/src/graph/demoGraph.js`
  - 删除后端未被启动脚本、构建流程和文档引用的 Drogon 初始化遗留模板 `backend/config/config.yaml` 与 `backend/models/model.json`
  - 清理 `frontend/src/stores/navigationStore.js` 中未再使用的旧状态字段 `selectedCourse`、`recommendedPath`、`graphFilter`
  - 修正 `frontend/src/views/LearnerProfileView.vue` 中重复的 `.hero-note` 样式选择器
  当前图谱主链路以 `frontend/src/components/LearnerLearningGraph.vue` 与 `frontend/src/components/DetailLearningWorkspace.vue` 为准，不再保留旧图谱实现。
- 变更原因：
  项目经过多轮功能迭代后，旧首页图谱组件和 Drogon 默认模板文件已经不再参与实际运行，但继续保留会增加目录噪音和维护误导；同时 `navigationStore` 中仍残留早期状态字段，容易模糊当前真正生效的跨页面上下文。
- 影响范围：
  `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/KnowledgeGraphPreview.test.js`、`frontend/src/graph/demoGraph.js`、`frontend/src/stores/navigationStore.js`、`frontend/src/views/LearnerProfileView.vue`、`backend/config/config.yaml`、`backend/models/model.json`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响既有前后端接口、数据库结构和路径规划算法；本次仅清理未使用结构、旧实现和重复样式。
- 验证方式：
  前端执行 `npm run test -- --run` 与 `npm run build`，后端执行 `cmake -S .. -B .`、`cmake --build . -j$(nproc)`、`ctest --output-on-failure`，均通过。

### [2026-03-24] 页面级公共面板样式收敛

- 变更类型：系统改动
- 变更内容：
  将多个独立页面中重复出现的页面级面板样式收敛为全局工具类：
  - 在 `frontend/src/style.css` 中新增 `surface-panel`，统一承接页面卡片的底色、边框、圆角、阴影、`contain` 与 `content-visibility` 能力
  - 新增 `page-section-label`，统一承接页面头部小标签的字重、字号和字距
  - 新增 `page-error-panel`，统一承接页面级错误卡片的底色与文字颜色
  - `LearnerProfileView`、`LearningGraphView`、`DetailLearningView`、`ResourceRecommendationView` 改为组合使用上述工具类
  - 各页面局部保留自身特有的布局、说明文案、按钮和资源列表样式，不做业务逻辑调整
- 变更原因：
  当前多个独立页面反复复制相同的面板基类和标签样式，继续保留会增加维护成本，也不利于后续做更细的页面审查；本次收敛只处理证据明确且不会改变视觉结果的页面级公共样式。
- 影响范围：
  `frontend/src/style.css`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/views/LearningGraphView.vue`、`frontend/src/views/DetailLearningView.vue`、`frontend/src/views/ResourceRecommendationView.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和算法；仅调整前端页面级公共样式的组织方式。
- 验证方式：
  前端执行 `npm run test -- --run` 与 `npm run build`，均通过。

### [2026-03-24] 细化学习图谱删除重复目标设置按钮

- 变更类型：系统改动
- 变更内容：
  删除 `frontend/src/components/DetailLearningWorkspace.vue` 中节点详情区的“设为当前细化学习目标”按钮，并同步移除对应的无用样式定义。
  当前细化图谱仍保留“点击图谱节点后自动同步为下方细化路径规划目标”的主交互，不影响选择目标、生成细化路径和后续反馈流程。
- 变更原因：
  节点详情按钮与图谱点击选目标承担的是同一职责，继续并存会形成重复入口，增加页面认知负担。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；仅删除细化学习页中一个重复的前端交互入口。
- 验证方式：
  前端执行定向测试 `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js` 与生产构建 `npm run build`，确认通过。

### [2026-03-24] 细化学习分支状态记忆与细化路径结果导出

- 变更类型：系统改动
- 变更内容：
  为细化学习页补齐“按二级分支记住上次工作区状态”的前端上下文能力：
  - `frontend/src/stores/navigationStore.js` 新增 `detailLearningViewStates`，按 `scopeCode` 持久化当前细化分支的工作区状态
  - 持久化内容包括目标节点、时间预算、掌握度临时调整、反馈草稿、局部路径结果、局部反馈摘要、路径前后对比等
  - `frontend/src/components/DetailLearningWorkspace.vue` 在进入或切换细化分支时，会优先恢复对应 `scopeCode` 的局部状态；若该分支已有历史局部路径结果，则直接恢复，不再重复自动规划
  - 细化学习页新增“导出当前细化路径”能力，复用 `frontend/src/utils/learningPathExport.js`，并补充 `scopeLabel`、`documentTitle`、`filePrefix` 等导出元信息，导出文件可明确区分主学习路径与细化学习路径
  - 为减少图谱测试受 jsdom 图形环境影响，新增 `frontend/src/graph/loadCytoscape.js` 作为细化学习页的 Cytoscape 加载入口，并在 `frontend/src/test/setup.js` 补充最小 canvas stub，用于保证图谱相关前端测试稳定
- 变更原因：
  当前系统已经具备首页路径规划到细化学习的完整链路，但如果用户在多个二级分支之间来回切换，每次都重新开始会影响演示连续性；同时细化路径缺少独立导出结果，不利于论文附录、答辩展示和局部学习计划留存。
- 影响范围：
  `frontend/src/stores/navigationStore.js`、`frontend/src/components/DetailLearningWorkspace.vue`、`frontend/src/utils/learningPathExport.js`、`frontend/src/stores/navigationStore.test.js`、`frontend/src/utils/learningPathExport.test.js`、`frontend/src/components/DetailLearningWorkspace.test.js`、`frontend/src/views/DetailLearningView.test.js`、`frontend/src/graph/loadCytoscape.js`、`frontend/src/test/setup.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；本次仅增强前端细化学习上下文记忆、细化路径导出能力与相关测试支撑。
- 验证方式：
  前端执行 `npm run test -- --run src/stores/navigationStore.test.js src/utils/learningPathExport.test.js src/components/DetailLearningWorkspace.test.js src/views/DetailLearningView.test.js`、`npm run test -- --run` 与 `npm run build`，均通过。

### [2026-03-24] 细化学习掌握度持久化与父节点进度聚合

- 变更类型：系统改动
- 变更内容：
  为细化学习补齐“可持久化的细化节点掌握度 + 上级节点进度聚合 + 图谱刷新链路”：
  - 在 `database/init/001_schema.sql` 新增 `learner_detail_mastery` 与 `detail_learning_feedback_records`，用于存储细化节点掌握度与细化反馈审计记录
  - 新增 `backend/repositories/DetailLearningRepository.*` 负责细化节点掌握度和细化反馈记录的 SQLite 读写
  - 新增 `backend/services/DetailScopeCatalogService.*`，把细化图谱 JSON 目录读取从 `KnowledgeGraphService` 中抽出，供图谱构建、画像聚合与细化反馈共用
  - 新增 `backend/services/DetailLearningProgressService.*`，用于构建图谱用掌握度映射，并按直属细化节点的 `estimatedMinutes * (0.7 + 0.3 * difficultyLevel)` 权重聚合父节点进度
  - `PathPlanningService::buildAdjustedDetailPathPayload` 现在会在细化反馈成功后持久化当前 scope 的细化节点掌握度、写入细化反馈记录，并把聚合后的父节点进度回写到 `learner_mastery`
  - `LearnerProfileService` 新增 `graphMasteryByCode` 与 `detailMasteryByCode`，让前端图谱既能读到课程主节点掌握度，也能读到细化节点掌握度；其中根级二级细化节点在尚未持久化前会默认继承上级节点掌握度作为展示基线
  - 前端 `LearnerLearningGraph` 改为优先消费 `graphMasteryByCode`；`DetailLearningWorkspace` 在细化反馈成功后向上触发 `profile-updated`，`DetailLearningView` 再重新拉取学习者画像，从而使当前细化图谱、学习图谱页和首页后续路径规划共享同一份最新数据
  - `DemoResetRepository` 同步纳入细化掌握度与细化反馈表清理，确保“恢复演示初始状态”仍然完整可用
- 变更原因：
  原先细化学习反馈只在局部页面内存中生效，离开页面后细化节点掌握度不会保留，主学习图谱中的一级节点也不会反映二级细化学习结果，导致“细化学习”和“课程主图谱”两套进度脱节。
- 影响范围：
  `database/init/001_schema.sql`、`backend/repositories/DetailLearningRepository.*`、`backend/services/DetailScopeCatalogService.*`、`backend/services/DetailLearningProgressService.*`、`backend/services/KnowledgeGraphService.cc`、`backend/services/LearnerProfileService.cc`、`backend/services/PathPlanningService.cc`、`backend/repositories/DemoResetRepository.cc`、`frontend/src/components/LearnerLearningGraph.vue`、`frontend/src/components/DetailLearningWorkspace.vue`、`frontend/src/views/DetailLearningView.vue`、相关前后端测试、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  影响数据库结构与学习者画像响应字段，但保持现有主路径规划、主反馈和细化路径规划接口入口不变；细化反馈接口新增了“写回画像数据源”的副作用，并在返回中增加细化反馈已持久化标记。
- 验证方式：
  后端执行 `cmake -S .. -B .`、`cmake --build . -j$(nproc)`、`ctest --output-on-failure`，69 个测试全部通过；前端执行 `npm run test -- --run` 与 `npm run build`，12 个测试文件、44 个测试全部通过。

### [2026-03-24] 细化学习状态恢复改为“最新画像基线 + 局部覆盖值”

- 变更类型：系统改动
- 变更内容：
  调整细化学习页的分支状态记忆策略：
  - `navigationStore` 的细化学习分支状态新增 `masteryOverridePercentByCode`
  - `DetailLearningWorkspace` 不再把旧会话中的整份 `masteryPercentByCode` 直接作为恢复结果
  - 页面恢复时先读取当前学习者画像里的 `graphMasteryByCode` 作为细化学习基线，再叠加当前分支里用户真正改动过的局部覆盖值
  - 旧版本仅保存 `masteryPercentByCode` 的会话状态，只有在当前画像基线完全不可用时才作为兜底使用
  这样主学习图谱中的一级节点掌握度变化会反哺对应细化学习页，而用户在细化学习中主动调过的掌握度仍可记忆。
- 变更原因：
  之前细化学习页会把旧的整包掌握度快照原样恢复，导致一旦某个分支在较早会话里保存过 0 进度，就会压住后续从主学习图谱和学习者画像传来的新掌握度，形成“主图谱有进度、细化学习仍显示 0”的错觉。
- 影响范围：
  `frontend/src/components/DetailLearningWorkspace.vue`、`frontend/src/stores/navigationStore.js`、`frontend/src/components/DetailLearningWorkspace.test.js`、`frontend/src/stores/navigationStore.test.js`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构和路径规划算法；本次仅调整前端细化学习页的 session 级状态恢复逻辑。
- 验证方式：
  前端执行 `npm run test -- --run src/components/DetailLearningWorkspace.test.js src/stores/navigationStore.test.js` 与 `npm run build`，确认通过；新增测试覆盖“旧快照不覆盖新画像”和“局部覆盖值仍能恢复”两类场景。

### [2026-03-24] 细化学习复用统一资源页

- 变更类型：系统改动
- 变更内容：
  资源页继续保持单一入口，不新增“细化资源页”路由，而是让现有 `resource-recommendation` 页面同时支持两种上下文：
  - `main`：来自首页路径规划的一级节点资源
  - `detail`：来自细化学习页的二级节点资源

  本次实现包含：
  1. `navigationStore` 的资源上下文新增 `contextMode`、`sourcePage`、`scopeCode`、`scopeLabel`、`parentNodeCode`、`parentNodeLabel` 等字段；
  2. 首页路径规划显式以 `main` 模式写入资源上下文；
  3. 细化学习页在“本轮推荐学习 / 下一轮建议”条目中新增“查看推荐资源”按钮，并把当前一级节点、细化范围和细化资源列表写入统一资源上下文；
  4. 资源页根据 `contextMode` 自动切换文案与返回逻辑：细化模式下返回“细化学习”，并显示所属一级节点与当前细化范围；
  5. 细化模式下的资源行为记录继续复用现有后端资源行为接口，但行为归档落在所属一级知识点上，同时通过 `scopeCode` 标记当前细化范围。
- 变更原因：
  细化节点下一步需要补资源，但如果单独再拆一个“细化资源页”，页面会继续变碎，不利于答辩演示；如果直接塞回细化学习页，页面又会重新变长。复用现有资源页可以同时保持页面数量稳定和资源展示清晰。
- 影响范围：
  `frontend/src/stores/navigationStore.js`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/DetailLearningWorkspace.vue`、`frontend/src/views/ResourceRecommendationView.vue` 及相关前端测试。
- 是否影响既有接口/数据库/算法：
  不改动后端路径规划接口、数据库结构和资源推荐算法；仅增强前端资源上下文，并在细化模式下复用现有资源行为记录接口。
- 验证方式：
  前端执行 `npm run test -- --run src/components/DetailLearningWorkspace.test.js src/views/ResourceRecommendationView.test.js src/components/PathPlannerPanel.test.js` 与 `npm run build`，确认通过。

### [2026-03-25] 推荐资源页升级为“一级页面 / 二级页面”双层导航

- 变更类型：系统改动
- 变更内容：
  在保留现有 `/resource-recommendation/:code` 单一路由的前提下，扩展资源页为“双层导航”模式：
  1. `frontend/src/stores/navigationStore.js` 的资源上下文从单份快照升级为三部分：
     - 当前活跃资源快照
     - `main` 主路径资源快照
     - `detail` 细化学习资源快照
  2. `frontend/src/views/ResourceRecommendationView.vue` 新增“一级页面 / 二级页面”切换卡片：
     - 一级页面展示课程主线路径中的一级资源节点导航
     - 二级页面展示细化学习当前一级节点下的二级节点资源导航
  3. `frontend/src/components/PathPlannerPanel.vue` 打开资源页时显式带 `level=main`
  4. `frontend/src/components/DetailLearningWorkspace.vue` 打开资源页时显式带 `level=detail`，且细化资源快照只清理 `detail` 上下文，不再误清主路径资源
  5. `frontend/src/views/LearnerProfileView.vue` 的薄弱点资源入口统一写入一级资源上下文
  6. `frontend/src/components/PageLayout.vue` 的顶部“推荐资源”导航会携带当前层级查询参数，保证返回资源页时仍落在正确层级
  7. 补充并更新相关前端测试，覆盖双上下文持久化、资源页层级切换、导航栏层级链接与细化资源入口路由参数
- 变更原因：
  原有资源页一次只能保存一个上下文，用户从细化学习进入资源页时会覆盖掉首页主路径资源，导致无法在同一页面里来回切换“一级资源”和“二级资源”。用户希望资源页保留单页面形态，但增加层级导航能力，因此需要改为双快照并存。
- 影响范围：
  `frontend/src/stores/navigationStore.js`、`frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/components/PathPlannerPanel.vue`、`frontend/src/components/DetailLearningWorkspace.vue`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/components/PageLayout.vue` 及相关前端测试、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构与路径规划算法；仅调整前端资源上下文持久化结构、资源页路由查询参数和资源页展示逻辑。
- 验证方式：
  前端执行 `npm run test -- --run src/stores/navigationStore.test.js src/views/ResourceRecommendationView.test.js src/components/PageLayout.test.js src/components/DetailLearningWorkspace.test.js src/views/LearnerProfileView.test.js` 与 `npm run build`，均通过。

### [2026-03-25] 资源推荐增加“同层关联补充 + 多样性排序”

- 变更类型：系统改动
- 变更内容：
  为提升一级与二级资源页的丰富度，后端资源推荐链路新增两项轻量规则：
  1. 对于细化学习节点，若节点本身资源不足，除了原有“上层继承”外，还会从同一细化范围内按显示顺序挑选相邻节点的焦点资源作为“关联节点资源”补充；
  2. 资源排序从单次静态分值排序调整为“基础优先级 + 行为降权 + 多样性重排”的确定性策略。重排会对重复的 `source`、`type`、`recommendedPhase`、`focusNodeCode`、`resourceCoverageType` 施加轻量递减，使第 2 到第 N 条资源更容易覆盖不同来源和不同学习阶段。

  同时，前端资源页把资源来源标签细分为：
  - `来自上层：xxx`
  - `来自关联节点：xxx`

  并同步调整覆盖说明文案，避免把关联补充误写成上层继承。
- 变更原因：
  当前项目已经补充了较多一级/二级资源，但用户体感仍会觉得“每次都是固定那几个”，问题主要不在于资源数量不足，而在于细化节点兜底来源单一、排序过于静态。需要在不引入随机推荐或复杂模型的前提下，让推荐结果更丰富且仍然可解释。
- 影响范围：
  `backend/services/LearningResourceService.cc`、`backend/tests/PathPlanningServiceTest.cc`、`frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/views/ResourceRecommendationView.test.js`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不改动 HTTP 接口与数据库结构；属于资源推荐策略层的系统逻辑增强。现有返回结构仅新增更细的 `resourceCoverageType` 使用方式以及排序相关字段，不破坏既有页面链路。
- 验证方式：
  后端执行 `cmake -S .. -B .`、`cmake --build . -j$(nproc)`、`ctest -R 'LearningResourceService|PathPlanningService' --output-on-failure` 均通过；前端执行 `npm run test -- --run src/views/ResourceRecommendationView.test.js` 与 `npm run build` 均通过。全量 `ctest --output-on-failure` 仅剩一个既有 SQLite 线程配置失败，与本次资源推荐改动无直接关系。

### [2026-03-25] 一级节点直连资源池补齐

- 变更类型：系统改动
- 变更内容：
  在不调整后端推荐算法和前端展示结构的前提下，继续补齐一级节点的“直连资源池”，本次重点覆盖一级页重复感最强的 8 个节点：
  - `ds-intro`
  - `algorithm-analysis`
  - `kmp`
  - `tree-basic`
  - `binary-tree-traversal`
  - `huffman-tree`
  - `graph-basic`
  - `topological-sort`

  新增资源均采用 `focusNodeCode` 为空的直连形式，使它们直接参与一级页面推荐，而不是仅作为二级/三级细化节点的焦点资源存在。
  补齐后，上述节点的直连资源数量已从原先的 1 到 2 条提升到 3 到 4 条，一级页的资源候选池明显变厚。
- 变更原因：
  虽然上一轮已经通过“同层关联补充 + 多样性排序”缓解了资源列表前几项过度重复的问题，但一级页面本身的直连资源仍然偏少，导致某些节点即使排序更合理，也仍然容易在少量固定资源之间来回切换。继续补直连资源，是更可控、更适合毕设演示的数据层修正方式。
- 影响范围：
  `backend/config/learning_resources.json`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构与路径规划算法；本次仅增强一级节点资源配置数据，属于资源推荐输入数据层面的系统补齐。
- 验证方式：
  执行 `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"` 验证 JSON 语法；
  另执行统计脚本确认上述 8 个一级节点的直连资源数已提升到 `3~4` 条，且直连来源数同步增加。

### [2026-03-25] 一级节点直连资源池第二轮补齐

- 变更类型：系统改动
- 变更内容：
  在不改动现有推荐逻辑、资源页结构和细化节点焦点资源的前提下，继续补齐剩余 6 个一级节点的直连资源池：
  - `linear-list`
  - `sequence-list`
  - `linked-list`
  - `stack`
  - `queue`
  - `string`

  每个节点新增 2 条 `focusNodeCode` 为空的直连资源，来源覆盖 `稀土掘金`、`慕课网`、`腾讯云开发者社区`、`计算机考研杂货铺`、`labuladong`、`浙江大学课程资料`、`网易公开课`、`CSDN` 等。
  补齐后，这 6 个节点的直连资源数和直连来源数都已提升到 `4`，一级资源页在这些主线节点上不再容易反复只落到原先那 1 到 2 条资源。
- 变更原因：
  上一轮补齐之后，树、图、KMP 等主线节点的一级资源丰富度已经明显改善，但线性表、顺序表、链表、栈、队列、串这些基础高频主线仍然偏薄。继续补直连资源，是提升一级页体感最直接、最可控、也最适合毕设演示的做法。
- 影响范围：
  `backend/config/learning_resources.json`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库结构与路径规划算法；本次仅增强一级节点资源配置数据，属于资源推荐输入层面的第二轮系统补齐。
- 验证方式：
  执行 `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"` 验证 JSON 语法；
  另执行统计脚本确认 `linear-list`、`sequence-list`、`linked-list`、`stack`、`queue`、`string` 均已达到 `direct=4 / directSources=4`。

### [2026-03-25] 资源页改为“动态限量推荐展示”

- 变更类型：系统改动
- 变更内容：
  1. 后端资源推荐输出补充节点学习状态字段：`masteryScore`、`masteryPercent`、`difficultyLevel`、`estimatedMinutes`。
  2. 前端推荐资源页由“全量直接展示”调整为“动态限量展示”策略：
     - 默认展示上限由综合因素计算（时间预算、当前状态、本轮/下一轮、掌握度、难度、主线/细化模式）。
     - 当前实现中展示上限被约束在 `2~5` 条，避免单页过长。
     - 其余资源折叠，并提供“展开全部资源 / 收起额外资源”按钮，保证完整资源仍可访问。
  3. 资源页新增“当前智能推荐策略”提示区，明确说明当前展示条数来自“进度 + 能力 + 时间预算”的综合决策。
- 变更原因：
  用户反馈当前推荐资源页一次展示过多，页面过长且干扰主学习任务。项目目标是本科毕设可演示、可讲解，因此需要在不引入复杂模型的前提下，用可解释规则实现“少而精”的资源展示。
- 影响范围：
  `backend/services/LearningResourceService.cc`、`frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/views/ResourceRecommendationView.test.js`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响数据库结构和路径规划算法；后端接口仅增加资源推荐项的补充字段，不破坏既有字段；主要行为变化在前端资源页展示层。
- 验证方式：
  执行 `npm run test -- --run src/views/ResourceRecommendationView.test.js`，7 项测试通过，包含新增“动态限量展示 + 展开/收起”用例。

### [2026-03-26] 时间预算提示细化为“紧凑 / 标准 / 充裕”三档

- 变更类型：系统改动
- 变更内容：
  1. 资源页新增时间预算档位计算：
     - `< 45` 分钟：`紧凑`
     - `45 ~ 89` 分钟：`标准`
     - `>= 90` 分钟：`充裕`
  2. “当前智能推荐策略”说明区文案接入档位提示，并根据档位输出简短建议文案。
  3. 资源详情摘要里的“时间预算”字段改为“分钟数 + 档位”展示，例如：`60 分钟（标准）`。
  4. 前端测试新增预算档位覆盖用例，验证三档文案均能正确显示。
- 变更原因：
  资源页已具备“按进度/能力/时间限量展示”的机制，但时间预算信息仍是纯分钟数，不够直观。增加三档文案后，用户和答辩评审可以更快理解当前推荐强度与展示策略。
- 影响范围：
  `frontend/src/views/ResourceRecommendationView.vue`、`frontend/src/views/ResourceRecommendationView.test.js`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库与路径规划算法；不改变资源排序逻辑，仅增强前端策略解释与摘要展示。
- 验证方式：
  执行 `npm run test -- --run src/views/ResourceRecommendationView.test.js`，8 项测试通过（含新增三档文案测试）。

### [2026-03-26] 资源页预算阈值策略统一抽离（前端）

- 变更类型：系统改动
- 变更内容：
  1. 新增 `frontend/src/utils/resourceRecommendationPolicy.js`，把资源页预算策略统一收敛到单一策略文件，包含：
     - `RESOURCE_TIME_BUDGET_THRESHOLDS`（`45/90` 分钟阈值）
     - `resolveTimeBudgetTier`
     - `buildTimeBudgetTierHint`
     - `resolveTimeBudgetResourceLimitBonus`
  2. `frontend/src/views/ResourceRecommendationView.vue` 删除页面内部硬编码阈值与重复计算逻辑，改为统一调用上述策略函数。
  3. 新增 `frontend/src/utils/resourceRecommendationPolicy.test.js`，覆盖阈值常量、档位映射、展示奖励与提示文案，确保后续改动可回归。
- 变更原因：
  资源页此前已引入预算三档提示与动态限量展示，但阈值和策略函数仍散落在页面内部，后续维护时容易出现“文案阈值”和“展示阈值”不一致。抽离成单一策略文件可降低重复、避免漂移，并提升可测试性。
- 影响范围：
  `frontend/src/utils/resourceRecommendationPolicy.js`、`frontend/src/utils/resourceRecommendationPolicy.test.js`、`frontend/src/views/ResourceRecommendationView.vue`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响后端接口、数据库和路径规划算法；不改变当前资源排序规则，只统一前端预算策略的实现位置与调用方式。
- 验证方式：
  执行 `npm run test -- --run src/views/ResourceRecommendationView.test.js src/utils/resourceRecommendationPolicy.test.js`，2 个测试文件共 12 项通过。

### [2026-03-26] 修复后端已知测试故障（SQLite 多线程配置 + 画像字段稳定性）

- 变更类型：系统改动
- 变更内容：
  1. 调整后端测试数据库初始化流程：在 `backend/tests/TestSupport.cc` 中，完成测试库建库与种子脚本灌入后，显式调用 `sqlite3_shutdown()` 复位 SQLite 全局状态。
  2. 修复画像接口结构稳定性：在 `backend/services/LearnerProfileService.cc` 中，将 `masteryByCode`、`graphMasteryByCode`、`detailMasteryByCode` 显式初始化为 `Json::objectValue`，确保“无细化记录”时返回空对象 `{}`，而不是 `null`。
  3. `backend/tests/test_main.cc` 保持原有 `is_fast = false` 配置不变，仅通过上述初始化修复消除 `SQLITE_CONFIG_MULTITHREAD` 失败。
- 变更原因：
  后端全量测试中存在已知失败 `LearnerProfileServiceBuildsGraphMasteryForDetailNodes`。根因是测试前使用 SQLite C API 建库后未复位全局状态，导致 Drogon 初始化连接时触发 `SQLITE_CONFIG_MULTITHREAD` 报错；同时该用例还暴露了画像返回中空映射字段结构不稳定的问题。
- 影响范围：
  `backend/tests/TestSupport.cc`、`backend/services/LearnerProfileService.cc`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响业务接口语义与算法流程；仅修复测试环境初始化顺序与 JSON 返回结构一致性。画像字段在无数据时从 `null` 变为 `{}`，属于向前端更稳健的兼容增强。
- 验证方式：
  1. 执行 `ctest -R LearnerProfileServiceBuildsGraphMasteryForDetailNodes --output-on-failure`，由失败转为通过。
  2. 执行 `ctest --output-on-failure`，后端 71/71 全部通过。

### [2026-03-26] 新增一键后端冒烟脚本（演示链路快速回归）

- 变更类型：系统改动
- 变更内容：
  1. 新增 `scripts/smoke_demo_flow.sh`，用于一键执行后端核心链路冒烟：
     - `GET /api/health`
     - `POST /api/demo/reset`
     - `POST /api/path/generate`
     - `POST /api/feedback/submit`
     - `POST /api/feedback/rollback`
     - `GET /api/learner/profile`
  2. 脚本默认行为为“独立实例模式”：自动初始化数据库、构建后端、在 `18080` 端口启动临时后端实例并执行完整校验，结束后自动回收进程。
  3. 脚本支持“复用模式”：通过 `SMOKE_REUSE_EXISTING_BACKEND=1` 复用已有后端；未显式传 `API_BASE_URL` 时默认访问 `http://127.0.0.1:8080`。
  4. 更新 `README.md`，补充一键冒烟入口与常用参数说明。
- 变更原因：
  当前主功能链路较长（重置、路径生成、反馈、回退、画像联动），每次手动点查成本高。新增一键冒烟脚本可以把“可答辩、可复现”的验证步骤标准化，降低回归风险并提升演示准备效率。
- 影响范围：
  `scripts/smoke_demo_flow.sh`、`README.md`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响既有接口语义、数据库结构与算法逻辑；仅新增自动化验证脚本与文档说明。
- 验证方式：
  执行 `./scripts/smoke_demo_flow.sh`，脚本输出 `✅ 冒烟通过`，确认 6 段接口链路全部返回 `status=ok` 且关键计数字段校验通过。

### [2026-03-26] 新增一键演示启动/停止脚本（demo_up / demo_down）

- 变更类型：系统改动
- 变更内容：
  1. 新增 `scripts/demo_up.sh`：一键完成演示环境启动，默认流程为
     `init_database -> build backend -> start backend(8080) -> auto demo/reset -> start frontend(5173)`。
  2. 新增 `scripts/demo_down.sh`：按 PID 安全停止由 `demo_up` 启动的前后端进程，并清理运行态文件。
  3. `demo_up` 支持端口与行为参数：
     - `DEMO_BACKEND_PORT`（默认 `8080`）
     - `DEMO_FRONTEND_PORT`（默认 `5173`）
     - `DEMO_AUTO_RESET`（默认 `1`）
     - `DEMO_START_TIMEOUT_SEC`（默认 `35`）
  4. `demo_up` 运行态写入 `backend/build/.demo-runtime/`（`backend.pid`、`frontend.pid`、日志文件），便于 `demo_down` 收敛回收。
  5. 更新 `README.md`，补充 demo 启停脚本入口、参数与运行态说明。
- 变更原因：
  当前项目已具备完整演示链路，但现场演示仍依赖多条命令手动启动。新增 `demo_up / demo_down` 可以把启动、重置和回收标准化，降低演示操作复杂度与失误风险。
- 影响范围：
  `scripts/demo_up.sh`、`scripts/demo_down.sh`、`README.md`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口语义、数据库结构和算法逻辑；仅新增演示环境编排脚本与文档。
- 验证方式：
  1. 执行 `bash -n scripts/demo_up.sh scripts/demo_down.sh` 通过。
  2. 执行 `./scripts/demo_up.sh`，应输出前后端地址、日志路径与停止命令。
  3. 执行 `./scripts/demo_down.sh`，应完成按 PID 停止与清理提示。

### [2026-03-26] 新增一键演示验收脚本（demo_check）

- 变更类型：系统改动
- 变更内容：
  1. 新增 `scripts/demo_check.sh`，把“演示启动 + 服务可用性检查 + 主链路冒烟”串成一个入口。
  2. `demo_check` 默认流程：
     - 先执行 `demo_up`（可通过 `DEMO_CHECK_AUTO_UP=0` 关闭）
     - 检查后端 `api/health` 与前端首页可访问
     - 复用当前后端执行 `smoke_demo_flow`（可通过 `DEMO_CHECK_RUN_SMOKE=0` 关闭）
  3. 支持 `DEMO_CHECK_AUTO_DOWN=1`，验收结束后自动执行 `demo_down` 回收前后端进程。
  4. 更新 `README.md`，补充 `demo_check` 用法与参数说明。
- 变更原因：
  当前已有 `demo_up` 与 `smoke`，但答辩前仍需要多步手动串联。`demo_check` 用于把“能启动 + 能跑主链路”固化为单命令验收，提升演示准备效率与可复现性。
- 影响范围：
  `scripts/demo_check.sh`、`README.md`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响既有接口语义、数据库结构与算法逻辑；仅新增演示验收编排脚本与文档。
- 验证方式：
  执行
  `DEMO_CHECK_BACKEND_PORT=18082 DEMO_CHECK_FRONTEND_PORT=5176 DEMO_CHECK_AUTO_UP=1 DEMO_CHECK_RUN_SMOKE=1 DEMO_CHECK_AUTO_DOWN=1 ./scripts/demo_check.sh`
  应输出 `✅ demo_check 通过`，并在结束时自动回收进程。

### [2026-03-26] 修复 demo_check 复用既有实例导致只读数据库的问题

- 变更类型：系统改动
- 变更内容：
  1. `scripts/demo_check.sh` 默认端口改为独立实例端口：后端 `18080`、前端 `5174`，并新增 `DEMO_CHECK_REUSE_EXISTING`（默认 `0`）。
  2. `scripts/demo_up.sh` 新增 `DEMO_REUSE_EXISTING` 参数（默认 `1`）控制是否复用既有服务；当禁用复用且目标端口已有服务时会明确报错而不是静默复用。
  3. 更新 `README.md`：同步 `demo_check` 与 `demo_up` 的复用策略与默认端口说明。
- 变更原因：
  用户在默认 `demo_check` 流程中复用了现有 `8080/5173` 服务，出现 `POST /api/demo/reset` 写库失败（`attempt to write a readonly database`）。根因是复用的既有后端实例数据库上下文不可写，导致验收阶段失败。
- 影响范围：
  `scripts/demo_check.sh`、`scripts/demo_up.sh`、`README.md`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口语义、数据库结构与算法逻辑；仅调整演示验收脚本默认编排策略与复用开关。
- 验证方式：
  执行 `./scripts/demo_check.sh`（新默认配置）可通过，输出 `✅ demo_check 通过`，且 `demo/reset` 不再触发只读数据库错误。

### [2026-03-26] 新增答辩演示 Runbook 文档（demo-runbook）

- 变更类型：系统改动
- 变更内容：
  1. 新增 `docs/demo-runbook.md`，固化“答辩前准备 -> 主流程演示 -> 兜底命令 -> 收尾回收”的标准化脚本。
  2. Runbook 默认基于 `demo_check` 独立实例流程编排，使用 `18080/5174` 作为稳定演示端口。
  3. 在 `README.md` 的“当前基线”中新增 `docs/demo-runbook.md` 入口，便于快速定位答辩材料。
- 变更原因：
  当前功能链路已完整，下一步重点是“可答辩、可复现、可控演示”。新增 Runbook 可减少现场临场操作与口述偏差，提升演示稳定性。
- 影响范围：
  `docs/demo-runbook.md`、`README.md`、`AI_GUIDE.md`、`AIREAD.md`
- 是否影响既有接口/数据库/算法：
  不影响接口、数据库与算法；仅新增演示流程文档与入口说明。
- 验证方式：
  1. 文档中引用命令与现有脚本一致（`demo_check / demo_down / demo_reset`）。
  2. 在 README 可直接定位到 `docs/demo-runbook.md`。
