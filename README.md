# Personalized-learning-navigation-system

基于知识点依赖图的个性化学习导航系统，本科毕业设计项目。

## 当前基线

- 架构与系统改动留痕统一记录在 `AIREAD.md`
- 课程范围文档位于 `docs/course-scope.md`
- 开发顺序文档位于 `docs/development-plan.md`

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
- `GET /api/knowledge/graph`：从 SQLite 读取“数据结构”课程知识图谱
- `GET /api/learner/profile`：读取默认演示学习者画像、掌握度分布和反馈记录统计
- `POST /api/feedback/submit`：保存学习反馈、更新画像掌握度并返回最新画像
- `POST /api/feedback/rollback`：按最近一次反馈批次逐步回退学习记录，恢复上一版掌握度画像
- `POST /api/path/generate`：根据学习目标、掌握度和时间预算生成最小版学习路径
- `POST /api/path/adjust`：根据学习反馈更新掌握度并重新生成当前推荐路径
- `path[].explanation`：基于 `PathReasonTrace` 输出推荐理由摘要、标签和说明列表
- `path[].learningResources`：为路径节点附加静态资源建议，当前已覆盖课程主图全部 14 个顶层知识点
- `resourceRecommendations`：按“本轮推荐学习 / 下一轮建议”聚合当前路径的配套资源列表
- `learningResources[].recommendedPhase / focusTags / focusNodeLabel / selectionReason`：为资源增加“适合阶段 / 聚焦点 / 对应细化节点 / 选择原因”字段，其中 `selectionReason` 直接消费当前 `LearningPathItem` 的规划证据
- `recentFeedbackItems`：在学习者画像响应中返回最近学习记录，供前端展示反馈历史
- 图谱节点详情面板：点击知识点后可查看章节、难度、说明、直接前置与后继关系
- 图谱多层下钻：当前支持通过 `scopeCode` 加载细化图谱，前端支持双击节点进入下一层并通过面包屑返回上层，当前第三层子图已覆盖 `ds-intro-logical-structure`、`ds-intro-storage-structure`、`algorithm-analysis-time-complexity`、`algorithm-analysis-case-study`、`linear-list-sequential-representation`、`linear-list-linked-representation`、`sequence-list-insert`、`sequence-list-delete`、`sequence-list-merge`、`linked-list-head-insert`、`linked-list-tail-insert`、`linked-list-merge`、`stack-recursion`、`stack-applications`、`queue-circular`、`queue-enqueue`、`queue-applications`、`string-matching-problem`、`string-naive-match`、`kmp-next-definition`、`kmp-main-process`、`tree-basic-binary-storage`、`binary-tree-traversal-recursive`、`binary-tree-traversal-level-order`、`binary-tree-traversal-iterative`、`huffman-tree-construction-process`、`huffman-tree-prefix-code`、`huffman-tree-code-decode`、`graph-basic-adjacency-matrix`、`graph-basic-adjacency-list`、`graph-basic-dfs`、`graph-basic-bfs`、`topological-sort-aoe`、`topological-sort-algorithm`、`topological-sort-critical-path-concept`、`topological-sort-critical-path-solve`
- 已落地细化 scope：`ds-intro / 数据结构基本概念`、`algorithm-analysis / 算法复杂度分析`、`linear-list / 线性表`、`sequence-list / 顺序表`、`linked-list / 链表`、`stack / 栈`、`queue / 队列`、`string / 串`、`kmp / KMP 模式匹配`、`tree-basic / 树与二叉树基础`、`binary-tree-traversal / 二叉树遍历`、`huffman-tree / 哈夫曼树与编码`、`graph-basic / 图的存储与遍历`、`topological-sort / 拓扑排序与关键路径`
- 已落地第三层 scope：`ds-intro-logical-structure / 逻辑结构`、`ds-intro-storage-structure / 存储结构`、`algorithm-analysis-time-complexity / 时间复杂度`、`algorithm-analysis-case-study / 复杂度案例解析`、`linear-list-sequential-representation / 顺序表示（线性表）`、`linear-list-linked-representation / 链式表示（线性表）`、`sequence-list-insert / 插入操作`、`sequence-list-delete / 删除操作`、`sequence-list-merge / 合并操作（顺序表）`、`linked-list-head-insert / 头插法建表`、`linked-list-tail-insert / 尾插法建表`、`linked-list-merge / 有序表合并（链表）`、`stack-recursion / 递归工作栈`、`stack-applications / 典型应用（栈）`、`queue-circular / 循环队列`、`queue-enqueue / 入队操作（队列）`、`queue-applications / 队列典型应用`、`string-matching-problem / 模式匹配问题`、`string-naive-match / 简单模式匹配`、`kmp-next-definition / next 数组含义`、`kmp-main-process / KMP 主过程`、`tree-basic-binary-storage / 二叉树存储结构`、`binary-tree-traversal-recursive / 递归遍历思想`、`binary-tree-traversal-level-order / 层序遍历`、`binary-tree-traversal-iterative / 非递归遍历思想`、`huffman-tree-construction-process / 构造过程`、`huffman-tree-prefix-code / 前缀编码约束`、`huffman-tree-code-decode / 哈夫曼编码与译码`、`graph-basic-adjacency-matrix / 邻接矩阵存储`、`graph-basic-adjacency-list / 邻接表存储`、`graph-basic-dfs / 深度优先遍历`、`graph-basic-bfs / 广度优先遍历`、`topological-sort-aoe / AOE 网`、`topological-sort-algorithm / 拓扑排序算法`、`topological-sort-critical-path-concept / 关键路径概念`、`topological-sort-critical-path-solve / 关键路径求解`
- 推荐理由展开区：路径卡片支持展开 explanation 细项与关键指标
- 资源建议展示区：路径规划面板会根据当前导航结果展示视频教程、文本教程、选择原因和使用建议
- 高频细化节点资源：当前已为 `queue-circular`、`graph-basic-bfs`、`topological-sort-algorithm`、`kmp-next-definition`、`binary-tree-traversal-level-order`、`topological-sort-critical-path-solve` 追加定向资源标签
- 学习记录展示区：学习者画像卡片支持查看最近学习反馈、掌握度变化与记录时间
- 路径前后对比区：提交反馈后可查看调整前本轮安排、调整后本轮安排和关键变化
- 反馈逐步回退：路径规划面板支持按最近一次反馈批次逐步撤销误点或误判，并展示回退前后路径对比
- 首页路径规划布局：首屏右侧只保留目标设置与路径结果，资源建议、学习反馈和路径对比下沉为整宽扩展区，减轻与左侧图谱预览的高度失衡
- 数据库演示数据：1 门课程、14 个核心知识点、15 条依赖边、1 个默认演示学习者、14 条初始掌握度记录
- 前端导航：顶部已提供“首页 / 学习者画像”切换入口，首页聚焦知识图谱与路径规划，学习者画像改为独立页面展示

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

说明：

- 前端安装脚本会自动探测当前代理、WSL 宿主机代理改写和直连三种方式，并打印当前实际使用的联网模式
- 安装命令默认关闭 `audit` 和 `fund`，减少首次安装时的额外等待
- Vite 已默认代理 `/api` 到 `http://127.0.0.1:8080`
- 后端启动前建议先执行一次 `./scripts/init_database.sh`，确保 `database/data/app.db` 已生成

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

5. 浏览器打开 `http://localhost:5173`

首次页面可先检查两点：

- 健康检查卡片是否显示后端状态正常
- 顶部导航是否可以在“首页 / 学习者画像”之间切换
- 进入“学习者画像”页面后，画像卡片是否显示“演示学习者”的掌握度摘要、待补强知识点与最近学习记录
- 图谱预览区域是否展示“数据结构”课程的真实知识点依赖图
- 双击“数据结构基本概念”“算法复杂度分析”“线性表”“顺序表”“链表”“栈”“队列”“串”“KMP 模式匹配”“树与二叉树基础”“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”或“拓扑排序与关键路径”节点后，是否能进入对应细化图谱，并在上方面包屑返回课程主图
- 进入“数据结构基本概念”“算法复杂度分析”“线性表”“顺序表”“链表”“栈”“队列”“串”“KMP 模式匹配”“树与二叉树基础”“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”“拓扑排序与关键路径”细化图谱后，继续双击“逻辑结构”“存储结构”“时间复杂度”“复杂度案例解析”“顺序表示”“链式表示”“插入操作”“删除操作”“合并操作”“头插法建表”“尾插法建表”“有序表合并”“典型应用”“循环队列”“入队操作”“队列典型应用”“模式匹配问题”“简单模式匹配”“next 数组含义”“KMP 主过程”“二叉树存储结构”“递归遍历思想”“层序遍历”“非递归遍历思想”“构造过程”“前缀编码约束”“哈夫曼编码与译码”“邻接矩阵存储”“邻接表存储”“深度优先遍历”“广度优先遍历”“AOE 网”“拓扑排序算法”“关键路径概念”“关键路径求解”等节点，确认能继续进入第三层图谱；其中“顺序表 / 链表 / 队列 / 串”新增的入口也都应能分别进入对应第三层，并通过面包屑逐级返回上层
- 路径规划面板是否会基于学习者画像自动生成一版默认学习路径
- 点击图谱节点后，右侧是否显示节点详情、直接前置依赖和直接后继节点
- 路径卡片点击“展开理由”后，是否能看到 explanation 细项和相关度/重要度/时间占比
- 路径规划结果下方是否会出现“当前学习导航配套资源”卡片，并能随着目标切换显示课程主图各顶层节点的资源建议
- 展开带资源映射的路径节点后，是否能看到该节点对应的视频教程、文本教程、阶段标签、聚焦标签和使用建议
- 当路径中出现 `queue`、`graph-basic`、`kmp`、`binary-tree-traversal`、`topological-sort` 等节点时，资源列表中是否能看到“对应细化节点：循环队列 / 广度优先遍历 / next 数组含义 / 层序遍历 / 关键路径求解”等定向提示
- 路径节点资源和“当前学习导航配套资源”卡片中，是否会显示基于规划依据生成的“选择原因”
- 提交一次学习反馈后，是否能看到“调整前后路径对比”和最近一次调整说明
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

页面侧可额外检查：

- “本轮推荐学习”和“下一轮建议”卡片中，推荐理由应显示为解释摘要，而不是直接拼接底层 `triggerReasons`
- 每个节点应显示如“目标节点 / 前置依赖 / 本轮学习 / 下一轮建议”这类解释标签
- “当前学习导航配套资源”卡片应按当前路径聚合资源，点击链接后可跳转到外部视频或文本教程

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
- 页面提交反馈后，学习者画像卡片和路径规划面板会一起刷新
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

后端当前已覆盖最小路径规划、反馈更新规则、推荐解释，以及学习者画像读取与反馈持久化链路的定向测试。测试运行时会自动创建隔离的 SQLite 测试库，不会直接修改开发演示用的 `database/data/app.db`。可通过以下命令运行：

```bash
ctest --test-dir backend/build/tests --output-on-failure
```
