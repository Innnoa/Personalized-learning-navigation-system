import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherCoursesView from "./TeacherCoursesView.vue";
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

  return mount(TeacherCoursesView, {
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

describe("TeacherCoursesView", () => {
  beforeEach(() => {
    fetchTeacherCourses.mockReset();
  });

  it("displays data-structures course data", async () => {
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

    expect(wrapper.get("[data-testid='teacher-course-list']").text()).toContain(
      "data-structures",
    );
    expect(wrapper.text()).toContain("2022级计算机类本科生");
  });
});
