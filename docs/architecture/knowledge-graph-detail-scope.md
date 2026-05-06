# Knowledge Graph Detail Scope

最后更新：2026-04-28

## 1. 目标

本文件用于说明本项目中的 `knowledge graph detail scope` 是什么，以及它如何在后端和前端之间流转。

这里关注的不是某一个课程节点的内容细节，而是：

- `scopeCode` 的含义
- `KnowledgeGraphService` 如何构造细化图谱 payload
- 前端如何进入和展示细化图谱
- 细化图谱与资源推荐、路径规划之间的关系

## 2. 核心概念

当前系统中的 `knowledge graph detail scope` 可以理解为：

- 课程主图之外的二级图谱范围
- 由 `scopeCode` 唯一标识
- 用于表达某个课程节点的细化知识结构

常见边界是：

- `root`
  - 表示课程主图
- `<node>-detail`
  - 表示某个节点的细化图谱范围

在实现上，`scopeCode` 不是临时页面参数，而是后端图谱 payload 和前端细化学习页面共同使用的核心标识。

## 3. 后端边界

当前 `knowledge graph detail scope` 的后端主实现位于：

- `backend/services/KnowledgeGraphService.cc`

关键入口是：

- `KnowledgeGraphService::buildGraphPayload(const std::string &scopeCode)`

当前逻辑如下：

1. 先通过 `loadKnowledgeGraph()` 读取课程主图
2. 再从 `DetailScopeCatalogService::getCatalog()` 读取细化 scope 目录
3. 对传入的 `scopeCode` 做归一化：
   - 空值或 `root` -> 课程主图
   - 其他值 -> 对应细化 scope
4. 如果是 `root`
   - 返回 `buildRootPayload(...)`
5. 如果是细化 scope
   - 校验该 `scopeCode` 是否存在
   - 返回 `buildDetailScopePayload(...)`

这意味着当前 `knowledge graph detail scope` 的主入口不是前端组件，也不是路由层，而是 `KnowledgeGraphService`。

## 4. payload 结构

细化图谱 payload 的关键字段包括：

- `view.scopeCode`
- `view.scopeName`
- `view.isRoot`
- `nodes`
- `edges`
- `breadcrumbs`

其中最重要的是：

- `view.scopeCode`
  - 当前正在浏览的细化图谱范围
- `breadcrumbs`
  - 从课程主图到当前细化图谱的返回路径

因此，如果查询主题是：

- `knowledge graph detail scope`
- `scopeCode`
- `buildGraphPayload`
- `细化图谱范围`

最应该命中的实现就是 `KnowledgeGraphService.cc` 以及围绕该 payload 的说明文档。

## 5. 前端进入方式

当前前端与 `knowledge graph detail scope` 相关的主要位置是：

- `frontend/src/components/LearnerLearningGraph.vue`
- `frontend/src/components/DetailLearningWorkspace.vue`
- `frontend/src/views/LearningGraphView.vue`
- `frontend/src/stores/navigationStore.js`

主流程如下：

1. `LearnerLearningGraph.vue` 负责加载和切换图谱
2. 组件内部会根据当前 `scopeCode` 调用：
   - `fetchKnowledgeGraph({ scopeCode })`
3. 当用户在主图中进入某个细化节点后：
   - 前端会加载对应 detail scope 的 payload
4. `DetailLearningWorkspace.vue` 会围绕当前 `scopeCode` 展示：
   - 当前细化图谱
   - 当前 scope 下的路径规划结果
   - 当前 scope 下的资源推荐入口
5. `navigationStore.js` 负责保存：
   - 当前细化页面状态
   - 已选 `scopeCode`
   - 细化视图恢复信息

## 6. 与路径规划和资源推荐的关系

`knowledge graph detail scope` 本身不直接生成资源推荐，但它会影响两个下游环节：

### 6.1 影响细化学习路径

`DetailLearningWorkspace.vue` 会以当前 `scopeCode` 作为细化学习范围，围绕该范围生成更局部的学习路径。

### 6.2 影响资源推荐上下文

当细化学习页打开资源推荐时，会把当前 `scopeCode` 一起写入推荐上下文：

- `contextMode = detail`
- `scopeCode = 当前细化图谱范围`

因此，`knowledge graph detail scope` 是：

- 图谱展示范围标识
- 细化学习路径范围标识
- detail mode 资源推荐上下文标识

但它不是：

- 推荐生成服务本身
- 路由入口本身

## 7. 当前 service boundary

如果只从 `service boundary` 角度总结本主题：

- `KnowledgeGraphService`
  - 负责生成 detail scope 图谱 payload
- `DetailScopeCatalogService`
  - 负责提供 scope 目录与映射
- `LearnerLearningGraph`
  - 负责按 `scopeCode` 展示图谱
- `DetailLearningWorkspace`
  - 负责在当前 scope 下承接细化学习与推荐入口

这也是当前 `knowledge graph detail scope` 最重要的职责边界。

## 8. 检索提示

如果后续希望本地 RAG 更稳定命中本主题，优先应搜索：

- `knowledge graph detail scope`
- `scopeCode`
- `KnowledgeGraphService`
- `buildGraphPayload`
- `detail scope payload`
- `细化图谱范围`

