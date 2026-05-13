import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import AdminDashboardView from "./AdminDashboardView.vue";
import {
  fetchAdminAssignments,
  fetchAdminCourses,
  fetchAdminUsers,
} from "../api/admin";

vi.mock("../api/admin", () => ({
  fetchAdminUsers: vi.fn(),
  fetchAdminCourses: vi.fn(),
  fetchAdminAssignments: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  return mount(AdminDashboardView, {
    global: {
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

describe("AdminDashboardView", () => {
  beforeEach(() => {
    fetchAdminUsers.mockReset();
    fetchAdminCourses.mockReset();
    fetchAdminAssignments.mockReset();
  });

  it("renders summary cards", async () => {
    fetchAdminUsers.mockResolvedValue({
      users: [
        { username: "admin_demo", roles: ["admin"] },
        { username: "student_demo", roles: ["student"] },
        { username: "teacher_demo", roles: ["teacher"] },
      ],
    });
    fetchAdminCourses.mockResolvedValue({
      courses: [{ courseCode: "data-structures", courseName: "数据结构" }],
    });
    fetchAdminAssignments.mockResolvedValue({
      assignments: [{ teacherUsername: "teacher_demo", courseCode: "data-structures" }],
    });

    const wrapper = mountView();
    await flushUi();

    const summaryCards = wrapper.findAll(".summary-card");
    expect(summaryCards).toHaveLength(3);
    expect(summaryCards[0].text()).toContain("用户总数");
    expect(summaryCards[0].text()).toContain("3");
    expect(summaryCards[1].text()).toContain("课程数量");
    expect(summaryCards[1].text()).toContain("1");
    expect(summaryCards[2].text()).toContain("授课分配");
    expect(summaryCards[2].text()).toContain("1");
  });
});
