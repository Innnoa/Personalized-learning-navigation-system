import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherDashboardView from "./TeacherDashboardView.vue";
import { useAuthStore } from "../stores/authStore";
import { fetchTeacherCourses } from "../api/teacher";

vi.mock("../api/teacher", () => ({
  fetchTeacherCourses: vi.fn(),
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

  return mount(TeacherDashboardView, {
    global: {
      plugins: [pinia],
      stubs: {
        PageLayout: {
          template: "<div><slot /></div>",
        },
        RouterLink: {
          template: "<a><slot /></a>",
        },
      },
    },
  });
}

describe("TeacherDashboardView", () => {
  beforeEach(() => {
    fetchTeacherCourses.mockReset();
  });

  it("loads course summary cards", async () => {
    fetchTeacherCourses.mockResolvedValue({
      teacherUsername: "teacher_demo",
      courses: [
        {
          courseCode: "data-structures",
          courseName: "数据结构",
          targetAudience: "2022级计算机类本科生",
        },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    expect(fetchTeacherCourses).toHaveBeenCalledWith({
      username: "teacher_demo",
    });
    const summaryCards = wrapper.findAll(".summary-card");
    expect(summaryCards).toHaveLength(2);
    expect(summaryCards[0].text()).toContain("已分配课程");
    expect(summaryCards[0].text()).toContain("1");
    expect(summaryCards[1].text()).toContain("维护入口");
    expect(summaryCards[1].text()).toContain("course overview");
    expect(wrapper.findAll("[data-testid='course-summary-card']")).toHaveLength(1);
    expect(wrapper.text()).toContain("data-structures");
    expect(wrapper.text()).toContain("数据结构");
  });
});
