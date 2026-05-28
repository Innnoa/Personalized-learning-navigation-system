import { createPinia, setActivePinia } from "pinia";
import { mount, RouterLinkStub } from "@vue/test-utils";
import { nextTick } from "vue";
import { beforeEach, describe, expect, it, vi } from "vitest";

import PageLayout from "./PageLayout.vue";
import { useAuthStore } from "../stores/authStore";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();

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
    useRouter: () => ({ push: pushMock }),
  };
});

vi.mock("../api/auth", () => ({
  logout: vi.fn().mockResolvedValue({ authenticated: false }),
}));

function mountLayout(props = {}) {
  const pinia = createPinia();
  setActivePinia(pinia);
  if (props.session) {
    useAuthStore().setSession(props.session);
  }

  return {
    store: useNavigationStore(),
    authStore: useAuthStore(),
    wrapper: mount(PageLayout, {
      props: {
        ...props,
      },
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
    pushMock.mockReset();
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

  it("renders teacher navigation when roleScope is teacher", () => {
    const { wrapper } = mountLayout({ roleScope: "teacher" });

    const navText = wrapper.find("nav.page-nav").text();
    expect(navText).toContain("我的课程");
    expect(navText).toContain("课程列表");
    expect(navText).not.toContain("学习图谱");
    expect(navText).not.toContain("主图路径规划");
  });

  it("renders student navigation by default", () => {
    const { wrapper } = mountLayout();

    const navText = wrapper.find("nav.page-nav").text();
    expect(navText).toContain("学习图谱");
    expect(navText).toContain("主图路径规划");
    expect(navText).not.toContain("我的课程");
  });

  it("logout button clears session and redirects to login", async () => {
    const { wrapper, authStore } = mountLayout({
      roleScope: "teacher",
      session: {
        user: { username: "teacher_demo" },
        roles: ["teacher"],
        activeRole: "teacher",
      },
    });

    const logoutBtn = wrapper.find("button.logout-btn");
    expect(logoutBtn.exists()).toBe(true);

    await logoutBtn.trigger("click");
    await nextTick();

    expect(authStore.isAuthenticated).toBe(false);
    expect(pushMock).toHaveBeenCalledWith({ name: "login" });
  });
});
