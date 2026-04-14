# AI_GUIDE

最后更新：2026-03-30 15:35 CST

<!-- AI_GUIDE:SUMMARY:BEGIN -->
## 0. 文件说明

- 本文件用于记录 Codex 在本项目中的执行审计、当前状态、风险与后续待办。
- 每轮会话开始先读取，结束前补写本轮摘要。
- 不记录密码、令牌、密钥等敏感信息。
- 系统设计与架构级调整仍以 `AIREAD.md` 为主；本文件偏执行审计与会话留痕。

## 1. 当前项目状态

- 项目目录：`/home/zazaki/Projects/Personalized-learning-navigation-system`
- 技术栈：Vue + Drogon(C++) + SQLite + Cytoscape.js
- 当前课程范围：数据结构
- 当前主要页面：首页、学习图谱、细化学习、推荐资源、学习者画像
- 当前已知能力：路径规划、学习反馈、演示重置、资源推荐、资源页一级/二级双层导航、细化节点资源兜底、资源来源显式标注、同层关联资源补充、资源多样性排序、图谱多层下钻、学习图谱状态记忆、学习路径结果导出、路径总说明、二级细化学习导航、细化学习分支状态记忆、细化路径结果导出、细化学习掌握度持久化、父节点进度聚合联动、反馈状态与掌握度区间联动
- 后端验证入口：`cmake -S .. -B .`、`cmake --build . -j$(nproc)`、`ctest --output-on-failure`
- 前端验证入口：`npm run test -- --run`、`npm run build`

## 2. 当前约定与风险

- 本科毕设目标优先：可落地、可答辩、可演示，避免过度工程化。
- 架构或系统级改动需同步更新 `AIREAD.md`。
- 当前仓库原先只有 `AIREAD.md`，没有 `AI_GUIDE.md`；本文件于本轮会话补建。
- “恢复演示初始状态”目前依赖后端硬编码基线，不是从数据库中的独立快照表恢复。
- 后端全量 `ctest --output-on-failure` 已恢复为 71/71 通过；当前未发现已知阻塞型失败用例。
- 学习反馈中的“学习后掌握度”现在同时受完成情况区间和当前掌握度影响；前端负责显式约束输入区间，后端仍以 `previousMastery` 参与最终掌握度更新，其中 `partial` 已允许小幅回退。

## 3. 最近一次会话摘要

- 目标：让 `partial` 状态也能表达“学后发现仍有不足”的小幅回退语义。
- 结果：
  - 后端 `partial` 更新规则改为允许在 `40%-80%` 的巩固区间内小幅上调或小幅回退。
  - 当本次自评低于当前掌握度时，系统允许下调，但单次回退幅度限制为 `15%`，避免和 `blocked` 语义混淆。
  - 已完成与学习受阻的规则不变，继续分别承担“只升不降”和“明显下调”的语义。
- 验证：
  - `cmake --build . -j$(nproc)`
  - `./backend_test -r FeedbackUpdateRulesClampSelfRatedMasteryByCompletionStatus`
  - `./backend_test -r FeedbackUpdateRulesAllowPartialToSlightlyLowerMastery`
- 未完成：无，本轮语义调整已完成。
<!-- AI_GUIDE:SUMMARY:END -->

<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-25 11:16 CST id=session-20260325-111600 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-25 11:55 CST id=session-20260325-115500 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-25 12:39 CST id=session-20260325-123900 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-25 15:16 CST id=session-20260325-151600 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 08:55 CST id=session-20260326-085500 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 09:07 CST id=session-20260326-090700 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 09:12 CST id=session-20260326-091200 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 09:29 CST id=session-20260326-092900 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 09:49 CST id=session-20260326-094900 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 10:00 CST id=session-20260326-100000 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 11:09 CST id=session-20260326-110900 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 12:13 CST id=session-20260326-121300 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 13:31 CST id=session-20260326-133100 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-26 14:04 CST id=session-20260326-140400 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 17:42 CST id=session-20260330-174200 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 19:06 CST id=session-20260330-190600 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 19:18 CST id=session-20260330-191800 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 19:34 CST id=session-20260330-193400 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 11:39 CST id=session-20260330-113900 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 11:45 CST id=session-20260330-114500 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 12:01 CST id=session-20260330-120100 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 14:18 CST id=session-20260330-141800 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 14:28 CST id=session-20260330-142800 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 14:41 CST id=session-20260330-144100 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 14:48 CST id=session-20260330-144800 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 15:17 CST id=session-20260330-151700 -->
<!-- AI_GUIDE:READ_ANCHOR ts=2026-03-30 15:35 CST id=session-20260330-153500 -->

## 4. 执行记录

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 15:35 CST id=session-20260330-153500 -->
### 2026-03-30 15:35 CST

- 会话类型：反馈语义细化 / 后端规则微调
- 用户问题：确认“学习后掌握度是否可以比当前掌握度低，相当于发现仍有不足”，并要求将该语义落到系统实现中。
- 修改文件：
  - `backend/algorithm/adjuster/LearningPathAdjuster.cc`
  - `backend/tests/PathAdjusterTest.cc`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 将 `partial` 的后端掌握度更新规则从“只升不降”调整为“允许小幅上调，也允许小幅回退”。
  - 当 `selfRatedMastery >= previousMastery` 时，仍按巩固语义向上推进，但上限保持在 `80%`。
  - 当 `selfRatedMastery < previousMastery` 时，允许回退，但单次最多回退 `15%`，且不低于 `40%`。
  - `completed` 与 `blocked` 语义不变，继续分别表示“已掌握提升”和“明显受阻回落”。
- 验证命令：
  - `cmake --build . -j$(nproc)`
  - `./backend_test -r FeedbackUpdateRulesClampSelfRatedMasteryByCompletionStatus`
  - `./backend_test -r FeedbackUpdateRulesAllowPartialToSlightlyLowerMastery`
- 验证结论：
  - 后端构建通过。
  - 原有状态区间裁剪测试通过。
  - 新增“部分完成允许小幅回退”测试通过。
- 残留观察：
  - 这一轮只调整后端更新语义，没有改动前端区间，因为前端 `partial=40%-80%` 的输入边界本身已经允许用户表达“比当前略低”的判断。
  - 目前的 `15%` 回退上限是面向毕设演示的稳妥值，后续若需要更贴近真实课程数据，可再依据教师经验微调。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 15:35 CST id=session-20260330-153500 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 15:17 CST id=session-20260330-151700 -->
### 2026-03-30 15:17 CST

- 会话类型：反馈规则收口 / 前后端联动增强
- 用户问题：认为学习反馈模块中的“学习情况”和“学习后掌握度”联动过弱，希望“已完成 / 部分完成 / 学习受阻”分别只允许在合理区间内调整，并确认学习后掌握度是否与当前掌握度有关联。
- 修改文件：
  - `frontend/src/utils/feedbackQuickPreset.js`
  - `frontend/src/utils/feedbackQuickPreset.test.js`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `backend/algorithm/adjuster/LearningPathAdjuster.cc`
  - `backend/tests/PathAdjusterTest.cc`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 前端统一三档反馈掌握度区间：`completed=80%-100%`、`partial=40%-80%`、`blocked=0%-35%`。
  - 首页与细化学习页切换完成情况后，滑块区间会同步变化，当前草稿值会自动归一化到对应区间，并显式提示可调范围。
  - 后端在应用反馈更新规则前，先按完成情况裁剪 `selfRatedMastery`，防止前后端语义不一致。
  - 当前掌握度仍参与最终更新：`previousMastery` 会与本次自评掌握度共同影响结果，而不是完全被本次输入覆盖。
- 验证命令：
  - `npm run test -- src/utils/feedbackQuickPreset.test.js src/components/PathPlannerPanel.test.js`
  - `cmake --build . -j$(nproc)`
  - `./backend_test -r FeedbackUpdateRulesClampSelfRatedMasteryByCompletionStatus`
  - `npm run build`
- 验证结论：
  - 前端定向测试 15/15 通过。
  - 后端构建通过，定向规则测试通过。
  - 前端构建通过。
- 残留观察：
  - 当前规则是面向毕设演示的三档轻量语义约束，足够解释“完成情况”和“学习后掌握度”的关系。
  - 如果后续还要更细分教学语义，可以再加“完成但仍需巩固”等档位，但当前不建议继续加复杂度。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 15:17 CST id=session-20260330-151700 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 14:48 CST id=session-20260330-144800 -->
### 2026-03-30 14:48 CST

- 会话类型：前端回退 / 组件恢复
- 用户问题：要求回退到今天开始修改“操作摘要”组件之前的版本。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 回退 `PathPlannerPanel.vue` 今日围绕“操作摘要固定/默认显示”做的所有布局试验。
  - 删除为该试验新增的默认摘要空态测试，恢复到原始 7 项测试集。
- 验证命令：
  - `git diff -- frontend/src/components/PathPlannerPanel.vue frontend/src/components/PathPlannerPanel.test.js`
  - `npm run test -- src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 组件与测试文件已回退为无差异状态。
  - 路径规划面板相关单测 7/7 通过。
  - 前端构建通过。
- 残留观察：
  - 审计文件中仍保留了今天多次试验的历史记录，这是留痕，不代表当前运行代码仍保留那些改动。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 14:48 CST id=session-20260330-144800 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 14:41 CST id=session-20260330-144100 -->
### 2026-03-30 14:41 CST

- 会话类型：前端布局修复 / 首页默认固定摘要栏
- 用户问题：希望“操作摘要”默认就出现并固定住，而不是提交反馈后才出现。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 将首页路径规划区整理为稳定双列布局，左侧为主内容，右侧为固定摘要栏。
  - 操作摘要默认渲染，未发生最近操作时显示默认空态说明。
  - 新增定向测试，约束“默认显示摘要空态”的行为，降低后续回归风险。
- 验证命令：
  - `npm run test -- src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 路径规划面板相关单测 8/8 通过。
  - 前端构建通过。
- 残留观察：
  - 当前默认空态只说明“本次会话内还没有最近操作”，并不回读历史反馈的详细摘要；这符合当前接口能力边界。
  - 若后续需要首页一加载就展示“数据库中的最近一次反馈摘要”，则需要后端新增“最近一次反馈摘要查询”接口。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 14:41 CST id=session-20260330-144100 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 14:28 CST id=session-20260330-142800 -->
### 2026-03-30 14:28 CST

- 会话类型：前端布局修复 / 首页右侧固定摘要栏
- 用户问题：首页“操作摘要”模块仍会在“学习反馈 -> 路径规划”区间内滑动，希望其作为主页面固定右栏存在。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 将“操作摘要”从扩展区内部右栏提升到 `PathPlannerPanel` 顶层右侧栏。
  - 路径规划主卡片与反馈区保持在左侧主列，摘要栏在桌面端作为独立右栏 `sticky` 固定。
  - 扩展区内部仅保留“学习反馈”和“路径变化”主体，避免摘要固定范围继续受扩展区容器限制。
- 验证命令：
  - `npm run test -- src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 路径规划面板相关单测 7/7 通过。
  - 前端构建通过。
- 残留观察：
  - 这次修复针对的是首页主页面层级，不再依赖扩展区内部容器。
  - 若浏览器仍看到旧表现，需要刷新当前前端页面或等待热更新完成。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 14:28 CST id=session-20260330-142800 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 14:18 CST id=session-20260330-141800 -->
### 2026-03-30 14:18 CST

- 会话类型：前端布局修复 / 首页扩展区
- 用户问题：首页“操作摘要”模块会随着滚轮滚动而滑动，希望将其固定住。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 调整 `PathPlannerPanel` 扩展区结构，将“路径变化”模块并入左侧主列。
  - 保留右侧“操作摘要”使用 `sticky`，但把其固定作用范围扩展到整块“反馈 + 路径变化”区域，避免继续下滚时摘要过早滑走。
- 验证命令：
  - `npm run test -- src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 路径规划面板相关单测 7/7 通过。
  - 前端构建通过。
- 残留观察：
  - 这次修复针对的是首页扩展区的结构性原因，不是简单改 `top` 或随意改 `position`。
  - 如果后续还要进一步强化“完全固定不动”的视觉效果，可以再评估是否要把侧栏做成更独立的页面级两列布局，但当前版本已经能解决摘要过早滑走的问题。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 14:18 CST id=session-20260330-141800 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 12:01 CST id=session-20260330-120100 -->
### 2026-03-30 12:01 CST

- 会话类型：演示链路缺陷修复 / 前端代理配置
- 用户问题：`demo_check.sh` 显示通过，但打开 `http://127.0.0.1:5174` 后前端提示“后端未启动”。
- 修改文件：
  - `frontend/vite.config.js`
  - `scripts/demo_up.sh`
  - `scripts/run_frontend.sh`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 修复前端开发代理配置，支持通过环境变量动态指定后端地址。
  - 修复 `demo_up.sh` 与前端代理的联动，确保 `demo_check` 的 `5174 -> 18080` 链路一致。
  - 为手动前端启动脚本补充代理目标参数，减少后续同类问题。
- 验证命令：
  - `bash -n scripts/demo_up.sh scripts/run_frontend.sh scripts/demo_check.sh scripts/demo_check_down.sh`
  - `npm run build`
  - `node --input-type=module -e "process.env.VITE_DEV_PROXY_TARGET='http://127.0.0.1:18080'; ..."`
- 验证结论：
  - 脚本语法通过。
  - 前端构建通过。
  - Vite 配置在 `VITE_DEV_PROXY_TARGET=http://127.0.0.1:18080` 时，`/api` 代理目标解析正确。
- 残留观察：
  - 已经在运行的旧前端 dev server 不会自动热更新代理配置，必须重启前端或重新执行 `demo_check` 才能生效。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 12:01 CST id=session-20260330-120100 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 11:45 CST id=session-20260330-114500 -->
### 2026-03-30 11:45 CST

- 会话类型：演示脚本增强 / 文档同步
- 用户问题：`./demo_check.sh` 跑起来后不好关闭，希望能单独做一个关闭脚本。
- 修改文件：
  - `scripts/demo_check_down.sh`
  - `scripts/demo_check.sh`
  - `README.md`
  - `docs/demo-runbook.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增 `demo_check_down.sh`，按端口回收 `demo_check` 默认使用的 `18080 / 5174` 独立演示环境。
  - `demo_check.sh` 成功后会明确打印 `./scripts/demo_check_down.sh` 作为手动停止命令。
  - README 与答辩 runbook 已同步更新停止说明，减少脚本使用歧义。
- 验证命令：
  - `bash -n scripts/demo_check_down.sh scripts/demo_check.sh scripts/demo_up.sh scripts/demo_down.sh`
  - `sed -n '100,170p' README.md`
  - `sed -n '1,140p' docs/demo-runbook.md`
- 验证结论：
  - 四个脚本语法可通过。
  - 文档中 `demo_check` 的启动与停止入口已经成对出现。
- 残留观察：
  - `demo_check_down.sh` 是按端口停止，适合当前毕设演示链路；如果后续并行启动多组演示实例，再考虑是否要做更细的实例标识。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 11:45 CST id=session-20260330-114500 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 11:39 CST id=session-20260330-113900 -->
### 2026-03-30 11:39 CST

- 会话类型：演示环境排障 / 仅分析
- 用户问题：`./scripts/demo_check.sh` 报告 `18080` 端口已被后端服务占用，询问如何关闭该服务。
- 修改文件：
  - `AI_GUIDE.md`
- 实现结果：
  - 确认 `scripts/demo_down.sh` 依赖 `.demo-runtime/backend.pid` 与 `.demo-runtime/frontend.pid` 停止进程。
  - 形成按端口排查与关闭的建议，优先使用 `fuser 18080/tcp` 查看占用，再用 `fuser -k 18080/tcp` 直接结束监听进程。
- 验证命令：
  - `sed -n '1,220p' scripts/demo_down.sh`
  - `date '+%Y-%m-%d %H:%M CST'`
- 验证结论：
  - 当前已确认脚本的停服前提与端口冲突场景下更直接的处理手段。
- 残留观察：
  - 若该端口服务不是当前仓库启动的实例，`demo_down.sh` 可能无效，仍需按端口关闭。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 11:39 CST id=session-20260330-113900 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 19:34 CST id=session-20260330-193400 -->
### 2026-03-30 19:34 CST

- 会话类型：答辩材料收口 / 文档落地
- 用户问题：转去做答辩材料收口，比如固定截图、导出样例、论文模块对应说明。
- 修改文件：
  - `docs/demo-snapshots/README.md`
  - `docs/thesis/defense-material-pack.md`
  - `docs/thesis/thesis-module-mapping.md`
  - `docs/thesis/sample-learning-path-export.txt`
  - `docs/thesis/sample-detail-learning-path-export.txt`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 落地固定截图清单，统一了截图顺序、命名、页面和用途。
  - 落地两份固定路径导出样例，可直接用于论文附录或答辩展示。
  - 落地论文模块对应说明，把代码实现与论文章节逐项对应。
  - README 顶部已补答辩材料入口，降低后续查找成本。
- 验证命令：
  - `find docs/demo-snapshots docs/thesis -maxdepth 2 -type f | sort`
  - `sed -n '1,40p' README.md`
- 验证结论：
  - 新增文档文件已存在且入口可见。
  - 当前答辩材料链路已从“散落在项目里”推进为“有入口、有样例、有映射”的可复用状态。
- 残留观察：
  - 真实截图文件尚未生成；当前提供的是固定截图清单和命名规范。
  - 若后续页面视觉再调整，需要优先更新截图和导出样例，避免材料漂移。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 19:34 CST id=session-20260330-193400 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 19:18 CST id=session-20260330-191800 -->
### 2026-03-30 19:18 CST

- 会话类型：资源覆盖补齐 + 定向回归
- 用户问题：继续补细化资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `backend/tests/PathPlanningServiceTest.cc`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 为 `tree-basic / graph-basic / topological-sort` 三条后段章节分支补入第二批焦点资源。
  - 当前 `graph-basic`、`topological-sort`、`tree-basic` 各自所有二级节点都达到至少 2 条 `focused` 资源。
  - 复用既有资源数量约束测试，并将关键校验节点扩展到后段章节。
- 验证命令：
  - `node -e "const fs=require('fs'); JSON.parse(fs.readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node` 统计脚本：检查 `graph-basic / topological-sort / tree-basic` 各二级节点焦点资源计数
  - `cmake --build . -j$(nproc)`
  - `ctest -R LearningResourceService --output-on-failure`
- 验证结论：
  - JSON 解析通过。
  - 三个后段章节 scope 的二级节点焦点资源计数均达到 2。
  - 后端构建通过，资源相关测试 6/6 通过。
- 残留观察：
  - 课程主图对应的二级细化资源覆盖已经比较均衡。
  - 如果继续做资源补齐，下一步更值得转向三级细化节点，而不是继续重复加厚二级节点。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 19:18 CST id=session-20260330-191800 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 19:06 CST id=session-20260330-190600 -->
### 2026-03-30 19:06 CST

- 会话类型：资源覆盖补齐 + 定向回归
- 用户问题：继续回到当前毕设任务，优先做“细化资源继续补齐”。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `backend/tests/PathPlanningServiceTest.cc`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 为 `stack / queue / string` 三条高频细化分支的二级节点补入第二条焦点资源，提升节点专属资源厚度。
  - 继续优先复用当前项目里已经采用的课程风格资源与稳定图文来源，避免引入风格漂移明显的资源池。
  - 新增定向测试，约束 `stack-push`、`queue-enqueue`、`string-matching-problem` 至少保有两条 `focused` 资源。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `cmake --build . -j$(nproc)`
  - `ctest -R LearningResourceService --output-on-failure`
- 验证结论：
  - JSON 解析通过。
  - 后端构建通过。
  - 资源相关测试 6/6 通过，新加覆盖测试通过。
- 残留观察：
  - 当前高频细化分支已从“主要靠继承资源兜底”推进到“关键节点有两条焦点资源”。
  - 后续若继续补资源，更适合优先推进 `graph-basic`、`topological-sort`、`tree-basic` 等后段章节，而不是重复扩充已有前三章分支。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 19:06 CST id=session-20260330-190600 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-30 17:42 CST id=session-20260330-174200 -->
### 2026-03-30 17:42 CST

- 会话类型：论文材料支持 / 文献核对
- 用户问题：确认继续，要求开题报告参考文献满足“10 篇、近三年优先、不超过 5 年、至少一篇期刊、至少一篇英文”。
- 修改文件：
  - `AI_GUIDE.md`
- 实现结果：
  - 基于已完成项目主题，整理并核对了一组与教育知识图谱、个性化学习路径、资源推荐和可解释推荐直接相关的中英文参考文献。
  - 本轮未改动项目代码、数据库和脚本，仅进行文献范围核对与材料支持。
- 验证命令：
  - 文献网页来源核对（标题 / 年份 / 期刊 / 主题相关性）。
- 验证结论：
  - 当前候选文献集合可用于开题报告参考文献部分，时间范围与中英文要求均满足。
- 残留观察：
  - 若学校要求极严格的参考文献格式，后续仍建议再统一做一版 GB/T 7714 排版。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-30 17:42 CST id=session-20260330-174200 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 14:04 CST id=session-20260326-140400 -->
### 2026-03-26 14:04 CST

- 会话类型：文档补齐 + 演示流程收口
- 用户问题：确认可以落地 `docs/demo-runbook.md`。
- 修改文件：
  - `docs/demo-runbook.md`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增答辩演示 Runbook，按“准备 -> 主流程 -> 兜底 -> 收尾”组织，包含可直接执行的命令和预期现象说明。
  - 在 README 增加 Runbook 入口，降低答辩前查找成本。
  - 在 AIREAD 追加系统改动记录，保持架构与流程留痕一致。
- 验证命令：
  - 文档对齐检查（命令路径与参数与现有脚本一致）。
- 验证结论：
  - Runbook 已可直接用于现场演示串讲与兜底。
- 残留观察：
  - 后续可补 `docs/demo-snapshots/` 固定样例输出，进一步增强论文附录可复现性。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 14:04 CST id=session-20260326-140400 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 13:31 CST id=session-20260326-133100 -->
### 2026-03-26 13:31 CST

- 会话类型：问题修复 + 脚本默认策略调整
- 用户问题：执行 `demo_check.sh` 时在 `demo/reset` 发生只读数据库错误。
- 修改文件：
  - `scripts/demo_up.sh`
  - `scripts/demo_check.sh`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 将 `demo_check` 默认改为独立端口并默认禁用复用，规避受外部实例数据库状态污染。
  - `demo_up` 补充复用开关和端口冲突显式报错，提升错误可诊断性。
- 验证命令：
  - `bash -n scripts/demo_up.sh scripts/demo_check.sh`
  - `./scripts/demo_check.sh`
  - `./scripts/demo_down.sh`
- 验证结论：
  - 语法检查通过；默认 `demo_check` 全链路通过，且可执行回收。
- 残留观察：
  - 在受控执行环境中，后台进程生命周期可能受会话机制影响；本地终端按脚本执行不影响主流程。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 13:31 CST id=session-20260326-133100 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 12:13 CST id=session-20260326-121300 -->
### 2026-03-26 12:13 CST

- 会话类型：验收脚本开发 + 联动回归
- 用户问题：做 `demo_check.sh`（高优先级）。
- 修改文件：
  - `scripts/demo_check.sh`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增 `demo_check`，联动 `demo_up`、服务可访问性校验和 `smoke_demo_flow` 主链路冒烟，形成单命令演示验收入口。
  - 增加 `DEMO_CHECK_AUTO_UP / RUN_SMOKE / AUTO_DOWN / 端口 / 目标 / 预算` 等参数，便于答辩前快速复测与进程回收。
- 验证命令：
  - `bash -n scripts/demo_check.sh`
  - `DEMO_CHECK_BACKEND_PORT=18082 DEMO_CHECK_FRONTEND_PORT=5176 DEMO_CHECK_AUTO_UP=1 DEMO_CHECK_RUN_SMOKE=1 DEMO_CHECK_AUTO_DOWN=1 ./scripts/demo_check.sh`
- 验证结论：
  - 语法检查通过；联动验收通过，含自动回收流程。
- 残留观察：
  - 当前 `demo_check` 仍偏后端链路与服务可达性，不覆盖前端视觉与交互回归。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 12:13 CST id=session-20260326-121300 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 11:09 CST id=session-20260326-110900 -->
### 2026-03-26 11:09 CST

- 会话类型：演示编排脚本开发 + 回归验证
- 用户问题：做 `demo_up / demo_down` 脚本。
- 修改文件：
  - `scripts/demo_up.sh`
  - `scripts/demo_down.sh`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 落地 `demo_up`：一键启动后端和前端，默认自动重置演示状态并打印访问地址、日志路径和停止命令。
  - 落地 `demo_down`：读取运行态 pid 文件进行安全停止，支持进程不存在时的幂等清理。
  - 补充脚本参数与运行态目录文档，保证演示流程可复现。
- 验证命令：
  - `bash -n scripts/demo_up.sh scripts/demo_down.sh`
  - `DEMO_BACKEND_PORT=18081 DEMO_FRONTEND_PORT=5175 DEMO_AUTO_RESET=1 ./scripts/demo_up.sh`
  - `./scripts/demo_down.sh`
- 验证结论：
  - 语法检查通过；`demo_up` 能完成启动编排并返回可访问地址；`demo_down` 能完成停止与清理流程。
- 残留观察：
  - 在当前受控执行环境中，后台进程生命周期可能受会话回收机制影响；用户在本地终端执行时应可按 pid 管理脚本正常启停。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 11:09 CST id=session-20260326-110900 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 10:00 CST id=session-20260326-100000 -->
### 2026-03-26 10:00 CST

- 会话类型：脚本可用性微调 + 回归复测
- 用户问题：继续完善一键冒烟脚本可用性。
- 修改文件：
  - `scripts/smoke_demo_flow.sh`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 调整冒烟脚本的复用模式默认地址：`SMOKE_REUSE_EXISTING_BACKEND=1` 且未传 `API_BASE_URL` 时，默认访问 `http://127.0.0.1:8080`。
  - README 与 AIREAD 对应说明同步更新，避免参数理解偏差。
- 验证命令：
  - `bash -n scripts/smoke_demo_flow.sh`
  - `./scripts/smoke_demo_flow.sh`
- 验证结论：
  - 语法检查通过；冒烟脚本再次执行通过，输出 `✅ 冒烟通过`。
- 残留观察：
  - 当前脚本已满足后端主链路回归；如需覆盖前端行为仍需额外 UI 级测试。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 10:00 CST id=session-20260326-100000 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 09:49 CST id=session-20260326-094900 -->
### 2026-03-26 09:49 CST

- 会话类型：脚本开发 + 接口链路冒烟 + 文档同步
- 用户问题：优先做一个一键冒烟脚本。
- 修改文件：
  - `scripts/smoke_demo_flow.sh`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增一键后端冒烟脚本，默认自启独立后端实例并执行 6 段接口校验，覆盖重置、路径生成、反馈提交与回退、画像读取全链路。
  - 脚本支持复用模式（`SMOKE_REUSE_EXISTING_BACKEND=1`），并暴露目标节点、预算、端口、超时等常用参数。
  - README 已补充脚本入口和参数说明，AIREAD 已同步系统改动留痕。
- 验证命令：
  - `./scripts/smoke_demo_flow.sh`
- 验证结论：
  - 脚本执行通过，输出 `✅ 冒烟通过：health/reset/generate/feedback/rollback/profile 全链路正常。`
- 残留观察：
  - 当前脚本只验证后端 API 链路，不覆盖前端页面交互与视觉回归。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 09:49 CST id=session-20260326-094900 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 09:29 CST id=session-20260326-092900 -->
### 2026-03-26 09:29 CST

- 会话类型：后端故障修复 + 全量回归
- 用户问题：修掉后端已知测试故障（高优先级）。
- 修改文件：
  - `backend/tests/TestSupport.cc`
  - `backend/services/LearnerProfileService.cc`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 在测试数据库初始化后执行 `sqlite3_shutdown()`，避免 SQLite 已初始化状态下再次配置线程模式导致 Drogon 报错。
  - 修复画像响应结构稳定性：无细化记录时，`detailMasteryByCode` 等映射字段返回 `{}` 而非 `null`。
  - 核对 `test_main.cc` 后保持 `is_fast = false`，本次不通过修改连接性能参数规避问题。
- 验证命令：
  - `ctest -R LearnerProfileServiceBuildsGraphMasteryForDetailNodes --output-on-failure`
  - `ctest --output-on-failure`
- 验证结论：
  - 目标失败用例通过。
  - 后端全量测试 71/71 通过。
- 残留观察：
  - 当前修复聚焦测试初始化与返回结构稳定性，不影响路径规划、反馈调整和资源推荐主链路行为。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 09:29 CST id=session-20260326-092900 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 09:12 CST id=session-20260326-091200 -->
### 2026-03-26 09:12 CST

- 会话类型：审计补录 + 前端定向复测
- 用户问题：确认继续，要求把未完成收尾项做完。
- 修改文件：
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 补写 `AIREAD.md` 系统变更记录，新增“资源页预算阈值策略统一抽离（前端）”条目，明确策略文件、调用替换和测试覆盖。
  - 未新增功能代码，保持当前资源推荐行为与展示结果不变。
- 验证命令：
  - `npm run test -- --run src/views/ResourceRecommendationView.test.js src/utils/resourceRecommendationPolicy.test.js`
- 验证结论：
  - 2 个测试文件、12 个测试全部通过。
- 残留观察：
  - 预算阈值当前统一在前端；若后续需要前后端单一真源，可将阈值策略进一步统一到后端配置或接口输出。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 09:12 CST id=session-20260326-091200 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 09:07 CST id=session-20260326-090700 -->
### 2026-03-26 09:07 CST

- 会话类型：前端策略统一化 + 定向验证
- 用户问题：确认继续，把预算三档阈值抽为统一常量/策略，避免后续不一致。
- 修改文件：
  - `frontend/src/utils/resourceRecommendationPolicy.js`
  - `frontend/src/utils/resourceRecommendationPolicy.test.js`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 把 `45/90` 分钟阈值与三档提示逻辑抽到 `resourceRecommendationPolicy`。
  - 资源页展示条数计算和预算档位文案全部改为调用统一策略函数，不再在页面内部写死阈值。
  - 新增策略级单元测试，确保阈值、档位与提示文案的一致性可回归。
- 验证命令：
  - `npm run test -- --run src/views/ResourceRecommendationView.test.js src/utils/resourceRecommendationPolicy.test.js`
- 验证结论：
  - 2 个测试文件、12 个测试全部通过。
- 残留观察：
  - 当前是前端统一策略；若后续资源策略要在后端参与裁剪，可再推进跨端策略一致性。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 09:07 CST id=session-20260326-090700 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-26 08:55 CST id=session-20260326-085500 -->
### 2026-03-26 08:55 CST

- 会话类型：资源页文案细化 + 前端定向验证
- 用户问题：确认继续，把时间预算提示细化为三档文案（紧凑 / 标准 / 充裕）。
- 修改文件：
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/views/ResourceRecommendationView.test.js`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 资源页新增 `resolveTimeBudgetTier` 与 `buildTimeBudgetTierHint`，按预算分档输出“紧凑 / 标准 / 充裕”。
  - 资源页“当前智能推荐策略”文案已包含预算档位提示，资源摘要区“时间预算”字段也同步显示档位。
  - 未改变既有资源排序、展示上限与折叠策略，只增强提示清晰度。
- 验证命令：
  - `npm run test -- --run src/views/ResourceRecommendationView.test.js`
- 验证结论：
  - 资源页定向测试 8 项全部通过。
  - 新增三档文案测试通过，旧有限量展示与展开/收起行为未回归。
- 残留观察：
  - 档位阈值当前在前端定义，后续若需多端一致建议下沉到后端统一输出。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-26 08:55 CST id=session-20260326-085500 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 15:16 CST id=session-20260325-151600 -->
### 2026-03-25 15:16 CST

- 会话类型：资源页展示策略收口 + 前端定向验证
- 用户问题：不要一次显示太多推荐资源，而是根据当前学习进度与能力与时间综合因素推荐资源。
- 修改文件：
  - `backend/services/LearningResourceService.cc`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/views/ResourceRecommendationView.test.js`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 在后端资源推荐输出中补充节点级学习状态字段：`masteryScore`、`masteryPercent`、`difficultyLevel`、`estimatedMinutes`。
  - 资源页改为“动态限量展示”模式：默认只展示前 `2~5` 条最匹配资源（基于 `availableMinutes`、`status`、`masteryPercent`、`difficultyLevel`、`detail/main` 模式综合计算）。
  - 新增“智能推荐策略”说明区与“展开全部资源 / 收起额外资源”按钮，避免信息过载同时保留完整资源可访问性。
- 验证命令：
  - `npm run test -- --run src/views/ResourceRecommendationView.test.js`
- 验证结论：
  - 资源页定向测试 7 项全部通过。
  - 新增测试已覆盖动态限量展示和展开/收起交互行为。
- 残留观察：
  - 当前限量策略位于资源页展示层，能快速落地且风险小；后续如需更强约束，可将预算裁剪进一步下沉到后端候选生成。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 15:16 CST id=session-20260325-151600 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 12:39 CST id=session-20260325-123900 -->
### 2026-03-25 12:39 CST

- 会话类型：一级资源补齐（数据配置）+ 最小验证
- 用户问题：继续补一级节点资源，不要每次都推荐相同资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 为剩余直连资源较薄的 6 个一级节点补充了新的直连资源：`linear-list`、`sequence-list`、`linked-list`、`stack`、`queue`、`string`。
  - 每个目标节点都新增了 2 条 `focusNodeCode` 为空的资源，因此一级页面会直接把这些资源纳入候选池，而不是只能依赖细化节点继承或同层补充。
  - 这批新增资源覆盖了课程文章、教程站点、公开课视频与考研梳理型资料，优先增强一级页面的资源来源多样性，而没有改动既有排序策略。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node - <<'NODE' ... 统计全部一级节点的 direct/directSources 分布 ... NODE`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `linear-list`、`sequence-list`、`linked-list`、`stack`、`queue`、`string` 当前均已达到 `direct=4 / directSources=4`。
- 残留观察：
  - 当前 14 个一级节点里，除 `kmp` 外其余主线节点的直连资源池已基本达到 4 条左右；`kmp` 由于节点本身更聚焦，当前保留 3 条直连资源已足够支撑一级页展示。
  - 后续如继续补资源，更适合围绕高频二级节点或三级细化节点做专属资源补强，而不是继续扩大一级节点直连池。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 12:39 CST id=session-20260325-123900 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 11:55 CST id=session-20260325-115500 -->
### 2026-03-25 11:55 CST

- 会话类型：一级资源补齐（数据配置）+ 最小验证
- 用户问题：对一级节点补充资源，不要每次都推荐相同资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 为一级页面重复感最强的 8 个知识点补充直连资源：`ds-intro`、`algorithm-analysis`、`kmp`、`tree-basic`、`binary-tree-traversal`、`huffman-tree`、`graph-basic`、`topological-sort`。
  - 这些新增资源全部保持 `focusNodeCode` 为空，因此会直接进入一级节点资源池，而不是只在二级/三级细化节点中被动继承。
  - 补齐后，上述 8 个一级节点的直连资源数量分别提升到 3 到 4 条，其中 `ds-intro`、`algorithm-analysis`、`binary-tree-traversal`、`topological-sort` 的直连来源数已提升到 4 个。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node - <<'NODE' ... 统计 8 个一级节点的 direct/directSources 分布 ... NODE`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - 目标一级节点的直连资源池明显增厚，满足“一级页不再总是那几条固定资源”的本轮目标。
- 残留观察：
  - 这轮优先补的是体感最差的一批一级节点，未一次性重做全部 14 个一级节点的直连池。
  - 下一轮若继续补资源，更适合继续补其它一级节点的直连资源，或按高频主线路径继续完善三级细化节点专属资源。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 11:55 CST id=session-20260325-115500 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 11:16 CST id=session-20260325-111600 -->
### 2026-03-25 11:16 CST

- 会话类型：资源推荐逻辑增强 + 前后端对齐 + 定向验证
- 用户问题：希望一级与二级资源推荐更丰富，不要总是固定那几个资源。
- 修改文件：
  - `backend/services/LearningResourceService.cc`
  - `backend/tests/PathPlanningServiceTest.cc`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/views/ResourceRecommendationView.test.js`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 细化节点资源组装新增“同层关联节点”补充逻辑；当节点专属资源不足时，除了上层继承，还会引入当前细化范围内相邻节点的焦点资源。
  - 资源排序从单次静态分数排序改为“基础分数排序后做确定性多样性重排”，重排会对重复 `source / type / recommendedPhase / focusNodeCode / resourceCoverageType` 施加轻量递减。
  - 保留首条主推荐的强解释性，同时让第 2 至第 N 条资源更容易覆盖不同来源、不同学习阶段与不同补充层次。
  - 资源页来源标签与覆盖说明文案已对齐：`related` 资源显示为“来自关联节点”，不再误标成“来自上层”。
  - 更新资源相关测试，改为校验“资源来源更丰富、行为降权仍生效、路径响应里仍能带出交互状态”，不再依赖旧的固定资源数量。
- 验证命令：
  - `cmake -S .. -B .`
  - `cmake --build . -j$(nproc)`
  - `ctest --output-on-failure`
  - `ctest -R 'LearningResourceService|PathPlanningService' --output-on-failure`
  - `npm run test -- --run src/views/ResourceRecommendationView.test.js`
  - `npm run build`
- 验证结论：
  - 后端构建通过。
  - 资源相关后端定向 10 项测试全部通过。
  - 前端资源页定向测试通过，生产构建通过。
  - 全量后端测试除既有 `LearnerProfileServiceBuildsGraphMasteryForDetailNodes` SQLite 线程配置失败外，其余通过。
- 残留观察：
  - 当前资源丰富度提升主要依赖“同层关联补充 + 多样性排序”；如果后续继续增强，优先方向应是补三级细化节点专属资源，而不是引入随机或黑盒排序。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 11:16 CST id=session-20260325-111600 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 10:47 CST id=session-20260325-104700 -->
### 2026-03-25 10:47 CST

- 会话类型：前端交互增强 + 状态持久化调整 + 定向验证
- 用户问题：希望在推荐资源页面增加“一级页面 / 二级页面”二级导航：一级页面展示主路径一级资源，二级页面展示细化学习当前一级节点下的二级节点资源。
- 修改文件：
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/views/LearnerProfileView.vue`
  - `frontend/src/components/PageLayout.vue`
  - `frontend/src/stores/navigationStore.test.js`
  - `frontend/src/views/ResourceRecommendationView.test.js`
  - `frontend/src/components/PageLayout.test.js`
  - `frontend/src/components/DetailLearningWorkspace.test.js`
  - `frontend/src/views/LearnerProfileView.test.js`
  - `AI_GUIDE.md`
  - `AIREAD.md`
- 实现结果：
  - 资源上下文从原先“单份当前快照”扩展为“`main` + `detail` 两套快照并存 + 当前活跃快照”。
  - 资源页新增“一级页面 / 二级页面”切换卡片；一级页面显示课程主线路径里的一级资源，二级页面显示细化学习当前一级节点下的二级资源。
  - 从首页路径规划进入资源页时显式带 `level=main`，从细化学习进入资源页时显式带 `level=detail`，学习者画像的资源入口也统一按一级页面写入。
  - 顶部导航栏中的“推荐资源”链接现在会携带当前层级查询参数，避免资源页从二级页面跳回一级页面或反之。
  - 保持现有后端接口、资源记录接口和资源页单路由不变，仅调整前端状态结构与页面交互。
- 验证命令：
  - `npm run test -- --run src/stores/navigationStore.test.js src/views/ResourceRecommendationView.test.js src/components/PageLayout.test.js src/components/DetailLearningWorkspace.test.js src/views/LearnerProfileView.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向 5 个测试文件、22 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 这次已经把推荐资源页从“单上下文页面”调整为“单页面双层导航”；后续如果要继续增强资源模块，优先方向应是补三级细化节点资源，而不是继续拆页面。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 10:47 CST id=session-20260325-104700 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 10:20 CST id=session-20260325-102000 -->
### 2026-03-25 10:20 CST

- 会话类型：细化节点资源补齐（数据配置）+ 最小验证
- 用户问题：确认继续补别的细化分支资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
- 实现结果：
  - 核对后确认当前项目并不存在独立的“排序主线 / 查找主线”一级节点编码，因此本轮改为补齐剩余未覆盖的一级主线 `ds-intro` 与 `algorithm-analysis`。
  - 在 `learning_resources.json` 中为 `ds-intro` 新增 9 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 在 `learning_resources.json` 中为 `algorithm-analysis` 新增 10 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 继续保持“数据优先”策略，不改前端资源页、不改后端推荐逻辑，也不涉及架构调整。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node - <<'NODE' ... ds-intro / algorithm-analysis 焦点资源覆盖统计 ... NODE`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `ds-intro` 资源总数为 11，其中 9 条为细化节点焦点资源。
  - `algorithm-analysis` 资源总数为 12，其中 10 条为细化节点焦点资源。
- 残留观察：
  - 当前项目中所有现有一级知识点主线都已具备较完整的二级资源覆盖。
  - 下一阶段若继续扩展，更适合沿已有 `detailScope` 下沉到三级资源，而不是继续寻找不存在的一级编码。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 10:20 CST id=session-20260325-102000 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 10:10 CST id=session-20260325-101000 -->
### 2026-03-25 10:10 CST

- 会话类型：细化节点资源补齐（数据配置）+ 最小验证
- 用户问题：确认继续补别的细化分支资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
- 实现结果：
  - 确认 `backend/config/knowledge_graph_details.json` 中 `huffman-tree` 共有 8 个二级细化节点，`topological-sort` 共有 8 个二级细化节点。
  - 在 `learning_resources.json` 中为 `huffman-tree` 新增 8 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 在 `learning_resources.json` 中为 `topological-sort` 保留既有 `topological-sort-critical-path-solve` 焦点资源，并补充其余 7 个关键节点资源。
  - 继续保持“数据优先”策略，不改前端资源页、不改后端推荐逻辑，也不涉及架构调整。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node - <<'NODE' ... huffman-tree / topological-sort 焦点资源覆盖统计 ... NODE`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `huffman-tree` 资源总数为 10，其中 8 条为细化节点焦点资源。
  - `topological-sort` 资源总数为 10，其中 8 条为细化节点焦点资源。
- 残留观察：
  - 当前树与图的主展示链路中，`tree-basic`、`binary-tree-traversal`、`huffman-tree`、`graph-basic`、`topological-sort` 已具备较完整的二级资源覆盖。
  - `topological-sort` 当前同时保留课堂风格视频、关键路径视频与 OI Wiki 文字资料，适合演示时做“概念 + 算法 + 应用”串讲。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 10:10 CST id=session-20260325-101000 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 09:58 CST id=session-20260325-095800 -->
### 2026-03-25 09:58 CST

- 会话类型：细化节点资源补齐（数据配置）+ 最小验证
- 用户问题：确认继续补别的细化分支资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
- 实现结果：
  - 确认 `backend/config/knowledge_graph_details.json` 中 `tree-basic` 共有 8 个二级细化节点，`binary-tree-traversal` 共有 8 个二级细化节点，`graph-basic` 共有 8 个二级细化节点。
  - 在 `learning_resources.json` 中为 `tree-basic` 新增 8 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 在 `learning_resources.json` 中为 `binary-tree-traversal` 新增 8 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 在 `learning_resources.json` 中为 `graph-basic` 新增 7 条缺失节点资源，保留原有 2 条 `graph-basic-bfs` 焦点资源不动。
  - 继续保持“数据优先”策略，不改前端资源页、不改后端推荐逻辑，也不涉及架构调整。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node - <<'NODE' ... tree-basic / binary-tree-traversal / graph-basic 焦点资源覆盖统计 ... NODE`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `tree-basic` 资源总数为 10，其中 8 条为细化节点焦点资源。
  - `binary-tree-traversal` 资源总数为 10，其中 8 条为细化节点焦点资源。
  - `graph-basic` 资源总数为 11，其中 9 条为细化节点焦点资源，对应 8 个唯一细化节点。
- 残留观察：
  - 当前树与图主线里，`tree-basic`、`binary-tree-traversal`、`graph-basic` 已具备较完整的二级资源覆盖。
  - `graph-basic-bfs` 仍保留视频 + 文章两条焦点资源，便于在资源页中同时提供主学与巩固入口。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 09:58 CST id=session-20260325-095800 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 09:49 CST id=session-20260325-094900 -->
### 2026-03-25 09:49 CST

- 会话类型：细化节点资源补齐（数据配置）+ 最小验证
- 用户问题：确认继续补别的细化分支资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
- 实现结果：
  - 确认 `backend/config/knowledge_graph_details.json` 中 `stack` 共有 9 个二级细化节点，`queue` 共有 9 个二级细化节点，`string` 共有 8 个二级细化节点。
  - 在 `learning_resources.json` 中为 `stack` 新增 9 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 在 `learning_resources.json` 中为 `queue` 保留既有 `queue-circular` 焦点资源，并补充其余 8 个关键节点资源。
  - 在 `learning_resources.json` 中为 `string` 新增 8 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源，且未混入 `kmp` 专属资源。
  - 继续保持“数据优先”策略，不改前端资源页、不改后端推荐逻辑，也不涉及架构调整。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node - <<'NODE' ... stack / queue / string 焦点资源覆盖统计 ... NODE`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `stack` 资源总数为 11，其中 9 条为细化节点焦点资源。
  - `queue` 资源总数为 11，其中 9 条为细化节点焦点资源。
  - `string` 资源总数为 10，其中 8 条为细化节点焦点资源。
- 残留观察：
  - 当前常用基础结构分支里，`linear-list`、`sequence-list`、`linked-list`、`stack`、`queue`、`string` 已具备较完整的二级资源覆盖。
  - `queue-circular` 仍保留原有细化资源入口，未被本轮覆盖掉。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 09:49 CST id=session-20260325-094900 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 09:34 CST id=session-20260325-093400 -->
### 2026-03-25 09:34 CST

- 会话类型：细化节点资源补齐（数据配置）+ 最小验证
- 用户问题：确认继续补别的细化分支资源。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
- 实现结果：
  - 确认 `sequence-list` 共有 9 个二级细化节点，`linked-list` 共有 10 个二级细化节点。
  - 在 `learning_resources.json` 中为 `sequence-list` 新增 9 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 在 `learning_resources.json` 中为 `linked-list` 新增 10 条按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 继续保持“数据优先”策略，不改前端资源页、不改后端推荐逻辑。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node -e "const data=JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); const names=['sequence-list','linked-list']; const result=Object.fromEntries(names.map(name=>{ const entry=data.knowledgePointResources.find(item=>item.knowledgePointCode===name); const focused=entry.resources.filter(item=>String(item.focusNodeCode||'').startsWith(name+'-')); return [name,{resourceCount: entry.resources.length, focusedCount: focused.length, focusCodes: focused.map(item=>item.focusNodeCode)}]; })); console.log(JSON.stringify(result,null,2));"`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `sequence-list` 资源总数为 11，其中 9 条为细化节点焦点资源。
  - `linked-list` 资源总数为 12，其中 10 条为细化节点焦点资源。
- 残留观察：
  - 当前线性表主链路三组细化分支里，`linear-list`、`sequence-list`、`linked-list` 已基本补齐。
  - 其他分支仍以继承资源为主，后续可按答辩演示频率继续补 `stack`、`queue`、`string` 等节点。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 09:34 CST id=session-20260325-093400 -->

<!-- AI_GUIDE:ENTRY:BEGIN ts=2026-03-25 09:16 CST id=session-20260325-091600 -->
### 2026-03-25 09:16 CST

- 会话类型：细化节点资源补齐（数据配置）+ 最小验证
- 用户问题：继续推进细化节点资源补充，希望资源仍统一落在现有资源页中。
- 修改文件：
  - `backend/config/learning_resources.json`
  - `AI_GUIDE.md`
- 实现结果：
  - 确认 `backend/config/knowledge_graph_details.json` 中 `linear-list` 共有 9 个二级细化节点。
  - 在 `learning_resources.json` 的 `linear-list` 资源组下，为上述 9 个节点补充按 `focusNodeCode / focusNodeLabel` 绑定的焦点资源。
  - 资源覆盖保持“数据优先”策略，只补配置，不改后端推荐逻辑、不改前端资源页结构。
- 验证命令：
  - `node -e "JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); console.log('learning_resources.json OK')"`
  - `node -e "const data=JSON.parse(require('fs').readFileSync('backend/config/learning_resources.json','utf8')); const entry=data.knowledgePointResources.find(item=>item.knowledgePointCode==='linear-list'); const focused=entry.resources.filter(item=>String(item.focusNodeCode||'').startsWith('linear-list-')); console.log(JSON.stringify({linearListResourceCount: entry.resources.length, focusedCount: focused.length, focusCodes: focused.map(item=>item.focusNodeCode)}, null, 2));"`
- 验证结论：
  - `learning_resources.json` 语法正确。
  - `linear-list` 资源总数为 11，其中 9 条为细化节点焦点资源，节点编码与细化图谱一致。
- 残留观察：
  - 当前只补了 `linear-list` 这一个细化分支，其他细化分支仍以继承资源为主。
  - 本机未安装 `jq`，因此本轮 JSON 校验改用现有 `node` 完成。
<!-- AI_GUIDE:ENTRY:END ts=2026-03-25 09:16 CST id=session-20260325-091600 -->

### 2026-03-24 17:29 CST

- 会话类型：细化资源入口接入 + 统一资源页细化模式 + 前端验证
- 用户问题：同意采用“复用现有推荐资源页，不单开细化资源页”的方案，继续实现。
- 修改文件：
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/components/DetailLearningWorkspace.test.js`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/views/ResourceRecommendationView.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 统一资源上下文新增 `main / detail` 模式。
  - 细化学习页的二级路径项现在可以直接打开现有资源页查看对应细化节点资源。
  - 资源页在细化模式下会显示所属一级节点与细化范围，并改为返回细化学习页。
  - 为兼容当前后端资源行为记录接口，细化资源行为仍记录到所属一级知识点，同时 `scopeCode` 保留当前细化范围。
- 验证命令：
  - `npm run test -- --run src/components/DetailLearningWorkspace.test.js src/views/ResourceRecommendationView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向 3 个测试文件、16 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 细化资源入口和上下文已经通，但具体细化节点的资源覆盖度还需要后续继续补数据。

### 2026-03-24 16:33 CST

- 会话类型：主学习图谱交互入口收口 + 定向测试
- 用户问题：删除“该节点支持继续细化。可直接双击图中节点，或点击下方按钮进入下一层。”提示与“进入下一层图谱”按钮，统一使用双击节点进入下一层。
- 修改文件：
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/components/LearnerLearningGraph.test.js`
  - `AI_GUIDE.md`
- 实现结果：
  - 删除节点详情区中的继续细化提示区块与按钮。
  - 删除未再使用的 `openSelectedNodeDetailScope()` 和对应样式。
  - 双击节点进入下一层图谱的既有链路保持不变。
- 验证命令：
  - `npm run test -- --run src/components/LearnerLearningGraph.test.js`
- 验证结论：
  - 主学习图谱定向测试 6 个全部通过。
- 残留观察：
  - 页面顶部说明文案仍保留“双击可细化节点可进入下一层图谱”，这与当前统一交互保持一致，不属于冗余入口。

### 2026-03-24 16:22 CST

- 会话类型：细化图谱节点尺寸二次微调
- 用户问题：确认继续放大一档细化图谱节点。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页图谱节点宽高由 `120x88` 调整为 `124x90`
  - 节点文本最大宽度由 `98` 调整为 `102`
- 验证命令：
  - `npm run build`
- 验证结论：
  - 前端生产构建通过。
- 残留观察：
  - 当前尺寸已经接近建议上限，再继续放大可能会让节点较多的 scope 出现边线拥挤。

### 2026-03-24 16:20 CST

- 会话类型：细化图谱节点尺寸微调
- 用户问题：希望把细化图谱中的节点格扩大一些，但不要太大。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AI_GUIDE.md`
- 实现结果：
  - 仅调整细化学习页图谱的节点尺寸与文本可用宽度，主学习图谱不变。
  - 节点宽高由 `112x82` 调整为 `120x88`，文本最大宽度由 `92` 调整为 `98`。
- 验证命令：
  - `npm run build`
- 验证结论：
  - 前端生产构建通过。
- 残留观察：
  - 当前放大量属于温和档位，若后续继续放大，需要同步关注 breadthfirst 布局在节点较多 scope 下的拥挤程度。

### 2026-03-24 16:17 CST

- 会话类型：前端图谱角标位置微调
- 用户问题：希望把节点右下角的百分比角标再往边角贴一点，并缩小一号。
- 修改文件：
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AI_GUIDE.md`
- 实现结果：
  - 百分比角标 SVG 从 `44x20` 调整为 `38x18`
  - 角标文字字号从 `11` 调整为 `10`
  - Cytoscape 节点背景角标位置调整为更靠右下角的 `90% / 90%`
- 验证命令：
  - `npm run build`
- 验证结论：
  - 前端生产构建通过。
- 残留观察：
  - 当前改动仅为视觉参数微调，不影响图谱数据、掌握度逻辑与交互。

### 2026-03-24 16:15 CST

- 会话类型：前端图谱标签呈现调整 + 定向验证
- 用户问题：希望把所有图谱节点中的“掌握 XX%”改为右下角单独显示的 `XX%`，避免误读。
- 修改文件：
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AI_GUIDE.md`
- 实现结果：
  - `LearnerLearningGraph` 与 `DetailLearningWorkspace` 中的节点主标签不再拼接“掌握 XX%”。
  - 改为为每个节点生成独立的 SVG 百分比角标，通过 Cytoscape `background-image` 叠加到节点右下角。
  - 主标签继续用于展示知识点名称，进度逻辑、节点颜色和边状态均保持不变。
- 验证命令：
  - `npm run test -- --run src/components/LearnerLearningGraph.test.js src/components/DetailLearningWorkspace.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向 2 个测试文件、10 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 当前百分比角标尺寸固定为 44x20 像素，若后续出现极端长标签或更小节点尺寸场景，可再单独微调角标锚点和尺寸。

### 2026-03-24 16:06 CST

- 会话类型：前端状态恢复逻辑修复 + 定向测试 + 文档同步
- 用户问题：主学习图谱中的一级节点已有进度，但进入细化学习页后，对应细化节点仍不显示进度，希望主图谱进度能够反哺细化学习。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/components/DetailLearningWorkspace.test.js`
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/stores/navigationStore.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 确认问题根因不在后端聚合，而在细化学习页的分支状态记忆：旧会话里保存的整包 `masteryPercentByCode` 会在恢复页面时直接覆盖最新学习者画像。
  - 新增 `masteryOverridePercentByCode`，只保存相对当前画像基线真正被用户改动过的局部掌握度。
  - 细化学习页恢复状态时改为“最新画像基线 + 局部覆盖值”，旧版整包快照仅在当前画像基线不可用时兜底。
  - 新增前端测试，覆盖“旧快照不覆盖新画像”和“局部覆盖值仍能正确恢复”两种场景。
- 验证命令：
  - `npm run test -- --run src/components/DetailLearningWorkspace.test.js src/stores/navigationStore.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向 2 个测试文件、11 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 本轮未改动后端接口与数据库，属于前端 session 级状态恢复修复。

### 2026-03-24 15:36 CST

- 会话类型：本地数据库重建 + 初始状态校验
- 用户问题：重建数据库。
- 涉及文件：
  - `AI_GUIDE.md`
- 执行操作：
  - 确认初始化脚本为 `./scripts/init_database.sh`，目标库为 `database/data/app.db`。
  - 确认当前后端进程未明显占用数据库后，执行脚本重建数据库。
  - 使用 `sqlite3` 校验表结构与核心表记录数。
- 验证命令：
  - `./scripts/init_database.sh`
  - `sqlite3 database/data/app.db ".tables"`
  - `sqlite3 database/data/app.db "select 'courses', count(*) from courses union all select 'knowledge_points', count(*) from knowledge_points union all select 'knowledge_dependencies', count(*) from knowledge_dependencies union all select 'learners', count(*) from learners union all select 'learner_mastery', count(*) from learner_mastery union all select 'learning_feedback_records', count(*) from learning_feedback_records union all select 'learner_detail_mastery', count(*) from learner_detail_mastery union all select 'detail_learning_feedback_records', count(*) from detail_learning_feedback_records union all select 'learning_resource_view_records', count(*) from learning_resource_view_records;"`
- 验证结论：
  - 数据库已成功重建。
  - 当前演示基线状态为：`learning_feedback_records = 0`、`learner_detail_mastery = 0`、`detail_learning_feedback_records = 0`、`learning_resource_view_records = 0`。
  - 课程与演示学习者基础数据已恢复。
- 残留观察：
  - 重建刚完成时出现过一次瞬时 `database is locked`，随后复查已恢复正常，未见持续锁库问题。

### 2026-03-24 15:31 CST

- 会话类型：问题复核 + 定向验证 + 审计补录
- 用户问题：询问为什么细化学习后细化图谱没有变化，并提出下级节点学习情况应联动上级节点进度的需求。
- 涉及文件：
  - `AI_GUIDE.md`
- 复核结论：
  - 根因已在上一轮修复：原细化学习调整接口只在内存里更新局部 `masteryByCode`，没有持久化细化节点掌握度，也没有把细化结果聚合回父节点。
  - 当前代码已新增细化掌握度持久化、细化反馈记录和父节点进度聚合逻辑，且前端在细化反馈成功后会重新拉取学习者画像并刷新图谱。
  - 因此当前二级细化学习主链路上，“细化节点变化”和“父节点进度联动”都已具备。
- 验证命令：
  - `ctest --output-on-failure -R "PathPlanningService|LearnerProfile|FeedbackService|DemoResetService"`
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/DetailLearningWorkspace.test.js src/components/LearnerLearningGraph.test.js`
- 验证结论：
  - 后端定向 9 个测试全部通过。
  - 前端定向 3 个测试文件、11 个测试全部通过。
- 残留观察：
  - 如果本地数据库是在新增 `learner_detail_mastery` / `detail_learning_feedback_records` 之前创建的，需要重建数据库或补执行建表脚本，否则新持久化链路无法生效。

### 2026-03-24 15:25 CST

- 会话类型：后端持久化扩展 + 图谱掌握度聚合 + 前端刷新链路打通 + 全量验证 + 文档同步
- 用户问题：细化学习后学习图谱中的细化图谱没有变化，希望细化节点学习结果能保留下来，并让上级节点进度随下级节点学习情况联动增长。
- 修改文件：
  - `database/init/001_schema.sql`
  - `backend/repositories/DetailLearningRepository.h`
  - `backend/repositories/DetailLearningRepository.cc`
  - `backend/services/DetailScopeCatalogService.h`
  - `backend/services/DetailScopeCatalogService.cc`
  - `backend/services/DetailLearningProgressService.h`
  - `backend/services/DetailLearningProgressService.cc`
  - `backend/services/KnowledgeGraphService.cc`
  - `backend/services/LearnerProfileService.cc`
  - `backend/services/PathPlanningService.cc`
  - `backend/repositories/DemoResetRepository.cc`
  - `backend/tests/PathPlanningServiceTest.cc`
  - `backend/tests/LearnerProfileFeedbackServiceTest.cc`
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/views/DetailLearningView.vue`
  - `frontend/src/views/DetailLearningView.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增 `learner_detail_mastery` 与 `detail_learning_feedback_records`，细化学习不再只停留在局部内存状态。
  - 抽出 `DetailScopeCatalogService` 统一读取 `knowledge_graph_details.json`，避免图谱服务和进度聚合服务各自维护一份细化目录解析逻辑。
  - 新增 `DetailLearningProgressService`：一方面构建图谱用 `graphMasteryByCode`，另一方面按直属细化节点的学习时长与难度做可解释加权平均，并把聚合结果写回父节点的 `learner_mastery`。
  - `PathPlanningService::buildAdjustedDetailPathPayload` 现在会在局部路径调整成功后写回当前 scope 的细化掌握度、记录细化反馈批次，并同步父节点掌握度。
  - `LearnerProfileService` 新增 `graphMasteryByCode` 与 `detailMasteryByCode` 输出，前端图谱与细化学习页改为消费该统一映射。
  - `DetailLearningWorkspace` 提交细化反馈成功后会 `emit('profile-updated')`，`DetailLearningView` 收到后重新拉取 LearnerProfile 并刷新工作区。
  - 演示重置链路已同步删除细化掌握度与细化反馈记录，避免残留状态污染。
- 验证命令：
  - `cmake -S .. -B .`
  - `cmake --build . -j$(nproc)`
  - `ctest --output-on-failure -R "PathPlanningService|LearnerProfile|FeedbackService|DemoResetService"`
  - `ctest --output-on-failure`
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/DetailLearningWorkspace.test.js src/components/LearnerLearningGraph.test.js`
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 后端 69 个测试全部通过。
  - 前端 12 个测试文件、44 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 当前细化学习的持久化与父节点聚合面向二级细化学习主链路，三级细化节点仍主要用于图谱下钻展示，不作为独立学习反馈入口。
  - 根级二级细化节点在尚未有细化持久化记录时，会默认继承所属父节点掌握度作为展示基线；一旦发生细化反馈，该 scope 下的细化节点将以持久化值为准。

### 2026-03-24 14:47 CST

- 会话类型：前端状态持久化 + 导出能力补齐 + 测试修复 + 文档同步
- 用户问题：实现“细化学习页的分支状态记忆”与“细化路径结果导出”。
- 修改文件：
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/utils/learningPathExport.js`
  - `frontend/src/stores/navigationStore.test.js`
  - `frontend/src/utils/learningPathExport.test.js`
  - `frontend/src/components/DetailLearningWorkspace.test.js`
  - `frontend/src/views/DetailLearningView.test.js`
  - `frontend/src/graph/loadCytoscape.js`
  - `frontend/src/test/setup.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - `navigationStore` 新增 `detailLearningViewStates`，按 `scopeCode` 记住细化学习页的目标节点、时间预算、掌握度临时调整、反馈草稿、局部路径结果、局部反馈摘要与路径前后对比。
  - `DetailLearningWorkspace` 进入或切换二级分支时，会优先恢复该分支的局部状态；若已有局部路径结果，则直接恢复而不重复自动规划。
  - 细化学习页新增“导出当前细化路径”按钮，复用 `learningPathExport`，导出文本可带 `scopeLabel`、`documentTitle` 与 `filePrefix`。
  - 为稳定图谱相关测试，新增 `loadCytoscape` 加载入口，并在测试 setup 中补充最小 canvas stub。
- 验证命令：
  - `npm run test -- --run src/stores/navigationStore.test.js src/utils/learningPathExport.test.js src/components/DetailLearningWorkspace.test.js src/views/DetailLearningView.test.js`
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 4 个前端测试文件、14 个测试全部通过。
  - 前端 12 个测试文件、43 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 当前细化学习分支状态记忆只针对二级细化学习页，不影响主学习图谱页的状态持久化逻辑。
  - `frontend/src/test/setup.js` 中的 canvas stub 仅服务于 jsdom 测试环境，不影响运行时浏览器行为。

### 2026-03-24 14:16 CST

- 会话类型：前端交互去重 + 验证 + 文档同步
- 用户问题：删除细化学习图谱模块中的“设置当前细化学习目标”按钮，因为它与点击图谱选择目标重复。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 删除节点详情区“设为当前细化学习目标”按钮。
  - 同步移除 `.detail-actions`、`.detail-action-button` 及其 hover/disabled 相关无用样式。
  - 细化图谱点击节点后自动同步为下方细化路径规划目标的逻辑保持不变。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 细化学习相关定向测试通过。
  - 前端生产构建通过。
- 残留观察：
  - 细化学习页目前“图谱点击选目标”已成为唯一目标切换入口，交互更单一；如后续再加辅助入口，需要优先审查是否真正必要。

### 2026-03-24 14:07 CST

- 会话类型：前端页面级样式收敛 + 验证 + 文档同步
- 用户问题：确认继续后，按“轻量复用、不改现有效果”的方向继续做第二轮清理。
- 修改文件：
  - `frontend/src/style.css`
  - `frontend/src/views/LearnerProfileView.vue`
  - `frontend/src/views/LearningGraphView.vue`
  - `frontend/src/views/DetailLearningView.vue`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增 `surface-panel`，统一页面卡片底色、边框、圆角、阴影和 `content-visibility` 行为。
  - 新增 `page-section-label`，统一页面头部小标签的字号、字距和大写风格。
  - 新增 `page-error-panel`，统一页面级错误卡片颜色语义。
  - 4 个独立页面改为复用上述工具类，并删除原先重复的页面级面板和标签样式定义。
  - 没有改动页面交互、接口调用、状态流和后端逻辑。
- 验证命令：
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 前端 11 个测试文件、39 个测试全部通过。
  - 前端生产构建通过。
- 残留观察：
  - 目前只收敛了页面级通用面板样式，组件级 `.label` 与局部按钮样式仍有重复，但再往下抽象的边际收益开始下降，需谨慎评估。
  - `LearningGraphView`、`DetailLearningView`、`ResourceRecommendationView` 仍保留各自的局部布局和动作按钮样式，这是刻意保留的页面差异，不属于本轮冗余。

### 2026-03-24 13:55 CST

- 会话类型：全仓审查式清理 + 验证 + 文档同步
- 用户问题：检查当前所有前后端代码与逻辑，去除冗余 / 重复 / 无效 / 失效结构，使整体更清晰且不影响现有功能与展示效果。
- 修改文件：
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/views/LearnerProfileView.vue`
  - `frontend/src/components/KnowledgeGraphPreview.vue`
  - `frontend/src/components/KnowledgeGraphPreview.test.js`
  - `frontend/src/graph/demoGraph.js`
  - `backend/config/config.yaml`
  - `backend/models/model.json`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 通过源码引用回扫确认 `KnowledgeGraphPreview` 已完全退出当前运行链路，当前图谱主链路由 `LearnerLearningGraph` 与 `DetailLearningWorkspace` 承接，因此删除旧组件与其专属测试。
  - 删除未被任何运行代码引用的 `frontend/src/graph/demoGraph.js`，避免把早期本地示例数据继续误认为当前图谱数据来源。
  - 删除后端未被启动脚本、构建流程、测试和文档引用的 `backend/config/config.yaml` 与 `backend/models/model.json`，收敛 Drogon 初始化遗留模板噪音，明确当前运行配置以 `backend/config/config.json` 为准。
  - 清理 `navigationStore` 中未使用的旧状态字段 `selectedCourse`、`recommendedPath`、`graphFilter`。
  - 修复 `LearnerProfileView` 中重复的 `.hero-note` 选择器，去除无效样式定义。
- 验证命令：
  - `npm run test -- --run`
  - `npm run build`
  - `cmake -S .. -B .`
  - `cmake --build . -j$(nproc)`
  - `ctest --output-on-failure`
- 验证结论：
  - 前端 11 个测试文件、39 个测试全部通过。
  - 前端生产构建通过。
  - 后端重新配置、构建通过。
  - 后端 69 个测试全部通过。
- 残留观察：
  - `AIREAD.md` 与 `AI_GUIDE.md` 中仍保留大量 `KnowledgeGraphPreview` 的历史记录，这是审计留痕，不属于运行冗余，本轮不清理。
  - 本轮没有做跨组件样式抽象或更深层的后端接口重构，以避免在当前演示稳定阶段引入回归。

### 2026-03-24 13:42 CST

- 会话类型：前端样式收敛 + 文档同步
- 用户问题：确认继续优化后，按建议将细化学习页下半部分卡片配色向主学习图谱风格收紧。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页下半部分的路径规划、反馈、操作摘要、路径变化卡片已改为偏冷灰蓝的面板色与边框。
  - 表单下拉框、路径项、反馈项同步调整为灰蓝底色。
  - 主操作按钮改为粉色渐变，次级按钮与快捷预设按钮改为灰蓝 / 浅蓝语义。
  - 当前整页在视觉上更接近主学习图谱的上下统一风格。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建均通过，确认本轮样式收敛未破坏细化学习页与首页路径规划链路。

### 2026-03-24 13:37 CST

- 会话类型：前端视觉语义调整 + 文档同步
- 用户问题：细化图谱改用“浅粉 0%-35 / 中粉 36%-70 / 深粉 71%-100”颜色，并将描边统一改为虚线。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页图谱的节点颜色已切换为与主学习图谱一致的三段式粉色渐变。
  - 细化图谱图例同步改为“浅粉：0% - 35% / 中粉：36% - 70% / 深粉：71% - 100%”。
  - 连线默认颜色已调整为主学习图谱同款粉色系。
  - 节点描边统一改为虚线，包括默认节点、当前选中节点和上下关联高亮节点。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建均通过，确认本轮视觉语义调整未破坏细化学习页与首页路径规划链路。

### 2026-03-24 13:28 CST

- 会话类型：前端 UI 收敛 + 文档同步
- 用户问题：将细化学习的 ui 与 style 改为主学习图谱的一样。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页上半部分图谱卡片已改为与主学习图谱一致的结构：
    - 当前范围条 / 面包屑样式
    - 图谱统计区
    - 同款图例样式
    - 图谱下方节点详情面板
    - 直接前置 / 直接后继关系区
  - 图谱高亮语义也已对齐为主学习图谱同款：当前选中节点强调、上下关联节点与边增强、其它节点变暗。
  - 细化学习特有的下半部分路径规划、反馈与路径变化区保持不变。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建均通过，确认本轮 UI 收敛未破坏细化学习页与首页路径规划链路。

### 2026-03-24 13:11 CST

- 会话类型：前端默认规则调整 + 文档同步
- 用户问题：将细化图谱的默认规划目标设为最上边的节点。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页默认目标节点的选择逻辑由“优先末端节点”改为“优先最上游节点”。
  - 当前实现通过统计入度，优先选择入度为 0 的节点；多个候选时按 `displayOrder` 和标签排序选最靠前者。
  - 若图谱里不存在明确的最上游节点，则退回到排序后的第一个节点。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建均通过，确认本轮规则调整未破坏细化学习页和首页路径规划链路。

### 2026-03-24 13:03 CST

- 会话类型：前端交互收紧 + 文档同步
- 用户问题：当前细化学习图谱已经能看到了，希望图谱固定住，不可拖拽/缩放，仅可点击选择学习节点。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习图谱关闭了画布拖拽与滚轮缩放。
  - 节点仍保持不可被用户拖动。
  - 新增节点点击事件：点击图谱节点后，会把该节点同步为下方“当前范围目标节点”。
  - 图谱顶部说明文案与规则提示已同步改为“固定展示 + 点击选目标”。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建均通过，确认本轮交互收紧未破坏细化学习页与首页路径规划链路。

### 2026-03-24 12:56 CST

- 会话类型：前端问题定位 + 视口初始化修复 + 文档同步
- 用户问题：细化学习页仍然“看不到其他节点”，要求先理顺逻辑，找出真正问题所在，再修改。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 重新排查后确认：后端 `linear-list` 细化 scope 数据完整，首页到细化学习页的 `scopeCode` 传递链路也没有发现被错误替换成三级 scope。
  - 继续比对其它正常显示的图谱组件后，最终定位到更直接的根因：细化学习页独有的 `autolock: true` 会把节点默认锁定，布局算法无法把多个节点从默认位置铺开，导致多个节点重叠在同一点。
  - `DetailLearningWorkspace` 已移除 `autolock`，保留 `autoungrabify`，因此节点仍不可被用户拖拽，但布局算法可以正常展开二级图谱。
  - 未修改细化学习的数据过滤、scope 解析、二级路径规划接口或后端结构。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建均通过，确认本轮修复未破坏首页到细化学习页的既有链路和前端编译。

### 2026-03-24 12:27 CST

- 会话类型：前端交互调整 + 文档同步
- 用户问题：细化学习页图谱还是不好看，要求改成可以放大缩小和可拖动。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页二级图谱现在支持拖动画布和平滑滚轮缩放。
  - 节点仍保持固定，不允许直接拖动节点本身。
  - 已增加缩放上下限和较低的滚轮灵敏度，避免一滚就跳太远。
  - 图谱说明文案同步更新为“支持拖动画布与滚轮缩放查看细节”。
- 验证命令：
  - `npm run build`
- 验证结论：
  - 前端生产构建通过，Cytoscape 交互配置调整未引入编译问题。

### 2026-03-24 12:24 CST

- 会话类型：前端样式诊断 + 修复 + 文档同步
- 用户问题：细化学习页里仍然看不到想要的灰色未学习节点 / 路径，要求理清逻辑并一次修好。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 诊断确认：当前图谱不是节点缺失，而是把“学习状态图”和“局部路径结果图”混在一张图里画，自动生成路径后的描边覆盖了未学习灰色语义。
  - 细化学习图谱现在改为只表达“学习状态 + 当前规划目标”：
    - 未学习节点 / 路径保持灰色
    - 已学习节点按掌握度使用粉色梯度
    - 图上只保留当前规划目标的红色描边
  - 局部路径结果仍保留在图谱下方的列表、反馈和路径变化区，不再叠加到图上。
  - 图例已同步去掉“本轮学习 / 下一轮建议”这类图上路径说明。
- 验证命令：
  - `npm run build`
- 验证结论：
  - 前端生产构建通过，图谱样式收紧未引入编译问题。

### 2026-03-24 12:16 CST

- 会话类型：前端样式调整 + 文档同步
- 用户问题：细化学习的细化图谱要把所有当前节点的细化节点展示出来，灰色为未学习节点/路径，尽量按照学习图谱的样式来。
- 修改文件：
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 细化学习页继续展示当前二级 scope 内的全部细化节点和依赖边。
  - 未学习节点改为灰色背景、灰色描边和灰色文字，未学习路径改为灰色连线。
  - 已学习 / 学习中的节点继续按掌握度使用浅粉到深粉的进度色。
  - 本轮学习、下一轮建议、当前目标的路径描边高亮仍保留。
  - 细化学习图谱图例新增“灰色：未学习节点 / 路径”。
- 验证命令：
  - `npm run test -- --run src/views/DetailLearningView.test.js src/components/PathPlannerPanel.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试通过。
  - 前端生产构建通过，样式与脚本调整未引入编译问题。

### 2026-03-24 12:07 CST

- 会话类型：前端功能扩展 + 文档同步
- 用户问题：开展细化学习路径开发，保持首页一级路径规划不动，在首页本轮推荐学习节点增加“细化学习”按钮，进入新的细化学习页面，展示静态二级图谱和二级路径规划。
- 修改文件：
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/PageLayout.vue`
  - `frontend/src/router/index.js`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/views/DetailLearningView.vue`
  - `frontend/src/components/DetailLearningWorkspace.vue`
  - `frontend/src/stores/navigationStore.test.js`
  - `frontend/src/components/PageLayout.test.js`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/views/HomeView.test.js`
  - `frontend/src/views/DetailLearningView.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 前端新增 `detailLearningContext`，可跨页面保存当前路径中的可细化一级节点与当前选中的二级 scope。
  - 首页路径规划结果中的本轮节点在存在 `detailScopeCode` 时会显示“细化学习”按钮，点击后直接跳转到新页面。
  - 全站导航新增“细化学习”，在无细化上下文时会自动禁用。
  - 新增 `DetailLearningView` 统一承接所有二级细化学习分支，只展示当前路径中存在细化图谱的一级节点。
  - 新增 `DetailLearningWorkspace`，以“只读静态二级图谱 + 二级路径规划 + 局部反馈 + 路径变化”的结构展示当前二级范围，不再继续下钻三级节点。
  - 首页恢复演示初始状态时，会同步清空细化学习上下文，避免旧分支残留。
- 验证命令：
  - `npm run test -- --run src/stores/navigationStore.test.js src/components/PageLayout.test.js src/components/PathPlannerPanel.test.js src/views/HomeView.test.js src/views/DetailLearningView.test.js`
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 前端定向测试 `21/21` 通过。
  - 前端全量测试 `42/42` 通过。
  - 前端生产构建通过，新增路由、页面和组件无模板或脚本错误。

### 2026-03-24 11:20 CST

- 会话类型：前端局部裁剪 + 文档同步
- 用户问题：把辅助摘要删除了。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/views/HomeView.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 首页扩展区中的“辅助摘要 / 当前导航速览”卡片已删除。
  - 右侧“最近一次反馈或回退”与下方“路径变化”仍保留。
  - 首页不再向 `PathPlannerPanel` 传递仅供该卡片使用的 `resourceViewRecordCount`。
- 验证命令：
  - `npm run test -- --run src/components/PathPlannerPanel.test.js src/views/HomeView.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建通过，确认本次删除只影响辅助摘要卡片本身。

### 2026-03-24 11:09 CST

- 会话类型：误删恢复 + 文档同步
- 用户问题：恢复改动，我的意思不是这样的。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/views/HomeView.test.js`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 首页“扩展区：反馈与路径变化”整体恢复。
  - 首页重新显示反馈表单、快捷预设、回退按钮、辅助速览、最近一次反馈或回退摘要，以及路径变化对比区。
  - 首页说明文案、测试桩和 README 已恢复到与当前界面一致的状态。
- 验证命令：
  - `npm run test -- --run src/components/PathPlannerPanel.test.js src/views/HomeView.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建通过，确认上一轮误删已恢复。

### 2026-03-24 10:48 CST

- 会话类型：前端裁剪 + 文档同步
- 用户问题：把首页的扩展区反馈与路径变化模块删除。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/views/HomeView.test.js`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 首页路径规划结果下方整块“扩展区：反馈与路径变化”已移除。
  - 首页不再展示学习反馈表单、回退按钮、当前导航速览、最近一次操作摘要和路径变化对比。
  - 首页说明文案已收紧为“目标设置、路径生成与结果导出”。
  - 定向测试已同步改为校验首页不再渲染扩展区相关文案。
- 验证命令：
  - `npm run test -- --run src/components/PathPlannerPanel.test.js src/views/HomeView.test.js`
  - `npm run build`
- 验证结论：
  - 前端定向测试与生产构建通过后，可确认首页已不再出现“反馈与路径变化”“当前导航速览”“提交反馈并调整路径”等界面元素。

### 2026-03-24 10:35 CST

- 会话类型：样式修复 + 验证
- 用户问题：推荐资源页中的标签出现重叠，需要合理调整位置。
- 修改文件：
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `AI_GUIDE.md`
- 实现结果：
  - 资源详情卡片中的标签区域新增 `resource-tag-stack` 容器。
  - “预习 / 对应细化节点”保留在上层 `resource-tag-row`。
  - “线性结构 / 顺序表示 / 链式表示”这类聚焦标签放到下层 `resource-focus-tags`，并增加层间距与换行行距。
  - 这样在两列展示和标签自动换行时，不会再与上层标签发生视觉压叠。
- 验证命令：
  - `npm run build`
- 验证结论：
  - 前端生产构建通过，模板与样式改动无语法问题。

### 2026-03-24 10:28 CST

- 会话类型：实现 + 验证 + 文档同步
- 用户问题：把这几个还没做的做一下。
- 修改文件：
  - `backend/services/LearningResourceService.cc`
  - `backend/config/learning_resources.json`
  - `backend/tests/PathPlanningServiceTest.cc`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/views/LearningGraphView.vue`
  - `frontend/src/views/LearnerProfileView.vue`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/components/PageLayout.vue`
  - `frontend/src/components/HealthStatusCard.vue`
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/components/KnowledgeGraphPreview.vue`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `README.md`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 后端资源装配新增细化 scope 目录解析与上层节点回溯逻辑，细化节点现在可按“直配资源 -> 焦点资源 -> 上层继承资源”顺序拿到推荐资源。
  - 资源响应新增 `resourceCoverageType`、`inheritedFromCode`、`inheritedFromLabel` 字段，前端资源卡片与图谱局部资源区会显式显示“来自上层：某节点”。
  - `learning_resources.json` 中部分过度绑到三级节点的资源被放宽为上层通用资源，方便在细化节点缺少专属资源时安全复用。
  - 首页、学习图谱、推荐资源、学习者画像页的顶部说明文案已压缩，资源快照缺失、资源页脱离快照、后端未启动、画像读取失败等关键异常态提示已收敛为更直接的可回退说明。
  - `README.md`、`AIREAD.md`、`AI_GUIDE.md` 已同步记录本轮能力与验证结论。
- 验证命令：
  - `cmake -S .. -B .`
  - `cmake --build . -j$(nproc)`（`backend/build`）
  - `ctest --output-on-failure`（`backend/build`）
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 后端完整测试 `69/69` 通过，且已包含新增的 `LearningResourceServiceFallsBackToAncestorResourcesForDetailNode`。
  - 前端测试 `36/36` 通过，生产构建通过。
  - 当前已落地细化节点资源覆盖已补齐到 `409/409`，前端可通过“来自上层”标签诚实展示资源来源。

### 2026-03-24 09:52 CST

- 会话类型：文档同步 + 自检
- 用户问题：可以（继续下一项 README 收口）。
- 修改文件：
  - `README.md`
  - `AI_GUIDE.md`
- 实现结果：
  - README 的“当前已实现”补入了 `overallExplanation`、学习路径导出、图谱状态记忆与反馈快捷录入说明。
  - README 新增“推荐演示顺序”，把最短答辩链路整理成 8 步，便于直接照着演示。
  - README 手动测试清单补入图谱状态记忆、路径总说明、路径导出、反馈快捷预设等检查点。
  - README 测试章节同步补充了前端最小测试目前覆盖的重点能力。
- 验证命令：
  - `sed -n '30,120p' README.md`
  - `sed -n '120,200p' README.md`
  - `sed -n '280,320p' README.md`
- 验证结论：
  - README 新增内容与当前实现相符。
  - 本轮仅为文档同步，不涉及代码逻辑变更，因此未额外执行构建或自动化测试。

### 2026-03-24 09:45 CST

- 会话类型：实现 + 验证
- 用户问题：可以（继续推进下一项收口功能）。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/KnowledgeGraphPreview.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/utils/feedbackQuickPreset.js`
  - `frontend/src/utils/feedbackQuickPreset.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 首页路径规划反馈区新增三种快捷录入按钮：`已完成并掌握`、`看过但不熟`、`未完成`。
  - 细化图谱的局部反馈区同步接入同一套快捷录入按钮，保证两套反馈入口的默认语义一致。
  - 新增 `feedbackQuickPreset` 工具，统一把快捷操作映射为 `completionStatus + selfRatedMastery`，避免组件内各自维护不同规则。
  - 快捷录入只负责快速填值，原有下拉框与滑块仍可继续手动微调，不影响精细演示。
- 验证命令：
  - `npm run test -- --run src/utils/feedbackQuickPreset.test.js src/components/PathPlannerPanel.test.js src/components/KnowledgeGraphPreview.test.js`
  - `npm run build`
  - `npm run test -- --run`
- 验证结论：
  - 新增工具测试通过，覆盖三种快捷预设输出。
  - 首页路径规划组件测试通过，确认点击快捷预设后会同步更新完成状态与掌握度滑块。
  - 细化图谱现有测试继续通过，前端全量测试与生产构建通过，未发现回归。

### 2026-03-24 09:33 CST

- 会话类型：实现 + 验证
- 用户问题：可以（继续做路径总说明）。
- 修改文件：
  - `backend/services/PathPlanningService.cc`
  - `backend/tests/PathPlanningServiceTest.cc`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/utils/learningPathExport.js`
  - `frontend/src/utils/learningPathExport.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 后端新增 `overallExplanation`，基于目标节点、前置依赖、掌握度差距、本轮安排、下一轮建议与预算判断生成整条路径的总体说明。
  - 前端在路径规划摘要下方新增“路径总说明”卡片，优先展示整条路径的解释。
  - 导出文本同步带上该说明，保证页面展示和导出结果一致。
- 验证命令：
  - `npm run test -- --run src/components/PathPlannerPanel.test.js src/utils/learningPathExport.test.js`
  - `cmake --build . -j$(nproc)`（`backend/build`）
  - `ctest --output-on-failure`（`backend/build`）
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 前端定向测试通过。
  - 后端编译通过。
  - 后端完整测试在沙箱内首次执行时，最后 5 个测试因执行权限未启动；提权重跑后 `68/68` 全部通过。
  - 前端全量测试与生产构建通过。

### 2026-03-24 09:20 CST

- 会话类型：实现 + 验证
- 用户问题：继续。
- 修改文件：
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/PathPlannerPanel.test.js`
  - `frontend/src/utils/learningPathExport.js`
  - `frontend/src/utils/learningPathExport.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 首页路径规划结果区新增“导出本次学习路径”按钮。
  - 新增 `learningPathExport` 工具，用于生成导出文本与触发浏览器下载。
  - 导出文本内容覆盖目标、时间预算、规划摘要、本轮路径、下一轮建议及资源建议摘要。
  - 导出失败时会在路径规划区显示明确错误信息。
- 验证命令：
  - `npm run test -- --run src/utils/learningPathExport.test.js src/components/PathPlannerPanel.test.js`
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 导出工具文本拼装测试通过。
  - 组件下载动作测试通过。
  - 全量前端测试与生产构建通过。

### 2026-03-24 09:06 CST

- 会话类型：实现 + 验证
- 用户问题：图谱状态记忆先把这个弄了。
- 修改文件：
  - `frontend/src/stores/navigationStore.js`
  - `frontend/src/stores/navigationStore.test.js`
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/components/LearnerLearningGraph.test.js`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/views/HomeView.test.js`
  - `AIREAD.md`
  - `AI_GUIDE.md`
- 实现结果：
  - 新增 `learningGraphViewState`，使用 `sessionStorage` 记忆当前图谱 scope、当前选中节点、最近一次 focus 节点及其所在 scope。
  - 学习图谱页在首次加载、切换 scope、点击节点后都会自动写回状态。
  - 从首页带 `focus` 跳到图谱页时，显式 focus 优先于旧状态；无新 focus 时，图谱页恢复到上次浏览层级与选中节点。
  - 首页演示重置新增清理学习图谱状态，保证“恢复演示初始状态”后图谱页回到根层。
- 验证命令：
  - `npm run test -- --run src/stores/navigationStore.test.js src/components/LearnerLearningGraph.test.js src/views/HomeView.test.js src/views/LearningGraphView.test.js`
  - `npm run test -- --run`
  - `npm run build`
- 验证结论：
  - 定向测试通过，覆盖 store 持久化、图谱状态恢复、显式 focus 优先、首页重置清空图谱状态。
  - 全量前端测试通过，未发现回归。
  - 前端生产构建通过。

### 2026-03-24 08:55 CST

- 会话类型：仅分析 / 仅答复
- 用户问题：前面列出的收口优化任务，还有哪些没完成？
- 读取文件：
  - `AI_GUIDE.md`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/views/LearningGraphView.vue`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/components/LearnerProfileCard.vue`
  - `frontend/src/views/LearnerProfileView.vue`
  - `frontend/src/stores/navigationStore.js`
  - `backend/services/PathPlanningService.cc`
  - `backend/config/learning_resources.json`
  - `backend/config/knowledge_graph_details.json`
- 结论：
  - “恢复演示初始状态”入口已完成并接通前后端。
  - 学习图谱支持 focus 跳转、面包屑与逐层下钻，但没有跨页面记忆最近 scope / 选中节点 / focus 状态。
  - 路径项级推荐理由已完成，但“整条路径总说明”尚未实现。
  - 路径结果导出功能尚未实现。
  - 资源覆盖对 14 个顶层节点已完整配置，但细化节点仅有部分焦点资源，尚未系统补齐。
  - 学习反馈已有回退，但缺少“已完成并掌握 / 看过但不熟 / 未完成”这类一键预设按钮。
- 验证依据：
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/components/LearnerLearningGraph.vue`
  - `frontend/src/views/LearningGraphView.vue`
  - `frontend/src/views/ResourceRecommendationView.vue`
  - `frontend/src/components/LearnerProfileCard.vue`
  - `frontend/src/stores/navigationStore.js`
  - `backend/services/PathPlanningService.cc`
  - `backend/config/learning_resources.json`
  - `backend/config/knowledge_graph_details.json`

### 2026-03-24 08:50 CST

- 会话类型：仅分析 / 仅答复
- 用户问题：初始演示状态是什么状态？
- 读取文件：
  - `AIREAD.md`
  - `backend/services/DemoResetService.cc`
  - `backend/services/LearnerProfileService.cc`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/stores/navigationStore.js`
  - `backend/tests/LearnerProfileFeedbackServiceTest.cc`
- 结论：
  - 演示重置会恢复默认学习者 `demo-learner` 的 14 个知识点掌握度基线。
  - 会清空该学习者的反馈记录与资源行为记录。
  - 首页路径规划器在无显式目标时，会默认选中知识图谱中的最后一个顶层节点作为目标。
- 验证依据：
  - `backend/services/DemoResetService.cc`
  - `backend/services/LearnerProfileService.cc`
  - `backend/tests/LearnerProfileFeedbackServiceTest.cc`
  - `frontend/src/components/PathPlannerPanel.vue`
  - `frontend/src/views/HomeView.vue`
  - `frontend/src/stores/navigationStore.js`

## 5. 修改标记 / 审计轨迹

- 本轮新增：`AI_GUIDE.md`
- 本轮反馈语义细化改动：
  - `backend/algorithm/adjuster/LearningPathAdjuster.cc`：将 `partial` 更新规则改为允许小幅上调与小幅回退，单次回退上限为 `15%`
  - `backend/tests/PathAdjusterTest.cc`：新增“部分完成允许小幅回退”用例，覆盖真实教学语义下的回退边界
- 本轮反馈联动改动：
  - `frontend/src/utils/feedbackQuickPreset.js`：抽出完成情况对应的掌握度区间、默认草稿与归一化逻辑，统一首页与细化学习页的反馈规则
  - `frontend/src/components/PathPlannerPanel.vue`：首页反馈滑块改为按状态动态限制区间，并显式显示可调范围
  - `frontend/src/components/DetailLearningWorkspace.vue`：细化学习反馈滑块同步接入同一套状态区间规则
  - `backend/algorithm/adjuster/LearningPathAdjuster.cc`：后端在应用反馈更新前按完成情况裁剪 `selfRatedMastery`
  - `backend/tests/PathAdjusterTest.cc`、`frontend/src/utils/feedbackQuickPreset.test.js`、`frontend/src/components/PathPlannerPanel.test.js`：补充前后端联动规则的回归测试
- 本轮前端状态管理改动：
  - `frontend/src/stores/navigationStore.js`：新增学习图谱状态持久化
  - `frontend/src/components/LearnerLearningGraph.vue`：新增图谱页恢复与写回逻辑
  - `frontend/src/views/HomeView.vue`：演示重置时同步清空图谱浏览状态
- 本轮路径导出改动：
  - `frontend/src/components/PathPlannerPanel.vue`：新增路径导出按钮与导出失败提示
  - `frontend/src/utils/learningPathExport.js`：新增导出文本生成与下载逻辑
- 本轮路径总说明改动：
  - `backend/services/PathPlanningService.cc`：新增整体解释聚合
  - `frontend/src/components/PathPlannerPanel.vue`：新增路径总说明展示卡片
  - `frontend/src/utils/learningPathExport.js`：导出文本同步纳入整体解释
- 本轮反馈快捷录入改动：
  - `frontend/src/components/PathPlannerPanel.vue`：新增首页学习反馈快捷预设按钮
  - `frontend/src/components/KnowledgeGraphPreview.vue`：新增细化图谱局部反馈快捷预设按钮
  - `frontend/src/utils/feedbackQuickPreset.js`：新增反馈快捷预设统一逻辑
- 本轮代码清理改动：
  - `frontend/src/components/KnowledgeGraphPreview.vue`、`frontend/src/components/KnowledgeGraphPreview.test.js`、`frontend/src/graph/demoGraph.js`：删除已退出当前运行链路的旧图谱实现与示例数据
  - `backend/config/config.yaml`、`backend/models/model.json`：删除未被运行、构建和文档使用的 Drogon 初始化模板
  - `frontend/src/stores/navigationStore.js`：移除未使用的旧状态字段
  - `frontend/src/views/LearnerProfileView.vue`：移除重复的 `.hero-note` 样式选择器
  - `frontend/src/style.css`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/views/LearningGraphView.vue`、`frontend/src/views/DetailLearningView.vue`、`frontend/src/views/ResourceRecommendationView.vue`：抽出页面级公共面板样式工具类，收敛重复页面样式
- 本轮资源覆盖与提示文案改动：
  - `backend/services/LearningResourceService.cc`：新增细化节点资源继承兜底与资源来源字段
  - `backend/services/LearningResourceService.cc`：新增同层关联节点资源补充与确定性多样性排序，减少资源列表前几项长期被同一来源占满
  - `backend/config/learning_resources.json`：为 `ds-intro`、`algorithm-analysis`、`kmp`、`tree-basic`、`binary-tree-traversal`、`huffman-tree`、`graph-basic`、`topological-sort` 这 8 个一级节点补齐直连资源，提升一级资源页的候选池厚度
  - `backend/config/learning_resources.json`：收紧部分资源的焦点绑定，改为可被上层节点复用的通用资源
  - `backend/config/learning_resources.json`：为 `linear-list` 的 9 个二级细化节点补齐按 `focusNodeCode` 绑定的焦点资源
  - `backend/config/learning_resources.json`：为 `sequence-list` 的 9 个二级细化节点补齐按 `focusNodeCode` 绑定的焦点资源
  - `backend/config/learning_resources.json`：为 `linked-list` 的 10 个二级细化节点补齐按 `focusNodeCode` 绑定的焦点资源
  - `backend/config/learning_resources.json`：为 `stack` 的 9 个二级细化节点、`queue` 的 9 个二级细化节点、`string` 的 8 个二级细化节点补入第二条焦点资源
  - `backend/tests/PathPlanningServiceTest.cc`：新增关键细化节点焦点资源数量约束测试
  - `frontend/src/views/ResourceRecommendationView.vue`：资源页新增“来自上层 / 来自关联节点”来源标签区分与统一兜底提示
  - `frontend/src/components/KnowledgeGraphPreview.vue`：局部资源区同步显示继承来源标记
  - `frontend/src/views/HomeView.vue`、`frontend/src/views/LearningGraphView.vue`、`frontend/src/views/LearnerProfileView.vue`、`frontend/src/components/PageLayout.vue`：页面说明文案压缩并统一关键异常态提示语气
  - `frontend/src/views/ResourceRecommendationView.vue`：资源页标签区改为上下两层堆叠，避免阶段标签与聚焦标签视觉重叠
  - `frontend/src/components/PathPlannerPanel.vue`、`frontend/src/views/HomeView.vue`：首页删除“反馈与路径变化”扩展区的改动已在后续会话中撤回
- 目的：
  - 补齐项目级 AI 审计文件
  - 提升学习图谱页在跨页面切换时的连续性与答辩演示效率
  - 提升路径规划结果在答辩与论文附录中的可复用性
  - 让整条路径的推荐逻辑更容易直接讲给老师听
  - 缩短学习反馈录入步骤，提升答辩演示连贯性
  - 用可解释的轻量策略补齐细化节点资源覆盖，同时避免把继承资源伪装成节点专属资源
- 残留风险：
  - 当前只记忆“当前 scope + 当前节点 + 最近 focus”，还没有做按 scope 维度的多层历史缓存
  - 如果后续引入从细化资源页直接跳图谱的深层 focus，可能还需要补更强的节点到 scope 映射
  - 当前导出格式为纯文本，适合最小可用版本；若后续要做打印页或 PDF 风格导出，需要再做版式层封装
  - 当前整体解释仍是规则聚合文本，适合毕设阶段；若后续文案要更精细，可继续微调聚合模板
  - 快捷预设当前是面向演示的固定三档规则；如果后续需要更贴近真实教学场景，可以再加“完成但仍需复习”等更细分档位
  - 细化节点资源覆盖目前依赖“上层通用资源继承”，适合毕设演示与主链路展示；若后续要做课程级精品资源库，仍需逐步补更多节点专属资源
- 首页扩展区较长，后续如果继续调版式，应优先做局部压缩或折叠，而不是直接删除整块功能
- 当前仍可能存在少量“仅历史文档提及但已不在运行链路中”的旧文件名，这是审计与演进记录，不应与运行冗余混为一谈
- 页面级样式收敛已经完成一轮，但组件级公共样式是否值得继续抽象，仍需以“不改变现有效果”为前提逐项评估

## 6. 待办与后续动作

- 优先待办建议：
- 测试环境收口：排查 `LearnerProfileServiceBuildsGraphMasteryForDetailNodes` 在全量 `ctest` 中触发的 SQLite `SQLITE_CONFIG_MULTITHREAD is not supported` 既有问题，避免后续把资源逻辑改动与数据库线程问题混淆。
- 若继续做前端审查，下一步更适合检查“相似错误态 / 空态文案是否值得统一组件化”，而不是继续扩大 CSS 抽象面。
- 如后续继续做结构性清理，可优先审查各页面顶部说明卡与共享卡片样式是否值得做轻量复用，但不建议在当前答辩演示阶段做大抽象。
- 资源质量细修：优先继续补“高频路径节点 + 薄弱点节点”的专属资源，逐步减少对上层继承资源的依赖。
- 细化资源补齐建议：下一轮优先继续补 `stack`、`queue`、`string` 这类答辩里更容易演示完整学习路径的分支。
- 细化资源补齐建议：下一轮更适合继续补 `graph-basic`、`topological-sort`、`tree-basic` 这类后段章节分支，形成更完整的全课程覆盖。
- 演示材料整理：可把导出路径文本、资源页截图、图谱下钻截图整理进论文附录和答辩 PPT。
- 异常态抽象：当前关键空态和错误态文案已统一，但若后续页面继续增加，可考虑抽统一状态组件，前提是不引入过度抽象。
- 标签样式复用：如果后续首页或图谱页也出现类似的多层标签排布，可把当前资源页的堆叠样式抽成可复用片段。
- 如后续希望支持多个演示场景，可把当前硬编码基线抽为数据库种子或配置文件。
