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
