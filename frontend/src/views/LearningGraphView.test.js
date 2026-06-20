import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import LearningGraphView from "./LearningGraphView.vue";
import { useAuthStore } from "../stores/authStore";

const pushMock = vi.fn();
const routeQuery = {
  focus: "queue",
};

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock }),
    useRoute: () => ({ query: routeQuery }),
  };
});

vi.mock("../api/learnerProfile", () => ({
  fetchLearnerProfile: vi.fn().mockResolvedValue({
    learner: {
      code: "demo-learner",
    },
    masteryItems: [],
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
  authStore.setSession({
    currentUser: {
      id: 1,
      username: "student-demo",
    },
    currentRoles: ["student"],
    activeRole: "student",
    linkedLearner: {
      learnerCode: "demo-learner",
    },
  });

  return mount(LearningGraphView, {
    global: {
      plugins: [pinia],
      stubs: options.stubs,
    },
  });
}

describe("LearningGraphView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    routeQuery.focus = "queue";
  });

  it("passes route focus query into learning graph component", async () => {
    const wrapper = mountView({
      stubs: {
        PageLayout: {
          template: "<div><slot name='hero-side' /><slot /></div>",
        },
        LearnerLearningGraph: {
          props: ["preferredSelectedCode"],
          template:
            "<div data-testid='learning-graph' :data-focus='preferredSelectedCode'></div>",
        },
      },
    });

    await flushUi();

    expect(wrapper.get("[data-testid='learning-graph']").attributes("data-focus")).toBe(
      "queue",
    );
  });

  it("redirects selected node back to home planner for root scope", async () => {
    const wrapper = mountView({
      stubs: {
        PageLayout: {
          template: "<div><slot name='hero-side' /><slot /></div>",
        },
        LearnerLearningGraph: {
          emits: ["set-target"],
          template:
            "<button data-testid='set-target' @click=\"$emit('set-target', { code: 'stack', scopeCode: 'root' })\">set</button>",
        },
      },
    });

    await flushUi();
    await wrapper.get("[data-testid='set-target']").trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "home",
      query: { target: "stack" },
    });
  });

  it("redirects selected node to unified planner for non-root scope", async () => {
    const wrapper = mountView({
      stubs: {
        PageLayout: {
          template: "<div><slot name='hero-side' /><slot /></div>",
        },
        LearnerLearningGraph: {
          emits: ["set-target"],
          template:
            "<button data-testid='set-target' @click=\"$emit('set-target', { code: 'stack-push', scopeCode: 'stack-detail' })\">set</button>",
        },
      },
    });

    await flushUi();
    await wrapper.get("[data-testid='set-target']").trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "home",
      query: { scope: "stack-detail", target: "stack-push" },
    });
  });
});
