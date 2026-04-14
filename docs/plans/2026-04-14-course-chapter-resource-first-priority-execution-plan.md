# 课程直链资源优先 Implementation Plan

> For agentic workers: REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 把已确认的超星课程小章直链接入推荐资源系统，并稳定放到第一顺位。

**Architecture:** 新增独立课程直链配置文件，由 `LearningResourceService` 读取并在资源装配阶段优先注入。顶层与细化节点都只使用显式映射，不做运行时模糊匹配，同时生成缺失报告用于后续补齐。

**Tech Stack:** C++ / Drogon / JSON config / Drogon tests

---

## 文件范围

- 新增：`backend/config/course_chapter_links.json`
- 新增：`docs/learn_html/url/course-chapter-link-report.json`
- 修改：`backend/config/config.json`
- 修改：`backend/services/LearningResourceService.cc`
- 修改：`backend/tests/test_main.cc`
- 修改：`backend/tests/PathPlanningServiceTest.cc`

## 执行步骤

- [ ] 先补失败测试，锁定“课程直链排第一”和“未映射节点不误注入”。
- [ ] 新增课程直链配置与配置路径。
- [ ] 在 `LearningResourceService` 中读取课程直链配置，并将其作为 `course-direct` 资源注入。
- [ ] 调整排序、分层和选择原因文案，让课程直链稳定排第一且前端可读。
- [ ] 生成缺失报告，列出顶层与细化节点当前未接入的课程直链。
- [ ] 运行后端测试，确认主路径和细化路径响应保持兼容。
