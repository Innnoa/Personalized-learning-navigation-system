import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCourseOverviewView from "./TeacherCourseOverviewView.vue";
import { useAuthStore } from "../stores/authStore";
import { fetchTeacherCourseOverview } from "../api/teacher";

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => ({
      params: {
        courseCode: "data-structures",
      },
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
  });

  it("displays knowledge point count and maintenance-entry hints", async () => {
    fetchTeacherCourseOverview.mockResolvedValue({
      course: {
        courseCode: "data-structures",
        courseName: "数据结构",
        description: "本科毕设演示课程，覆盖数据结构课程的核心知识点依赖主线。",
        targetAudience: "2022级计算机类本科生",
        knowledgePointCount: 14,
      },
    });

    const wrapper = mountView();
    await flushUi();

    expect(fetchTeacherCourseOverview).toHaveBeenCalledWith({
      username: "teacher_demo",
      courseCode: "data-structures",
    });
    expect(wrapper.text()).toContain("14");
    expect(wrapper.text()).toContain("maintenance-entry");
  });
});
