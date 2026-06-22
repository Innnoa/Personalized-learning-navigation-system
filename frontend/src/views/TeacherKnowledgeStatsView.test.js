import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherKnowledgeStatsView from "./TeacherKnowledgeStatsView.vue";
import { useAuthStore } from "../stores/authStore";
import {
  fetchDependencies,
  fetchTeacherKnowledgeStats,
  updateDependencies,
} from "../api/teacher";

let routeState;

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRoute: () => routeState,
  };
});

vi.mock("../api/teacher", () => ({
  fetchTeacherKnowledgeStats: vi.fn(),
  createKnowledgePoint: vi.fn(),
  updateKnowledgePoint: vi.fn(),
  deleteKnowledgePoint: vi.fn(),
  fetchDependencies: vi.fn(),
  updateDependencies: vi.fn(),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  routeState = { params: { courseCode: "custom-course" } };
  const pinia = createPinia();
  setActivePinia(pinia);
  useAuthStore().setSession({
    currentUser: { username: "teacher_demo" },
    currentRoles: ["teacher"],
    activeRole: "teacher",
  });

  return mount(TeacherKnowledgeStatsView, {
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

describe("TeacherKnowledgeStatsView", () => {
  beforeEach(() => {
    fetchTeacherKnowledgeStats.mockReset();
    fetchDependencies.mockReset();
    updateDependencies.mockReset();
  });

  it("renders a dedicated wrapper inside dependency cells for prerequisite checkboxes", async () => {
    fetchTeacherKnowledgeStats.mockResolvedValue({
      knowledgePoints: [
        {
          code: "aaa",
          name: "111",
          chapterNo: 1,
          chapterName: "第一章",
          difficultyLevel: 1,
          avgMastery: 0.3,
          studentCount: 2,
        },
        {
          code: "sss",
          name: "2222",
          chapterNo: 1,
          chapterName: "第一章",
          difficultyLevel: 1,
          avgMastery: 0.4,
          studentCount: 2,
        },
      ],
    });
    fetchDependencies.mockResolvedValue({
      dependencies: [],
    });

    const wrapper = mountView();
    await flushUi();

    await wrapper.get(".deps-section .btn").trigger("click");
    await flushUi();

    const prereqCell = wrapper.get(".deps-table tbody tr .deps-prereqs");
    expect(prereqCell.element.tagName).toBe("TD");
    const prereqList = prereqCell.find(".deps-prereq-list");
    expect(prereqList.exists()).toBe(true);
    expect(prereqList.findAll(".dep-checkbox")).toHaveLength(1);
  });
});
