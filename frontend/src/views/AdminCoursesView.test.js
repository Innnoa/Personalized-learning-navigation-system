import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import AdminCoursesView from "./AdminCoursesView.vue";
import { fetchAdminCourses } from "../api/admin";

vi.mock("../api/admin", () => ({
  fetchAdminCourses: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  return mount(AdminCoursesView, {
    global: {
      stubs: {
        PageLayout: {
          template: "<div><slot /></div>",
        },
      },
    },
  });
}

describe("AdminCoursesView", () => {
  beforeEach(() => {
    fetchAdminCourses.mockReset();
  });

  it("contains data-structures course", async () => {
    fetchAdminCourses.mockResolvedValue({
      courses: [
        {
          courseCode: "data-structures",
          courseName: "数据结构",
          knowledgePointCount: 14,
          targetAudience: "2022级计算机类本科生",
        },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    expect(wrapper.get("[data-testid='admin-course-list']").text()).toContain(
      "data-structures",
    );
  });
});
