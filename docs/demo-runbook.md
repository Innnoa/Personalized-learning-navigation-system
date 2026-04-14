# Demo Runbook（答辩演示脚本）

适用项目：`Personalized-learning-navigation-system`  
目标：确保演示可复现、可控、可回滚。

## 1. 演示前准备（1 分钟）

在项目根目录执行：

```bash
cd /home/zazaki/Projects/Personalized-learning-navigation-system
```

推荐先做一键验收并保留服务运行：

```bash
DEMO_CHECK_AUTO_DOWN=0 ./scripts/demo_check.sh
```

默认会启动独立演示实例：
- 前端：`http://127.0.0.1:5174`
- 后端：`http://127.0.0.1:18080`

如果上面命令通过，说明：
- 服务可启动
- 核心接口链路可用
- 数据重置与反馈回退正常

## 2. 答辩主流程（5~8 分钟）

### 步骤 1：展示“系统已就绪”

1. 打开前端页面：`http://127.0.0.1:5174`
2. 展示首页健康状态模块为正常
3. 说明当前课程为“数据结构”，系统基于知识点依赖图做学习导航

预期现象：
- 首页可正常加载
- 健康检查通过

### 步骤 2：从学习图谱设定目标

1. 顶部导航进入“学习图谱”
2. 选中一个节点（例如“拓扑排序与关键路径”）
3. 点击“设为当前学习目标”跳回首页

预期现象：
- 首页路径规划目标已带入所选节点
- 图谱上下文可解释“为什么学这个”

### 步骤 3：生成路径并解释理由

1. 在首页点击“生成学习路径”
2. 先讲“本次路径为什么这样安排”（整体说明）
3. 再展开单节点推荐理由（labels / bullets / reason trace）

预期现象：
- 出现“本轮推荐学习 / 下一轮建议”
- 路径节点带推荐理由解释

### 步骤 4：反馈、调整与回退

1. 对本轮节点提交一条反馈（可用快捷预设）
2. 展示路径前后对比变化
3. 点击“回退最近一次反馈”

预期现象：
- 掌握度发生可解释变化
- 调整后路径变化可见
- 回退后路径与画像恢复上一状态

### 步骤 5：资源与画像联动展示

1. 在路径节点点击“查看推荐资源”
2. 展示资源推荐（阶段、来源、选择原因）
3. 进入“学习者画像”页展示近期记录与掌握度状态

预期现象：
- 资源与当前路径节点关联
- 画像能看到反馈后的变化与记录

## 3. 答辩现场兜底命令

### 场景 A：页面卡住或状态混乱

```bash
./scripts/demo_check_down.sh
DEMO_CHECK_AUTO_DOWN=0 ./scripts/demo_check.sh
```

### 场景 B：只想快速重置演示状态

```bash
curl -X POST http://127.0.0.1:18080/api/demo/reset \
  -H 'Content-Type: application/json' \
  -d '{"learnerCode":"demo-learner"}'
```

### 场景 C：端口冲突（例如 5174/18080 被占用）

```bash
DEMO_CHECK_BACKEND_PORT=18082 DEMO_CHECK_FRONTEND_PORT=5176 DEMO_CHECK_AUTO_DOWN=0 ./scripts/demo_check.sh
```

## 4. 演示结束收尾

结束后执行：

```bash
./scripts/demo_check_down.sh
```

说明：
- 会按端口回收 `demo_check.sh` 使用的独立演示环境
- 幂等执行，重复执行也安全
