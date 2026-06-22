import { mount } from "@vue/test-utils";
import { createPinia, setActivePinia } from "pinia";
import { beforeEach, describe, expect, it, vi } from "vitest";

import PathPlannerPanel from "./PathPlannerPanel.vue";
import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import { generateLearningPath } from "../api/path";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();
let routeQuery = {
  target: "queue",
};

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => ({
      query: routeQuery,
    }),
    useRouter: () => ({
      push: pushMock,
    }),
  };
});

vi.mock("../api/knowledgeGraph", () => ({
  fetchKnowledgeGraph: vi.fn(),
}));

vi.mock("../api/path", () => ({
  generateLearningPath: vi.fn(),
}));

vi.mock("../api/feedback", () => ({
  rollbackLatestLearningFeedback: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

async function submitPlanner(wrapper) {
  await wrapper.get("form").trigger("submit");
  await flushUi();
}

describe("PathPlannerPanel", () => {
  beforeEach(() => {
    pushMock.mockReset();
    routeQuery = {
      target: "",
    };
  });

  it("uses route target query as selected target and initial planning target", async () => {
    routeQuery = {
      target: "queue",
    };
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
    });
    generateLearningPath.mockResolvedValue({
      summary: {
        targetReachableWithinBudget: true,
        scheduledCount: 1,
        deferredCount: 0,
        masteredCount: 0,
        scheduledMinutes: 60,
        totalRequiredMinutes: 60,
        availableMinutes: 120,
      },
      path: [],
      resourceRecommendations: [],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        courseCode: "custom-course",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);

    expect(generateLearningPath).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      courseCode: "custom-course",
      targetCodes: ["queue"],
      availableMinutes: 120,
      masteryByCode: {
        stack: 0.4,
        queue: 0.2,
      },
    });

    const targetSelect = wrapper.get("select");
    expect(targetSelect.element.value).toBe("queue");
  });

  it("replans when external target code changes from graph interaction", async () => {
    routeQuery = {
      target: "",
    };
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
    });
    generateLearningPath.mockResolvedValue({
      summary: {
        targetReachableWithinBudget: true,
        scheduledCount: 1,
        deferredCount: 0,
        masteredCount: 0,
        scheduledMinutes: 60,
        totalRequiredMinutes: 60,
        availableMinutes: 120,
      },
      path: [],
      resourceRecommendations: [],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
        externalTargetCode: "",
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    generateLearningPath.mockClear();

    await wrapper.setProps({
      externalTargetCode: "stack",
    });
    await flushUi();
    await submitPlanner(wrapper);

    expect(generateLearningPath).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      targetCodes: ["stack"],
      availableMinutes: 120,
      masteryByCode: {
        stack: 0.4,
        queue: 0.2,
      },
    });

    const targetSelect = wrapper.get("select");
    expect(targetSelect.element.value).toBe("stack");
  });

  it("does not render locate graph buttons in path result cards", async () => {
    routeQuery = {
      target: "",
    };
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
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

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);

    expect(wrapper.text()).not.toContain("前往学习图谱");
    expect(wrapper.emitted("focus-node")).toBeFalsy();
  });

  it("keeps resource entry on path items instead of rendering standalone resource section", async () => {
    routeQuery = {
      target: "",
    };
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列", detailScopeCode: "queue-detail" },
      ],
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
      overallExplanation: {
        summary: "由于目标链路依赖队列，且当前掌握度仍偏低，因此本轮优先安排队列。",
        labels: ["前置依赖链路", "掌握度待提升", "本轮优先安排"],
        bullets: [
          "当前目标：队列",
          "待补强节点：队列",
          "本轮安排：1 个节点，优先学习 队列",
        ],
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
      resourceRecommendations: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 2,
        },
      ],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);

    expect(wrapper.text()).not.toContain("按学习节点查看推荐资源");
    expect(wrapper.text()).not.toContain("当前导航速览");
    expect(
      wrapper.findAll("button").some((button) => button.text() === "查看推荐资源"),
    ).toBe(true);
    expect(wrapper.text()).toContain("本次路径为什么这样安排");
    expect(wrapper.text()).toContain("由于目标链路依赖队列");
  });

  it("does not render detail learning buttons in scheduled path items", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列", chapterNo: 4, detailScopeCode: "queue-detail" },
      ],
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

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);

    expect(wrapper.find("[data-testid='detail-learning-queue']").exists()).toBe(false);
    expect(wrapper.text()).not.toContain("细化学习");
  });

  it("does not render detail learning buttons even when scheduled and deferred nodes have detail scopes", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "graph-basic", label: "图的存储与遍历", chapterNo: 6, detailScopeCode: "graph-basic" },
        { code: "topological-sort", label: "拓扑排序与关键路径", chapterNo: 6, detailScopeCode: "topological-sort" },
      ],
    });
    generateLearningPath.mockResolvedValue({
      summary: {
        targetReachableWithinBudget: false,
        scheduledCount: 1,
        deferredCount: 1,
        masteredCount: 0,
        scheduledMinutes: 35,
        totalRequiredMinutes: 70,
        availableMinutes: 35,
      },
      path: [
        {
          code: "graph-basic",
          name: "图的存储与遍历",
          chapterNo: 6,
          estimatedMinutes: 35,
          masteryPercent: 42,
          status: "scheduled",
          reasonTrace: {
            triggerReasons: ["需要先补图基础。"],
            relevanceScore: 0.9,
            importanceScore: 0.85,
            timeCostPenalty: 0.2,
          },
        },
        {
          code: "topological-sort",
          name: "拓扑排序与关键路径",
          chapterNo: 6,
          estimatedMinutes: 35,
          masteryPercent: 28,
          status: "deferred",
          reasonTrace: {
            triggerReasons: ["当前预算不足，顺延到下一轮。"],
            relevanceScore: 1,
            importanceScore: 0.95,
            timeCostPenalty: 0.4,
          },
        },
      ],
      resourceRecommendations: [],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          "graph-basic": 0.42,
          "topological-sort": 0.28,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);

    expect(wrapper.find("[data-testid='detail-learning-graph-basic']").exists()).toBe(false);
    expect(wrapper.text()).not.toContain("细化学习");
  });

  it("shows practice-check-only entry instead of subjective mastery controls", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
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

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);

    expect(wrapper.text()).toContain("通过练习检验客观更新掌握度");
    expect(wrapper.text()).toContain("进入练习检验");
    expect(wrapper.text()).not.toContain("学习后掌握度");
    expect(wrapper.text()).not.toContain("当前可调区间");
  });

  it("exports current learning path as downloadable text", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
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
          explanation: {
            summary: "由于当前目标直接依赖队列，因此本轮优先安排。",
            labels: ["本轮学习", "高目标相关"],
          },
          reasonTrace: {
            triggerReasons: ["该节点与当前目标直接相关。"],
            relevanceScore: 0.9,
            importanceScore: 0.85,
            timeCostPenalty: 0.2,
          },
        },
      ],
      resourceRecommendations: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 1,
          primaryResourceTitle: "Hello 算法 · 队列",
          resources: [
            {
              title: "Hello 算法 · 队列",
              type: "article",
              source: "Hello 算法",
              recommendedPhase: "巩固",
            },
          ],
        },
      ],
    });

    const originalCreateObjectURL = window.URL.createObjectURL;
    const originalRevokeObjectURL = window.URL.revokeObjectURL;
    const createObjectURLMock = vi.fn(() => "blob:learning-path");
    const revokeObjectURLMock = vi.fn();
    window.URL.createObjectURL = createObjectURLMock;
    window.URL.revokeObjectURL = revokeObjectURLMock;
    const anchorClickSpy = vi
      .spyOn(HTMLAnchorElement.prototype, "click")
      .mockImplementation(() => {});

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);
    await wrapper.get("[data-testid='export-path-button']").trigger("click");

    expect(createObjectURLMock).toHaveBeenCalledTimes(1);
    expect(anchorClickSpy).toHaveBeenCalledTimes(1);
    expect(revokeObjectURLMock).toHaveBeenCalledTimes(1);

    anchorClickSpy.mockRestore();
    window.URL.createObjectURL = originalCreateObjectURL;
    window.URL.revokeObjectURL = originalRevokeObjectURL;
  });

  it("shows practice-check-only entry instead of subjective mastery controls", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
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

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });

    await flushUi();
    await submitPlanner(wrapper);
    expect(wrapper.text()).toContain("通过练习检验客观更新掌握度");
    expect(wrapper.text()).toContain("进入练习检验");
    expect(wrapper.text()).not.toContain("学习后掌握度");
    expect(wrapper.text()).not.toContain("当前可调区间");
  });

  it("routes to practice check directly with pending feedback context", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
    });
    generateLearningPath.mockResolvedValueOnce({
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

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.2,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });
    const navigationStore = useNavigationStore();

    await flushUi();
    await submitPlanner(wrapper);
    const practiceCheckButton = wrapper
      .findAll("button")
      .find((button) => button.text() === "进入练习检验");

    expect(practiceCheckButton).toBeTruthy();

    await practiceCheckButton.trigger("click");

    expect(navigationStore.practiceCheckContext).toMatchObject({
      learnerCode: "demo-learner",
      sourcePage: "home",
      targetCode: "queue",
      targetName: "队列",
      scopeCode: "root",
      scopeLabel: "课程主图",
      previousMasteryPercent: 20,
      completionStatus: "completed",
      notes: "",
    });
    expect(pushMock).toHaveBeenCalledWith({ name: "practice-check" });
  });

  it("shows one practice-check button per scheduled item and routes the clicked item", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "linear-list", label: "线性表" },
        { code: "queue", label: "队列" },
        { code: "graph-basic", label: "图的存储与遍历" },
      ],
    });
    generateLearningPath.mockResolvedValueOnce({
      summary: {
        targetReachableWithinBudget: true,
        scheduledCount: 3,
        deferredCount: 0,
        masteredCount: 0,
        scheduledMinutes: 110,
        totalRequiredMinutes: 110,
        availableMinutes: 120,
      },
      path: [
        {
          code: "linear-list",
          name: "线性表",
          chapterNo: 2,
          estimatedMinutes: 35,
          masteryPercent: 82,
          status: "scheduled",
          reasonTrace: {
            triggerReasons: ["主链路节点。"],
            relevanceScore: 0.8,
            importanceScore: 0.7,
            timeCostPenalty: 0.1,
          },
        },
        {
          code: "queue",
          name: "队列",
          chapterNo: 3,
          estimatedMinutes: 30,
          masteryPercent: 35,
          status: "scheduled",
          reasonTrace: {
            triggerReasons: ["主链路节点。"],
            relevanceScore: 0.9,
            importanceScore: 0.85,
            timeCostPenalty: 0.2,
          },
        },
        {
          code: "graph-basic",
          name: "图的存储与遍历",
          chapterNo: 6,
          estimatedMinutes: 45,
          masteryPercent: 15,
          status: "scheduled",
          reasonTrace: {
            triggerReasons: ["主链路节点。"],
            relevanceScore: 0.95,
            importanceScore: 0.9,
            timeCostPenalty: 0.3,
          },
        },
      ],
      resourceRecommendations: [],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          "linear-list": 0.82,
          queue: 0.35,
          "graph-basic": 0.15,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });
    const navigationStore = useNavigationStore();

    await flushUi();
    await submitPlanner(wrapper);

    const practiceButtons = wrapper.findAll('[data-testid^="practice-check-"]');
    expect(practiceButtons).toHaveLength(3);
    expect(wrapper.text()).toContain("线性表");
    expect(wrapper.text()).toContain("队列");
    expect(wrapper.text()).toContain("图的存储与遍历");

    await wrapper.get('[data-testid="practice-check-graph-basic"]').trigger("click");

    expect(navigationStore.practiceCheckContext).toMatchObject({
      learnerCode: "demo-learner",
      sourcePage: "home",
      targetCode: "graph-basic",
      targetName: "图的存储与遍历",
      scopeCode: "root",
      scopeLabel: "课程主图",
      previousMasteryPercent: 15,
      completionStatus: "completed",
      notes: "",
    });
    expect(pushMock).toHaveBeenCalledWith({ name: "practice-check" });
  });

  it("marks scheduled main-path nodes as practiced when historical practice records exist", async () => {
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "linear-list", label: "线性表" },
      ],
    });
    generateLearningPath.mockResolvedValue({
      summary: {
        scheduledCount: 1,
        deferredCount: 0,
        masteredCount: 0,
        scheduledMinutes: 35,
        totalRequiredMinutes: 35,
        targetReachableWithinBudget: true,
      },
      path: [
        {
          code: "linear-list",
          name: "线性表",
          chapterNo: 2,
          estimatedMinutes: 35,
          masteryPercent: 82,
          status: "scheduled",
          reasonTrace: {
            relevanceScore: 0.9,
            importanceScore: 0.8,
            timeCostPenalty: 0.2,
          },
          explanation: {
            summary: "当前建议优先巩固线性表。",
          },
        },
      ],
      practiceStatusByCode: ["linear-list"],
      resourceRecommendations: [],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          "linear-list": 0.82,
        },
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });
    await flushUi();
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(wrapper.text()).toContain("线性表");
    expect(wrapper.text()).toContain("已练习");
  });

  it("auto-regenerates the plan when home passes a force refresh token after practice", async () => {
    routeQuery = {
      target: "queue",
      practiceUpdated: "1",
    };
    fetchKnowledgeGraph.mockResolvedValue({
      nodes: [
        { code: "stack", label: "栈" },
        { code: "queue", label: "队列" },
      ],
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
          chapterNo: 3,
          estimatedMinutes: 30,
          masteryPercent: 35,
          status: "scheduled",
          reasonTrace: {
            triggerReasons: ["练习后重新规划。"],
            relevanceScore: 0.9,
            importanceScore: 0.85,
            timeCostPenalty: 0.2,
          },
        },
      ],
      resourceRecommendations: [],
    });

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(PathPlannerPanel, {
      props: {
        learnerCode: "demo-learner",
        initialMasteryByCode: {
          stack: 0.4,
          queue: 0.35,
        },
        forceRefreshToken: 0,
        profileLoading: false,
      },
      global: {
        plugins: [pinia],
      },
    });
    await flushUi();
    generateLearningPath.mockClear();

    await wrapper.setProps({
      forceRefreshToken: 1,
    });
    await flushUi();
    await flushUi();

    expect(generateLearningPath).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      targetCodes: ["queue"],
      availableMinutes: 120,
      masteryByCode: {
        stack: 0.4,
        queue: 0.35,
      },
    });
    expect(wrapper.text()).toContain("通过练习检验客观更新掌握度");
    expect(wrapper.text()).toContain("队列");
  });
});
