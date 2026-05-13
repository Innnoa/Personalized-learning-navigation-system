import { nextTick } from "vue";
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({
      push: pushMock,
    }),
  };
});

vi.mock("../api/knowledgeGraph", () => ({
  fetchKnowledgeGraph: vi.fn(),
}));

vi.mock("../api/path", () => ({
  generateDetailLearningPath: vi.fn(),
  adjustDetailLearningPath: vi.fn(),
}));

vi.mock("../utils/learningPathExport", () => ({
  downloadLearningPathExport: vi.fn(),
}));

vi.mock("../graph/loadCytoscape", () => ({
  loadCytoscape: vi.fn(),
}));

function createCollection() {
  const collection = {
    addClass: vi.fn(() => collection),
    removeClass: vi.fn(() => collection),
    union: vi.fn(() => collection),
    nodes: vi.fn(() => collection),
    edges: vi.fn(() => collection),
  };
  return collection;
}

function createElement(id, collection, nodeDataMap) {
  const element = {
    id: () => id,
    data: (key) => nodeDataMap.get(id)?.[key],
    empty: () => false,
    addClass: vi.fn(() => element),
    removeClass: vi.fn(() => element),
    union: vi.fn(() => collection),
    connectedEdges: vi.fn(() => collection),
    predecessors: vi.fn(() => collection),
    successors: vi.fn(() => collection),
  };
  return element;
}

const { fetchKnowledgeGraph } = await import("../api/knowledgeGraph");
const { generateDetailLearningPath } = await import("../api/path");
const { adjustDetailLearningPath } = await import("../api/path");
const { downloadLearningPathExport } = await import("../utils/learningPathExport");
const { loadCytoscape } = await import("../graph/loadCytoscape");
const { useNavigationStore } = await import("../stores/navigationStore");
const { default: DetailLearningWorkspace } = await import("./DetailLearningWorkspace.vue");

function buildScopePayload() {
  return {
    view: {
      scopeCode: "queue-detail",
      scopeName: "队列",
      parentScopeCode: "root",
      parentNodeCode: "queue",
      isRoot: false,
    },
    breadcrumbs: [
      {
        scopeCode: "root",
        label: "课程主图",
      },
      {
        scopeCode: "queue-detail",
        label: "队列",
      },
    ],
    nodes: [
      {
        id: "queue-array",
        code: "queue-array",
        label: "顺序队列",
        chapterNo: 4,
        chapterName: "栈和队列",
        nodeType: "implementation",
        estimatedMinutes: 20,
        displayOrder: 1,
        description: "顺序队列基础",
      },
      {
        id: "queue-linked",
        code: "queue-linked",
        label: "链式队列",
        chapterNo: 4,
        chapterName: "栈和队列",
        nodeType: "implementation",
        estimatedMinutes: 25,
        displayOrder: 2,
        description: "链式队列基础",
      },
    ],
    edges: [
      {
        id: "queue-array-to-linked",
        source: "queue-array",
        target: "queue-linked",
        relationType: "prerequisite",
      },
    ],
  };
}

async function flushUi(rounds = 1) {
  for (let index = 0; index < rounds; index += 1) {
    await nextTick();
    await Promise.resolve();
    await new Promise((resolve) => setTimeout(resolve, 0));
  }
}

function mountWorkspace() {
  const pinia = createPinia();
  setActivePinia(pinia);

  return {
    store: useNavigationStore(),
    wrapper: mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    }),
  };
}

describe("DetailLearningWorkspace", () => {
  beforeEach(() => {
    vi.clearAllMocks();
    pushMock.mockReset();
    window.sessionStorage.clear();
    fetchKnowledgeGraph.mockResolvedValue(buildScopePayload());
    generateDetailLearningPath.mockResolvedValue({
      summary: {
        scheduledCount: 1,
        deferredCount: 0,
        masteredCount: 0,
        scheduledMinutes: 20,
        totalRequiredMinutes: 20,
        targetReachableWithinBudget: true,
      },
      path: [
        {
          code: "queue-linked",
          name: "链式队列",
          chapterNo: 4,
          difficultyLevel: 2,
          estimatedMinutes: 20,
          masteryPercent: 30,
          status: "scheduled",
          explanation: {
            summary: "当前应先掌握链式队列。",
          },
        },
      ],
      resourceRecommendations: [
        {
          code: "queue-linked",
          name: "链式队列",
          status: "scheduled",
          resourceCount: 1,
          recommendedUsage: "先看链式队列资源。",
          resources: [
            {
              title: "链式队列讲解",
              type: "video",
              source: "课程视频",
              url: "https://example.com/queue-linked",
              description: "链式队列资源",
              recommendedUsage: "配合当前节点学习。",
            },
          ],
        },
      ],
    });
    adjustDetailLearningPath.mockResolvedValue({
      summary: {
        scheduledCount: 0,
        deferredCount: 0,
        masteredCount: 1,
        scheduledMinutes: 0,
        totalRequiredMinutes: 20,
        targetReachableWithinBudget: true,
      },
      path: [
        {
          code: "queue-linked",
          name: "链式队列",
          chapterNo: 4,
          difficultyLevel: 2,
          estimatedMinutes: 20,
          masteryPercent: 95,
          status: "mastered",
          explanation: {
            summary: "当前细化目标已完成本轮学习。",
          },
        },
      ],
      resourceRecommendations: [],
      updatedMasteryByCode: {
        "queue-array": 0.2,
        "queue-linked": 0.95,
      },
      feedbackSummary: {
        feedbackBatchId: "detail-batch-123",
        feedbackItemCount: 1,
        completedCount: 1,
        partialCount: 0,
        blockedCount: 0,
      },
      adjustments: [
        {
          code: "queue-linked",
          completionStatus: "completed",
          previousMastery: 0.1,
          updatedMastery: 0.95,
          adjustmentReasons: ["掌握度明显提升。"],
        },
      ],
    });
    loadCytoscape.mockResolvedValue((config) => {
      const collection = createCollection();
      const nodeDataMap = new Map(
        (config.elements || [])
          .filter((item) => item?.data?.id && !item?.data?.source)
          .map((item) => [item.data.id, item.data]),
      );
      const elementCache = new Map();

      return {
        destroy: vi.fn(),
        resize: vi.fn(),
        fit: vi.fn(),
        off: vi.fn(),
        one: vi.fn((eventName, handler) => {
          if (eventName === "layoutstop") {
            handler();
          }
        }),
        layout: vi.fn(() => ({
          run: vi.fn(),
        })),
        on: vi.fn(),
        elements: () => collection,
        getElementById: (id) => {
          if (!elementCache.has(id)) {
            elementCache.set(id, createElement(id, collection, nodeDataMap));
          }

          return elementCache.get(id);
        },
      };
    });
  });

  it("restores remembered branch state and skips regenerating local plan", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-linked",
      availableMinutes: 60,
      masteryPercentByCode: {
        "queue-array": 25,
        "queue-linked": 55,
      },
      masteryOverridePercentByCode: {
        "queue-array": 25,
        "queue-linked": 55,
      },
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 0,
          masteredCount: 0,
          scheduledMinutes: 25,
          totalRequiredMinutes: 25,
          targetReachableWithinBudget: true,
        },
        path: [
          {
            code: "queue-linked",
            name: "链式队列",
            chapterNo: 4,
            difficultyLevel: 2,
            estimatedMinutes: 25,
            masteryPercent: 55,
            status: "scheduled",
            explanation: {
              summary: "记忆状态中的局部结果。",
            },
          },
        ],
        resourceRecommendations: [],
      },
    });

    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);

    const selects = wrapper.findAll("select");
    expect(selects[0].element.value).toBe("queue-linked");
    expect(selects[1].element.value).toBe("60");
    expect(wrapper.text()).toContain("记忆状态中的局部结果");
    expect(generateDetailLearningPath).not.toHaveBeenCalled();
  });

  it("prefers latest learner profile mastery when legacy remembered snapshot is stale", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      masteryPercentByCode: {
        "queue-array": 0,
        "queue-linked": 0,
      },
    });

    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);

    const detailValues = wrapper.findAll(".detail-grid dd");
    expect(detailValues[1].text()).toBe("20%");
  });

  it("restores remembered mastery overrides on top of latest learner profile baseline", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      masteryPercentByCode: {
        "queue-array": 45,
        "queue-linked": 10,
      },
      masteryOverridePercentByCode: {
        "queue-array": 45,
      },
    });

    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);

    const detailValues = wrapper.findAll(".detail-grid dd");
    expect(detailValues[1].text()).toBe("45%");
  });

  it("exports current detail plan with scope metadata", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-linked",
      availableMinutes: 60,
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 0,
          masteredCount: 0,
          scheduledMinutes: 25,
          totalRequiredMinutes: 25,
          targetReachableWithinBudget: true,
        },
        path: [
          {
            code: "queue-linked",
            name: "链式队列",
            chapterNo: 4,
            difficultyLevel: 2,
            estimatedMinutes: 25,
            masteryPercent: 55,
            status: "scheduled",
            explanation: {
              summary: "队列细化学习应先掌握链式实现。",
            },
          },
        ],
        resourceRecommendations: [],
      },
    });
    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);
    await wrapper.get("[data-testid='detail-export-path-button']").trigger("click");

    expect(downloadLearningPathExport).toHaveBeenCalledWith(
      expect.objectContaining({
        learnerCode: "demo-learner",
        scopeLabel: "队列",
        documentTitle: "细化学习路径导出结果",
        filePrefix: "detail-learning-path",
      }),
    );
  });

  it("opens detail resource page with detail-scoped resource context", async () => {
    const { wrapper, store } = mountWorkspace();

    await flushUi(6);
    await wrapper.findAll("select")[0].setValue("queue-linked");
    await wrapper.get(".detail-path-form").trigger("submit.prevent");
    await flushUi(6);

    const resourceButton = wrapper.get("[data-testid='detail-resource-queue-linked']");
    await resourceButton.trigger("click");

    expect(store.resourceRecommendationSummary.contextMode).toBe("detail");
    expect(store.resourceRecommendationSummary.sourcePage).toBe("detail-learning");
    expect(store.resourceRecommendationSummary.scopeCode).toBe("queue-detail");
    expect(store.resourceRecommendationSummary.parentNodeCode).toBe("queue");
    expect(store.resourceRecommendationSummary.parentNodeLabel).toBe("队列");
    expect(store.resourceSectionByCode("queue-linked")?.recordingKnowledgePointCode).toBe(
      "queue",
    );
    expect(pushMock).toHaveBeenCalledWith({
      name: "resource-recommendation",
      params: { code: "queue-linked" },
      query: {
        level: "detail",
      },
    });
  });

  it("routes to practice check with detail-scoped context after successful feedback adjustment", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-linked",
      availableMinutes: 20,
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 0,
          masteredCount: 0,
          scheduledMinutes: 20,
          totalRequiredMinutes: 20,
          targetReachableWithinBudget: true,
        },
        path: [
          {
            code: "queue-linked",
            name: "链式队列",
            chapterNo: 4,
            difficultyLevel: 2,
            estimatedMinutes: 20,
            masteryPercent: 30,
            status: "scheduled",
            explanation: {
              summary: "当前应先掌握链式队列。",
            },
          },
        ],
        resourceRecommendations: [],
      },
    });

    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);
    await wrapper.get(".detail-feedback-form").trigger("submit.prevent");
    await flushUi(6);

    expect(adjustDetailLearningPath).toHaveBeenCalled();
    expect(wrapper.text()).toContain("是否进入练习检验");

    const practiceCheckButton = wrapper
      .findAll("button")
      .find((button) => button.text().includes("进入练习检验"));

    expect(practiceCheckButton).toBeTruthy();

    await practiceCheckButton.trigger("click");

    expect(store.practiceCheckContext).toMatchObject({
      learnerCode: "demo-learner",
      sourcePage: "detail-learning",
      targetCode: "queue-linked",
      targetName: "链式队列",
      scopeCode: "queue-detail",
      scopeLabel: "队列",
      feedbackBatchId: "detail-batch-123",
      feedbackItemCount: 1,
    });
    expect(pushMock).toHaveBeenCalledWith({ name: "practice-check" });
  });

  it("only shows practice prompt after a successful current detail adjustment", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-linked",
      availableMinutes: 20,
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 0,
          masteredCount: 0,
          scheduledMinutes: 20,
          totalRequiredMinutes: 20,
          targetReachableWithinBudget: true,
        },
        path: [
          {
            code: "queue-linked",
            name: "链式队列",
            chapterNo: 4,
            difficultyLevel: 2,
            estimatedMinutes: 20,
            masteryPercent: 30,
            status: "scheduled",
            explanation: {
              summary: "当前应先掌握链式队列。",
            },
          },
        ],
        resourceRecommendations: [],
      },
    });

    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);

    expect(wrapper.text()).not.toContain("是否进入练习检验");

    await wrapper.get(".detail-feedback-form").trigger("submit.prevent");
    await flushUi(6);

    expect(wrapper.text()).toContain("是否进入练习检验");
  });

  it("clears stale practice prompt when a later detail adjustment attempt fails", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
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
    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-linked",
      availableMinutes: 20,
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 0,
          masteredCount: 0,
          scheduledMinutes: 20,
          totalRequiredMinutes: 20,
          targetReachableWithinBudget: true,
        },
        path: [
          {
            code: "queue-linked",
            name: "链式队列",
            chapterNo: 4,
            difficultyLevel: 2,
            estimatedMinutes: 20,
            masteryPercent: 30,
            status: "scheduled",
            explanation: {
              summary: "当前应先掌握链式队列。",
            },
          },
        ],
        resourceRecommendations: [],
      },
    });

    const wrapper = mount(DetailLearningWorkspace, {
      global: {
        plugins: [pinia],
      },
      props: {
        learnerCode: "demo-learner",
        masteryByCode: {
          "queue-array": 0.2,
          "queue-linked": 0.1,
        },
        section: {
          code: "queue",
          name: "队列",
          scopeCode: "queue-detail",
          scopeLabel: "队列",
          chapterNo: 4,
          estimatedMinutes: 30,
          status: "scheduled",
        },
      },
    });

    await flushUi(6);
    await wrapper.get(".detail-feedback-form").trigger("submit.prevent");
    await flushUi(6);

    expect(wrapper.text()).toContain("是否进入练习检验");

    adjustDetailLearningPath.mockRejectedValueOnce(new Error("detail adjust failed"));

    generateDetailLearningPath.mockResolvedValueOnce({
      summary: {
        scheduledCount: 1,
        deferredCount: 0,
        masteredCount: 0,
        scheduledMinutes: 20,
        totalRequiredMinutes: 20,
        targetReachableWithinBudget: true,
      },
      path: [
        {
          code: "queue-linked",
          name: "链式队列",
          chapterNo: 4,
          difficultyLevel: 2,
          estimatedMinutes: 20,
          masteryPercent: 30,
          status: "scheduled",
          explanation: {
            summary: "当前应先掌握链式队列。",
          },
        },
      ],
      resourceRecommendations: [],
    });

    await wrapper.get(".detail-path-form").trigger("submit.prevent");
    await flushUi(6);

    await wrapper.get(".detail-feedback-form").trigger("submit.prevent");
    await flushUi(6);

    expect(wrapper.text()).not.toContain("是否进入练习检验");
    expect(wrapper.text()).toContain("本范围学习反馈调整失败");
  });
});
