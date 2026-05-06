# Path Planning Service Aggregation Layer

最后更新：2026-04-28

## 1. 目标

本文件用于回答一个具体问题：

- `why PathPlanningService is an aggregation layer`

也就是说明，为什么当前系统中的 `PathPlanningService` 不是单纯的路径算法外壳，而是一个把多个结果拼装成统一响应的聚合层。

## 2. 关键判断

当前 `PathPlanningService` 之所以应被视为 `aggregation layer`，核心原因不是类名，而是它返回的 payload 同时包含多种来源不同、职责不同的数据。

在当前实现里，路径规划响应至少聚合了以下四类结果：

1. 路径项本身
2. 路径项解释信息
3. 路径项级资源
4. 全局资源推荐结果

因此，它不是只负责“算出路径”，而是负责“把路径相关的多个结果组装成前端可直接消费的响应”。

## 3. 代码证据

当前最直接的代码证据位于：

- `backend/services/PathPlanningService.cc`
- `buildPlanningPayload(...)`

在这个函数中，可以看到：

### 3.1 先组装 path item

每个 `pathItem` 不仅包含知识点基础字段，还包含：

- `reasonTrace`
- `explanation`
- `learningResources`

其中：

- `reasonTrace` 和 `explanation` 来自路径解释逻辑
- `learningResources` 来自 `LearningResourceService::buildResourcesForLearningPathItem(...)`

这说明单个路径项已经是一个聚合后的结构，而不是纯算法节点。

### 3.2 再组装全局 payload

最终 payload 里还会继续写入：

- `summary`
- `path`
- `overallExplanation`
- `resourceRecommendations`

其中：

- `summary` 是路径整体统计
- `overallExplanation` 是全局解释
- `resourceRecommendations` 来自 `LearningResourceService::buildResourceRecommendations(...)`

这意味着 `PathPlanningService` 最终返回的是一个“路径 + 解释 + 推荐”的复合响应。

## 4. 它聚合了哪些边界

如果从 `service boundary` 角度看，`PathPlanningService` 当前至少连接了以下边界：

### 4.1 知识结构边界

它依赖知识图谱来决定路径项与依赖顺序。

### 4.2 路径算法边界

它依赖路径规划结果来决定：

- 哪些节点进入当前路径
- 哪些节点是 `scheduled`
- 哪些节点是 `deferred`

### 4.3 路径解释边界

它会把算法结果继续转换成前端可读解释：

- `reasonTrace`
- `explanation`
- `overallExplanation`

### 4.4 资源推荐边界

它不会自己生成资源推荐策略，但会调用 `LearningResourceService` 把资源结果并入统一响应：

- `learningResources`
- `resourceRecommendations`

因此，`PathPlanningService` 的角色更接近：

- orchestration layer
- aggregation layer

而不是单独的 recommendation service，也不是单独的 graph service。

## 5. 它不负责什么

虽然 `PathPlanningService` 是聚合层，但它并不替代其他 service 的领域职责。

它不负责：

- 定义课程图谱结构本身
- 维护 detail scope catalog
- 决定资源推荐具体排序策略
- 决定资源推荐页面如何展示
- 控制路由跳转

这些职责仍分别属于：

- `KnowledgeGraphService`
- `DetailScopeCatalogService`
- `LearningResourceService`
- `ResourceRecommendationView.vue`
- `router/index.js`

所以，`aggregation layer` 的意思不是“什么都做”，而是“负责把其他边界的结果装配进同一响应”。

## 6. 为什么这样设计有价值

把 `PathPlanningService` 作为聚合层，当前带来三个直接收益：

1. 前端调用简单  
   前端不需要再额外拼接路径、解释、资源推荐多个接口结果。

2. 上下文一致  
   同一份响应里的 `path`、`summary`、`resourceRecommendations` 来自同一轮规划结果，不容易出现页面上下文错位。

3. 更容易做页面承接  
   `PathPlannerPanel.vue` 和 `DetailLearningWorkspace.vue` 可以直接把 `resourceRecommendations` 写入 `navigationStore`，而不必重新请求推荐服务。

## 7. 当前一个典型误解

最容易出现的误解是：

- 看到 `LearningResourceService` 负责资源推荐，就以为 `PathPlanningService` 只负责路径

这并不准确。

更准确的说法是：

- `LearningResourceService` 负责“生成推荐内容”
- `PathPlanningService` 负责“把推荐内容与路径结果一起装配回主响应”

所以如果问题是：

- 资源推荐是谁算的？
  - 答案偏向 `LearningResourceService`
- 为什么前端一次拿到路径、解释和推荐？
  - 答案偏向 `PathPlanningService` aggregation layer

## 8. 检索提示

如果后续希望本地 RAG 更稳定命中本主题，优先应搜索：

- `PathPlanningService aggregation layer`
- `why PathPlanningService is an aggregation layer`
- `buildPlanningPayload`
- `path overallExplanation resourceRecommendations`
- `PathPlanningService orchestrates learningResources`

