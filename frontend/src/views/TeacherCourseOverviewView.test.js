import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCourseOverviewView from "./TeacherCourseOverviewView.vue";
import { useAuthStore } from "../stores/authStore";
import { fetchTeacherCourseOverview } from "../api/teacher";

const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => ({
      params: {
        courseCode: "data-structures",
      },
    }),
    useRouter: () => ({
      push: pushMock,
    }),
  };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherCourseOverview: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  const pinia = createPinia();
  setActivePinia(pinia);

  const authStore = useAuthStore();
  authStore.setSession({
    currentUser: {
      id: "teacher-001",
      username: "teacher_demo",
    },
    currentRoles: ["teacher"],
    activeRole: "teacher",
  });

  return mount(TeacherCourseOverviewView, {
    global: {
      plugins: [pinia],
      stubs: {
        PageLayout: {
          template: "<div><slot /></div>",
        },
      },
    },
  });
}

describe("TeacherCourseOverviewView", () => {
  beforeEach(() => {
    fetchTeacherCourseOverview.mockReset();
    pushMock.mockReset();
  });

  it("displays maintenance status, uncovered point list, and chapter progress bars", async () => {
    fetchTeacherCourseOverview.mockResolvedValue({
      course: {
        courseCode: "data-structures",
        courseName: "数据结构",
        description: "本科毕设演示课程，覆盖数据结构课程的核心知识点依赖主线。",
        targetAudience: "2022级计算机类本科生",
        teacherUsername: "teacher_demo",
        knowledgePointCount: 14,
        dependencyCount: 18,
        resourceCount: 219,
        questionBankCount: 15,
        questionCount: 1421,
        resourceCoveredPointCount: 12,
        questionCoveredPointCount: 10,
        uncoveredPointCount: 4,
        maintenanceStatus: "待补题库",
      },
      chapterStats: [
        { chapterNo: 1, chapterName: "绪论", knowledgePointCount: 2 },
        { chapterNo: 2, chapterName: "线性表", knowledgePointCount: 3 },
      ],
      uncoveredPoints: [
        { code: "queue", name: "队列", missingCoverage: ["question_bank"] },
        { code: "kmp", name: "KMP 模式匹配", missingCoverage: ["resource", "question_bank"] },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    expect(fetchTeacherCourseOverview).toHaveBeenCalledWith({
      username: "teacher_demo",
      courseCode: "data-structures",
    });
    expect(wrapper.text()).toContain("14");
    expect(wrapper.text()).toContain("18");
    expect(wrapper.text()).toContain("219");
    expect(wrapper.text()).toContain("1421");
    expect(wrapper.text()).toContain("资源覆盖率");
    expect(wrapper.text()).toContain("86%");
    expect(wrapper.text()).toContain("71%");
    expect(wrapper.text()).toContain("章节分布");
    expect(wrapper.text()).toContain("绪论");
    expect(wrapper.text()).toContain("线性表");
    expect(wrapper.text()).toContain("teacher_demo");
    expect(wrapper.text()).toContain("待补题库");
    expect(wrapper.text()).toContain("覆盖缺口");
    expect(wrapper.text()).toContain("队列");
    expect(wrapper.text()).toContain("KMP 模式匹配");
    expect(wrapper.findAll(".chapter-bar")).toHaveLength(2);
    expect(wrapper.text()).not.toContain("班级学情总览");
    expect(wrapper.findAll('[data-testid="priority-student"]')).toHaveLength(0);
  });

  it("routes uncovered points to matching maintenance pages", async () => {
    fetchTeacherCourseOverview.mockResolvedValue({
      course: {
        courseCode: "data-structures",
        courseName: "数据结构",
        description: "课程概览",
        targetAudience: "2022级计算机类本科生",
        teacherUsername: "teacher_demo",
        knowledgePointCount: 14,
        dependencyCount: 18,
        resourceCount: 219,
        questionBankCount: 15,
        questionCount: 1421,
        resourceCoveredPointCount: 12,
        questionCoveredPointCount: 10,
        uncoveredPointCount: 4,
        maintenanceStatus: "待补题库",
      },
      chapterStats: [],
      uncoveredPoints: [
        { code: "queue", name: "队列", missingCoverage: ["question_bank"] },
        { code: "kmp", name: "KMP 模式匹配", missingCoverage: ["resource", "question_bank"] },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    await wrapper.findAll(".gap-action")[0].trigger("click");
    expect(pushMock).toHaveBeenCalledWith({
      name: "teacher-course-questions",
      params: { courseCode: "data-structures" },
      query: { focusPoint: "queue" },
    });

    await wrapper.findAll(".gap-action")[1].trigger("click");
    expect(pushMock).toHaveBeenCalledWith({
      name: "teacher-course-resources",
      params: { courseCode: "data-structures" },
      query: { focusPoint: "kmp" },
    });
  });
});
