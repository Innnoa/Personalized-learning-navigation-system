import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { nextTick } from "vue";
import { beforeEach, describe, expect, it, vi } from "vitest";

import PracticeCheckView from "./PracticeCheckView.vue";
import { submitLearningFeedback } from "../api/feedback";
import { useNavigationStore } from "../stores/navigationStore";

const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock }),
  };
});

vi.mock("../api/feedback", () => ({
  submitLearningFeedback: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await nextTick();
}

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
            props: ["title", "description", "eyebrow"],
            template:
              '<div><h1>{{ title }}</h1><p>{{ eyebrow }}</p><p>{{ description }}</p><slot /></div>',
          },
        },
      },
    }),
  };
}

describe("PracticeCheckView", () => {
  beforeEach(() => {
    pushMock.mockReset();
    submitLearningFeedback.mockReset();
    window.sessionStorage.clear();
  });

  it("shows fixed demo questions for supported practice targets", async () => {
    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      targetCode: "queue",
      targetName: "队列",
      previousMasteryPercent: 20,
    });

    await nextTick();

    expect(wrapper.text()).toContain("队列练习检验");
    expect(wrapper.text()).toContain("第 1 题");
    expect(wrapper.text()).toContain("第 2 题");
    expect(wrapper.text()).toContain("第 3 题");
  });

  it("shows unsupported placeholder when the target has no fixed demo questions", async () => {
    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      targetCode: "unsupported-node",
      targetName: "未配置节点",
      previousMasteryPercent: 20,
    });

    await nextTick();

    expect(wrapper.text()).toContain("该知识点练习题待补充");
  });

  it("reuses queue practice questions for detail-learning queue-linked targets", async () => {
    submitLearningFeedback.mockResolvedValue({
      masteryByCode: {
        "queue-linked": 0.71,
      },
    });

    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      sourcePage: "detail-learning",
      targetCode: "queue-linked",
      targetName: "链式队列",
      previousMasteryPercent: 50,
    });

    await nextTick();

    expect(wrapper.text()).toContain("第 1 题");
    expect(wrapper.text()).not.toContain("该知识点练习题待补充");

    const radios = wrapper.findAll('input[type="radio"]');
    await radios[1].setValue();
    await radios[4].setValue();
    await radios[10].setValue();
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(submitLearningFeedback).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      masteryByCode: {
        "queue-linked": 0.5,
      },
      feedbackItems: [
        {
          code: "queue-linked",
          completionStatus: "completed",
          selfRatedMastery: 0.71,
        },
      ],
    });
  });

  it("requires all questions to be answered before submit succeeds", async () => {
    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      targetCode: "queue",
      targetName: "队列",
      previousMasteryPercent: 20,
    });

    await nextTick();

    await wrapper.get('input[type="radio"]').setValue();
    await wrapper.get("form").trigger("submit");

    expect(wrapper.text()).toContain("请先完成全部题目再提交。");
    expect(submitLearningFeedback).not.toHaveBeenCalled();
  });

  it("submits weighted mastery as decimal and routes home on success", async () => {
    submitLearningFeedback.mockResolvedValue({
      masteryByCode: {
        queue: 0.59,
      },
    });

    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      sourcePage: "home",
      targetCode: "queue",
      targetName: "队列",
      previousMasteryPercent: 50,
      completionStatus: "completed",
      notes: "本轮已完成队列学习",
    });

    await nextTick();

    const radios = wrapper.findAll('input[type="radio"]');
    await radios[1].setValue();
    await radios[5].setValue();
    await radios[10].setValue();
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(submitLearningFeedback).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      masteryByCode: {
        queue: 0.5,
      },
      feedbackItems: [
        {
          code: "queue",
          completionStatus: "partial",
          selfRatedMastery: 0.59,
        },
      ],
    });
    expect(pushMock).toHaveBeenCalledWith({
      name: "home",
      query: { practiceUpdated: "1" },
    });
  });

  it("maps low-scoring practice attempts to a non-completed feedback status", async () => {
    submitLearningFeedback.mockResolvedValue({
      masteryByCode: {
        queue: 0.26,
      },
    });

    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      sourcePage: "home",
      targetCode: "queue",
      targetName: "队列",
      previousMasteryPercent: 35,
      completionStatus: "completed",
      notes: "原始上下文来自已完成学习",
    });

    await nextTick();

    const radios = wrapper.findAll('input[type="radio"]');
    await radios[0].setValue();
    await radios[6].setValue();
    await radios[8].setValue();
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(submitLearningFeedback).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      masteryByCode: {
        queue: 0.35,
      },
      feedbackItems: [
        {
          code: "queue",
          completionStatus: "blocked",
          selfRatedMastery: 0.26,
        },
      ],
    });
  });

  it("shows submission failure message when feedback write fails", async () => {
    submitLearningFeedback.mockRejectedValue(new Error("network"));

    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      targetCode: "queue",
      targetName: "队列",
      previousMasteryPercent: 50,
    });

    await nextTick();

    const radios = wrapper.findAll('input[type="radio"]');
    await radios[1].setValue();
    await radios[5].setValue();
    await radios[10].setValue();
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(wrapper.text()).toContain("练习结果未成功写入，请重试。");
    expect(pushMock).not.toHaveBeenCalled();
  });
});
