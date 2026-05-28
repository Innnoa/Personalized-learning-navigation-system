import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherStudentsView from "./TeacherStudentsView.vue";
import { fetchTeacherStudents } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return { ...actual, useRoute: () => routeState };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherStudents: vi.fn().mockResolvedValue({
    courseCode: "data-structures",
    students: [
      {
        learnerCode: "demo-learner",
        learnerName: "演示学生",
        major: "计算机科学",
        gradeLabel: "大二",
        overallMastery: 0.62,
        lastActivityAt: "2026-05-20 10:30 CST",
      },
    ],
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
  return mount(TeacherStudentsView, {
    global: {
      plugins: [pinia],
      stubs: { RouterLink: true },
    },
  });
}

describe("TeacherStudentsView", () => {
  beforeEach(() => {
    fetchTeacherStudents.mockClear();
  });

  it("renders student cards with names and mastery", async () => {
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("演示学生");
    expect(wrapper.text()).toContain("计算机科学");
    expect(wrapper.text()).toContain("62%");
  });

  it("renders empty state when no students", async () => {
    fetchTeacherStudents.mockResolvedValueOnce({
      courseCode: "data-structures",
      students: [],
    });
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("暂无学生");
  });

  it("renders error state on API failure", async () => {
    fetchTeacherStudents.mockRejectedValueOnce(new Error("network error"));
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("未能读取");
  });

  it("shows filter dropdowns", async () => {
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.find("select.filter-select").exists()).toBe(true);
  });
});
