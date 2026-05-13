import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import HomeView from "./HomeView.vue";
import { useAuthStore } from "../stores/authStore";
import { fetchLearnerProfile } from "../api/learnerProfile";

const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock }),
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

async function flushUi() {
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
    wrapper: mount(HomeView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            template: "<div><slot /></div>",
          },
          PathPlannerPanel: {
            props: [
              "learnerCode",
              "feedbackRecordCount",
              "initialMasteryByCode",
            ],
            emits: ["feedback-saved", "focus-node"],
            template:
            "<div data-testid='planner-panel' :data-learner-code='learnerCode' :data-feedback='feedbackRecordCount' :data-mastery='initialMasteryByCode.queue'><button data-testid='planner-focus-node' @click=\"$emit('focus-node', 'queue')\">focus</button></div>",
          },
        },
      },
    }),
  };
}

describe("HomeView", () => {
  beforeEach(() => {
    pushMock.mockReset();
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
});
