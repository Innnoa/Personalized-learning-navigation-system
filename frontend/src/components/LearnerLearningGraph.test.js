import { nextTick } from "vue";
import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import LearnerLearningGraph from "./LearnerLearningGraph.vue";
import { useNavigationStore } from "../stores/navigationStore";

vi.mock("../api/knowledgeGraph", () => ({
  fetchKnowledgeGraph: vi.fn(),
}));

const { fetchKnowledgeGraph } = await import("../api/knowledgeGraph");

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

vi.mock("cytoscape", () => ({
  default: vi.fn((config) => {
    const collection = createCollection();
    const nodeDataMap = new Map(
      (config.elements || [])
        .filter((item) => item?.data?.id && !item?.data?.source)
        .map((item) => [item.data.id, item.data]),
    );
    const elementCache = new Map();
    const handlers = new Map();

    return {
      destroy: vi.fn(),
      on: vi.fn((eventName, selector, handler) => {
        handlers.set(`${eventName}:${selector}`, handler);
      }),
      elements: () => collection,
      getElementById: (id) => {
        if (!elementCache.has(id)) {
          elementCache.set(id, createElement(id, collection, nodeDataMap));
        }

        return elementCache.get(id);
      },
    };
  }),
}));

async function flushUi() {
  await nextTick();
  await Promise.resolve();
  await nextTick();
  await Promise.resolve();
  await nextTick();
  await Promise.resolve();
  await nextTick();
  await Promise.resolve();
}

function buildRootPayload() {
  return {
    view: {
      scopeCode: "root",
      scopeName: "数据结构课程主线",
      isRoot: true,
    },
    breadcrumbs: [
      {
        scopeCode: "root",
        label: "数据结构课程主线",
      },
    ],
    nodes: [
      {
        id: "stack",
        code: "stack",
        label: "栈",
        chapterNo: 4,
        chapterName: "栈与队列",
        nodeType: "structure",
        estimatedMinutes: 30,
        displayOrder: 1,
        description: "栈基础",
        hasDetailGraph: true,
        detailScopeCode: "stack-detail",
      },
      {
        id: "queue",
        code: "queue",
        label: "队列",
        chapterNo: 4,
        chapterName: "栈与队列",
        nodeType: "structure",
        estimatedMinutes: 30,
        displayOrder: 2,
        description: "队列基础",
        hasDetailGraph: false,
        detailScopeCode: "",
      },
    ],
    edges: [
      {
        id: "stack-to-queue",
        source: "stack",
        target: "queue",
        relationType: "prerequisite",
      },
    ],
  };
}

function buildSecondLevelPayload() {
  return {
    view: {
      scopeCode: "stack-detail",
      scopeName: "栈的细化图谱",
      parentScopeCode: "root",
      parentNodeCode: "stack",
      isRoot: false,
    },
    breadcrumbs: [
      {
        scopeCode: "root",
        label: "数据结构课程主线",
      },
      {
        scopeCode: "stack-detail",
        label: "栈的细化图谱",
      },
    ],
    nodes: [
      {
        id: "push",
        code: "push",
        label: "入栈",
        chapterNo: 4,
        chapterName: "栈与队列",
        nodeType: "implementation",
        estimatedMinutes: 25,
        displayOrder: 1,
        description: "入栈基础",
        hasDetailGraph: true,
        detailScopeCode: "push-detail",
      },
      {
        id: "pop",
        code: "pop",
        label: "出栈",
        chapterNo: 4,
        chapterName: "栈与队列",
        nodeType: "implementation",
        estimatedMinutes: 25,
        displayOrder: 2,
        description: "出栈基础",
        hasDetailGraph: false,
        detailScopeCode: "",
      },
    ],
    edges: [
      {
        id: "push-to-pop",
        source: "push",
        target: "pop",
        relationType: "prerequisite",
      },
    ],
  };
}

function buildThirdLevelPayload() {
  return {
    view: {
      scopeCode: "push-detail",
      scopeName: "入栈流程细化图谱",
      parentScopeCode: "stack-detail",
      parentNodeCode: "push",
      isRoot: false,
    },
    breadcrumbs: [
      {
        scopeCode: "root",
        label: "数据结构课程主线",
      },
      {
        scopeCode: "stack-detail",
        label: "栈的细化图谱",
      },
      {
        scopeCode: "push-detail",
        label: "入栈流程细化图谱",
      },
    ],
    nodes: [
      {
        id: "push-check",
        code: "push-check",
        label: "判满检查",
        chapterNo: 4,
        chapterName: "栈与队列",
        nodeType: "algorithm",
        estimatedMinutes: 18,
        displayOrder: 1,
        description: "入栈前检查容量",
        hasDetailGraph: false,
        detailScopeCode: "",
      },
      {
        id: "push-write",
        code: "push-write",
        label: "写入元素",
        chapterNo: 4,
        chapterName: "栈与队列",
        nodeType: "implementation",
        estimatedMinutes: 18,
        displayOrder: 2,
        description: "写入并更新栈顶",
        hasDetailGraph: false,
        detailScopeCode: "",
      },
    ],
    edges: [
      {
        id: "push-check-to-write",
        source: "push-check",
        target: "push-write",
        relationType: "prerequisite",
      },
    ],
  };
}

function buildProfilePayload(overrides = {}) {
  return {
    masteryItems: [
      {
        code: "stack",
        masteryPercent: 90,
      },
      {
        code: "push",
        masteryPercent: 35,
      },
      {
        code: "queue",
        masteryPercent: 0,
      },
      {
        code: "pop",
        masteryPercent: 0,
      },
      {
        code: "push-check",
        masteryPercent: 0,
      },
      {
        code: "push-write",
        masteryPercent: 0,
      },
    ],
    recentFeedbackItems: [
      {
        code: "stack",
      },
    ],
    ...overrides,
  };
}

function mockScopedGraphFetch() {
  fetchKnowledgeGraph.mockImplementation(async (params = {}) => {
    const scopeCode = params.scopeCode || "root";

    if (scopeCode === "root") {
      return buildRootPayload();
    }

    if (scopeCode === "stack-detail") {
      return buildSecondLevelPayload();
    }

    if (scopeCode === "push-detail") {
      return buildThirdLevelPayload();
    }

    throw new Error(`unexpected scope ${scopeCode}`);
  });
}

describe("LearnerLearningGraph", () => {
  beforeEach(() => {
    vi.clearAllMocks();
    setActivePinia(createPinia());
  });

  it("shows current scope only and keeps second-level nodes out until drilldown", async () => {
    mockScopedGraphFetch();

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [createPinia()],
      },
      props: {
        profile: buildProfilePayload(),
      },
    });

    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledTimes(1);
    expect(fetchKnowledgeGraph).toHaveBeenCalledWith();
    expect(wrapper.get("[data-testid='current-scope-name']").text()).toBe("数据结构课程主线");
    expect(wrapper.get("[data-testid='graph-node-count']").text()).toBe("2 个");
    expect(wrapper.get("[data-testid='learned-node-count']").text()).toBe("1 个");
    expect(wrapper.get("[data-testid='graph-edge-count']").text()).toBe("1 条");
    expect(wrapper.text()).toContain("双击可细化节点可进入下一层图谱");
    expect(wrapper.text()).not.toContain("入栈流程细化图谱");
  });

  it("supports drilling down by double tap and returning through breadcrumbs", async () => {
    mockScopedGraphFetch();

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [createPinia()],
      },
      props: {
        profile: buildProfilePayload(),
      },
    });

    expect(typeof wrapper.vm.navigateToScope).toBe("function");

    await wrapper.vm.navigateToScope("stack-detail");
    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledWith({ scopeCode: "stack-detail" });
    expect(wrapper.get("[data-testid='current-scope-name']").text()).toBe("栈的细化图谱");
    expect(wrapper.get("[data-testid='graph-node-count']").text()).toBe("2 个");
    expect(wrapper.get("[data-testid='learned-node-count']").text()).toBe("1 个");
    expect(wrapper.find("button.detail-entry-button").exists()).toBe(false);
    expect(wrapper.find(".detail-entry").exists()).toBe(false);

    await wrapper.vm.navigateToScope("push-detail");
    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledWith({ scopeCode: "push-detail" });
    expect(wrapper.get("[data-testid='current-scope-name']").text()).toBe("入栈流程细化图谱");
    expect(wrapper.get("[data-testid='graph-node-count']").text()).toBe("2 个");
    expect(wrapper.get("[data-testid='learned-node-count']").text()).toBe("0 个");

    const rootBreadcrumb = wrapper
      .findAll("button")
      .find((button) => button.text().includes("数据结构课程主线"));
    expect(rootBreadcrumb).toBeTruthy();

    await rootBreadcrumb.trigger("click");
    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenLastCalledWith();
    expect(wrapper.get("[data-testid='current-scope-name']").text()).toBe("数据结构课程主线");
  });

  it("still renders current scope when learner has not started any node", async () => {
    mockScopedGraphFetch();

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [createPinia()],
      },
      props: {
        profile: buildProfilePayload({
          masteryItems: [
            { code: "stack", masteryPercent: 0 },
            { code: "push", masteryPercent: 0 },
          ],
          recentFeedbackItems: [],
        }),
      },
    });

    await flushUi();

    expect(wrapper.get("[data-testid='graph-node-count']").text()).toBe("2 个");
    expect(wrapper.get("[data-testid='learned-node-count']").text()).toBe("0 个");
    expect(wrapper.text()).toContain("未学习节点");
    expect(wrapper.text()).not.toContain("当前画像里还没有掌握度大于 0% 的知识点");
  });

  it("emits selected node as current learning target", async () => {
    mockScopedGraphFetch();

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [createPinia()],
      },
      props: {
        profile: buildProfilePayload(),
      },
    });

    await flushUi();
    await wrapper.get("[data-testid='graph-set-target-button']").trigger("click");

    expect(wrapper.emitted("set-target")).toEqual([[{ code: "stack", scopeCode: "root" }]]);
  });

  it("restores remembered scope and selected node when revisiting learning graph page", async () => {
    mockScopedGraphFetch();

    const pinia = createPinia();
    setActivePinia(pinia);
    const navigationStore = useNavigationStore();
    navigationStore.setLearningGraphViewState({
      currentScopeCode: "stack-detail",
      selectedNodeCode: "push",
      lastFocusedNodeCode: "push",
      lastFocusedScopeCode: "stack-detail",
    });

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [pinia],
      },
      props: {
        profile: buildProfilePayload(),
      },
    });

    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledWith({ scopeCode: "stack-detail" });
    expect(wrapper.get("[data-testid='current-scope-name']").text()).toBe("栈的细化图谱");
    expect(wrapper.text()).toContain("入栈");
  });

  it("uses explicit focus query before remembered scope when user jumps from planner", async () => {
    mockScopedGraphFetch();

    const pinia = createPinia();
    setActivePinia(pinia);
    const navigationStore = useNavigationStore();
    navigationStore.setLearningGraphViewState({
      currentScopeCode: "push-detail",
      selectedNodeCode: "push-check",
      lastFocusedNodeCode: "push-check",
      lastFocusedScopeCode: "push-detail",
    });

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [pinia],
      },
      props: {
        profile: buildProfilePayload(),
        preferredSelectedCode: "queue",
      },
    });

    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledWith();
    expect(wrapper.get("[data-testid='current-scope-name']").text()).toBe("数据结构课程主线");
    expect(navigationStore.learningGraphViewState.currentScopeCode).toBe("root");
    expect(navigationStore.learningGraphViewState.selectedNodeCode).toBe("queue");
  });

  it("reloads graph with active course code after learner profile arrives", async () => {
    mockScopedGraphFetch();

    const wrapper = mount(LearnerLearningGraph, {
      global: {
        plugins: [createPinia()],
      },
      props: {
        profile: null,
      },
    });

    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledTimes(1);
    expect(fetchKnowledgeGraph).toHaveBeenNthCalledWith(1);

    await wrapper.setProps({
      profile: {
        ...buildProfilePayload(),
        course: {
          code: "custom-course",
          name: "自拟课程",
        },
      },
    });
    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledTimes(2);
    expect(fetchKnowledgeGraph).toHaveBeenNthCalledWith(2, {
      courseCode: "custom-course",
    });
  });
});
