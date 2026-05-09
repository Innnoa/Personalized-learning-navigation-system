import { createPinia, setActivePinia } from "pinia";
import { mount, RouterLinkStub } from "@vue/test-utils";
import { nextTick } from "vue";
import { beforeEach, describe, expect, it, vi } from "vitest";

import PageLayout from "./PageLayout.vue";
import { useNavigationStore } from "../stores/navigationStore";

const routeState = {
  name: "home",
  params: {},
  query: {},
};

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => routeState,
  };
});

function mountLayout() {
  const pinia = createPinia();
  setActivePinia(pinia);

  return {
    store: useNavigationStore(),
    wrapper: mount(PageLayout, {
      global: {
        plugins: [pinia],
        stubs: {
          RouterLink: RouterLinkStub,
        },
      },
    }),
  };
}

describe("PageLayout", () => {
  beforeEach(() => {
    routeState.name = "home";
    routeState.params = {};
    routeState.query = {};
    window.sessionStorage.clear();
  });

  it("shows disabled resource nav item when no resource snapshot exists", () => {
    const { wrapper } = mountLayout();

    expect(wrapper.text()).toContain("推荐资源");
    expect(wrapper.find(".page-nav-link--disabled").exists()).toBe(true);
  });

  it("links resource nav item to the latest cached resource section", async () => {
    const { wrapper, store } = mountLayout();

    store.setResourceRecommendationContext({
      learnerCode: "demo-learner",
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

    await nextTick();

    const resourceLink = wrapper
      .findAllComponents(RouterLinkStub)
      .find((item) => item.text() === "推荐资源");

    expect(resourceLink).toBeTruthy();
    expect(resourceLink.props("to")).toEqual({
      name: "resource-recommendation",
      params: {
        code: "queue",
      },
      query: {
        level: "main",
      },
    });
  });

  it("links detail learning nav item to the latest cached detail scope", async () => {
    const { wrapper, store } = mountLayout();

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
      sourceTargetLabel: "图",
      sourcePage: "home",
    });

    await nextTick();

    const detailLink = wrapper
      .findAllComponents(RouterLinkStub)
      .find((item) => item.text() === "细化路径规划");

    expect(detailLink).toBeTruthy();
    expect(detailLink.props("to")).toEqual({
      name: "detail-learning",
      query: {
        scope: "queue-detail",
      },
    });
  });
});
