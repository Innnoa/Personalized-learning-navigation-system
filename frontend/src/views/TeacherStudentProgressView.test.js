import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherStudentProgressView from "./TeacherStudentProgressView.vue";
import { useAuthStore } from "../stores/authStore";

let routeState;

const mockFetchProgress = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return { ...actual, useRoute: () => routeState };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherStudentProgress: (...args) => mockFetchProgress(...args),
}));

function mountView() {
  routeState = { params: { courseCode: "data-structures", learnerCode: "demo-learner" } };
  const pinia = createPinia();
  setActivePinia(pinia);
  useAuthStore().setSession({
    user: { username: "teacher_demo" },
    roles: ["teacher"],
    activeRole: "teacher",
    linkedLearner: null,
  });
  return mount(TeacherStudentProgressView, {
    props: { courseCode: "data-structures", learnerCode: "demo-learner" },
    global: {
      plugins: [pinia],
      stubs: { PageLayout: { template: "<div><slot /></div>" }, RouterLink: true },
    },
  });
}

const mockData = {
  learner: {
    code: "demo-learner",
    name: "演示学生",
    major: "计算机科学",
    gradeLabel: "大二",
    courseCode: "data-structures",
    courseName: "数据结构",
  },
  masteryByCode: { "ds-intro": 0.9, "linear-list": 0.85 },
  recentFeedback: [
    {
      nodeCode: "queue",
      nodeName: "队列",
      completionStatus: "completed",
      previousMastery: 0.35,
      updatedMastery: 0.85,
      recordedAt: "2026-05-20 10:30 CST",
    },
  ],
  recentPaths: [],
};

describe("TeacherStudentProgressView", () => {
  beforeEach(() => {
    mockFetchProgress.mockReset();
    mockFetchProgress.mockResolvedValue(mockData);
  });

  it("renders student info", async () => {
    const wrapper = mountView();
    await wrapper.vm.$nextTick();
    await new Promise((r) => setTimeout(r, 10));
    expect(wrapper.text()).toContain("演示学生");
    expect(wrapper.text()).toContain("计算机科学");
  });

  it("renders mastery list", async () => {
    const wrapper = mountView();
    await wrapper.vm.$nextTick();
    await new Promise((r) => setTimeout(r, 10));
    expect(wrapper.text()).toContain("ds-intro");
    expect(wrapper.text()).toContain("90%");
  });

  it("renders feedback records", async () => {
    const wrapper = mountView();
    await wrapper.vm.$nextTick();
    await new Promise((r) => setTimeout(r, 10));
    expect(wrapper.text()).toContain("队列");
    expect(wrapper.text()).toContain("已完成");
  });

  it("renders error on failure", async () => {
    mockFetchProgress.mockRejectedValueOnce(new Error("network"));
    const wrapper = mountView();
    await wrapper.vm.$nextTick();
    await new Promise((r) => setTimeout(r, 10));
    expect(wrapper.text()).toContain("未能读取");
  });
});
