# Service Boundary

最后更新：2026-04-28

## 1. 目标

本文件用于说明当前系统中与学习路径、知识图谱和资源推荐最相关的 `service boundary`。

这里的重点不是完整列出所有类，而是说明：

- 哪一层负责生成知识结构
- 哪一层负责生成学习路径
- 哪一层负责生成资源推荐
- 哪一层只负责页面展示与跳转

## 2. 核心边界概览

当前主边界可以概括为四层：

1. 知识结构层
2. 路径规划层
3. 资源推荐层
4. 前端展示层

对应的主要实现如下：

- 知识结构层：`backend/services/KnowledgeGraphService.cc`
- 路径规划层：`backend/services/PathPlanningService.cc`
- 资源推荐层：`backend/services/LearningResourceService.cc`
- 前端展示层：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/router/index.js`

## 3. KnowledgeGraphService 的边界

`KnowledgeGraphService` 的职责是：

- 加载课程知识图谱
- 根据 `scopeCode` 构造细化图谱 payload
- 为前端提供图谱节点、边和视图范围

它负责回答的是：

- 当前课程主图是什么
- 当前细化 scope 的知识结构是什么

它不负责：

- 具体学习路径排序
- 资源推荐生成
- 前端资源页跳转

## 4. PathPlanningService 的边界

`PathPlanningService` 是学习路径聚合层。

它负责：

- 接收学习目标、时间预算、学习者状态
- 基于知识图谱生成路径项
- 计算 `scheduled` / `deferred` / `mastered` 等状态
- 汇总 `summary`
- 组装最终路径规划 payload

它同时负责把其他服务结果聚合进统一响应：

- 为每个路径项附带 `learningResources`
- 为整个响应附带 `resourceRecommendations`

它不负责：

- 直接展示推荐页面
- 维护推荐页的本地缓存

因此，`PathPlanningService` 更像总装配层，而不是单一领域服务。

## 5. LearningResourceService 的边界

`LearningResourceService` 是当前 `learning resource recommendation` 的主服务边界。

它负责：

- 读取学习资源目录与相关配置
- 为知识点或路径项挑选资源
- 生成资源分层、优先资源、推荐理由、使用建议
- 输出前端可直接消费的 `resourceRecommendations`

它回答的是：

- 某个学习路径项应该推荐哪些资源
- 推荐结果如何按层次和优先级组织

它不负责：

- 决定路径项是否应学
- 决定图谱结构
- 控制页面路由

因此，如果查询主题是：

- `learning resource recommendation`
- `resource recommendation flow`
- `recommendation service boundary`

最应该命中的后端实现就是 `LearningResourceService.cc`。

## 6. 前端页面与路由边界

前端当前要区分三种角色：

### 6.1 页面/组件层

- `PathPlannerPanel.vue`
  - 展示主线路径结果
  - 把推荐 section 写入 `navigationStore`
  - 发起跳转到资源推荐页

- `DetailLearningWorkspace.vue`
  - 展示细化学习结果
  - 写入 detail mode 的推荐上下文
  - 跳转到资源推荐页

- `ResourceRecommendationView.vue`
  - 读取最近一次资源推荐快照
  - 按 section 展示资源推荐
  - 记录用户资源交互

### 6.2 路由层

- `frontend/src/router/index.js`

它只负责：

- 把 `/resource-recommendation/:code` 映射到 `ResourceRecommendationView`

它不负责：

- 生成推荐结果
- 解释推荐策略
- 管理推荐上下文

也就是说，`router/index.js` 是入口文件，不是资源推荐主实现。

### 6.3 前端状态层

- `frontend/src/stores/navigationStore.js`

它负责：

- 暂存 `resourceRecommendationSections`
- 记录 `contextMode`
- 在页面切换后恢复最近一次推荐快照

它不负责：

- 真正计算推荐结果

## 7. 当前边界结论

如果从 `service boundary` 视角总结当前系统：

- `KnowledgeGraphService` 负责知识结构
- `PathPlanningService` 负责路径结果聚合
- `LearningResourceService` 负责资源推荐生成
- `navigationStore` 负责推荐快照缓存
- `ResourceRecommendationView` 负责推荐结果展示
- `router/index.js` 只负责路由入口

这也是后续补文档和做本地检索时最重要的职责划分。

## 8. 具体边界实例

为了避免把 `service boundary` 理解成抽象分层口号，下面给出当前项目里最典型的三条真实流转。

### 8.1 细化图谱进入流程

当用户从课程主图进入某个细化节点时：

1. 前端图谱组件 `LearnerLearningGraph.vue` 决定当前要加载的 `scopeCode`
2. 前端调用 `fetchKnowledgeGraph({ scopeCode })`
3. 后端进入 `KnowledgeGraphService::buildGraphPayload(scopeCode)`
4. `KnowledgeGraphService` 根据 `scopeCode` 返回：
   - root payload，或
   - detail scope payload
5. 前端根据返回的 `view.scopeCode`、`nodes`、`edges`、`breadcrumbs` 更新图谱页面

这里的边界是：

- 前端负责决定“想看哪个 scope”
- `KnowledgeGraphService` 负责返回“这个 scope 长什么样”
- 前端不负责自己拼接 detail scope payload

### 8.2 主线路径资源推荐流程

当用户在首页生成学习路径后：

1. `PathPlanningService` 生成 `path`
2. 同一响应里再调用 `LearningResourceService::buildResourceRecommendations(...)`
3. 后端把结果写入 `payload["resourceRecommendations"]`
4. 前端 `PathPlannerPanel.vue` 读取 `planResult.resourceRecommendations`
5. `PathPlannerPanel.vue` 通过 `syncResourceRecommendationContext(...)` 写入 `navigationStore`
6. 用户点击某个 section 后，前端跳转到 `ResourceRecommendationView.vue`

这里的边界是：

- `PathPlanningService` 负责总装配
- `LearningResourceService` 负责资源推荐生成
- `PathPlannerPanel.vue` 负责把推荐快照带到前端状态
- `ResourceRecommendationView.vue` 负责展示
- `router/index.js` 只负责把 URL 映射到页面

### 8.3 细化学习资源推荐流程

当用户在二级细化学习页中打开资源推荐时：

1. `DetailLearningWorkspace.vue` 读取 `detailPlanResult.resourceRecommendations`
2. 前端把当前 `scopeCode`、父节点、section 信息一并写入 `navigationStore`
3. 页面跳转到 `resource-recommendation` route，并带上 `level=detail`
4. `ResourceRecommendationView.vue` 再按 `detail` mode 恢复上下文并展示推荐结果

这里的边界是：

- `DetailLearningWorkspace.vue` 负责“把 detail scope 语义带进推荐上下文”
- `navigationStore` 负责“跨页保存最近一次 detail mode 快照”
- `ResourceRecommendationView.vue` 负责“按当前 mode 解释和展示推荐结果”
- 推荐结果本身仍来自后端 `LearningResourceService`

### 8.4 一个常见误判

当前最容易被误判的文件是：

- `frontend/src/router/index.js`

它会频繁出现在检索结果里，因为它含有：

- `ResourceRecommendationView`
- `resource-recommendation`

但它的真实边界只有一个：

- 路由入口映射

它不负责：

- 生成 recommendation payload
- 解释 recommendation strategy
- 维护 scope context

因此，如果查询目标是“推荐逻辑在哪里”或“service boundary 怎么划分”，`router/index.js` 只能算入口线索，不能算主实现。
