import { createPinia, setActivePinia } from "pinia";
import { mount, RouterLinkStub } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import ResourceRecommendationView from "./ResourceRecommendationView.vue";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();
let routeParams = { code: "queue" };
let routeQuery = {};

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => ({ params: routeParams, query: routeQuery }),
    useRouter: () => ({ push: pushMock }),
  };
});

vi.mock("../api/resource", () => ({
  recordResourceView: vi.fn(),
}));

function mountView() {
  const pinia = createPinia();
  setActivePinia(pinia);

  return {
    store: useNavigationStore(),
    wrapper: mount(ResourceRecommendationView, {
      global: {
        plugins: [pinia],
        stubs: {
          RouterLink: RouterLinkStub,
        },
      },
    }),
  };
}

describe("ResourceRecommendationView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    routeParams = { code: "queue" };
    routeQuery = {};
    window.sessionStorage.clear();
  });

  it("shows empty snapshot guidance when no cached resource context exists", () => {
    const { wrapper } = mountView();

    expect(wrapper.text()).toContain("当前还没有可用的资源推荐快照");
    expect(wrapper.text()).toContain("返回首页路径规划");
  });

  it("shows fallback guidance when requested code is missing from cached snapshot", async () => {
    const { wrapper, store } = mountView();

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [
        {
          code: "stack",
          name: "栈",
          status: "scheduled",
          resourceCount: 2,
          recommendedUsage: "先看课程视频，再补图文。",
          resources: [],
        },
      ],
      targetLabel: "栈",
      availableMinutes: 90,
      scheduledCount: 1,
      deferredCount: 0,
    });

    await wrapper.vm.$forceUpdate();
    await Promise.resolve();

    expect(wrapper.text()).toContain("当前节点资源快照已失效");
    expect(wrapper.text()).toContain("查看最近缓存资源");

    const button = wrapper
      .findAll("button")
      .find((item) => item.text().includes("查看最近缓存资源"));
    expect(button).toBeTruthy();

    await button.trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "resource-recommendation",
      params: { code: "stack" },
      query: {
        level: "main",
      },
    });
  });

  it("returns home with current target query when current section exists", async () => {
    const { wrapper, store } = mountView();

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 2,
          recommendedUsage: "先看课程视频，再补图文。",
          resources: [],
        },
      ],
      targetLabel: "队列",
      availableMinutes: 90,
      scheduledCount: 1,
      deferredCount: 0,
    });

    await wrapper.vm.$forceUpdate();
    await Promise.resolve();

    const button = wrapper
      .findAll("button")
      .find((item) => item.text().includes("返回首页路径规划"));
    expect(button).toBeTruthy();

    await button.trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "home",
      query: {
        target: "queue",
      },
    });
  });

  it("returns to detail learning when current resource context comes from detail mode", async () => {
    routeParams = { code: "queue-linked" };
    routeQuery = { level: "detail" };
    const { wrapper, store } = mountView();

    store.setResourceRecommendationContext({
      learnerCode: "demo-learner",
      contextMode: "detail",
      sourcePage: "detail-learning",
      resourceRecommendationSections: [
        {
          code: "queue-linked",
          name: "链式队列",
          status: "scheduled",
          resourceCount: 1,
          recommendedUsage: "先看链式队列讲解。",
          scopeCode: "queue-detail",
          recordingKnowledgePointCode: "queue",
          resources: [],
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

    await wrapper.vm.$forceUpdate();
    await Promise.resolve();

    expect(wrapper.text()).toContain("返回细化学习");
    expect(wrapper.text()).toContain("所属一级节点：队列");

    const button = wrapper
      .findAll("button")
      .find((item) => item.text().includes("返回细化学习"));
    expect(button).toBeTruthy();

    await button.trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "detail-learning",
      query: {
        scope: "queue-detail",
      },
    });
  });

  it("supports switching between 一级页面 and 二级页面", async () => {
    routeParams = { code: "queue-linked" };
    routeQuery = { level: "detail" };
    const { wrapper, store } = mountView();

    store.setResourceRecommendationContext({
      learnerCode: "demo-learner",
      contextMode: "main",
      resourceRecommendationSections: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 2,
          recommendedUsage: "先看课程视频，再补图文。",
          resources: [],
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
      sourcePage: "detail-learning",
      resourceRecommendationSections: [
        {
          code: "queue-linked",
          name: "链式队列",
          status: "scheduled",
          resourceCount: 1,
          recommendedUsage: "先看链式队列讲解。",
          scopeCode: "queue-detail",
          recordingKnowledgePointCode: "queue",
          resources: [],
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

    await wrapper.vm.$forceUpdate();
    await Promise.resolve();

    expect(wrapper.text()).toContain("一级页面");
    expect(wrapper.text()).toContain("二级页面");

    const mainTab = wrapper
      .findAll("button")
      .find((item) => item.text().includes("一级页面"));
    expect(mainTab).toBeTruthy();

    await mainTab.trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "resource-recommendation",
      params: { code: "queue" },
      query: {
        level: "main",
      },
    });
  });

  it("distinguishes inherited and related resource origin labels", async () => {
    routeParams = { code: "queue-definition" };
    const { wrapper, store } = mountView();

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [
        {
          code: "queue-definition",
          name: "队列定义",
          status: "scheduled",
          resourceCount: 2,
          recommendedUsage: "先看主学资源，再补关联材料。",
          resources: [
            {
              url: "https://example.com/inherited",
              title: "队列基础课",
              type: "video",
              source: "课程视频",
              recommendedPhase: "主学",
              inheritedFromLabel: "队列",
              resourceCoverageType: "inherited",
            },
            {
              url: "https://example.com/related",
              title: "顺序队列讲解",
              type: "article",
              source: "课程讲义",
              recommendedPhase: "巩固",
              inheritedFromLabel: "顺序队列",
              resourceCoverageType: "related",
            },
          ],
        },
      ],
      targetLabel: "队列",
      availableMinutes: 90,
      scheduledCount: 1,
      deferredCount: 0,
    });

    await wrapper.vm.$forceUpdate();
    await Promise.resolve();

    expect(wrapper.text()).toContain("来自上层：队列");
    expect(wrapper.text()).toContain("来自关联节点：顺序队列");
    expect(wrapper.text()).toContain(
      "以下内容会混合复用上层节点材料与同层关联节点材料",
    );
  });

  it("limits displayed resources by current progress, ability and time budget", async () => {
    routeParams = { code: "queue" };
    const { wrapper, store } = mountView();

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          masteryPercent: 35,
          difficultyLevel: 4,
          resourceCount: 8,
          recommendedUsage: "优先看主学资源。",
          resources: [
            {
              url: "https://example.com/r1",
              title: "资源1",
              type: "video",
              source: "来源1",
            },
            {
              url: "https://example.com/r2",
              title: "资源2",
              type: "article",
              source: "来源2",
            },
            {
              url: "https://example.com/r3",
              title: "资源3",
              type: "article",
              source: "来源3",
            },
            {
              url: "https://example.com/r4",
              title: "资源4",
              type: "video",
              source: "来源4",
            },
            {
              url: "https://example.com/r5",
              title: "资源5",
              type: "article",
              source: "来源5",
            },
            {
              url: "https://example.com/r6",
              title: "资源6",
              type: "article",
              source: "来源6",
            },
            {
              url: "https://example.com/r7",
              title: "资源7",
              type: "article",
              source: "来源7",
            },
            {
              url: "https://example.com/r8",
              title: "资源8",
              type: "article",
              source: "来源8",
            },
          ],
        },
      ],
      targetLabel: "队列",
      availableMinutes: 45,
      scheduledCount: 1,
      deferredCount: 0,
    });

    await wrapper.vm.$forceUpdate();
    await Promise.resolve();

    const resourceLinks = wrapper.findAll(".resource-link");
    expect(resourceLinks.length).toBe(5);
    expect(wrapper.text()).toContain("展开全部资源（+3）");

    const expandButton = wrapper
      .findAll("button")
      .find((item) => item.text().includes("展开全部资源"));
    expect(expandButton).toBeTruthy();

    await expandButton.trigger("click");

    expect(wrapper.findAll(".resource-link").length).toBe(8);
    expect(wrapper.text()).toContain("收起额外资源");
  });

  it("shows time budget tier copy as 紧凑 / 标准 / 充裕", async () => {
    routeParams = { code: "queue" };
    const { wrapper, store } = mountView();

    const buildSection = () => ({
      code: "queue",
      name: "队列",
      status: "scheduled",
      masteryPercent: 35,
      difficultyLevel: 3,
      resourceCount: 1,
      recommendedUsage: "优先看主学资源。",
      resources: [
        {
          url: "https://example.com/r1",
          title: "资源1",
          type: "video",
          source: "来源1",
        },
      ],
    });

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [buildSection()],
      targetLabel: "队列",
      availableMinutes: 20,
      scheduledCount: 1,
      deferredCount: 0,
    });
    await wrapper.vm.$forceUpdate();
    await Promise.resolve();
    expect(wrapper.text()).toContain("20 分钟（紧凑）");

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [buildSection()],
      targetLabel: "队列",
      availableMinutes: 60,
      scheduledCount: 1,
      deferredCount: 0,
    });
    await wrapper.vm.$forceUpdate();
    await Promise.resolve();
    expect(wrapper.text()).toContain("60 分钟（标准）");

    store.setResourceRecommendationContext({
      learnerCode: "test-learner",
      resourceRecommendationSections: [buildSection()],
      targetLabel: "队列",
      availableMinutes: 120,
      scheduledCount: 1,
      deferredCount: 0,
    });
    await wrapper.vm.$forceUpdate();
    await Promise.resolve();
    expect(wrapper.text()).toContain("120 分钟（充裕）");
  });
});
