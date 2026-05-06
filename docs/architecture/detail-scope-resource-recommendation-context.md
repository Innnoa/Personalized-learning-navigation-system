# Detail Scope Resource Recommendation Context

最后更新：2026-04-28

## 1. 目标

本文件用于说明 `detail scope` 如何影响 `resource recommendation context`。

这里关注的是一个非常具体的问题：

- 当用户从细化学习页打开资源推荐时
- 当前 `scopeCode` 是如何进入前端上下文的
- 这个上下文又如何影响资源推荐页展示与资源行为记录

这也是当前系统里最容易被误解的一条跨层流转。

## 2. 起点：detail scope 来自哪里

`detail scope` 的来源不是资源推荐页，而是细化图谱与细化学习页。

当前关键位置是：

- `frontend/src/components/DetailLearningWorkspace.vue`
- `backend/services/KnowledgeGraphService.cc`

在细化学习页中：

- 当前图谱范围由 `props.section?.scopeCode` 或 `currentScopeCode` 表示
- 这个 `scopeCode` 对应某个二级细化图谱范围

也就是说，资源推荐页里的 detail mode 并不会自己推导 scope，而是继承细化学习页已确定好的 scope。

## 3. detail scope 如何写入推荐上下文

当前主入口在：

- `DetailLearningWorkspace.vue`
- `syncDetailResourceRecommendationContext(...)`

该函数会做两件事：

### 3.1 重写 section 元数据

它会把 `currentPlan.resourceRecommendations` 映射成新的 section 列表，并为每个 section 增加：

- `recordingKnowledgePointCode`
- `scopeCode`
- `scopeLabel`
- `parentNodeCode`
- `parentNodeLabel`

其中最关键的是：

- `scopeCode: props.section?.scopeCode || currentScopeCode.value || ""`

这说明 detail mode 的推荐 section 会显式携带当前细化图谱范围。

### 3.2 写入 navigationStore

之后该函数会调用：

- `navigationStore.setResourceRecommendationContext(...)`

并写入以下关键字段：

- `contextMode: "detail"`
- `sourcePage: "detail-learning"`
- `scopeCode`
- `scopeLabel`
- `parentNodeCode`
- `parentNodeLabel`

这里形成了当前 detail mode resource recommendation context 的核心快照。

## 4. detail mode 如何进入资源推荐页

在细化学习页里，用户点击某个推荐 section 后，会进入：

- `openDetailResourceRecommendationSection(code)`

该函数会先同步 detail mode 的推荐上下文，再跳转：

- route name: `resource-recommendation`
- params: `{ code: section.code }`
- query: `{ level: "detail" }`

这里有一个关键点：

- detail mode 的页面身份来自 `query.level=detail`
- detail scope 的业务上下文来自 `navigationStore` 里写入的 `scopeCode`

所以：

- `level` 决定页面按 main 还是 detail 模式恢复
- `scopeCode` 决定当前 detail 模式到底属于哪个细化图谱范围

## 5. ResourceRecommendationView 如何消费这个上下文

当前资源推荐页主实现位于：

- `frontend/src/views/ResourceRecommendationView.vue`

它会从 `navigationStore` 中恢复：

- 对应 mode 的 `resourceRecommendationSections`
- 对应 mode 的 `resourceRecommendationSummary`

在 detail mode 下，`summary.scopeCode` 有两个关键作用：

### 5.1 返回细化学习页

`goToPlannerHome()` 中，如果当前是 detail mode 且存在 `summary.scopeCode`，页面不会回首页，而是：

- 跳回 `detail-learning`
- 并带上 `query.scope = summary.scopeCode`

这意味着资源推荐页知道自己属于哪个细化图谱范围。

### 5.2 记录资源交互

在 `recordResourceInteraction(...)` 中，提交给后端的 payload 会包含：

- `sourceContext`
  - detail mode 下是 `detail_scope_resource_panel`
- `scopeCode`
  - `section.scopeCode || summary.scopeCode || "root"`

因此，detail scope 不只是展示状态，还会进入资源行为记录链路。

## 6. 为什么这条上下文很重要

如果没有这条 `detail scope -> resource recommendation context` 链路，系统会丢失三类关键信息：

1. 当前资源推荐属于哪个细化图谱范围
2. 用户资源交互是发生在课程主线，还是发生在某个 detail scope 内
3. 用户从资源推荐页返回时，应回首页还是回细化学习页

所以这条上下文链路的意义不是“多带几个字段”，而是维持：

- 正确页面返回
- 正确资源行为记录
- 正确 detail mode 解释

## 7. service boundary 结论

从 `service boundary` 角度看，这条流转的职责划分是：

- `DetailLearningWorkspace.vue`
  - 负责把 detail scope 语义写进推荐上下文
- `navigationStore.js`
  - 负责跨页保存 detail mode resource recommendation context
- `ResourceRecommendationView.vue`
  - 负责读取该上下文并用于展示、返回与交互记录
- 后端资源记录接口
  - 负责消费 `scopeCode` 与 `sourceContext`

因此，当前 `detail scope affects resource recommendation context` 的关键不在路由层，而在：

- detail learning page
- navigation store
- resource recommendation view

## 8. 检索提示

如果后续希望本地 RAG 更稳定命中本主题，优先应搜索：

- `detail scope resource recommendation context`
- `scopeCode resource recommendation`
- `detail mode resource recommendation`
- `syncDetailResourceRecommendationContext`
- `detail_scope_resource_panel`

