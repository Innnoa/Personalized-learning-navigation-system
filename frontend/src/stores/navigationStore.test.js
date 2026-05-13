import { createPinia, setActivePinia } from "pinia";
import { beforeEach, describe, expect, it } from "vitest";

import { useNavigationStore } from "./navigationStore";

describe("navigationStore", () => {
  beforeEach(() => {
    window.sessionStorage.clear();
  });

  it("persists resource recommendation context and restores it on next store init", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 2,
        },
      ],
      targetLabel: "队列",
      availableMinutes: 90,
      scheduledCount: 1,
      deferredCount: 0,
    });

    expect(store.resourceSectionByCode("queue")?.name).toBe("队列");

    setActivePinia(createPinia());
    const restoredStore = useNavigationStore();

    expect(restoredStore.activeLearnerCode).toBe("test-learner");
    expect(restoredStore.resourceRecommendationSummary.targetLabel).toBe("队列");
    expect(restoredStore.resourceSectionByCode("queue")?.status).toBe("scheduled");
    expect(restoredStore.resourceSectionByModeAndCode("main", "queue")?.status).toBe(
      "scheduled",
    );
  });

  it("clears persisted context and resets learner code", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [
        {
          code: "stack",
          name: "栈",
          status: "deferred",
          resourceCount: 1,
        },
      ],
      targetLabel: "栈",
      availableMinutes: 60,
      scheduledCount: 0,
      deferredCount: 1,
    });

    store.clearResourceRecommendationContext();

    expect(store.activeLearnerCode).toBe("demo-learner");
    expect(store.resourceRecommendationSections).toEqual([]);
    expect(store.resourceRecommendationSummary.targetLabel).toBe("");
  });

  it("preserves main and detail resource contexts independently", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setResourceRecommendationContext({
      learnerCode: "demo-learner",
      contextMode: "main",
      resourceRecommendationSections: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 2,
        },
      ],
      targetLabel: "队列",
      availableMinutes: 90,
      scheduledCount: 1,
      deferredCount: 0,
    });

    store.setResourceRecommendationContext({
      learnerCode: "demo-learner",
      contextMode: "detail",
      resourceRecommendationSections: [
        {
          code: "queue-linked",
          name: "链式队列",
          status: "scheduled",
          resourceCount: 1,
          recordingKnowledgePointCode: "queue",
        },
      ],
      targetLabel: "链式队列",
      availableMinutes: 40,
      scheduledCount: 1,
      deferredCount: 0,
      scopeCode: "queue-detail",
      scopeLabel: "队列",
      parentNodeCode: "queue",
      parentNodeLabel: "队列",
    });

    expect(store.resourceSectionByCode("queue-linked")?.name).toBe("链式队列");
    expect(store.resourceSectionByModeAndCode("main", "queue")?.name).toBe("队列");
    expect(store.resourceSectionByModeAndCode("detail", "queue-linked")?.name).toBe(
      "链式队列",
    );

    store.setActiveResourceRecommendationMode("main");

    expect(store.resourceSectionByCode("queue")?.name).toBe("队列");
    expect(store.resourceRecommendationSummary.contextMode).toBe("main");

    store.clearResourceRecommendationContext("detail");

    expect(store.resourceSectionByModeAndCode("main", "queue")?.name).toBe("队列");
    expect(store.resourceSectionByModeAndCode("detail", "queue-linked")).toBeNull();
  });

  it("persists learning graph view state and restores it on next store init", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setLearningGraphViewState({
      currentScopeCode: "stack-detail",
      selectedNodeCode: "push",
      lastFocusedNodeCode: "push",
      lastFocusedScopeCode: "stack-detail",
    });

    expect(store.learningGraphViewState.currentScopeCode).toBe("stack-detail");
    expect(store.learningGraphViewState.selectedNodeCode).toBe("push");

    setActivePinia(createPinia());
    const restoredStore = useNavigationStore();

    expect(restoredStore.learningGraphViewState.currentScopeCode).toBe("stack-detail");
    expect(restoredStore.learningGraphViewState.selectedNodeCode).toBe("push");
    expect(restoredStore.learningGraphViewState.lastFocusedScopeCode).toBe(
      "stack-detail",
    );
  });

  it("clears learning graph view state back to root", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setLearningGraphViewState({
      currentScopeCode: "push-detail",
      selectedNodeCode: "push-check",
      lastFocusedNodeCode: "push-check",
      lastFocusedScopeCode: "push-detail",
    });

    store.clearLearningGraphViewState();

    expect(store.learningGraphViewState.currentScopeCode).toBe("root");
    expect(store.learningGraphViewState.selectedNodeCode).toBe("");
    expect(store.learningGraphViewState.lastFocusedNodeCode).toBe("");
    expect(store.learningGraphViewState.lastFocusedScopeCode).toBe("root");
  });

  it("persists practice-check context and restores it on next store init", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setPracticeCheckContext({
      learnerCode: "test-learner",
      sourcePage: "detail-learning",
      targetCode: "queue-array",
      targetName: "顺序队列",
      scopeCode: "queue-detail",
      feedbackBatchId: "batch-001",
      feedbackItemCount: 3,
    });

    expect(store.practiceCheckContext.targetCode).toBe("queue-array");
    expect(store.practiceCheckContext.feedbackBatchId).toBe("batch-001");

    setActivePinia(createPinia());
    const restoredStore = useNavigationStore();

    expect(restoredStore.practiceCheckContext.targetCode).toBe("queue-array");
    expect(restoredStore.practiceCheckContext.feedbackBatchId).toBe("batch-001");
  });

  it("persists detail learning context and restores selected scope", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setDetailLearningContext({
      learnerCode: "demo-learner",
      detailLearningSections: [
        {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      ],
      selectedScopeCode: "queue-detail",
      sourceTargetLabel: "队列",
      sourcePage: "home",
    });

    expect(store.detailLearningSectionByScopeCode("queue-detail")?.name).toBe("队列");

    setActivePinia(createPinia());
    const restoredStore = useNavigationStore();

    expect(restoredStore.detailLearningLearnerCode).toBe("demo-learner");
    expect(restoredStore.detailLearningSummary.selectedScopeCode).toBe("queue-detail");
    expect(
      restoredStore.detailLearningSectionByScopeCode("queue-detail")?.scopeLabel,
    ).toBe("队列");
  });

  it("clears detail learning context back to empty state", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setDetailLearningContext({
      learnerCode: "demo-learner",
      detailLearningSections: [
        {
          code: "stack",
          name: "栈",
          scopeCode: "stack-detail",
          scopeLabel: "栈",
          chapterNo: 3,
          estimatedMinutes: 25,
          status: "scheduled",
        },
      ],
      selectedScopeCode: "stack-detail",
      sourceTargetLabel: "图",
      sourcePage: "home",
    });

    store.clearDetailLearningContext();

    expect(store.detailLearningLearnerCode).toBe("demo-learner");
    expect(store.detailLearningSections).toEqual([]);
    expect(store.detailLearningSummary.selectedScopeCode).toBe("");
  });

  it("persists detail learning branch view state and filters it by valid scope", () => {
    setActivePinia(createPinia());
    const store = useNavigationStore();

    store.setDetailLearningContext({
      learnerCode: "demo-learner",
      detailLearningSections: [
        {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
        {
          code: "tree-basic",
          name: "树与二叉树",
          scopeCode: "tree-basic-detail",
          scopeLabel: "树与二叉树",
          chapterNo: 6,
          estimatedMinutes: 40,
          status: "scheduled",
        },
      ],
      selectedScopeCode: "queue-detail",
      sourceTargetLabel: "图",
      sourcePage: "home",
    });

    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-array",
      availableMinutes: 60,
      masteryPercentByCode: {
        "queue-array": 45,
      },
      masteryOverridePercentByCode: {
        "queue-array": 45,
      },
      planResult: {
        summary: {
          scheduledCount: 1,
        },
        path: [
          {
            code: "queue-array",
            status: "scheduled",
          },
        ],
      },
    });

    expect(store.detailLearningViewStateByScopeCode("queue-detail")?.targetNodeCode).toBe(
      "queue-array",
    );
    expect(
      store.detailLearningViewStateByScopeCode("queue-detail")?.masteryOverridePercentByCode,
    ).toEqual({
      "queue-array": 45,
    });

    store.setDetailLearningContext({
      learnerCode: "demo-learner",
      detailLearningSections: [
        {
          code: "tree-basic",
          name: "树与二叉树",
          scopeCode: "tree-basic-detail",
          scopeLabel: "树与二叉树",
          chapterNo: 6,
          estimatedMinutes: 40,
          status: "scheduled",
        },
      ],
      selectedScopeCode: "tree-basic-detail",
      sourceTargetLabel: "图",
      sourcePage: "home",
    });

    expect(store.detailLearningViewStateByScopeCode("queue-detail")).toBeNull();

    setActivePinia(createPinia());
    const restoredStore = useNavigationStore();

    expect(restoredStore.detailLearningViewStateByScopeCode("tree-basic-detail")).toBeNull();
  });
});
