import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCourseStatsView from "./TeacherCourseStatsView.vue";
import { fetchTeacherCourseStats } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return { ...actual, useRoute: () => routeState };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherCourseStats: vi.fn().mockResolvedValue({
    courseCode: "data-structures",
    courseName: "数据结构",
    studentCount: 3,
    avgMastery: 0.58,
    masteryDistribution: {
      low: { min: 0, max: 0.35, count: 0 },
      mid: { min: 0.35, max: 0.7, count: 2 },
      high: { min: 0.7, max: 1.0, count: 1 },
    },
  }),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  routeState = { params: { courseCode: "data-structures" } };
  const pinia = createPinia();
  setActivePinia(pinia);
  useAuthStore().setSession({
    user: { username: "teacher_demo" },
    roles: ["teacher"],
    activeRole: "teacher",
    linkedLearner: null,
  });
  return mount(TeacherCourseStatsView, {
    global: { plugins: [pinia], stubs: { RouterLink: true } },
  });
}

describe("TeacherCourseStatsView", () => {
  beforeEach(() => {
    fetchTeacherCourseStats.mockClear();
  });

  it("renders student count and average mastery", async () => {
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("3");
    expect(wrapper.text()).toContain("58%");
  });

  it("renders mastery distribution categories", async () => {
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("较高");
    expect(wrapper.text()).toContain("中等");
    expect(wrapper.text()).toContain("较低");
  });

  it("renders error state on API failure", async () => {
    fetchTeacherCourseStats.mockRejectedValueOnce(new Error("fail"));
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("未能读取");
  });
});
