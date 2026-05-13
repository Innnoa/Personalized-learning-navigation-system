import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import AdminUsersView from "./AdminUsersView.vue";
import { fetchAdminUsers } from "../api/admin";

vi.mock("../api/admin", () => ({
  fetchAdminUsers: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  return mount(AdminUsersView, {
    global: {
      stubs: {
        PageLayout: {
          template: "<div><slot /></div>",
        },
      },
    },
  });
}

describe("AdminUsersView", () => {
  beforeEach(() => {
    fetchAdminUsers.mockReset();
  });

  it("contains student_demo, teacher_demo and admin_demo", async () => {
    fetchAdminUsers.mockResolvedValue({
      users: [
        { username: "admin_demo", roles: ["admin"] },
        { username: "student_demo", roles: ["student"] },
        { username: "teacher_demo", roles: ["teacher"] },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    const text = wrapper.get("[data-testid='admin-user-list']").text();
    expect(text).toContain("student_demo");
    expect(text).toContain("teacher_demo");
    expect(text).toContain("admin_demo");
  });
});
