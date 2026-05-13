import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import AdminAssignmentsView from "./AdminAssignmentsView.vue";
import { fetchAdminAssignments } from "../api/admin";

vi.mock("../api/admin", () => ({
  fetchAdminAssignments: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  return mount(AdminAssignmentsView, {
    global: {
      stubs: {
        PageLayout: {
          template: "<div><slot /></div>",
        },
      },
    },
  });
}

describe("AdminAssignmentsView", () => {
  beforeEach(() => {
    fetchAdminAssignments.mockReset();
  });

  it("shows teacher_demo assigned to data-structures", async () => {
    fetchAdminAssignments.mockResolvedValue({
      assignments: [
        {
          teacherUsername: "teacher_demo",
          courseCode: "data-structures",
          courseName: "数据结构",
        },
      ],
    });

    const wrapper = mountView();
    await flushUi();

    const text = wrapper.get("[data-testid='admin-assignment-list']").text();
    expect(text).toContain("teacher_demo");
    expect(text).toContain("data-structures");
  });
});
