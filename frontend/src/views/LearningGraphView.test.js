import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import LearningGraphView from "./LearningGraphView.vue";

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

describe("LearningGraphView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    routeQuery.focus = "queue";
  });

  it("passes route focus query into learning graph component", async () => {
    const wrapper = mount(LearningGraphView, {
      global: {
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
      },
    });

    await flushUi();

    expect(wrapper.get("[data-testid='learning-graph']").attributes("data-focus")).toBe(
      "queue",
    );
  });

  it("redirects selected node back to home planner", async () => {
    const wrapper = mount(LearningGraphView, {
      global: {
        stubs: {
          PageLayout: {
            template: "<div><slot name='hero-side' /><slot /></div>",
          },
          LearnerLearningGraph: {
            emits: ["set-target"],
            template:
              "<button data-testid='set-target' @click=\"$emit('set-target', 'stack')\">set</button>",
          },
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
});
