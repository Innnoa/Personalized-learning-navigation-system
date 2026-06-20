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

vi.mock("../api/knowledgeGraph", () => ({
  fetchKnowledgeGraph: vi.fn().mockResolvedValue({
    view: {
      scopeCode: "queue-detail",
      scopeName: "队列",
      parentScopeCode: "root",
      parentNodeCode: "queue",
      isRoot: false,
    },
    nodes: [
      {
        code: "queue-array",
        chapterNo: 4,
      },
    ],
  }),
}));

const { fetchLearnerProfile } = await import("../api/learnerProfile");
const { fetchKnowledgeGraph } = await import("../api/knowledgeGraph");

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

function mountView(options = {}) {
  const pinia = createPinia();
  setActivePinia(pinia);

  const authStore = useAuthStore();
  authStore.setSession(
    options.session || {
      currentUser: {
        id: 1,
        username: "student-demo",
      },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: {
        learnerCode: "demo-learner",
      },
    },
  );

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
    fetchKnowledgeGraph.mockClear();
  });

  it("shows empty state when no route scope exists", async () => {
    const { wrapper } = mountView();

    await flushUi();

    expect(wrapper.text()).toContain("当前页尚未指定可细化范围");
    expect(wrapper.find("[data-testid='detail-workspace']").exists()).toBe(false);
  });

  it("renders workspace from route scope without home detail context", async () => {
    routeState.query = {
      scope: "queue-detail",
    };

    const { wrapper } = mountView();

    await flushUi();

    expect(fetchKnowledgeGraph).toHaveBeenCalledWith({
      scopeCode: "queue-detail",
    });
    expect(wrapper.get("[data-testid='detail-workspace']").text()).toContain(
      "queue-detail",
    );
  });

  it("renders current section workspace when route scope is available", async () => {
    routeState.query = {
      scope: "queue-detail",
    };

    const { wrapper } = mountView();

    await flushUi();

    expect(wrapper.get("[data-testid='detail-workspace']").text()).toContain(
      "queue-detail",
    );
  });

  it("reloads learner profile after detail workspace reports profile update", async () => {
    routeState.query = {
      scope: "queue-detail",
    };

    const { wrapper } = mountView({
      detailWorkspaceStub: {
        props: ["section"],
        template:
          "<div><div data-testid='detail-workspace'>{{ section?.scopeCode || 'empty' }}</div><button data-testid='detail-profile-updated' @click=\"$emit('profile-updated')\">refresh</button></div>",
      },
    });

    await flushUi();
    expect(fetchLearnerProfile).toHaveBeenCalledTimes(1);

    await wrapper.get("[data-testid='detail-profile-updated']").trigger("click");
    await flushUi();

    expect(fetchLearnerProfile).toHaveBeenCalledTimes(2);
  });

  it("keeps route-driven detail page usable when auth learner differs from stored context", async () => {
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

    expect(fetchLearnerProfile).toHaveBeenCalledWith({
      learnerCode: "learner-003",
    });
    expect(wrapper.get("[data-testid='detail-workspace']").text()).toContain(
      "queue-detail",
    );
  });
});
