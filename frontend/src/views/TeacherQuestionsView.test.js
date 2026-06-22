import { flushPromises, mount } from "@vue/test-utils";
import { nextTick } from "vue";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherQuestionsView from "./TeacherQuestionsView.vue";

const route = {
  params: {
    courseCode: "data-structures",
  },
  query: {},
};

const authStore = {
  currentUser: {
    username: "teacher_demo",
  },
};

const fetchQuestionBanks = vi.fn();
const createQuestionBank = vi.fn();
const updateQuestionBank = vi.fn();
const deleteQuestionBank = vi.fn();
const fetchBankQuestions = vi.fn();
const createBankQuestion = vi.fn();
const updateBankQuestion = vi.fn();
const deleteBankQuestion = vi.fn();

vi.mock("vue-router", () => ({
  useRoute: () => route,
}));

vi.mock("../stores/authStore", () => ({
  useAuthStore: () => authStore,
}));

vi.mock("../api/teacher", () => ({
  fetchQuestionBanks: (...args) => fetchQuestionBanks(...args),
  createQuestionBank: (...args) => createQuestionBank(...args),
  updateQuestionBank: (...args) => updateQuestionBank(...args),
  deleteQuestionBank: (...args) => deleteQuestionBank(...args),
  fetchBankQuestions: (...args) => fetchBankQuestions(...args),
  createBankQuestion: (...args) => createBankQuestion(...args),
  updateBankQuestion: (...args) => updateBankQuestion(...args),
  deleteBankQuestion: (...args) => deleteBankQuestion(...args),
}));

function mountView() {
  return mount(TeacherQuestionsView, {
    global: {
      stubs: {
        teleport: true,
      },
    },
  });
}

describe("TeacherQuestionsView", () => {
  beforeEach(() => {
    fetchQuestionBanks.mockReset();
    createQuestionBank.mockReset();
    updateQuestionBank.mockReset();
    deleteQuestionBank.mockReset();
    fetchBankQuestions.mockReset();
    createBankQuestion.mockReset();
    updateBankQuestion.mockReset();
    deleteBankQuestion.mockReset();
    vi.stubGlobal("confirm", vi.fn(() => true));

    fetchQuestionBanks.mockResolvedValue({
      questionBanks: [
        {
          id: 10,
          knowledgePointCode: "ds-intro",
          bankCode: "ds-intro-default",
          bankName: "绪论题库",
          questionCount: 1,
        },
        {
          id: 11,
          knowledgePointCode: "stack",
          bankCode: "stack-default",
          bankName: "栈题库",
          questionCount: 0,
        },
      ],
    });

    fetchBankQuestions.mockResolvedValue({
      questions: [
        {
          id: 101,
          questionCode: "intro-1",
          questionType: "single_choice",
          difficultyLevel: 1,
          prompt: "什么是数据结构？",
          options: ["A", "B", "C", "D"],
          answer: { correctIndexes: [0] },
          explanation: "解释",
          estimatedSeconds: 60,
          importanceWeight: 1.7,
          minMastery: 0.0,
          maxMastery: 0.45,
          status: "active",
        },
      ],
      totalCount: 120,
    });
  });

  it("renders bank list and loads questions for the selected bank", async () => {
    const wrapper = mountView();
    await flushPromises();

    expect(wrapper.text()).toContain("绪论题库");
    expect(fetchBankQuestions).toHaveBeenCalledWith(
      "data-structures",
      10,
      { username: "teacher_demo", page: 1, pageSize: 50, keyword: "", questionType: "" },
    );
    expect(wrapper.text()).toContain("什么是数据结构？");
    expect(wrapper.text()).toContain("1.7");
    expect(wrapper.text()).toContain("60 秒");
  });

  it("selects bank by focusPoint query on initial load", async () => {
    route.query = { focusPoint: "stack" };
    fetchBankQuestions.mockResolvedValueOnce({ questions: [] });

    const wrapper = mountView();
    await flushPromises();

    expect(fetchBankQuestions).toHaveBeenCalledWith(
      "data-structures",
      11,
      { username: "teacher_demo", page: 1, pageSize: 50, keyword: "", questionType: "" },
    );

    route.query = {};
  });

  it("renders bank list before question loading finishes", async () => {
    let resolveQuestions;
    fetchBankQuestions.mockReturnValueOnce(new Promise((resolve) => {
      resolveQuestions = resolve;
    }));

    const wrapper = mountView();
    await flushPromises();

    expect(wrapper.text()).toContain("绪论题库");
    expect(wrapper.text()).toContain("栈题库");
    expect(wrapper.text()).toContain("正在加载题目...");

    resolveQuestions({ questions: [] });
    await flushPromises();
  });

  it("renders pagination summary from backend totalCount", async () => {
    const wrapper = mountView();
    await flushPromises();

    expect(wrapper.text()).toContain("第 1 / 3 页");
    expect(wrapper.text()).toContain("共 120 题");
  });

  it("creates a new bank", async () => {
    createQuestionBank.mockResolvedValue({ created: true });

    const wrapper = mountView();
    await flushPromises();

    const bankButtons = wrapper.findAll("button");
    await bankButtons.find((item) => item.text().includes("新增题库")).trigger("click");

    const inputs = wrapper.findAll(".dialog-panel--small .edit-input");
    await inputs[0].setValue("stack");
    await inputs[1].setValue("stack-default");
    await inputs[2].setValue("栈题库");
    await wrapper.find(".dialog-panel--small .dialog-actions .btn--primary").trigger("click");

    expect(createQuestionBank).toHaveBeenCalled();
  });

  it("creates a question with dynamic single-choice payload", async () => {
    createBankQuestion.mockResolvedValue({ created: true });

    const wrapper = mountView();
    await flushPromises();

    const buttons = wrapper.findAll("button");
    await buttons.find((item) => item.text().includes("新增题目")).trigger("click");

    wrapper.vm.questionForm.questionCode = "intro-2";
    wrapper.vm.questionForm.questionType = "single_choice";
    wrapper.vm.questionForm.difficultyLevel = 1;
    wrapper.vm.questionForm.estimatedSeconds = 45;
    wrapper.vm.questionForm.importanceWeight = 2.3;
    wrapper.vm.questionForm.minMastery = 0.1;
    wrapper.vm.questionForm.maxMastery = 0.6;
    wrapper.vm.questionForm.prompt = "数据结构研究对象是什么？";
    wrapper.vm.questionForm.options = ["逻辑结构", "显示器", "鼠标", "键盘"];
    wrapper.vm.questionForm.answerIndexesText = "1";
    wrapper.vm.questionForm.explanation = "";

    await wrapper.vm.submitQuestion();

    expect(createBankQuestion).toHaveBeenCalledWith(
      "data-structures",
      10,
      expect.objectContaining({
        questionCode: "intro-2",
        importanceWeight: 2.3,
        estimatedSeconds: 45,
        minMastery: 0.1,
        maxMastery: 0.6,
      }),
      { username: "teacher_demo" },
    );
  });

});
