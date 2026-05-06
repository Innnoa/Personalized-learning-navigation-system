# Resource Recommendation Flow

最后更新：2026-04-28

## 1. 目标

本文件用于说明本项目中的 `learning resource recommendation` / `resource recommendation flow` 是如何从路径规划结果进入前端资源推荐页的。

这里关注的不是单条资源的内容质量，而是推荐结果在系统中的主流程、关键载荷和前后端衔接方式。

## 2. 入口位置

当前资源推荐主流程涉及以下实现位置：

- 后端路径规划聚合：`backend/services/PathPlanningService.cc`
- 后端资源推荐生成：`backend/services/LearningResourceService.cc`
- 首页学习路径面板：`frontend/src/components/PathPlannerPanel.vue`
- 细化学习工作区：`frontend/src/components/DetailLearningWorkspace.vue`
- 前端资源推荐页：`frontend/src/views/ResourceRecommendationView.vue`
- 前端路由入口：`frontend/src/router/index.js`
- 推荐上下文缓存：`frontend/src/stores/navigationStore.js`

## 3. 后端生成流程

资源推荐的后端主入口在：

- `PathPlanningService.cc`
- `LearningResourceService::buildResourceRecommendations(...)`

当前流程如下：

1. `PathPlanningService` 先根据学习目标、可用时间和画像状态生成 `path`
2. 每个路径项会先附带 `learningResources`
3. 路径整体生成完成后，再调用 `LearningResourceService::buildResourceRecommendations(...)`
4. 该服务只处理 `scheduled` 和 `deferred` 状态的路径项
5. 对每个路径项，服务会构造：
   - `code`
   - `name`
   - `status`
   - `scheduledNow`
   - `masteryScore` / `masteryPercent`
   - `resourceCount`
   - `recommendedUsage`
   - `primaryResourceTitle`
   - `resourceLayerSummary`
   - `resources`
6. 最终结果写入 `payload["resourceRecommendations"]`

这意味着当前 `resource recommendation` 并不是独立请求出来的副表，而是路径规划响应中的派生结果。

## 4. 前端承接流程

前端当前有两条进入资源推荐页的路径：

- 首页主线路径：`PathPlannerPanel.vue`
- 细化学习路径：`DetailLearningWorkspace.vue`

首页路径下：

1. `PathPlannerPanel.vue` 从 `planResult.value?.resourceRecommendations` 读取推荐结果
2. `syncResourceRecommendationContext(...)` 将推荐 section 和摘要写入 `navigationStore`
3. `openResourceRecommendationSection(code)` 跳转到：
   - route name: `resource-recommendation`
   - params: `code`
   - query: `level=main`

细化学习路径下：

1. `DetailLearningWorkspace.vue` 从 `detailPlanResult.value?.resourceRecommendations` 读取推荐结果
2. `syncDetailResourceRecommendationContext(...)` 把当前 scope、父节点和 section 一起写入 `navigationStore`
3. `openDetailResourceRecommendationSection(code)` 跳转到：
   - route name: `resource-recommendation`
   - params: `code`
   - query: `level=detail`

## 5. 资源推荐页职责

`ResourceRecommendationView.vue` 是当前 `learning resource recommendation` 的主展示页。

它的职责包括：

- 读取路由参数中的 `code`
- 读取 query 中的 `level`
- 从 `navigationStore` 中恢复对应 mode 的 `resourceRecommendationSections`
- 根据当前 section 展示：
  - 推荐资源列表
  - 主资源标题
  - 资源分层
  - 推荐理由
  - 行为调整提示
  - 使用建议
- 支持在兄弟节点资源之间切换
- 支持记录“已看 / 学完 / 稍后再看”等反馈动作

因此，当前资源推荐页本质上是一个“读取最近一次推荐快照并按 section 展示”的页面，而不是实时重新计算推荐的页面。

## 6. 当前数据边界

当前 `resource recommendation flow` 的数据边界如下：

- 路径规划负责决定要学哪些节点，以及节点状态
- `LearningResourceService` 负责把节点转换成资源推荐 section
- `navigationStore` 负责跨页面缓存推荐快照
- `ResourceRecommendationView` 负责展示与交互
- `router/index.js` 只负责把 `/resource-recommendation/:code` 映射到页面组件

也就是说：

- 路由文件不是推荐逻辑本身
- 推荐页不是推荐生成逻辑本身
- 真正的推荐生成仍在后端 service 层

## 7. 检索提示

如果后续要让本地 RAG 更容易命中本主题，优先应搜索以下词组：

- `learning resource recommendation`
- `resource recommendation flow`
- `resourceRecommendations`
- `ResourceRecommendationView`
- `LearningResourceService`
- `PathPlanningService`

