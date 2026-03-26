import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import HomeView from "./HomeView.vue";
import { resetDemoState } from "../api/demo";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();
const replaceMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock, replace: replaceMock }),
  };
});

vi.mock("../api/health", () => ({
  fetchHealth: vi.fn().mockResolvedValue({
    status: "ok",
  }),
}));

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

vi.mock("../api/demo", () => ({
  resetDemoState: vi.fn().mockResolvedValue({
    learner: {
      code: "demo-learner",
    },
    summary: {
      feedbackRecordCount: 0,
      resourceViewRecordCount: 0,
    },
    masteryByCode: {
      queue: 0.35,
    },
    resetSummary: {
      clearedFeedbackRecordCount: 2,
      clearedResourceViewRecordCount: 3,
    },
  }),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView(options = {}) {
  const pinia = createPinia();
  setActivePinia(pinia);

  return mount(HomeView, {
    global: {
      plugins: [pinia],
      stubs: {
        PageLayout: {
          template: "<div><slot name='hero-side' /><slot /></div>",
        },
        HealthStatusCard: {
          template: "<div><slot name='actions' /></div>",
        },
        PathPlannerPanel: {
          props: [
            "feedbackRecordCount",
            "initialMasteryByCode",
          ],
          emits: ["feedback-saved", "focus-node"],
          template:
            "<div data-testid='planner-panel' :data-feedback='feedbackRecordCount' :data-mastery='initialMasteryByCode.queue'><button data-testid='planner-focus-node' @click=\"$emit('focus-node', 'queue')\">focus</button></div>",
        },
        ...(options.stubs || {}),
      },
    },
  });
}

describe("HomeView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    replaceMock.mockReset();
    resetDemoState.mockReset();
    resetDemoState.mockResolvedValue({
      learner: {
        code: "demo-learner",
      },
      summary: {
        feedbackRecordCount: 0,
        resourceViewRecordCount: 0,
      },
      masteryByCode: {
        queue: 0.35,
      },
      resetSummary: {
        clearedFeedbackRecordCount: 2,
        clearedResourceViewRecordCount: 3,
      },
    });
  });

  it("passes learner profile summary into planner panel", async () => {
    const wrapper = mountView();

    await flushUi();

    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-feedback")).toBe(
      "2",
    );
    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-mastery")).toBe(
      "0.2",
    );
  });

  it("redirects planner focus action to the learning graph page", async () => {
    const wrapper = mountView();

    await flushUi();
    await wrapper.get("[data-testid='planner-focus-node']").trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "learning-graph",
      query: { focus: "queue" },
    });
  });

  it("resets demo state and refreshes planner input", async () => {
    const wrapper = mountView();
    const navigationStore = useNavigationStore();
    navigationStore.setLearningGraphViewState({
      currentScopeCode: "stack-detail",
      selectedNodeCode: "push",
      lastFocusedNodeCode: "push",
      lastFocusedScopeCode: "stack-detail",
    });
    navigationStore.setDetailLearningContext({
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

    await flushUi();
    await wrapper.get(".demo-reset-button").trigger("click");
    await flushUi();

    expect(resetDemoState).toHaveBeenCalledWith();
    expect(replaceMock).toHaveBeenCalledWith({
      name: "home",
    });
    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-feedback")).toBe(
      "0",
    );
    expect(wrapper.get("[data-testid='planner-panel']").attributes("data-mastery")).toBe(
      "0.35",
    );
    expect(navigationStore.learningGraphViewState.currentScopeCode).toBe("root");
    expect(navigationStore.learningGraphViewState.selectedNodeCode).toBe("");
    expect(navigationStore.detailLearningSections).toEqual([]);
    expect(navigationStore.detailLearningSummary.selectedScopeCode).toBe("");
    expect(wrapper.text()).toContain("已恢复演示初始状态");
  });
});
