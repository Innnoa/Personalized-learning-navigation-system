import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { nextTick } from "vue";
import { beforeEach, describe, expect, it, vi } from "vitest";

import PracticeCheckView from "./PracticeCheckView.vue";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock }),
  };
});

function mountView() {
  const pinia = createPinia();
  setActivePinia(pinia);

  return {
    store: useNavigationStore(),
    wrapper: mount(PracticeCheckView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            template: "<div><slot /></div>",
          },
        },
      },
    }),
  };
}

describe("PracticeCheckView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    window.sessionStorage.clear();
  });

  it("renders practice check handoff context from navigation store", async () => {
    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "learner-001",
      sourcePage: "detail-learning",
      targetCode: "queue",
      targetName: "队列",
      scopeCode: "queue-detail",
      scopeLabel: "队列细化范围",
      feedbackBatchId: "batch-123",
      feedbackItemCount: 3,
    });

    await nextTick();

    expect(wrapper.text()).toContain("队列");
    expect(wrapper.text()).toContain("batch-123");
    expect(wrapper.text()).toContain("题目模块后续接入");
    expect(wrapper.text()).toContain("队列细化范围（queue-detail）");
  });

  it("renders placeholder values when practice check context is empty", () => {
    const { wrapper } = mountView();

    expect(wrapper.text()).toContain("未指定");
    expect(wrapper.text()).toContain("未生成");
    expect(wrapper.text()).toContain("demo-learner");
    expect(wrapper.text()).toContain("home");
    expect(wrapper.text()).toContain("题目模块后续接入");
  });

  it("navigates home when clicking the home button", async () => {
    const { wrapper } = mountView();

    const button = wrapper
      .findAll("button")
      .find((item) => item.text().includes("返回首页"));

    expect(button).toBeTruthy();

    await button.trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "home",
    });
  });

  it("navigates to learner profile when clicking the learner profile button", async () => {
    const { wrapper } = mountView();

    const button = wrapper
      .findAll("button")
      .find((item) => item.text().includes("前往学习者画像"));

    expect(button).toBeTruthy();

    await button.trigger("click");

    expect(pushMock).toHaveBeenCalledWith({
      name: "learner-profile",
    });
  });
});
