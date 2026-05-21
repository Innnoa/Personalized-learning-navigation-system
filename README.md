# Personalized-learning-navigation-system

基于知识点依赖图的个性化学习导航系统，本科毕业设计项目。

## 当前基线

- 架构与系统改动留痕统一记录在 `AIREAD.md`
- 课程范围文档位于 `docs/course-scope.md`
- 开发顺序文档位于 `docs/development-plan.md`
- 答辩演示脚本位于 `docs/demo-runbook.md`
- 固定截图清单位于 `docs/demo-snapshots/README.md`
- 答辩材料收口说明位于 `docs/thesis/defense-material-pack.md`
- 论文模块对应说明位于 `docs/thesis/thesis-module-mapping.md`

## 固定技术栈

- 前端：Vue
- 后端：Drogon（C++）
- 数据库：SQLite
- 核心算法：C++
- 图谱展示：Cytoscape.js

## 当前分层约定

- 前端调用链：`views/components -> api -> 后端接口`
- 后端调用链：`Controller -> Service -> Repository / GraphBuilder / Planner / Explainer`
- 算法层不直接访问数据库，只处理内存中的图结构与路径规划逻辑
- 推荐理由解释必须消费规划阶段输出的依据，不允许凭空生成

## 当前项目骨架

- `frontend/`：Vue + Vite 前端骨架
- `backend/`：Drogon 后端骨架
- `database/`：SQLite 初始化脚本、种子数据和数据库文件目录
- `scripts/`：本地启动和依赖安装脚本

## 当前已实现

- `GET /api/health`：后端健康检查
- `POST /api/auth/login`：统一登录入口，返回当前会话和角色信息
- `POST /api/auth/logout`：清理当前登录会话
- `GET /api/auth/session`：读取当前登录会话，用于前端刷新后恢复身份状态
- `GET /api/knowledge/graph`：从 SQLite 读取“数据结构”课程知识图谱
- `GET /api/learner/profile`：读取默认演示学习者画像、掌握度分布、反馈记录统计和画像图表所需聚合数据
- `GET /api/teacher/courses`：读取教师演示工作台的课程列表
- `GET /api/teacher/courses/:courseCode/overview`：读取教师视角的单课程概览
- `GET /api/admin/users`：读取管理员演示工作台的用户列表
- `GET /api/admin/courses`：读取管理员演示工作台的课程列表
- `GET /api/admin/course-assignments`：读取管理员演示工作台的课程分配概览
- `POST /api/feedback/submit`：保存练习反馈、更新画像掌握度并返回最新画像
- `POST /api/feedback/rollback`：按最近一次反馈批次逐步回退学习记录，恢复上一版掌握度画像
- `POST /api/demo/reset`：一键清空默认演示学习者的反馈记录、资源行为，并恢复初始掌握度
- `POST /api/path/generate`：根据学习目标、掌握度和时间预算生成最小版学习路径
- `POST /api/path/adjust`：根据练习反馈更新掌握度并重新生成当前推荐路径
- `overallExplanation`：为整条路径返回总体说明、标签和要点列表，便于先解释“为什么整体这样安排”
- `path[].explanation`：基于 `PathReasonTrace` 输出推荐理由摘要、标签和说明列表
- `path[].learningResources`：为路径节点附加静态资源建议，当前已覆盖课程主图全部 14 个顶层知识点
- `resourceRecommendations`：按掌握度阶段聚合“本轮推荐学习 / 下一轮建议”的配套资源列表，并限制每个路径节点最多返回 4 条资源
- `learningResources[].recommendedPhase / focusTags / focusNodeLabel / selectionReason`：为资源增加“适合阶段 / 聚焦点 / 对应细化节点 / 选择原因”字段，其中 `selectionReason` 直接消费当前 `LearningPathItem` 的规划证据
- `recentFeedbackItems`：在学习者画像响应中返回最近学习记录，供前端展示反馈历史
- 图谱节点详情面板：点击知识点后可查看章节、难度、说明、直接前置与后继关系
- 图谱多层下钻：当前支持通过 `scopeCode` 加载细化图谱，前端支持双击节点进入下一层并通过面包屑返回上层，当前第三层子图已覆盖 `ds-intro-logical-structure`、`ds-intro-storage-structure`、`algorithm-analysis-time-complexity`、`algorithm-analysis-case-study`、`linear-list-sequential-representation`、`linear-list-linked-representation`、`sequence-list-insert`、`sequence-list-delete`、`sequence-list-merge`、`linked-list-head-insert`、`linked-list-tail-insert`、`linked-list-merge`、`stack-recursion`、`stack-applications`、`queue-circular`、`queue-enqueue`、`queue-applications`、`string-matching-problem`、`string-naive-match`、`kmp-next-definition`、`kmp-main-process`、`tree-basic-binary-storage`、`binary-tree-traversal-recursive`、`binary-tree-traversal-level-order`、`binary-tree-traversal-iterative`、`huffman-tree-construction-process`、`huffman-tree-prefix-code`、`huffman-tree-code-decode`、`graph-basic-adjacency-matrix`、`graph-basic-adjacency-list`、`graph-basic-dfs`、`graph-basic-bfs`、`topological-sort-aoe`、`topological-sort-algorithm`、`topological-sort-critical-path-concept`、`topological-sort-critical-path-solve`
- 已落地细化 scope：`ds-intro / 数据结构基本概念`、`algorithm-analysis / 算法复杂度分析`、`linear-list / 线性表`、`sequence-list / 顺序表`、`linked-list / 链表`、`stack / 栈`、`queue / 队列`、`string / 串`、`kmp / KMP 模式匹配`、`tree-basic / 树与二叉树基础`、`binary-tree-traversal / 二叉树遍历`、`huffman-tree / 哈夫曼树与编码`、`graph-basic / 图的存储与遍历`、`topological-sort / 拓扑排序与关键路径`
- 已落地第三层 scope：`ds-intro-logical-structure / 逻辑结构`、`ds-intro-storage-structure / 存储结构`、`algorithm-analysis-time-complexity / 时间复杂度`、`algorithm-analysis-case-study / 复杂度案例解析`、`linear-list-sequential-representation / 顺序表示（线性表）`、`linear-list-linked-representation / 链式表示（线性表）`、`sequence-list-insert / 插入操作`、`sequence-list-delete / 删除操作`、`sequence-list-merge / 合并操作（顺序表）`、`linked-list-head-insert / 头插法建表`、`linked-list-tail-insert / 尾插法建表`、`linked-list-merge / 有序表合并（链表）`、`stack-recursion / 递归工作栈`、`stack-applications / 典型应用（栈）`、`queue-circular / 循环队列`、`queue-enqueue / 入队操作（队列）`、`queue-applications / 队列典型应用`、`string-matching-problem / 模式匹配问题`、`string-naive-match / 简单模式匹配`、`kmp-next-definition / next 数组含义`、`kmp-main-process / KMP 主过程`、`tree-basic-binary-storage / 二叉树存储结构`、`binary-tree-traversal-recursive / 递归遍历思想`、`binary-tree-traversal-level-order / 层序遍历`、`binary-tree-traversal-iterative / 非递归遍历思想`、`huffman-tree-construction-process / 构造过程`、`huffman-tree-prefix-code / 前缀编码约束`、`huffman-tree-code-decode / 哈夫曼编码与译码`、`graph-basic-adjacency-matrix / 邻接矩阵存储`、`graph-basic-adjacency-list / 邻接表存储`、`graph-basic-dfs / 深度优先遍历`、`graph-basic-bfs / 广度优先遍历`、`topological-sort-aoe / AOE 网`、`topological-sort-algorithm / 拓扑排序算法`、`topological-sort-critical-path-concept / 关键路径概念`、`topological-sort-critical-path-solve / 关键路径求解`
- 推荐理由展开区：路径卡片支持展开 explanation 细项与关键指标
- 路径总说明：首页路径规划结果顶部会先展示“本次路径为什么这样安排”，把目标、前置依赖、掌握度差距、本轮安排和预算限制汇总成一段整体解释
- 学习路径导出：首页路径规划结果支持导出 `.txt`，内容包含目标、时间预算、路径总说明、本轮安排、下一轮建议和资源摘要
- 路径项直达单节点资源页：在“本轮推荐学习 / 下一轮建议”的路径卡片中可直接点击“查看推荐资源”，进入对应节点的单节点资源页查看视频教程、文本教程、选择原因和使用建议
- 高频细化节点资源：当前已为 `queue-circular`、`graph-basic-bfs`、`topological-sort-algorithm`、`kmp-next-definition`、`binary-tree-traversal-level-order`、`topological-sort-critical-path-solve` 追加定向资源标签
- 细化节点资源兜底：二级 / 三级节点会按“节点直配资源 -> 焦点资源 -> 最近上层节点通用资源”的顺序补齐推荐；当前已落地细化节点都能得到可展示的资源建议
- 资源来源显式标注：资源响应会返回 `resourceCoverageType / inheritedFromCode / inheritedFromLabel`，前端会在资源卡片与图谱局部资源区标出“来自上层：某节点”，避免把继承资源伪装成精确直配
- 学习记录展示区：学习者画像卡片支持查看最近练习反馈记录、掌握度变化与记录时间
- 学习者画像图表：学习者画像页支持用图表展示掌握度分布与反馈统计等画像聚合信息
- 独立学习图谱页：顶部导航新增“学习图谱”页面，统一承接课程主图、二级 / 三级细化图、节点详情、掌握度粉色梯度与面包屑下钻展示
- 图谱状态记忆：学习图谱页会在同一浏览器会话内记住上次浏览层级、选中节点和最近一次 focus 节点，便于从首页、资源页、学习者画像页来回切换时保持上下文
- 图谱反向驱动路径：在学习图谱页节点详情区可直接点击“设为当前学习目标”，系统会自动跳回首页并重算对应学习路径
- 路径前后对比区：提交练习反馈后可查看调整前本轮安排、调整后本轮安排和关键变化
- practice-check 练习检验：当前已覆盖一级主图节点与二级细化 scope；每个节点维护至少 10 道题库，并会根据提交前掌握度按难度配比固定抽取 5 题，提交后展示逐题批改报告与掌握度变化
- 练习反馈快捷录入：首页路径规划和细化图谱局部练习反馈都支持“已完成并掌握 / 看过但不熟 / 未完成”三种快捷预设，点击后会同时填入完成状态和掌握度，后续仍可手动微调
- 练习反馈逐步回退：路径规划面板支持按最近一次反馈批次逐步撤销误点或误判，并展示回退前后路径对比
- 演示状态一键恢复：首页健康检查卡片下方提供“恢复演示初始状态”按钮，可一键清空反馈记录、资源行为并恢复默认掌握度，便于重复演示
- 首页路径规划布局：首页主区域当前专注于目标设置、路径结果、练习反馈、回退和路径对比，资源仍从路径项内直接进入单节点资源页
- 路径结果反向定位图谱：在“本轮推荐学习 / 下一轮建议”中点击“前往学习图谱”后，会跳到独立图谱页并定位对应节点，便于继续查看课程结构
- 页面文案与异常态收敛：首页、学习图谱、推荐资源、学习者画像四个主页面的顶部文案已压缩；资源快照缺失、当前节点不在快照内、后端未启动、画像读取失败等情况都会优先给出可回退提示，而不是只显示笼统报错
- 数据库演示数据：1 门课程、14 个核心知识点、15 条依赖边、1 个默认演示学习者、14 条初始掌握度记录
- 前端导航：学生登录后顶部已提供“首页 / 学习图谱 / 推荐资源 / 学习者画像”切换入口；其中“推荐资源”会在存在最近一次资源快照时作为可点击入口出现，便于在多个主页面之间随时回看资源页
- 统一登录入口：登录页当前统一承接学生 / 教师 / 管理员三类演示身份，并提供对应 demo 快捷入口，便于答辩时快速进入不同角色视角
- 角色路由分流：统一登录后会按当前身份进入学生学习系统、教师工作台或管理员工作台，前端路由已按角色拆分主入口
- 教师 / 管理员轻量工作台：当前阶段仅提供只读型概览与演示查看能力，用于展示多角色入口和看板结构，不包含注册流、完整 RBAC 或重型 CRUD 后台

## 本地运行

数据库初始化：

```bash
./scripts/init_database.sh
```

后端：

```bash
./scripts/run_backend.sh
```

前端依赖安装：

```bash
./scripts/install_frontend_deps.sh
```

前端开发服务器：

```bash
./scripts/run_frontend.sh
```

一键后端冒烟（推荐）：

```bash
./scripts/smoke_demo_flow.sh
```

一键演示启动 / 停止：

```bash
./scripts/demo_up.sh
./scripts/demo_down.sh
```

一键演示验收（启动 + 健康检查 + 冒烟 + 可选回收）：

```bash
./scripts/demo_check.sh
./scripts/demo_check_down.sh
```

说明：

- 冒烟脚本默认会在 `18080` 端口启动一个独立后端实例，自动执行：
  `health -> demo/reset -> path/generate -> feedback/submit -> feedback/rollback -> learner/profile`
- 脚本会自动初始化数据库、构建后端、启动临时实例并在结束后自动回收该实例
- 如需复用你已启动的后端，可设置 `SMOKE_REUSE_EXISTING_BACKEND=1`；此时若未额外指定 `API_BASE_URL`，脚本会默认访问 `http://127.0.0.1:8080`
- 常用可调参数：
  - `SMOKE_BACKEND_PORT`：独立实例端口（默认 `18080`）
  - `TARGET_CODE`：冒烟路径目标节点（默认 `topological-sort`）
  - `AVAILABLE_MINUTES`：冒烟路径时间预算（默认 `90`）
  - `LEARNER_CODE`：学习者编码（默认 `demo-learner`）
  - `START_TIMEOUT_SEC`：后端启动等待秒数（默认 `25`）
- `demo_up.sh` 默认会启动后端 `8080` + 前端 `5173`，并在后端就绪后自动执行一次 `demo/reset`（可通过 `DEMO_AUTO_RESET=0` 关闭）
- `demo_up.sh` 会把运行态信息写入 `backend/build/.demo-runtime/`（pid 与日志），`demo_down.sh` 会按 pid 停止并清理
- `demo_up.sh` 常用参数：
  - `DEMO_BACKEND_PORT`：后端端口（默认 `8080`）
  - `DEMO_FRONTEND_PORT`：前端端口（默认 `5173`）
  - `DEMO_AUTO_RESET`：是否自动重置演示状态（默认 `1`）
  - `DEMO_START_TIMEOUT_SEC`：服务启动等待秒数（默认 `35`）
  - `DEMO_REUSE_EXISTING`：是否复用已运行服务（默认 `1`）
- `demo_check.sh` 默认会：
  1. 执行 `demo_up` 启动演示环境
  2. 检查前后端可访问性
  3. 复用当前后端执行 `smoke_demo_flow` 主链路冒烟
- `demo_check_down.sh` 用于回收 `demo_check.sh` 默认使用的演示端口环境，会按端口直接停止 `18080 / 5174`（或你通过环境变量覆盖后的端口）
- `demo_check.sh` 常用参数：
  - `DEMO_CHECK_AUTO_UP`：是否自动执行 `demo_up`（默认 `1`）
  - `DEMO_CHECK_RUN_SMOKE`：是否执行冒烟链路（默认 `1`）
  - `DEMO_CHECK_AUTO_DOWN`：结束后是否自动执行 `demo_down`（默认 `0`）
  - `DEMO_CHECK_REUSE_EXISTING`：是否复用已有服务（默认 `0`，推荐保持）
  - `DEMO_CHECK_BACKEND_PORT` / `DEMO_CHECK_FRONTEND_PORT`：验收使用端口（默认 `18080` / `5174`）
  - `DEMO_CHECK_TARGET_CODE` / `DEMO_CHECK_AVAILABLE_MINUTES`：冒烟目标与预算（默认 `topological-sort` / `90`）
- `demo_check_down.sh` 常用参数：
  - `DEMO_CHECK_BACKEND_PORT` / `DEMO_CHECK_FRONTEND_PORT`：要回收的演示端口（默认 `18080` / `5174`）
  - `DEMO_CHECK_DOWN_TIMEOUT_SEC`：优雅停止等待秒数（默认 `6`）

说明：

- 前端安装脚本会自动探测当前代理、WSL 宿主机代理改写和直连三种方式，并打印当前实际使用的联网模式
- 安装命令默认关闭 `audit` 和 `fund`，减少首次安装时的额外等待
- Vite 开发代理会优先使用 `VITE_DEV_PROXY_TARGET`；未显式指定时会先探测 `http://127.0.0.1:8080`，再探测演示后端 `http://127.0.0.1:18080`，最后回落到 `8080`
- 后端启动前建议先执行一次 `./scripts/init_database.sh`，确保 `database/data/app.db` 已生成

## 推荐演示顺序

如果是答辩或给老师做最短链路演示，建议按下面顺序操作：

1. 先进入统一登录页，直接点击学生 / 教师 / 管理员的 demo 快捷入口，说明当前阶段采用单入口分流，不包含注册流程。
2. 如需展示核心毕设主链路，优先选择学生身份进入学习系统；首页点击“恢复演示初始状态”，确保画像、反馈记录、资源行为和图谱浏览状态都回到统一基线。
3. 进入“学习图谱”页，双击某个可细化节点下钻，点击一个主节点后使用“设为当前学习目标”跳回首页。
4. 在首页查看“本次路径为什么这样安排”，先讲整条路径逻辑，再讲“本轮推荐学习 / 下一轮建议”。
5. 点击“导出本次学习路径”，展示系统可以输出可复用的文本结果。
6. 在练习反馈区直接使用快捷预设按钮提交一轮练习反馈，观察路径前后对比。
7. 如需演示误判修正，再点击“撤销最近一次反馈”，展示系统支持逐步回退。
8. 从路径项进入“推荐资源”页，展示该节点的配套视频、文本教程、选择原因、对应推荐理由，以及需要时出现的“来自上层”继承提示。
9. 如需补充说明多角色扩展方向，可切换到教师或管理员入口，展示当前只读型工作台如何承接角色分流与概览信息，再回到学生主链路继续讲解。
10. 进入“学习者画像”页查看最新学习记录与能力变化，再回到“学习图谱”页，确认图谱仍停留在刚才浏览的层级。

## 手动测试

1. 初始化数据库：

```bash
./scripts/init_database.sh
```

2. 启动后端：

```bash
./scripts/run_backend.sh
```

3. 安装前端依赖：

```bash
./scripts/install_frontend_deps.sh
```

4. 启动前端开发服务器：

```bash
./scripts/run_frontend.sh
```

5. 浏览器打开 `http://localhost:5173`，未登录时应先进入统一登录页；可直接使用学生 / 教师 / 管理员 demo 入口进入对应视图

首次页面可先检查两点：

- 健康检查卡片是否显示后端状态正常
- 顶部导航是否可以在学生身份下的“首页 / 学习图谱 / 推荐资源 / 学习者画像”之间切换
- 未登录访问学生受保护页面时，是否会自动跳转到登录页；使用 `student_demo / teacher_demo / admin_demo`（密码统一为 `demo123`）后，是否会分别进入学生首页、教师工作台和管理员工作台
- 在尚未生成任何资源快照时，“推荐资源”是否显示为禁用态；一旦点击过“查看推荐资源”进入资源页后，顶部导航中的“推荐资源”是否变成可回访入口
- 进入“学习图谱”页面后，是否能看到完整课程图谱；图谱是否默认只展示当前层节点，并支持通过双击可细化节点逐层进入二级 / 三级图谱，同时节点按掌握度粉色梯度着色，并在上方显示颜色图例
- 在学习图谱页下钻到二级或三级节点后，切换到首页、推荐资源页或学习者画像页，再返回“学习图谱”，是否仍保留刚才浏览的层级与选中节点
- 双击“数据结构基本概念”“算法复杂度分析”“线性表”“顺序表”“链表”“栈”“队列”“串”“KMP 模式匹配”“树与二叉树基础”“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”或“拓扑排序与关键路径”节点后，是否能进入对应细化图谱，并在上方面包屑返回课程主图
- 进入“数据结构基本概念”“算法复杂度分析”“线性表”“顺序表”“链表”“栈”“队列”“串”“KMP 模式匹配”“树与二叉树基础”“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”“拓扑排序与关键路径”细化图谱后，继续双击“逻辑结构”“存储结构”“时间复杂度”“复杂度案例解析”“顺序表示”“链式表示”“插入操作”“删除操作”“合并操作”“头插法建表”“尾插法建表”“有序表合并”“典型应用”“循环队列”“入队操作”“队列典型应用”“模式匹配问题”“简单模式匹配”“next 数组含义”“KMP 主过程”“二叉树存储结构”“递归遍历思想”“层序遍历”“非递归遍历思想”“构造过程”“前缀编码约束”“哈夫曼编码与译码”“邻接矩阵存储”“邻接表存储”“深度优先遍历”“广度优先遍历”“AOE 网”“拓扑排序算法”“关键路径概念”“关键路径求解”等节点，确认能继续进入第三层图谱；其中“顺序表 / 链表 / 队列 / 串”新增的入口也都应能分别进入对应第三层，并通过面包屑逐级返回上层
- 在学习图谱页点击图谱节点后，下方是否显示节点详情、直接前置依赖和直接后继节点
- 在学习图谱页点击“设为当前学习目标”后，是否会自动跳回首页，并直接为该节点生成路径
- 路径规划面板是否会基于学习者画像自动生成一版默认学习路径
- 点击“恢复演示初始状态”后，首页是否会清空旧的路径规划内部状态，学习者画像是否恢复到默认掌握度，顶部“推荐资源”是否回到无快照状态
- 在“本轮推荐学习”或“下一轮建议”中点击“前往学习图谱”后，是否会跳到独立图谱页并定位对应节点
- 首页路径生成后，结果顶部是否优先显示“本次路径为什么这样安排”，且说明内容会覆盖目标、依赖链路、掌握度差距和预算判断
- 点击“导出本次学习路径”后，浏览器是否会下载一个 `.txt` 文件，文件中是否包含目标、时间预算、路径总说明、本轮安排和资源摘要
- 路径卡片点击“展开理由”后，是否能看到 explanation 细项和相关度/重要度/时间占比
- 路径规划卡片中的节点是否会直接显示“查看推荐资源”按钮，而不是要求继续下滑到首页下方再进入资源页
- 点击路径项中的“查看推荐资源”后，是否会进入 `/resource-recommendation/:code` 对应单节点资源页，而不是在首页直接展开全部资源
- 在单节点资源页点击“返回首页路径规划”后，首页是否会自动带回当前节点作为学习目标，并重新生成对应路径
- 单节点资源页中，是否能看到该节点对应的视频教程、文本教程、阶段标签、聚焦标签、选择原因和使用建议
- 当路径中出现 `queue`、`graph-basic`、`kmp`、`binary-tree-traversal`、`topological-sort` 等节点时，单节点资源页中是否能看到“对应细化节点：循环队列 / 广度优先遍历 / next 数组含义 / 层序遍历 / 关键路径求解”等定向提示
- 当路径或图谱局部资源区落到未单独直配资源的二级 / 三级节点时，资源卡片是否仍能正常显示，并在需要继承时标出“来自上层：队列 / 图的存储与遍历 / 拓扑排序与关键路径”等来源提示
- 单节点资源页中的资源卡片，是否会显示基于规划依据生成的“选择原因”和“对应推荐理由”标签
- 刷新单节点资源页或直接访问资源页链接时，若资源快照不存在或节点不在最近一次快照中，页面是否会给出明确兜底提示，并引导回首页重新生成路径
- 练习反馈区是否存在“已完成并掌握 / 看过但不熟 / 未完成”三种快捷预设；点击任一按钮后，对应节点的完成状态与掌握度滑块是否会同步更新
- 提交一次练习反馈后，是否能看到“调整前后路径对比”和最近一次调整说明
- 点击“撤销最近一次反馈”后，是否能恢复上一版掌握度，并看到“回退前后路径对比”和最近一次回退说明

如果只想快速验证接口，也可以直接访问：

```bash
curl http://127.0.0.1:8080/api/knowledge/graph
```

```bash
curl http://127.0.0.1:8080/api/learner/profile
```

或执行一个最小路径规划请求：

```bash
curl -X POST http://127.0.0.1:8080/api/path/generate \
  -H 'Content-Type: application/json' \
  -d '{
    "targetCodes": ["topological-sort"],
    "availableMinutes": 120,
    "masteryByCode": {
      "ds-intro": 0.9,
      "linear-list": 0.85,
      "queue": 0.7,
      "graph-basic": 0.2
    }
  }'
```

如果想继续验证“学习后反馈 -> 动态调整路径”，可以在上一次请求的基础上再执行：

```bash
curl -X POST http://127.0.0.1:8080/api/path/adjust \
  -H 'Content-Type: application/json' \
  -d '{
    "targetCodes": ["topological-sort"],
    "availableMinutes": 90,
    "masteryByCode": {
      "ds-intro": 0.9,
      "linear-list": 0.85,
      "queue": 0.2,
      "graph-basic": 0.15,
      "topological-sort": 0.0
    },
    "feedbackItems": [
      {
        "code": "queue",
        "completionStatus": "completed",
        "selfRatedMastery": 0.9
      },
      {
        "code": "graph-basic",
        "completionStatus": "partial",
        "selfRatedMastery": 0.55
      }
    ]
  }'
```

预期现象：

- 返回结果中 `updatedMasteryByCode` 会反映反馈后的掌握度更新
- `feedbackSummary` 会统计“已完成 / 部分完成 / 学习受阻”的节点数量
- `adjustments` 会给出每个反馈节点采用了哪条调整规则
- 若关键前置节点被补齐，`summary.targetReachableWithinBudget` 可能从 `false` 变为 `true`
- `path` 中的每个节点都应包含 `explanation.summary`、`explanation.labels` 和 `explanation.bullets`
- 若路径中包含已配置资源的节点，`path[].learningResources` 与顶层 `resourceRecommendations` 应返回对应资源列表，并包含 `recommendedPhase`、`focusTags`、`focusNodeLabel`、`selectionReason` 等字段
- 若路径中包含没有单独直配资源的细化节点，返回的资源列表也应能够通过上层通用资源补齐，并额外给出 `resourceCoverageType`、`inheritedFromCode` 与 `inheritedFromLabel`

页面侧可额外检查：

- “本轮推荐学习”和“下一轮建议”卡片中，推荐理由应显示为解释摘要，而不是直接拼接底层 `triggerReasons`
- 每个节点应显示如“目标节点 / 前置依赖 / 本轮学习 / 下一轮建议”这类解释标签
- 首页路径项应直接提供资源入口，点击“查看推荐资源”后进入对应单节点资源页；在资源页点击链接后可跳转到外部视频或文本教程

如果想验证“反馈保存 -> 画像更新 -> 路径重新规划”的主流程，可以执行：

```bash
curl -X POST http://127.0.0.1:8080/api/feedback/submit \
  -H 'Content-Type: application/json' \
  -d '{
    "learnerCode": "demo-learner",
    "masteryByCode": {
      "ds-intro": 0.9,
      "linear-list": 0.82,
      "queue": 0.35,
      "graph-basic": 0.15,
      "topological-sort": 0.0
    },
    "feedbackItems": [
      {
        "code": "queue",
        "completionStatus": "completed",
        "selfRatedMastery": 0.9
      },
      {
        "code": "graph-basic",
        "completionStatus": "partial",
        "selfRatedMastery": 0.55
      }
    ]
  }'
```

预期现象：

- 返回结果中 `masteryByCode` 会变成最新画像掌握度
- `summary.feedbackRecordCount` 会增加
- `savedRecordCount` 会等于本次写入的反馈条数
- `adjustments` 中会保留每个反馈节点的规则与原因说明
- 页面提交练习反馈后，学习者画像卡片和路径规划面板会一起刷新
- 若当前停留在首页，可通过顶部导航进入“学习者画像”页面确认画像结果已同步更新

如果想验证“误点反馈后逐步回退”的主流程，可以继续执行：

```bash
curl -X POST http://127.0.0.1:8080/api/feedback/rollback \
  -H 'Content-Type: application/json' \
  -d '{
    "learnerCode": "demo-learner"
  }'
```

预期现象：

- 返回结果中的 `rolledBackRecordCount` 会显示本次撤销的节点数量
- `rollbackSummary` 会给出最近一次反馈批次中的节点数与完成情况统计
- `rollbackItems` 会说明哪些节点被恢复，以及掌握度是从多少恢复到多少
- `masteryByCode` 会回到上一轮反馈提交前的状态
- 页面触发回退后，学习者画像卡片和路径规划面板会一起刷新
- 若当前停留在首页，可通过顶部导航进入“学习者画像”页面确认回退后的画像结果已同步更新

## 测试

后端当前已覆盖最小路径规划、反馈更新规则、推荐解释，以及学习者画像读取与反馈持久化链路的定向测试。前端当前补充了最小测试，主要覆盖资源上下文持久化、单节点资源页兜底、首页到独立图谱页的跳转、独立学习图谱页中的“设为当前学习目标”联动、图谱状态记忆、学习路径导出、路径总说明展示，以及反馈快捷预设对表单状态的同步更新。测试运行时不会直接修改开发演示用的 `database/data/app.db`。可通过以下命令运行：

后端：

```bash
ctest --test-dir backend/build --output-on-failure
```

前端：

```bash
cd frontend
npm run test
```
