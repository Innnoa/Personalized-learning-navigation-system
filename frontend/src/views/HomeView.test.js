import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import HomeView from "./HomeView.vue";
import { useAuthStore } from "../stores/authStore";
import { fetchLearnerProfile } from "../api/learnerProfile";

const pushMock = vi.fn();
const replaceMock = vi.fn();

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock, replace: replaceMock }),
    useRoute: () => routeState,
  };
});

vi.mock("../api/learnerProfile", () => ({
  fetchLearnerProfile: vi.fn().mockResolvedValue({
    learner: {
      code: "demo-learner",
    },
    summary: {
      feedbackRecordCount: 2,
      resourceViewRecordCount: 3,
    },
    masteryByCode: {
      queue: 0.2,
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

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

function mountView(options = {}) {
  routeState = {
    name: "home",
    query: options.query || {},
  };

  const pinia = createPinia();
  setActivePinia(pinia);

  const authStore = useAuthStore();
  authStore.setSession(options.session || {
    user: { username: "student_demo" },
    roles: ["student"],
    activeRole: "student",
    linkedLearner: { learnerCode: "demo-learner", learnerName: "槐诗" },
  });

  return {
    authStore,
    wrapper: mount(HomeView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            template: "<div><slot /></div>",
          },
          DetailLearningWorkspace: {
            props: ["section"],
            emits: ["profile-updated"],
            template:
              "<div data-testid='detail-workspace' :data-scope='section?.scopeCode' :data-parent='section?.code'></div>",
          },
          PathPlannerPanel: {
            props: [
              "learnerCode",
              "feedbackRecordCount",
              "initialMasteryByCode",
              "forceRefreshToken",
            ],
            emits: ["feedback-saved", "focus-node"],
            template:
            "<div data-testid='planner-panel' :data-learner-code='learnerCode' :data-feedback='feedbackRecordCount' :data-mastery='initialMasteryByCode.queue' :data-refresh-token='forceRefreshToken'><button data-testid='planner-focus-node' @click=\"$emit('focus-node', 'queue')\">focus</button></div>",
          },
        },
      },
    }),
  };
}

describe("HomeView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    replaceMock.mockReset();
    fetchLearnerProfile.mockClear();
  });

  it("passes learner profile summary into planner panel", async () => {
    const { wrapper } = mountView();

    await flushUi();

    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-feedback")).toBe(
      "2",
    );
    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-mastery")).toBe(
      "0.2",
    );
  });

  it("prefers linked learner code from auth store for logged-in students", async () => {
    const { wrapper } = mountView({
      session: {
        currentUser: {
          id: 7,
          username: "student7",
        },
        currentRoles: ["student"],
        activeRole: "student",
        linkedLearner: {
          learnerCode: "learner-007",
        },
      },
    });

    await flushUi();

    expect(fetchLearnerProfile).toHaveBeenCalledWith({
      learnerCode: "learner-007",
    });
    expect(
      wrapper.get("[data-testid='planner-panel']").attributes("data-learner-code"),
    ).toBe("learner-007");
  });

  it("redirects planner focus action to the learning graph page", async () => {
    const { wrapper } = mountView();

    await flushUi();
    await wrapper.get("[data-testid='planner-focus-node']").trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "learning-graph",
      query: { focus: "queue" },
    });
  });

  it("reloads learner profile and clears the practice updated query on home", async () => {
    const { wrapper } = mountView({
      query: {
        practiceUpdated: "1",
      },
    });

    await flushUi();

    expect(fetchLearnerProfile).toHaveBeenCalledTimes(2);
    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-refresh-token")).toBe("1");
    expect(replaceMock).toHaveBeenCalledWith({
      name: "home",
      query: {},
    });
  });

  it("renders detail workspace when scope query indicates detail planning mode", async () => {
    const { wrapper } = mountView({
      query: {
        scope: "queue-detail",
        target: "queue-array",
      },
    });

    await flushUi();

    expect(wrapper.find("[data-testid='planner-panel']").exists()).toBe(false);
    expect(wrapper.get("[data-testid='detail-workspace']").attributes("data-scope")).toBe(
      "queue-detail",
    );
  });

  it("clears cached detail workspace state for the current scope when practice updated returns from detail mode", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
    const authStore = useAuthStore();
    authStore.setSession({
      user: { username: "student_demo" },
      roles: ["student"],
      activeRole: "student",
      linkedLearner: { learnerCode: "demo-learner", learnerName: "槐诗" },
    });

    const { useNavigationStore } = await import("../stores/navigationStore");
    const store = useNavigationStore();
    store.setDetailLearningViewState("queue-detail", {
      targetNodeCode: "queue-linked",
      planResult: {
        path: [{ code: "queue-linked", status: "scheduled" }],
      },
    });

    routeState = {
      name: "home",
      query: {
        scope: "queue-detail",
        target: "queue-linked",
        practiceUpdated: "1",
      },
    };

    mount(HomeView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: { template: "<div><slot /></div>" },
          DetailLearningWorkspace: {
            props: ["section"],
            template: "<div data-testid='detail-workspace'></div>",
          },
          PathPlannerPanel: {
            template: "<div data-testid='planner-panel'></div>",
          },
        },
      },
    });

    await flushUi();

    expect(store.detailLearningViewStateByScopeCode("queue-detail")).toBe(null);
  });
});
