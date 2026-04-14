# 论文模块对应说明

本文件用于把当前项目实现与毕业论文各章节一一对应，避免写论文时出现“代码做了，但论文不知道放哪一章”的情况。

## 1. 论文题目

基于知识点依赖图的个性化学习导航系统

## 2. 章节对应建议

### 第一章 绪论

建议写入内容：

- 选题背景：数据结构课程知识点多、依赖关系强，学生常出现“学到后面不会回补前置基础”的问题
- 研究意义：把知识点依赖图、个性化路径规划、动态反馈调整和资源推荐整合为一个可演示系统
- 研究目标：构建知识点依赖图、生成学习路径、支持反馈调整、提供资源建议、可视化展示

可引用项目材料：

- `AIREAD.md`
- `docs/course-scope.md`
- `docs/development-plan.md`

### 第二章 需求分析

建议写入内容：

- 系统面向对象：单学习者演示场景
- 核心需求：
  - 课程知识图谱展示
  - 个性化学习路径生成
  - 学习反馈提交与回退
  - 细化学习导航
  - 推荐资源展示
  - 学习者画像展示
- 非功能需求：
  - 本地可部署
  - 单机可演示
  - 结果可导出
  - 界面可视化清晰

对应页面与模块：

- 首页：`frontend/src/views/HomeView.vue`
- 学习图谱：`frontend/src/views/LearningGraphView.vue`
- 细化学习：`frontend/src/views/DetailLearningView.vue`
- 推荐资源：`frontend/src/views/ResourceRecommendationView.vue`
- 学习者画像：`frontend/src/views/LearnerProfileView.vue`

### 第三章 系统总体设计

建议写入内容：

- 总体架构：前后端分离 + 单体后端 + SQLite 本地数据库
- 前端职责：
  - 页面展示
  - 图谱交互
  - 路径结果展示
  - 导出文本
- 后端职责：
  - 图谱读取
  - 路径规划
  - 推荐理由生成
  - 反馈处理
  - 画像聚合
  - 资源推荐
- 数据库职责：
  - 课程
  - 知识点
  - 依赖关系
  - 学习者
  - 掌握度
  - 反馈记录
  - 细化掌握度
  - 资源行为记录

对应代码与文件：

- 后端入口：`backend/main.cc`
- 配置：`backend/config/config.json`
- 数据库结构：`database/init/001_schema.sql`
- 课程种子：`database/seed/course_data_structures.sql`
- 构建入口：`backend/CMakeLists.txt`

### 第四章 数据库设计

建议重点说明以下表：

1. `courses`
2. `knowledge_points`
3. `knowledge_dependencies`
4. `learners`
5. `learner_mastery`
6. `learning_feedback_records`
7. `learner_detail_mastery`
8. `detail_learning_feedback_records`
9. `learning_resource_view_records`

建议写法：

- 先给 E-R 关系说明
- 再给关键字段说明
- 再解释这些表如何支撑“路径规划 + 动态调整 + 画像展示”

直接依据文件：

- `database/init/001_schema.sql`

### 第五章 系统核心功能设计与实现

建议按功能模块拆写。

#### 5.1 知识图谱构建与展示

可写内容：

- 顶层课程知识点来自 SQLite
- 二级 / 三级细化图谱来自配置文件
- 前端使用 Cytoscape.js 展示图谱
- 支持双击下钻、面包屑返回、节点详情展示

对应文件：

- `backend/services/KnowledgeGraphService.cc`
- `backend/repositories/KnowledgeGraphRepository.cc`
- `backend/config/knowledge_graph_details.json`
- `frontend/src/components/LearnerLearningGraph.vue`
- `frontend/src/views/LearningGraphView.vue`

#### 5.2 个性化学习路径规划

可写内容：

- 输入：目标节点、掌握度、时间预算
- 输出：本轮推荐、下一轮建议、整体说明
- 路径规划遵循知识点依赖关系
- 时间预算不足时自动顺延部分节点

对应文件：

- `backend/services/PathPlanningService.cc`
- `backend/tests/PathPlannerTest.cc`
- `backend/tests/PathPlanningServiceTest.cc`
- `frontend/src/components/PathPlannerPanel.vue`

#### 5.3 推荐理由解释

可写内容：

- 推荐理由不是凭空生成，而是消费规划过程输出的依据
- 系统同时输出整体说明和单节点 explanation
- 支持标签、摘要和文本说明

对应文件：

- `backend/services/PathPlanningService.cc`
- `backend/tests/PathExplanationTest.cc`
- `frontend/src/components/PathPlannerPanel.vue`

#### 5.4 学习反馈与动态调整

可写内容：

- 支持提交反馈
- 支持快捷反馈预设
- 支持最近一次反馈回退
- 支持路径前后变化对比

对应文件：

- `backend/services/FeedbackService.cc`
- `backend/repositories/FeedbackRepository.cc`
- `backend/tests/PathAdjusterTest.cc`
- `backend/tests/LearnerProfileFeedbackServiceTest.cc`
- `frontend/src/components/PathPlannerPanel.vue`

#### 5.5 细化学习导航

可写内容：

- 支持从课程级节点进入二级范围
- 支持在二级范围内继续生成细化路径
- 支持细化掌握度持久化与父节点进度聚合

对应文件：

- `backend/services/DetailLearningProgressService.cc`
- `backend/services/DetailScopeCatalogService.cc`
- `backend/repositories/DetailLearningRepository.cc`
- `frontend/src/components/DetailLearningWorkspace.vue`
- `frontend/src/views/DetailLearningView.vue`

#### 5.6 推荐资源与学习者画像

可写内容：

- 路径节点和细化节点都可查看对应资源
- 资源推荐区分阶段、来源、节点聚焦点和选择原因
- 学习者画像展示当前掌握度、薄弱点和近期记录

对应文件：

- `backend/services/LearningResourceService.cc`
- `backend/services/ResourceViewService.cc`
- `backend/services/LearnerProfileService.cc`
- `frontend/src/views/ResourceRecommendationView.vue`
- `frontend/src/views/LearnerProfileView.vue`

### 第六章 系统测试与结果分析

建议写入内容：

- 后端单元测试
- 前端最小测试
- 一键冒烟脚本
- 一键演示验收脚本
- 典型演示链路结果

对应文件与脚本：

- `backend/tests/`
- `frontend/src/**/*.test.js`
- `scripts/smoke_demo_flow.sh`
- `scripts/demo_up.sh`
- `scripts/demo_down.sh`
- `scripts/demo_check.sh`
- `docs/demo-runbook.md`

附录建议直接引用：

- `docs/thesis/sample-learning-path-export.txt`
- `docs/thesis/sample-detail-learning-path-export.txt`
- `docs/demo-snapshots/README.md` 中的固定截图

### 第七章 总结与展望

建议总结当前已完成能力：

- 课程知识图谱建模
- 个性化路径规划
- 学习反馈调整
- 推荐理由解释
- 细化学习导航
- 配套资源推荐
- 学习者画像展示

建议展望方向：

- 继续补三级细化节点专属资源
- 扩展更多课程
- 增强统计分析能力
- 导出更规范的打印版或 PDF 版学习报告

## 3. 推荐正文插图与表格

建议至少准备以下内容：

- 图 3-1：系统总体架构图
- 图 3-2：前后端模块关系图
- 图 4-1：数据库 E-R 关系图
- 图 5-1：课程级学习图谱页面
- 图 5-2：首页路径规划结果
- 图 5-3：细化学习页面
- 图 5-4：推荐资源页面
- 图 5-5：学习者画像页面
- 表 4-1：核心数据表说明
- 表 6-1：测试项与测试结果

## 4. 当前最适合答辩的主线

- 课程级目标：`topological-sort`
- 细化目标：`topological-sort-algorithm`

这条主线最适合写论文和做答辩，因为它能够同时覆盖：

- 依赖关系
- 路径规划
- 动态调整
- 细化学习
- 资源推荐
- 画像反馈
