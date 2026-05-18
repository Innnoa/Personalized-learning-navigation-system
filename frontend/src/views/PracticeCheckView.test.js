import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { nextTick } from "vue";
import { beforeEach, describe, expect, it, vi } from "vitest";

import PracticeCheckView from "./PracticeCheckView.vue";
import { submitLearningFeedback } from "../api/feedback";
import { useNavigationStore } from "../stores/navigationStore";
import { getPracticeCheckQuestionSet } from "../utils/practiceCheckQuestions";
import {
  computeWeightedMasteryPercent,
  mapCorrectCountToPracticeMastery,
  resolvePracticeCompletionStatus,
} from "../utils/practiceCheckScoring";

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

async function answerQuestions(wrapper, selectedOptionIndexes) {
  const radios = wrapper.findAll('input[type="radio"]');

  for (let questionIndex = 0; questionIndex < selectedOptionIndexes.length; questionIndex += 1) {
    const optionIndex = selectedOptionIndexes[questionIndex];
    await radios[questionIndex * 4 + optionIndex].setValue();
  }
}

function getExpectedResult(targetCode, previousMasteryPercent, selectedOptionIndexes) {
  const questionSet = getPracticeCheckQuestionSet(targetCode, { previousMasteryPercent });
  const correctCount = questionSet.reduce((count, question, index) => {
    if (question.correctIndex === selectedOptionIndexes[index]) {
      return count + 1;
    }

    return count;
  }, 0);
  const practiceMasteryPercent = mapCorrectCountToPracticeMastery({
    correctCount,
    totalCount: questionSet.length,
  });
  const weightedMasteryPercent = computeWeightedMasteryPercent({
    previousMasteryPercent,
    practiceMasteryPercent,
  });

  return {
    correctCount,
    totalCount: questionSet.length,
    practiceMasteryPercent,
    weightedMasteryPercent,
    completionStatus: resolvePracticeCompletionStatus({
      correctCount,
      totalCount: questionSet.length,
    }),
  };
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
    expect(wrapper.text()).toContain("第 5 题");
  });

  it("shows newly added graph-basic questions for common demo targets", async () => {
    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      targetCode: "graph-basic",
      targetName: "图的存储与遍历",
      previousMasteryPercent: 20,
    });

    await nextTick();

    expect(wrapper.text()).toContain("图的存储与遍历练习检验");
    expect(wrapper.text()).toContain("图的广度优先遍历使用的数据结构通常是？");
    expect(wrapper.text()).toContain("第 5 题");
    expect(wrapper.text()).not.toContain("该知识点练习题待补充");
  });

  it("shows reused linear-list questions for the main linear-list entry", async () => {
    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      targetCode: "linear-list",
      targetName: "线性表",
      previousMasteryPercent: 20,
    });

    await nextTick();

    expect(wrapper.text()).toContain("线性表练习检验");
    expect(wrapper.text()).toContain("线性表中的元素关系最典型特点是什么？");
    expect(wrapper.text()).not.toContain("该知识点练习题待补充");
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

  it("uses dedicated queue-linked questions for detail-learning targets and keeps payload semantics", async () => {
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

    const selectedOptionIndexes = [1, 0, 1, 1, 0];
    const expectedResult = getExpectedResult("queue-linked", 50, selectedOptionIndexes);

    expect(wrapper.text()).toContain("链式队列最直接的存储特点是什么？");
    expect(wrapper.text()).not.toContain("该知识点练习题待补充");

    await answerQuestions(wrapper, selectedOptionIndexes);
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
          completionStatus: expectedResult.completionStatus,
          selfRatedMastery: expectedResult.weightedMasteryPercent / 100,
        },
      ],
    });
  });

  it("reuses graph-basic practice questions for detail-learning bfs targets and keeps payload semantics", async () => {
    submitLearningFeedback.mockResolvedValue({
      masteryByCode: {
        "graph-basic-bfs": 0.62,
      },
    });

    const { store, wrapper } = mountView();

    store.setPracticeCheckContext({
      learnerCode: "demo-learner",
      sourcePage: "detail-learning",
      targetCode: "graph-basic-bfs",
      targetName: "广度优先遍历",
      previousMasteryPercent: 45,
    });

    await nextTick();

    const selectedOptionIndexes = [1, 0, 2, 1, 1];
    const expectedResult = getExpectedResult("graph-basic-bfs", 45, selectedOptionIndexes);

    expect(wrapper.text()).toContain("广度优先遍历的核心访问策略是什么？");
    expect(wrapper.text()).not.toContain("该知识点练习题待补充");

    await answerQuestions(wrapper, selectedOptionIndexes);
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(submitLearningFeedback).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      masteryByCode: {
        "graph-basic-bfs": 0.45,
      },
      feedbackItems: [
        {
          code: "graph-basic-bfs",
          completionStatus: expectedResult.completionStatus,
          selfRatedMastery: expectedResult.weightedMasteryPercent / 100,
        },
      ],
    });
  });

  it("shows an inline grading report and mastery change summary after successful submit", async () => {
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
    });

    const selectedOptionIndexes = [1, 1, 1, 1, 1];
    const expectedResult = getExpectedResult("queue", 50, selectedOptionIndexes);

    await nextTick();

    await answerQuestions(wrapper, selectedOptionIndexes);
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(wrapper.text()).toContain("批改报告");
    expect(wrapper.text()).toContain("第 1 题 · 回答正确");
    expect(wrapper.text()).toContain("第 2 题 · 回答错误");
    expect(wrapper.text()).toContain("正确答案：front");
    expect(wrapper.text()).toContain(`答对 ${expectedResult.correctCount} / ${expectedResult.totalCount} 题`);
    expect(wrapper.text()).toContain("提交前掌握度50%");
    expect(wrapper.text()).toContain(`练习掌握度${expectedResult.practiceMasteryPercent}%`);
    expect(wrapper.text()).toContain(`加权后掌握度${expectedResult.weightedMasteryPercent}%`);
    expect(wrapper.text()).toContain(`掌握度变化+${expectedResult.weightedMasteryPercent - 50}%`);
    expect(pushMock).not.toHaveBeenCalledWith({
      name: "home",
      query: { practiceUpdated: "1" },
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

  it("submits weighted mastery as decimal and stays on the grading report after success", async () => {
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

    const selectedOptionIndexes = [1, 1, 1, 1, 1];
    const expectedResult = getExpectedResult("queue", 50, selectedOptionIndexes);

    await nextTick();

    await answerQuestions(wrapper, selectedOptionIndexes);
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
          completionStatus: expectedResult.completionStatus,
          selfRatedMastery: expectedResult.weightedMasteryPercent / 100,
        },
      ],
    });
    expect(wrapper.text()).toContain("批改报告");
    expect(pushMock).not.toHaveBeenCalledWith({
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

    const selectedOptionIndexes = [0, 1, 0, 0, 1];
    const expectedResult = getExpectedResult("queue", 35, selectedOptionIndexes);

    await nextTick();

    await answerQuestions(wrapper, selectedOptionIndexes);
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
          completionStatus: expectedResult.completionStatus,
          selfRatedMastery: expectedResult.weightedMasteryPercent / 100,
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

    await answerQuestions(wrapper, [1, 0, 1, 1, 0]);
    await wrapper.get("form").trigger("submit");
    await flushUi();

    expect(wrapper.text()).toContain("练习结果未成功写入，请重试。");
    expect(pushMock).not.toHaveBeenCalled();
  });
});
