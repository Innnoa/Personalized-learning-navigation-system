import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { describe, expect, it, vi } from "vitest";

import LearnerProfileView from "./LearnerProfileView.vue";
import { useAuthStore } from "../stores/authStore";

vi.mock("../api/learnerProfile", () => ({
  fetchLearnerProfile: vi.fn(),
}));

describe("LearnerProfileView", () => {
  it("shows unassigned message when no linked learner", async () => {
    const pinia = createPinia();
    setActivePinia(pinia);
    useAuthStore().setSession({
      user: { username: "new_student" },
      roles: ["student"],
      activeRole: "student",
      linkedLearner: null,
    });

    const wrapper = mount(LearnerProfileView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: { template: "<div><slot /></div>" },
        },
      },
    });

    // Wait for async lifecycle
    await new Promise((r) => setTimeout(r, 0));
    await wrapper.vm.$nextTick();
    await wrapper.vm.$nextTick();

    expect(wrapper.text()).toContain("暂未分配课程");
  });
});
