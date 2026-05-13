import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import DetailLearningView from "./DetailLearningView.vue";
import { useAuthStore } from "../stores/authStore";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();
const replaceMock = vi.fn();
const routeState = {
  query: {},
};

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => routeState,
    useRouter: () => ({
      push: pushMock,
      replace: replaceMock,
    }),
  };
});

vi.mock("../api/learnerProfile", () => ({
  fetchLearnerProfile: vi.fn().mockResolvedValue({
    learner: {
      code: "demo-learner",
    },
    masteryByCode: {
      "queue-array": 0.25,
    },
  }),
}));

const { fetchLearnerProfile } = await import("../api/learnerProfile");

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

function mountView(options = {}) {
  const pinia = createPinia();
  setActivePinia(pinia);

  const authStore = useAuthStore();
  if (options.session) {
    authStore.setSession(options.session);
  }

  return {
    authStore,
    store: useNavigationStore(),
    wrapper: mount(DetailLearningView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            template: "<div><slot name='hero-side' /><slot /></div>",
          },
          DetailLearningWorkspace:
            options.detailWorkspaceStub || {
              props: ["section"],
              template:
                "<div data-testid='detail-workspace'>{{ section?.scopeCode || 'empty' }}</div>",
            },
        },
      },
    }),
  };
}

describe("DetailLearningView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    replaceMock.mockReset();
    routeState.query = {};
    fetchLearnerProfile.mockClear();
  });

  it("shows empty state when no detail learning context exists", async () => {
    const { wrapper } = mountView();

    await flushUi();

    expect(wrapper.text()).toContain("当前还没有可用的细化学习分支");
    expect(wrapper.text()).toContain("当前页尚未收到首页路径规划的细化分支上下文");
    expect(wrapper.find("[data-testid='detail-workspace']").exists()).toBe(false);
  });

  it("renders current section workspace and switches branches", async () => {
    routeState.query = {
      scope: "queue-detail",
    };

    const { wrapper, store } = mountView();
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

    await flushUi();

    expect(wrapper.get("[data-testid='detail-workspace']").text()).toContain(
      "queue-detail",
    );

    await wrapper.get("[data-testid='detail-section-tree-basic-detail']").trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "detail-learning",
      query: {
        scope: "tree-basic-detail",
      },
    });
  });

  it("reloads learner profile after detail workspace reports profile update", async () => {
    routeState.query = {
      scope: "queue-detail",
    };

    const { store, wrapper } = mountView({
      detailWorkspaceStub: {
        props: ["section"],
        template:
          "<div><div data-testid='detail-workspace'>{{ section?.scopeCode || 'empty' }}</div><button data-testid='detail-profile-updated' @click=\"$emit('profile-updated')\">refresh</button></div>",
      },
    });
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

    await flushUi();
    expect(fetchLearnerProfile).toHaveBeenCalledTimes(1);

    await wrapper.get("[data-testid='detail-profile-updated']").trigger("click");
    await flushUi();

    expect(fetchLearnerProfile).toHaveBeenCalledTimes(2);
  });

  it("clears stale detail learning context when stored learner differs from auth learner", async () => {
    routeState.query = {
      scope: "queue-detail",
    };

    const { store, wrapper } = mountView({
      session: {
        currentUser: {
          id: 3,
          username: "student3",
        },
        currentRoles: ["student"],
        activeRole: "student",
        linkedLearner: {
          learnerCode: "learner-003",
        },
      },
    });

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

    await flushUi();

    expect(store.detailLearningSections).toHaveLength(0);
    expect(store.detailLearningSummary.selectedScopeCode).toBe("");
    expect(wrapper.text()).toContain("当前页尚未收到首页路径规划的细化分支上下文");
    expect(fetchLearnerProfile).toHaveBeenCalledWith({
      learnerCode: "learner-003",
    });
  });
});
