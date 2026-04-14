# 答辩材料收口说明

本文件用于统一管理当前项目的答辩材料入口，避免后续出现：

- PPT 用的是一套截图
- 论文附录用的是另一套导出文本
- 现场演示讲的是第三套路径

当前建议统一围绕“拓扑排序与关键路径”主链路组织答辩材料。

## 1. 材料入口

- 演示脚本：`docs/demo-runbook.md`
- 固定截图清单：`docs/demo-snapshots/README.md`
- 主学习路径导出样例：`docs/thesis/sample-learning-path-export.txt`
- 细化学习路径导出样例：`docs/thesis/sample-detail-learning-path-export.txt`
- 论文模块对应说明：`docs/thesis/thesis-module-mapping.md`

## 2. 推荐答辩主链路

建议按下面顺序准备材料与演示：

1. 首页就绪截图
2. 学习图谱选目标截图
3. 首页路径生成截图
4. 首页反馈与路径变化截图
5. 推荐资源页截图
6. 细化学习页截图
7. 学习者画像截图
8. 主学习路径导出样例
9. 细化学习路径导出样例

这样组织的好处是：

- 先证明系统能运行
- 再证明系统能从图谱生成路径
- 再证明系统能根据反馈调整
- 再证明系统能给资源和画像结果
- 最后用导出文本证明系统结果可复用、可存档

## 3. 建议论文附录放什么

如果论文附录只保留最有价值的材料，建议放以下 4 项：

1. `03-home-path-generated.png`
2. `06-detail-learning-view.png`
3. `08-learner-profile.png`
4. `sample-learning-path-export.txt`

如果附录空间更充足，可再补：

1. `04-home-feedback-compare.png`
2. `07-resource-page-detail.png`
3. `sample-detail-learning-path-export.txt`

## 4. 当前建议的固定演示目标

- 课程级主目标：`topological-sort / 拓扑排序与关键路径`
- 细化学习目标：`topological-sort-algorithm / 拓扑排序算法`

选择这条主线的原因：

- 依赖关系清晰，便于解释“为什么这样排路径”
- 能体现图基础 -> 图算法 -> 工程调度这一条课程递进线
- 资源推荐、细化学习和论文叙述都更容易围绕这一条线统一

## 5. 当前材料状态

- 固定截图：已给出清单与命名规范，待按规范截图后放入 `docs/demo-snapshots/`
- 导出样例：已固化为文本文件，可直接用于论文附录
- 论文模块对应说明：已整理，可直接据此写“系统设计 / 功能实现 / 测试与演示”章节

## 6. 后续使用建议

- 如果界面再发生明显变化，需要优先更新截图，不要继续沿用旧图
- 如果路径规划文案或导出格式再变，需要同步刷新两个导出样例
- 如果论文结构调整，只改 `docs/thesis/thesis-module-mapping.md`，不要让代码、截图、附录三套材料各自漂移
