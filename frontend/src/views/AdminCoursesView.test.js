import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import AdminCoursesView from "./AdminCoursesView.vue";
import { fetchAdminCourses } from "../api/admin";

vi.mock("../api/admin", () => ({
  fetchAdminCourses: vi.fn(),
  updateAdminCourse: vi.fn(),
  deleteAdminCourse: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

describe("AdminCoursesView", () => {
  beforeEach(() => {
    fetchAdminCourses.mockReset();
  });

  it("contains data-structures course", async () => {
    fetchAdminCourses.mockResolvedValue({
      courses: [
        { courseCode: "data-structures", courseName: "数据结构", targetAudience: "大二", knowledgePointCount: 14 },
      ],
    });

    const wrapper = mount(AdminCoursesView, {
      global: { stubs: { PageLayout: { template: "<div><slot /></div>" } } },
    });
    await flushUi();

    const text = wrapper.find("table").text();
    expect(text).toContain("data-structures");
    expect(text).toContain("数据结构");
  });
});
