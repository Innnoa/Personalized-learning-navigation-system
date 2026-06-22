import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCourseStatsView from "./TeacherCourseStatsView.vue";
import {
  fetchTeacherCourseStats,
  fetchTeacherStudents,
} from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

let routeState;
const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => routeState,
    useRouter: () => ({
      push: pushMock,
    }),
  };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherCourseStats: vi.fn(),
  fetchTeacherStudents: vi.fn(),
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
    currentUser: { username: "teacher_demo" },
    currentRoles: ["teacher"],
    activeRole: "teacher",
  });
  return mount(TeacherCourseStatsView, {
    global: { plugins: [pinia], stubs: { RouterLink: true } },
  });
}

function summaryValues(wrapper) {
  return wrapper.findAll(".summary-panel .summary-card .summary-value").map((node) => node.text());
}

function distributionRows(wrapper) {
  return wrapper.findAll(".summary-panel .distribution-row").map((node) => node.text());
}

describe("TeacherCourseStatsView", () => {
  beforeEach(() => {
    fetchTeacherCourseStats.mockReset();
    fetchTeacherStudents.mockReset();
    pushMock.mockReset();
  });

  it("renders learning status summary and ranks priority students by mastery then activity", async () => {
    fetchTeacherCourseStats.mockResolvedValue({
      courseCode: "data-structures",
      courseName: "数据结构",
      studentCount: 3,
      avgMastery: 0.39,
      masteryDistribution: {
        low: { min: 0, max: 0.35, count: 2 },
        mid: { min: 0.35, max: 0.7, count: 1 },
        high: { min: 0.7, max: 1, count: 0 },
      },
    });
    fetchTeacherStudents.mockResolvedValue({
      courseCode: "data-structures",
      students: [
        {
          learnerCode: "no-activity-001",
          learnerName: "未活跃学生",
          major: "计算机科学",
          gradeLabel: "大二",
          overallMastery: 0.12,
          lastActivityAt: null,
        },
        {
          learnerCode: "active-002",
          learnerName: "活跃学生",
          major: "计算机科学",
          gradeLabel: "大二",
          overallMastery: 0.12,
          lastActivityAt: "2026-06-20 09:00 CST",
        },
        {
          learnerCode: "no-activity-003",
          learnerName: "高掌握学生",
          major: "计算机科学",
          gradeLabel: "大二",
          overallMastery: 0.28,
          lastActivityAt: null,
        },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    expect(fetchTeacherCourseStats).toHaveBeenCalledWith("data-structures", {
      username: "teacher_demo",
    });
    expect(fetchTeacherStudents).toHaveBeenCalledWith("data-structures", {
      username: "teacher_demo",
    });

    expect(wrapper.text()).toContain("班级学情总览");
    expect(summaryValues(wrapper)).toEqual(["3", "39%"]);
    expect(distributionRows(wrapper)[0]).toContain("较低（0-35%）");
    expect(distributionRows(wrapper)[0]).toContain("优先关注");
    expect(distributionRows(wrapper)[0]).toContain("2");
    expect(distributionRows(wrapper)[1]).toContain("中等（35-70%）");
    expect(distributionRows(wrapper)[1]).toContain("继续跟进");
    expect(distributionRows(wrapper)[1]).toContain("1");
    expect(distributionRows(wrapper)[2]).toContain("较高（70-100%）");
    expect(distributionRows(wrapper)[2]).toContain("状态稳定");
    expect(distributionRows(wrapper)[2]).toContain("0");

    const priorityStudents = wrapper.findAll('[data-testid="priority-student"]');
    expect(priorityStudents).toHaveLength(3);
    expect(priorityStudents[0].text()).toContain("未活跃学生");
    expect(priorityStudents[1].text()).toContain("活跃学生");
    expect(priorityStudents[2].text()).toContain("高掌握学生");

    await priorityStudents[0].find("button").trigger("click");
    expect(pushMock).toHaveBeenCalledWith({
      name: "teacher-student-progress",
      params: {
        courseCode: "data-structures",
        learnerCode: "no-activity-001",
      },
    });
  });

  it("keeps the priority list visible when summary stats fail", async () => {
    fetchTeacherCourseStats.mockRejectedValue(new Error("stats failed"));
    fetchTeacherStudents.mockResolvedValue({
      courseCode: "data-structures",
      students: [
        {
          learnerCode: "student-001",
          learnerName: "需要关注的学生",
          major: "计算机科学",
          gradeLabel: "大二",
          overallMastery: 0.22,
          lastActivityAt: null,
        },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    expect(wrapper.text()).toContain("班级学情总览");
    expect(wrapper.text()).toContain("未能读取班级学情，请稍后重试。");
    const priorityStudents = wrapper.findAll('[data-testid="priority-student"]');
    expect(priorityStudents).toHaveLength(1);
    expect(priorityStudents[0].text()).toContain("需要关注的学生");
  });

  it("keeps the summary visible and shows the empty state when no student learning data exists", async () => {
    fetchTeacherCourseStats.mockResolvedValue({
      courseCode: "data-structures",
      courseName: "数据结构",
      studentCount: 0,
      avgMastery: 0,
      masteryDistribution: {
        low: { min: 0, max: 0.35, count: 0 },
        mid: { min: 0.35, max: 0.7, count: 0 },
        high: { min: 0.7, max: 1, count: 0 },
      },
    });
    fetchTeacherStudents.mockResolvedValue({
      courseCode: "data-structures",
      students: [],
    });

    const wrapper = mountView();
    await flushUi();

    expect(summaryValues(wrapper)).toEqual(["0", "0%"]);
    expect(distributionRows(wrapper)[0]).toContain("较低（0-35%）");
    expect(distributionRows(wrapper)[0]).toContain("0");
    expect(distributionRows(wrapper)[1]).toContain("中等（35-70%）");
    expect(distributionRows(wrapper)[1]).toContain("0");
    expect(distributionRows(wrapper)[2]).toContain("较高（70-100%）");
    expect(distributionRows(wrapper)[2]).toContain("0");
    expect(wrapper.text()).toContain("暂无学生学情数据。");
    expect(wrapper.findAll('[data-testid="priority-student"]')).toHaveLength(0);
  });

  it("renders error state on full API failure", async () => {
    fetchTeacherCourseStats.mockRejectedValueOnce(new Error("stats failed"));
    fetchTeacherStudents.mockRejectedValueOnce(new Error("students failed"));
    const wrapper = mountView();
    await flushUi();
    expect(wrapper.text()).toContain("未能读取班级学情，请稍后重试。");
  });
});
