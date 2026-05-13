# Student Experience Enhancement Phase 1 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Deliver the first-stage student-side enhancements: mastery-aware resource recommendation with a four-resource cap, chart-based learner profile visualization, and a post-feedback practice-check branch that routes users into a new placeholder practice page.

**Architecture:** Keep the existing student workflow intact and extend the current backend/frontend contracts in place. Backend changes stay inside the current service-oriented Drogon stack, while frontend changes reuse the existing route, Pinia navigation context, and page/component structure so the new behavior remains explainable for defense demos without introducing a second subsystem.

**Tech Stack:** Drogon (C++17/20), SQLite, Vue 3, Vite, Pinia, Vitest, existing project test helpers

---

## Scope and decomposition

This plan intentionally covers only the spec's first phase:

1. Resource recommendation optimization
2. Learner profile chart visualization
3. Practice-check branching after feedback submission

The multi-role platform work from `docs/superpowers/specs/2026-05-12-platform-enhancement-design.md` is intentionally excluded from this execution plan because it is a separate subsystem involving authentication, authorization, schema expansion, and management UI. That work should be implemented as a dedicated second plan after phase 1 is stable.

## File map

### Backend files to modify

- `backend/services/LearningResourceService.cc`
  Responsibility: refine mastery-stage labeling, recommendation ranking, and hard-cap each node's returned resources to four items.
- `backend/services/LearnerProfileService.cc`
  Responsibility: extend learner profile payload with chart-friendly aggregates for mastery distribution, feedback trend, and feedback status composition.
- `backend/tests/PathPlanningServiceTest.cc`
  Responsibility: verify resource recommendation stage behavior, four-resource limit, and response payload expectations.
- `backend/tests/LearnerProfileFeedbackServiceTest.cc`
  Responsibility: verify new learner profile analytics payload shape and value semantics.

### Frontend files to modify

- `frontend/src/components/PathPlannerPanel.vue`
  Responsibility: insert the post-feedback practice-check choice into the main-path feedback flow.
- `frontend/src/components/PathPlannerPanel.test.js`
  Responsibility: verify the new branching behavior after successful feedback submission.
- `frontend/src/components/DetailLearningWorkspace.vue`
  Responsibility: optionally reuse the same post-feedback branch behavior for detail learning feedback so the experience is consistent.
- `frontend/src/components/DetailLearningWorkspace.test.js`
  Responsibility: verify detail feedback practice routing behavior.
- `frontend/src/components/LearnerProfileCard.vue`
  Responsibility: render the new chart sections and continue to display the existing text profile summary.
- `frontend/src/views/LearnerProfileView.vue`
  Responsibility: feed chart-capable profile data into the profile card and preserve existing reset/resource behavior.
- `frontend/src/views/LearnerProfileView.test.js`
  Responsibility: verify the profile page still loads and can render new chart-driven sections.
- `frontend/src/router/index.js`
  Responsibility: register the new practice-check route.
- `frontend/src/views/PracticeCheckView.vue`
  Responsibility: implement the placeholder practice-check landing page and display carried learning context.
- `frontend/src/views/PracticeCheckView.test.js`
  Responsibility: verify route-driven context rendering and empty-state fallback.
- `frontend/src/stores/navigationStore.js`
  Responsibility: persist practice-check navigation context across page transitions inside the existing session-scoped store.
- `frontend/src/stores/navigationStore.test.js`
  Responsibility: verify the new practice-check context persistence and clearing behavior.
- `frontend/src/views/ResourceRecommendationView.vue`
  Responsibility: present stage-oriented recommendation labels more clearly if needed by the capped resource model.

### Frontend files to create

- `frontend/src/components/profileCharts.js`
  Responsibility: provide small pure helpers that convert backend aggregates into renderable bar/line/donut data for the Vue component layer.
- `frontend/src/components/profileCharts.test.js`
  Responsibility: verify chart helper transformations without coupling tests to DOM structure.

### Docs files to modify

- `README.md`
  Responsibility: update implemented capability list and manual test notes once phase 1 ships.
- `AIREAD.md`
  Responsibility: record the system-level change because this alters learner-profile payload structure, recommendation semantics, and feedback flow.

## Task 1: Lock backend resource recommendation to stage-aware top-4 results

**Files:**
- Modify: `backend/services/LearningResourceService.cc`
- Test: `backend/tests/PathPlanningServiceTest.cc`

- [ ] **Step 1: Add a failing backend test for the four-resource cap and stage labels**

Add a new Drogon test near the existing `LearningResourceServiceDiversifiesTopRecommendations` coverage in `backend/tests/PathPlanningServiceTest.cc`:

```cpp
DROGON_TEST(LearningResourceServiceReturnsAtMostFourStageMatchedResources)
{
    algorithm::planner::LearningPathItem lowMasteryItem;
    lowMasteryItem.point.code = "queue";
    lowMasteryItem.point.name = "队列";
    lowMasteryItem.status = "scheduled";
    lowMasteryItem.masteryScore = 0.18;
    lowMasteryItem.reasonTrace.knowledgePointCode = "queue";
    lowMasteryItem.reasonTrace.relevanceScore = 0.8;
    lowMasteryItem.reasonTrace.masteryGap = 0.82;
    lowMasteryItem.reasonTrace.timeCostPenalty = 0.2;
    lowMasteryItem.reasonTrace.triggerReasons = {"当前节点掌握度较低，应优先建立基础概念。"};

    const auto lowMasteryResources =
        services::LearningResourceService::buildResourcesForLearningPathItem(
            lowMasteryItem);

    REQUIRE(lowMasteryResources.isArray());
    CHECK(lowMasteryResources.size() <= 4);
    REQUIRE(lowMasteryResources.size() >= 1);
    CHECK(lowMasteryResources[0]["recommendationStageCode"].asString() == "foundation");
    CHECK(lowMasteryResources[0]["recommendationStageLabel"].asString() == "入门讲解");

    algorithm::planner::LearningPathItem highMasteryItem;
    highMasteryItem.point.code = "sequence-list";
    highMasteryItem.point.name = "顺序表";
    highMasteryItem.status = "scheduled";
    highMasteryItem.masteryScore = 0.9;
    highMasteryItem.reasonTrace.knowledgePointCode = "sequence-list";
    highMasteryItem.reasonTrace.relevanceScore = 0.72;
    highMasteryItem.reasonTrace.masteryGap = 0.1;
    highMasteryItem.reasonTrace.timeCostPenalty = 0.18;
    highMasteryItem.reasonTrace.triggerReasons = {"当前节点已具备较高掌握度，可转入综合巩固。"};

    const auto highMasteryResources =
        services::LearningResourceService::buildResourcesForLearningPathItem(
            highMasteryItem);

    REQUIRE(highMasteryResources.isArray());
    CHECK(highMasteryResources.size() <= 4);
    REQUIRE(highMasteryResources.size() >= 1);
    CHECK(highMasteryResources[0]["recommendationStageCode"].asString() == "advanced");
    CHECK(highMasteryResources[0]["recommendationStageLabel"].asString() == "提升拓展");
}
```

- [ ] **Step 2: Run the focused backend test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R LearningResourceServiceReturnsAtMostFourStageMatchedResources
```

Expected: FAIL because `recommendationStageCode` / `recommendationStageLabel` do not yet exist and the result set is not yet explicitly capped at four.

- [ ] **Step 3: Implement mastery-stage classification and a hard top-4 cap**

Update `backend/services/LearningResourceService.cc` by extending the resource payload enrichment path. Introduce small local helpers in the anonymous namespace:

```cpp
std::string buildRecommendationStageCode(double masteryScore)
{
    if (masteryScore < 0.30)
    {
        return "foundation";
    }

    if (masteryScore < 0.60)
    {
        return "consolidation";
    }

    if (masteryScore < 0.85)
    {
        return "practice";
    }

    return "advanced";
}

std::string buildRecommendationStageLabel(const std::string &stageCode)
{
    if (stageCode == "foundation")
    {
        return "入门讲解";
    }

    if (stageCode == "consolidation")
    {
        return "巩固理解";
    }

    if (stageCode == "practice")
    {
        return "应用训练";
    }

    return "提升拓展";
}

std::string buildStageGuidance(const std::string &stageCode)
{
    if (stageCode == "foundation")
    {
        return "当前掌握度偏低，优先补基础概念与课程原文。";
    }

    if (stageCode == "consolidation")
    {
        return "当前已完成入门，需要用例题和总结巩固理解。";
    }

    if (stageCode == "practice")
    {
        return "当前已具备基础，优先进入典型题与过程训练。";
    }

    return "当前掌握度较高，减少重复入门内容，转向综合巩固与拓展。";
}
```

Then, in the final per-resource enrichment path used by `buildResourcesForLearningPathItem(...)`, write the stage metadata onto every returned resource and truncate after ranking:

```cpp
const auto stageCode = buildRecommendationStageCode(item.masteryScore);
const auto stageLabel = buildRecommendationStageLabel(stageCode);
const auto stageGuidance = buildStageGuidance(stageCode);

for (auto &resource : rankedResources)
{
    resource["recommendationStageCode"] = stageCode;
    resource["recommendationStageLabel"] = stageLabel;
    resource["stageGuidance"] = stageGuidance;
}

Json::Value topResources(Json::arrayValue);
for (Json::ArrayIndex index = 0; index < rankedResources.size() && index < 4; ++index)
{
    topResources.append(rankedResources[index]);
}

return topResources;
```

Do not rewrite the existing ranking model wholesale. Keep current diversity/ranking behavior and apply the cap only after the final ordering is computed.

- [ ] **Step 4: Run the focused backend test again**

Run from `backend/build`:

```bash
ctest --output-on-failure -R LearningResourceServiceReturnsAtMostFourStageMatchedResources
```

Expected: PASS.

- [ ] **Step 5: Strengthen the path response test to enforce the public contract**

Extend `DROGON_TEST(PathPlanningServiceBuildsLearningResourcesIntoResponse)` in `backend/tests/PathPlanningServiceTest.cc` with assertions like:

```cpp
CHECK(item["resourceCount"].asInt() <= 4);
CHECK(item["resources"][0]["recommendationStageCode"].asString().empty() == false);
CHECK(item["resources"][0]["recommendationStageLabel"].asString().empty() == false);
CHECK(item["resources"][0]["stageGuidance"].asString().empty() == false);
```

- [ ] **Step 6: Run the resource/planning backend test slice**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "LearningResourceService|PathPlanningServiceBuildsLearningResourcesIntoResponse"
```

Expected: PASS across the resource recommendation related tests.

## Task 2: Extend learner profile payload with chart-ready analytics

**Files:**
- Modify: `backend/services/LearnerProfileService.cc`
- Test: `backend/tests/LearnerProfileFeedbackServiceTest.cc`

- [ ] **Step 1: Add a failing backend test for learner profile analytics fields**

Add a new test in `backend/tests/LearnerProfileFeedbackServiceTest.cc`:

```cpp
DROGON_TEST(LearnerProfileServiceBuildsChartAnalyticsPayload)
{
    const auto payload = services::LearnerProfileService::buildProfilePayload();

    CHECK(payload["analytics"].isObject());
    CHECK(payload["analytics"]["masteryDistribution"].isArray());
    CHECK(payload["analytics"]["feedbackTrend"].isArray());
    CHECK(payload["analytics"]["feedbackStatusComposition"].isArray());

    REQUIRE(payload["analytics"]["masteryDistribution"].size() >= 1U);
    CHECK(payload["analytics"]["masteryDistribution"][0]["code"].asString().empty() == false);
    CHECK(payload["analytics"]["masteryDistribution"][0]["masteryPercent"].isInt());

    REQUIRE(payload["analytics"]["feedbackStatusComposition"].size() == 3U);
}
```

- [ ] **Step 2: Run the focused analytics test and confirm failure**

Run from `backend/build`:

```bash
ctest --output-on-failure -R LearnerProfileServiceBuildsChartAnalyticsPayload
```

Expected: FAIL because `analytics` is not yet present in the profile payload.

- [ ] **Step 3: Add analytics aggregation to the learner profile service**

Inside `backend/services/LearnerProfileService.cc`, keep all existing payload fields and append an `analytics` object. Use the already-loaded `masteryRecords` and `recentFeedbackRecords` so no extra repository is required.

Add logic equivalent to:

```cpp
Json::Value masteryDistribution(Json::arrayValue);
for (const auto &item : masteryRecords)
{
    Json::Value chartItem;
    chartItem["code"] = item.code;
    chartItem["name"] = item.name;
    chartItem["chapterNo"] = item.chapterNo;
    chartItem["masteryPercent"] = static_cast<int>(std::round(item.masteryScore * 100.0));
    masteryDistribution.append(chartItem);
}

Json::Value feedbackTrend(Json::arrayValue);
double cumulativeMasteryPercent = 0.0;
int feedbackPointCount = 0;
for (auto it = recentFeedbackRecords.rbegin(); it != recentFeedbackRecords.rend(); ++it)
{
    cumulativeMasteryPercent += std::round(it->updatedMastery * 100.0);
    ++feedbackPointCount;

    Json::Value trendItem;
    trendItem["index"] = feedbackPointCount;
    trendItem["label"] = "反馈" + std::to_string(feedbackPointCount);
    trendItem["recordedAt"] = it->recordedAt;
    trendItem["averageUpdatedMasteryPercent"] =
        static_cast<int>(std::round(cumulativeMasteryPercent / feedbackPointCount));
    trendItem["knowledgePointCode"] = it->code;
    feedbackTrend.append(trendItem);
}

int completedCount = 0;
int partialCount = 0;
int blockedCount = 0;
for (const auto &record : recentFeedbackRecords)
{
    if (record.completionStatus == "completed")
    {
        ++completedCount;
    }
    else if (record.completionStatus == "partial")
    {
        ++partialCount;
    }
    else if (record.completionStatus == "blocked")
    {
        ++blockedCount;
    }
}

Json::Value feedbackStatusComposition(Json::arrayValue);
for (const auto &[statusCode, statusLabel, statusCount] : std::vector<std::tuple<std::string, std::string, int>>{
         {"completed", "已完成", completedCount},
         {"partial", "部分完成", partialCount},
         {"blocked", "学习受阻", blockedCount}})
{
    Json::Value statusItem;
    statusItem["statusCode"] = statusCode;
    statusItem["statusLabel"] = statusLabel;
    statusItem["count"] = statusCount;
    feedbackStatusComposition.append(statusItem);
}

payload["analytics"]["masteryDistribution"] = masteryDistribution;
payload["analytics"]["feedbackTrend"] = feedbackTrend;
payload["analytics"]["feedbackStatusComposition"] = feedbackStatusComposition;
```

The trend here is intentionally simple and defense-friendly: it visualizes recent improvement direction without inventing a second analytics subsystem.

- [ ] **Step 4: Re-run the focused analytics test**

Run from `backend/build`:

```bash
ctest --output-on-failure -R LearnerProfileServiceBuildsChartAnalyticsPayload
```

Expected: PASS.

- [ ] **Step 5: Run the learner-profile/feedback backend slice**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "LearnerProfileService|FeedbackServicePersistsFeedbackAndUpdatesLearnerProfile"
```

Expected: PASS, proving the new payload shape did not break feedback/profile integration.

## Task 3: Add practice-check navigation context to the frontend store

**Files:**
- Modify: `frontend/src/stores/navigationStore.js`
- Test: `frontend/src/stores/navigationStore.test.js`

- [ ] **Step 1: Add a failing store test for practice-check context persistence**

Append a new test to `frontend/src/stores/navigationStore.test.js`:

```js
it("persists practice check context and restores it on next store init", () => {
  setActivePinia(createPinia());
  const store = useNavigationStore();

  store.setPracticeCheckContext({
    learnerCode: "demo-learner",
    sourcePage: "home",
    targetCode: "queue",
    targetName: "队列",
    scopeCode: "root",
    feedbackBatchId: "batch-123",
    feedbackItemCount: 2,
  });

  expect(store.practiceCheckContext.targetCode).toBe("queue");

  setActivePinia(createPinia());
  const restoredStore = useNavigationStore();

  expect(restoredStore.practiceCheckContext.targetCode).toBe("queue");
  expect(restoredStore.practiceCheckContext.feedbackBatchId).toBe("batch-123");
});
```

- [ ] **Step 2: Run the store test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/stores/navigationStore.test.js
```

Expected: FAIL because `practiceCheckContext` and related actions do not yet exist.

- [ ] **Step 3: Implement practice-check context state in the Pinia store**

In `frontend/src/stores/navigationStore.js`, add a new storage key, a default builder, persistence, getter, and clear/set actions following the existing resource/detail-learning pattern.

Add a default shape like:

```js
const PRACTICE_CHECK_CONTEXT_STORAGE_KEY = "plns-practice-check-context";

function buildDefaultPracticeCheckContext() {
  return {
    learnerCode: "demo-learner",
    sourcePage: "home",
    targetCode: "",
    targetName: "",
    scopeCode: "root",
    scopeLabel: "",
    feedbackBatchId: "",
    feedbackItemCount: 0,
    generatedAt: "",
  };
}
```

Then expose actions like:

```js
setPracticeCheckContext(payload = {}) {
  this.practiceCheckContext = {
    ...buildDefaultPracticeCheckContext(),
    ...payload,
    generatedAt: new Date().toISOString(),
  };
  this.persistPracticeCheckContext();
},
clearPracticeCheckContext() {
  this.practiceCheckContext = buildDefaultPracticeCheckContext();
  this.persistPracticeCheckContext();
},
```

Use a dedicated `sessionStorage` entry so the practice page can be refreshed during demos without losing context immediately.

- [ ] **Step 4: Re-run the store test**

Run from `frontend`:

```bash
npm run test -- --run src/stores/navigationStore.test.js
```

Expected: PASS.

## Task 4: Add the practice-check landing page and route

**Files:**
- Modify: `frontend/src/router/index.js`
- Create: `frontend/src/views/PracticeCheckView.vue`
- Create: `frontend/src/views/PracticeCheckView.test.js`

- [ ] **Step 1: Add a failing route/view test**

Create `frontend/src/views/PracticeCheckView.test.js` with:

```js
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { describe, expect, it } from "vitest";

import PracticeCheckView from "./PracticeCheckView.vue";
import { useNavigationStore } from "../stores/navigationStore";

describe("PracticeCheckView", () => {
  it("renders carried practice-check context", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();
    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      sourcePage: "home",
      targetCode: "queue",
      targetName: "队列",
      scopeCode: "root",
      feedbackBatchId: "batch-123",
      feedbackItemCount: 2,
    });

    const wrapper = mount(PracticeCheckView, {
      global: {
        plugins: [createPinia()],
        stubs: {
          PageLayout: {
            template: "<div><slot /></div>",
          },
        },
      },
    });

    expect(wrapper.text()).toContain("队列");
    expect(wrapper.text()).toContain("batch-123");
    expect(wrapper.text()).toContain("题目模块后续接入");
  });
});
```

- [ ] **Step 2: Run the new view test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/views/PracticeCheckView.test.js
```

Expected: FAIL because the route and view file do not exist yet.

- [ ] **Step 3: Register the route and create the placeholder page**

Update `frontend/src/router/index.js`:

```js
import PracticeCheckView from "../views/PracticeCheckView.vue";

{
  path: "/practice-check",
  name: "practice-check",
  component: PracticeCheckView,
},
```

Create `frontend/src/views/PracticeCheckView.vue` with a lightweight defense-friendly page. The page should:

1. Read `practiceCheckContext` from `useNavigationStore()`.
2. Display the current knowledge point, scope, feedback batch id, and feedback item count.
3. Show a prominent placeholder note that question authoring and objective checking are future work.
4. Offer buttons to return to the home page and learner profile page.

Use a structure like:

```vue
<template>
  <PageLayout
    eyebrow="练习检验 · 预留流程"
    title="学习反馈后的练习检验"
    description="本页承接学习反馈后的练习检验流程，当前先完成业务跳转与上下文展示。"
  >
    <section class="practice-check-layout">
      <article class="card">
        <div class="card-head">
          <div>
            <p class="label">当前检验目标</p>
            <h2>{{ context.targetName || "尚未指定节点" }}</h2>
          </div>
          <p class="caption">题目模块后续接入，当前先展示反馈后的检验上下文。</p>
        </div>

        <dl class="summary-grid">
          <div><dt>学习者</dt><dd>{{ context.learnerCode }}</dd></div>
          <div><dt>来源页面</dt><dd>{{ context.sourcePage }}</dd></div>
          <div><dt>范围</dt><dd>{{ context.scopeCode }}</dd></div>
          <div><dt>反馈批次</dt><dd>{{ context.feedbackBatchId || "暂无" }}</dd></div>
          <div><dt>反馈条数</dt><dd>{{ context.feedbackItemCount }}</dd></div>
        </dl>

        <div class="state state--info">
          当前版本只完成“反馈成功后是否进入练习检验”的业务分流。题目展示、作答记录和客观校验将在后续版本接入。
        </div>

        <div class="action-row">
          <button type="button" class="ghost-button" @click="goHome">返回首页路径规划</button>
          <button type="button" class="ghost-button" @click="goProfile">查看学习者画像</button>
        </div>
      </article>
    </section>
  </PageLayout>
</template>
```

- [ ] **Step 4: Re-run the practice-check view test**

Run from `frontend`:

```bash
npm run test -- --run src/views/PracticeCheckView.test.js
```

Expected: PASS.

## Task 5: Add the post-feedback practice branch to the main path planner

**Files:**
- Modify: `frontend/src/components/PathPlannerPanel.vue`
- Test: `frontend/src/components/PathPlannerPanel.test.js`

- [ ] **Step 1: Add a failing component test for practice-check branching**

Extend `frontend/src/components/PathPlannerPanel.test.js` with a test that mocks `submitLearningFeedback` and asserts routing to the new page after the user chooses the practice branch. Use a minimal interaction-oriented test like:

```js
it("offers practice check routing after successful feedback submission", async () => {
  const { submitLearningFeedback } = await import("../api/feedback");

  fetchKnowledgeGraph.mockResolvedValue({
    nodes: [{ code: "queue", label: "队列", chapterNo: 4 }],
  });
  generateLearningPath.mockResolvedValue({
    summary: {
      targetReachableWithinBudget: true,
      scheduledCount: 1,
      deferredCount: 0,
      masteredCount: 0,
      scheduledMinutes: 30,
      totalRequiredMinutes: 30,
      availableMinutes: 120,
    },
    path: [
      {
        code: "queue",
        name: "队列",
        chapterNo: 4,
        estimatedMinutes: 30,
        masteryPercent: 20,
        status: "scheduled",
        reasonTrace: {
          triggerReasons: ["该节点与当前目标直接相关。"],
          relevanceScore: 0.9,
          importanceScore: 0.85,
          timeCostPenalty: 0.2,
        },
      },
    ],
    resourceRecommendations: [],
  });
  submitLearningFeedback.mockResolvedValue({
    masteryByCode: { queue: 0.7 },
    feedbackBatchId: "batch-123",
    feedbackSummary: { feedbackItemCount: 1 },
    adjustments: [],
  });

  const wrapper = mount(PathPlannerPanel, {
    props: {
      learnerCode: "demo-learner",
      initialMasteryByCode: { queue: 0.2 },
      profileLoading: false,
    },
    global: {
      plugins: [createPinia()],
    },
  });

  await flushUi();
  await wrapper.find("form").trigger("submit.prevent");
  await flushUi();

  await wrapper.get("[data-testid='practice-check-confirm']").trigger("click");

  expect(pushMock).toHaveBeenCalledWith({
    name: "practice-check",
  });
});
```

Adjust selector names to match the actual buttons you add.

- [ ] **Step 2: Run the focused planner test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/components/PathPlannerPanel.test.js
```

Expected: FAIL because the practice branch prompt and route action do not exist yet.

- [ ] **Step 3: Implement the branch prompt and route action in `PathPlannerPanel.vue`**

After successful `submitLearningFeedback(...)` and before the function exits, keep the existing replan behavior, then expose a simple success panel with two buttons:

1. `暂不检验`
2. `进入练习检验`

Store the context by calling `navigationStore.setPracticeCheckContext(...)` with:

```js
navigationStore.setPracticeCheckContext({
  learnerCode: props.learnerCode,
  sourcePage: "home",
  targetCode: selectedTargetCode.value,
  targetName: selectedTargetLabel.value,
  scopeCode: "root",
  feedbackBatchId: payload.feedbackBatchId || "",
  feedbackItemCount: payload.feedbackSummary?.feedbackItemCount || 0,
});
```

Then route with:

```js
await router.push({
  name: "practice-check",
});
```

Implementation constraints:

1. Do not interrupt the current feedback success path.
2. The practice prompt should only appear after a successful submission.
3. Clearing a new plan generation should also clear any stale prompt state.

- [ ] **Step 4: Re-run the focused planner test**

Run from `frontend`:

```bash
npm run test -- --run src/components/PathPlannerPanel.test.js
```

Expected: PASS.

## Task 6: Mirror the post-feedback practice branch inside detail learning

**Files:**
- Modify: `frontend/src/components/DetailLearningWorkspace.vue`
- Test: `frontend/src/components/DetailLearningWorkspace.test.js`

- [ ] **Step 1: Add a failing detail-workspace test for the practice branch**

Add a test that mocks `adjustDetailLearningPath` to return a `feedbackSummary` and then asserts that clicking the new practice action routes to `practice-check` with detail context.

Use test expectations like:

```js
expect(pushMock).toHaveBeenCalledWith({
  name: "practice-check",
});
expect(store.practiceCheckContext.scopeCode).toBe("queue-detail");
expect(store.practiceCheckContext.targetCode).toBe("queue-linked");
```

- [ ] **Step 2: Run the focused detail-workspace test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/components/DetailLearningWorkspace.test.js
```

Expected: FAIL because no detail practice branch exists yet.

- [ ] **Step 3: Implement the detail branch in `DetailLearningWorkspace.vue`**

After a successful `adjustDetailLearningPath(...)`, preserve the existing result updates and add a second success action group that stores practice context via:

```js
navigationStore.setPracticeCheckContext({
  learnerCode: props.learnerCode,
  sourcePage: "detail-learning",
  targetCode: detailTargetCode.value,
  targetName: detailTargetLabel.value,
  scopeCode: props.section.scopeCode,
  scopeLabel: props.section.scopeLabel || props.section.name,
  feedbackBatchId: payload.feedbackBatchId || "",
  feedbackItemCount: payload.feedbackSummary?.feedbackItemCount || 0,
});
```

Then route to `{ name: "practice-check" }`.

Keep this behavior parallel to the main planner so the user experience stays consistent.

- [ ] **Step 4: Re-run the focused detail-workspace test**

Run from `frontend`:

```bash
npm run test -- --run src/components/DetailLearningWorkspace.test.js
```

Expected: PASS.

## Task 7: Render learner profile charts from backend analytics

**Files:**
- Create: `frontend/src/components/profileCharts.js`
- Create: `frontend/src/components/profileCharts.test.js`
- Modify: `frontend/src/components/LearnerProfileCard.vue`
- Modify: `frontend/src/views/LearnerProfileView.test.js`

- [ ] **Step 1: Add a failing pure-helper test for chart transformations**

Create `frontend/src/components/profileCharts.test.js`:

```js
import { describe, expect, it } from "vitest";

import {
  buildMasteryBarItems,
  buildFeedbackTrendPoints,
  buildFeedbackCompositionItems,
} from "./profileCharts";

describe("profileCharts helpers", () => {
  it("builds chart-ready items from learner analytics payload", () => {
    const analytics = {
      masteryDistribution: [
        { code: "queue", name: "队列", masteryPercent: 20 },
        { code: "stack", name: "栈", masteryPercent: 80 },
      ],
      feedbackTrend: [
        { label: "反馈1", averageUpdatedMasteryPercent: 45 },
        { label: "反馈2", averageUpdatedMasteryPercent: 60 },
      ],
      feedbackStatusComposition: [
        { statusCode: "completed", statusLabel: "已完成", count: 2 },
        { statusCode: "partial", statusLabel: "部分完成", count: 1 },
        { statusCode: "blocked", statusLabel: "学习受阻", count: 1 },
      ],
    };

    expect(buildMasteryBarItems(analytics)[0]).toMatchObject({
      code: "queue",
      width: "20%",
    });
    expect(buildFeedbackTrendPoints(analytics)[1]).toMatchObject({
      label: "反馈2",
      value: 60,
    });
    expect(buildFeedbackCompositionItems(analytics)[0]).toMatchObject({
      statusCode: "completed",
      count: 2,
    });
  });
});
```

- [ ] **Step 2: Run the helper test and confirm failure**

Run from `frontend`:

```bash
npm run test -- --run src/components/profileCharts.test.js
```

Expected: FAIL because the helper module does not yet exist.

- [ ] **Step 3: Implement the chart helpers as pure functions**

Create `frontend/src/components/profileCharts.js`:

```js
function safeArray(value) {
  return Array.isArray(value) ? value : [];
}

export function buildMasteryBarItems(analytics = {}) {
  return safeArray(analytics.masteryDistribution)
    .slice()
    .sort((left, right) => left.masteryPercent - right.masteryPercent)
    .map((item) => ({
      ...item,
      width: `${Math.max(0, Math.min(100, item.masteryPercent || 0))}%`,
    }));
}

export function buildFeedbackTrendPoints(analytics = {}) {
  return safeArray(analytics.feedbackTrend).map((item) => ({
    ...item,
    value: item.averageUpdatedMasteryPercent || 0,
  }));
}

export function buildFeedbackCompositionItems(analytics = {}) {
  const total = safeArray(analytics.feedbackStatusComposition).reduce(
    (sum, item) => sum + (item.count || 0),
    0,
  );

  return safeArray(analytics.feedbackStatusComposition).map((item) => ({
    ...item,
    percent: total > 0 ? Math.round(((item.count || 0) / total) * 100) : 0,
  }));
}
```

- [ ] **Step 4: Re-run the helper test**

Run from `frontend`:

```bash
npm run test -- --run src/components/profileCharts.test.js
```

Expected: PASS.

- [ ] **Step 5: Render the three charts in `LearnerProfileCard.vue`**

Keep the existing identity/summary/records sections and add a new chart section above the recent lists.

Use lightweight SVG/CSS-native rendering instead of adding a chart library. Render:

1. Mastery distribution as horizontal bars.
2. Feedback trend as a simple line-strip / dot-row using inline SVG.
3. Feedback status composition as stacked percentage chips or donut-like ring markup built with CSS gradients.

The component should import the helper functions and compute:

```js
const masteryChartItems = computed(() =>
  buildMasteryBarItems(props.profile?.analytics || {}),
);
const feedbackTrendPoints = computed(() =>
  buildFeedbackTrendPoints(props.profile?.analytics || {}),
);
const feedbackCompositionItems = computed(() =>
  buildFeedbackCompositionItems(props.profile?.analytics || {}),
);
```

Keep the visuals simple and defense-oriented. Do not introduce `echarts`, `chart.js`, or another dependency.

- [ ] **Step 6: Extend `LearnerProfileView.test.js` to assert chart section rendering**

Add expectations after the existing mount flow such as:

```js
expect(wrapper.text()).toContain("掌握度分布");
expect(wrapper.text()).toContain("学习反馈趋势");
expect(wrapper.text()).toContain("学习活动结构");
```

- [ ] **Step 7: Run the learner-profile frontend test slice**

Run from `frontend`:

```bash
npm run test -- --run src/components/profileCharts.test.js src/views/LearnerProfileView.test.js
```

Expected: PASS.

## Task 8: Clarify stage-oriented resource presentation in the resource page

**Files:**
- Modify: `frontend/src/views/ResourceRecommendationView.vue`
- Test: existing resource page tests if present, otherwise full frontend suite later

- [ ] **Step 1: Add the stage label and guidance to the visible resource summary**

In `frontend/src/views/ResourceRecommendationView.vue`, surface the new backend fields in the summary card and each resource card. Add text elements like:

```vue
<div v-if="displayedResources[0]?.recommendationStageLabel" class="resource-stage-panel">
  <strong>{{ displayedResources[0].recommendationStageLabel }}</strong>
  <p>{{ displayedResources[0].stageGuidance }}</p>
</div>
```

And per resource badge:

```vue
<span v-if="resource.recommendationStageLabel" class="resource-stage-badge">
  {{ resource.recommendationStageLabel }}
</span>
```

Do not redesign the whole page. Only make the existing recommendation intent easier to read.

- [ ] **Step 2: Run the resource page frontend tests if present, otherwise defer to the full frontend suite**

If `frontend/src/views/ResourceRecommendationView.test.js` already covers this area, run:

```bash
npm run test -- --run src/views/ResourceRecommendationView.test.js
```

If not, verify this task as part of the final full frontend run in Task 10.

Expected: PASS, or no regressions visible in the final full-suite run.

## Task 9: Update docs for the new phase-1 behavior

**Files:**
- Modify: `README.md`
- Modify: `AIREAD.md`

- [ ] **Step 1: Update README implemented-capabilities section**

Add concise entries reflecting the shipped behavior:

```md
- 资源推荐分层：系统会基于当前掌握度判断“入门讲解 / 巩固理解 / 应用训练 / 提升拓展”阶段，并将每个节点推荐资源数量控制在 4 条以内
- 学习者画像图表：画像页新增掌握度分布、学习反馈趋势、学习活动结构三类图表化展示
- 练习检验入口：提交学习反馈后可选择跳转到练习检验页，当前先完成上下文承接与页面占位
```

- [ ] **Step 2: Append an architecture/system change record to `AIREAD.md`**

Append a dated change entry using the existing file format. Mention:

1. Resource recommendation semantics changed from broad recommendation to mastery-stage recommendation.
2. Learner profile API now includes analytics aggregates for charts.
3. Feedback flow now includes an optional practice-check branch.

- [ ] **Step 3: Manually review both docs for consistency with the spec**

Check that wording stays aligned with the defense-oriented scope and does not overclaim that the question bank is already implemented.

## Task 10: Run verification, then prepare a clean handoff

**Files:**
- No code changes expected unless verification reveals breakage

- [ ] **Step 1: Run the backend verification slice**

Run from `backend/build`:

```bash
ctest --output-on-failure -R "LearningResourceService|PathPlanningServiceBuildsLearningResourcesIntoResponse|LearnerProfileService|FeedbackServicePersistsFeedbackAndUpdatesLearnerProfile"
```

Expected: PASS.

- [ ] **Step 2: Run the frontend targeted verification slice**

Run from `frontend`:

```bash
npm run test -- --run src/stores/navigationStore.test.js src/views/PracticeCheckView.test.js src/components/PathPlannerPanel.test.js src/components/DetailLearningWorkspace.test.js src/components/profileCharts.test.js src/views/LearnerProfileView.test.js
```

Expected: PASS.

- [ ] **Step 3: Run the frontend production build**

Run from `frontend`:

```bash
npm run build
```

Expected: build completes successfully.

- [ ] **Step 4: If verification is green, update the implementation status in your session notes**

Record:

1. Resource recommendation now returns at most four items per node.
2. Learner profile payload includes analytics.
3. Main and detail feedback both support practice-check routing.
4. Practice-check page is a placeholder shell, not a full question engine.

- [ ] **Step 5: Commit in logical units**

Recommended commit order:

```bash
git add backend/services/LearningResourceService.cc backend/tests/PathPlanningServiceTest.cc backend/services/LearnerProfileService.cc backend/tests/LearnerProfileFeedbackServiceTest.cc
git commit -m "feat: refine resource recommendations and learner analytics"

git add frontend/src/stores/navigationStore.js frontend/src/stores/navigationStore.test.js frontend/src/views/PracticeCheckView.vue frontend/src/views/PracticeCheckView.test.js frontend/src/router/index.js frontend/src/components/PathPlannerPanel.vue frontend/src/components/PathPlannerPanel.test.js frontend/src/components/DetailLearningWorkspace.vue frontend/src/components/DetailLearningWorkspace.test.js frontend/src/components/LearnerProfileCard.vue frontend/src/components/profileCharts.js frontend/src/components/profileCharts.test.js frontend/src/views/LearnerProfileView.test.js frontend/src/views/ResourceRecommendationView.vue
git commit -m "feat: add practice check branch and profile charts"

git add README.md AIREAD.md
git commit -m "docs: record phase one student experience enhancements"
```

Only commit if the user asks for commits in the execution phase.

## Self-review summary

Spec coverage check:

1. Resource recommendation precision and count control: covered by Task 1 and Task 8.
2. Learner profile charts: covered by Task 2 and Task 7.
3. Post-feedback practice-check flow: covered by Task 3, Task 4, Task 5, and Task 6.
4. Multi-role platform work: intentionally excluded from this plan because it is a separate second-phase subsystem.

Placeholder scan:

1. No `TODO` / `TBD` placeholders remain in tasks.
2. Every task includes file targets and concrete commands.
3. Future question-bank implementation is described explicitly as out of scope for this phase.

Type consistency check:

1. Practice-check context field names are consistent across store, route, and components.
2. Resource stage fields use one contract: `recommendationStageCode`, `recommendationStageLabel`, `stageGuidance`.
3. Learner analytics fields use one contract: `analytics.masteryDistribution`, `analytics.feedbackTrend`, `analytics.feedbackStatusComposition`.
