# 基于路径规划算法的自主学习导航系统

本科毕业设计项目，沈阳建筑大学 · 计算机科学与技术专业。

## 技术栈

| 层 | 技术 |
|------|------|
| 前端 | Vue 3 + Vite + Cytoscape.js + Pinia + Vitest |
| 后端 | Drogon (C++17) + GoogleTest |
| 数据库 | SQLite |
| 核心算法 | C++17（图遍历、拓扑排序、多因子评分） |

## 项目结构

```
frontend/         Vue 3 前端（23 个路由页面，11 个 API 模块）
  src/
    api/          HTTP 接口封装（admin, auth, teacher, path, feedback 等）
    components/   可复用组件（图谱、路径规划、画像卡片等）
    views/        页面视图（学生 x5, 教师 x12, 管理员 x6）
    router/       路由配置与角色守卫
    stores/       Pinia 状态管理（auth, navigation）
    graph/        Cytoscape.js 动态加载
    utils/        练习检验、路径导出、资源推荐策略

backend/          Drogon 后端（13 个 Controller, 17 个 Service）
    controllers/  HTTP 控制器（45 个 API 端点）
    services/     业务服务层
    repositories/ 数据访问层
    algorithm/    算法层（planner、adjuster、explainer）
    models/       数据模型
    dto/          数据传输对象
    config/       配置文件与知识图谱数据
    tests/        单元测试（113 项）

database/
    init/         DDL 建表脚本（12 张表）
    seed/         演示数据（数据结构课程知识点、题库、默认账号）
    data/         运行时 SQLite 数据库文件

scripts/          本地运行、测试、演示环境脚本（11 个）
docs/             设计文档、论文材料、演示截图
```

## 系统功能

### 学生端（5 个页面）

- 课程知识图谱浏览 — 多层下钻，节点详情面板，上下游依赖高亮
- 学习路径规划 — 目标选择 → 拓扑排序 → 多因子评分，支持时间预算
- 推荐理由解释 — 整体说明 + 节点级解释，可展开查看依据
- 练习检验反馈闭环 — 提交练习 → 掌握度更新 → 路径重算
- 反馈回退 — 撤销最近一次检验结果
- 推荐资源联动 — 按节点推荐视频/文章，支持上层资源继承
- 细化学习导航 — 课程级图谱 → 局部范围下钻路径规划
- 学习者画像 — 掌握度分布、反馈历史、学习趋势、进度可视化
- 路径导出 — TXT 格式，含节点顺序与推荐理由
- 多课程支持 — 学生可被分配多门课程，导航栏切换
- 账号注册 — 学生/教师自助注册

### 教师端（12 个页面）

- 仪表盘 — 课程概览与快捷入口
- 课程管理 — 创建课程，查看已分配课程
- 知识点 CRUD — 行内编辑 + 新建 + 删除
- 依赖关系编辑 — 复选框面板，全量替换保存
- 学习资源管理 — 表格化筛选、增删改，数据库持久化
- 题库管理 — 题库列表 + 题目表格管理，支持单选/多选/判断/简答题
- 知识点掌握统计 — 按知识点查看全班掌握度分布
- 学生进度概览 — 列表筛选，掌握度分布
- 个体进度详情 — 按学生查看知识点掌握情况
- 学员分配 — 批量添加学生至课程
- 课程统计 — 学生人数、平均掌握度等汇总数据

### 管理员端（6 个页面）

- 仪表盘 — 系统总览
- 用户管理 — 创建、启用/禁用、角色分配
- 课程管理 — 编辑元信息、删除
- 教师分配 — 指派/撤销课程授课权限
- 学习者管理 — 全局查看、编辑、转课
- 操作日志 — 关键操作记录，时间倒序查看

## API 概览

共 **45 个 HTTP 端点**，分布在 13 个 Controller 中：

| 模块 | 端点 | 说明 |
|------|------|------|
| Auth | `/api/auth/{login,logout,session,register}` | 认证与会话管理 |
| Admin | `/api/admin/{users,courses,assignments,learners,logs}` | 管理员后台 |
| Teacher | `/api/teacher/courses/**` | 教师课程与学生管理 |
| KnowledgeGraph | `/api/knowledge/graph` | 知识图谱查询（支持 scopeCode 下钻） |
| PathPlanning | `/api/path/{generate,adjust}` | 路径规划与动态调整 |
| Feedback | `/api/feedback/{submit,rollback}` | 练习反馈提交与回退 |
| LearnerProfile | `/api/learner/profile` | 学习者画像 |
| Resource | `/api/resource/view` | 资源访问记录 |
| Demo | `/api/demo/reset` | 演示状态重置 |
| Health | `/api/health` | 健康检查 |

## 数据库

**16 张表**：`courses`, `knowledge_points`, `knowledge_dependencies`, `learners`, `users`, `user_roles`, `user_learner_links`, `learner_mastery`, `learner_detail_mastery`, `learning_feedback_records`, `detail_learning_feedback_records`, `learning_resource_view_records`, `teacher_course_assignments`, `learning_resources`, `question_banks`, `questions`

## 内置账号

| 账号 | 角色 | 密码 |
|------|------|------|
| student_demo | 学生 | demo123 |
| teacher_demo | 教师 | demo123 |
| admin_demo | 管理员 | demo123 |

密码使用 SHA-256 哈希存储。

## 本地运行

```bash
./scripts/install_ubuntu_deps.sh   # Ubuntu 一键安装依赖
./scripts/init_database.sh        # 初始化数据库
./scripts/run_backend.sh          # 启动后端 (port 8080)
./scripts/install_frontend_deps.sh # 安装前端依赖
./scripts/run_frontend.sh         # 启动前端 (port 5173)
```

一键演示环境：

```bash
./scripts/demo_up.sh      # 启动（初始化数据库 + 构建后端 + 启动前后端）
./scripts/demo_down.sh    # 停止
```

## 测试

```bash
# 后端（含新增资源/题库数据库化测试）
cd backend/build && ctest --output-on-failure

# 前端（建议优先跑定向页面测试）
cd frontend && npm run test -- --run src/views/TeacherResourcesView.test.js src/views/TeacherQuestionsView.test.js src/views/PracticeCheckView.test.js
```
