# 项目概览
- 名称：Personalized-learning-navigation-system
- 目标：基于知识点依赖图，为课程学习提供个性化学习导航、路径规划、反馈调整、资源推荐与画像展示，适合作为本科毕业设计演示系统。
- 技术栈：前端 Vue + Vite + Cytoscape.js；后端 Drogon（C++）；数据库 SQLite；核心算法为 C++ 路径规划与解释逻辑。
- 主要模块：frontend/ 前端页面与组件；backend/ 控制器、服务、仓储、算法与测试；database/ 初始化脚本与种子数据；docs/ 论文与演示材料；scripts/ 本地运行和验收脚本。
- 关键分层：前端调用链 views/components -> api -> 后端接口；后端调用链 Controller -> Service -> Repository / GraphBuilder / Planner / Explainer；算法层不直接访问数据库。