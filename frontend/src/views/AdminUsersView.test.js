import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import AdminUsersView from "./AdminUsersView.vue";
import { fetchAdminUsers } from "../api/admin";

vi.mock("../api/admin", () => ({
  fetchAdminUsers: vi.fn(),
  createAdminUser: vi.fn(),
  toggleAdminUserStatus: vi.fn(),
  setAdminUserRoles: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

describe("AdminUsersView", () => {
  beforeEach(() => {
    fetchAdminUsers.mockReset();
  });

  it("contains student_demo, teacher_demo and admin_demo", async () => {
    fetchAdminUsers.mockResolvedValue({
      users: [
        { username: "admin_demo", displayName: "Admin", roles: ["admin"], status: "active" },
        { username: "student_demo", displayName: "Student", roles: ["student"], status: "active" },
        { username: "teacher_demo", displayName: "Teacher", roles: ["teacher"], status: "active" },
      ],
    });

    const wrapper = mount(AdminUsersView, {
      global: { stubs: { PageLayout: { template: "<div><slot /></div>" } } },
    });
    await flushUi();

    const text = wrapper.find("table").text();
    expect(text).toContain("student_demo");
    expect(text).toContain("teacher_demo");
    expect(text).toContain("admin_demo");
  });
});
