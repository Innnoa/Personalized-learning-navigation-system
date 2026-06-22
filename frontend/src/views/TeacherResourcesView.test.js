import { flushPromises, mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import TeacherResourcesView from "./TeacherResourcesView.vue";

const route = {
  params: {
    courseCode: "data-structures",
  },
  query: {},
};

const authStore = {
  currentUser: {
    username: "teacher_demo",
  },
};

const fetchCourseResources = vi.fn();
const createCourseResource = vi.fn();
const updateCourseResource = vi.fn();
const deleteCourseResource = vi.fn();

vi.mock("vue-router", () => ({
  useRoute: () => route,
}));

vi.mock("../stores/authStore", () => ({
  useAuthStore: () => authStore,
}));

vi.mock("../api/teacher", () => ({
  fetchCourseResources: (...args) => fetchCourseResources(...args),
  createCourseResource: (...args) => createCourseResource(...args),
  updateCourseResource: (...args) => updateCourseResource(...args),
  deleteCourseResource: (...args) => deleteCourseResource(...args),
}));

function mountView() {
  return mount(TeacherResourcesView, {
    global: {
      stubs: {
        teleport: true,
      },
    },
  });
}

describe("TeacherResourcesView", () => {
  beforeEach(() => {
    fetchCourseResources.mockReset();
    createCourseResource.mockReset();
    updateCourseResource.mockReset();
    deleteCourseResource.mockReset();
    vi.stubGlobal("confirm", vi.fn(() => true));

    fetchCourseResources.mockResolvedValue({
      resources: [
        {
          id: 1,
          knowledgePointCode: "ds-intro",
          title: "绪论导学",
          resourceType: "article",
          source: "教材",
          url: "https://example.com/intro",
          description: "绪论资源",
          recommendedUsage: "先阅读",
          recommendedPhase: "preview",
          importanceWeight: 1.5,
          estimatedMinutes: 12,
          minMastery: 0.1,
          maxMastery: 0.5,
          status: "active",
        },
        {
          id: 2,
          knowledgePointCode: "stack",
          title: "栈视频",
          resourceType: "video",
          source: "课程视频",
          url: "https://example.com/stack",
          description: "栈资源",
          recommendedUsage: "边看边记",
          recommendedPhase: "learn",
          importanceWeight: 2.1,
          estimatedMinutes: 25,
          minMastery: 0.2,
          maxMastery: 0.9,
          status: "active",
        },
      ],
    });
  });

  it("renders resources as table rows", async () => {
    const wrapper = mountView();
    await flushPromises();

    expect(wrapper.findAll("tbody tr")).toHaveLength(2);
    expect(wrapper.text()).toContain("绪论导学");
    expect(wrapper.text()).toContain("栈视频");
    expect(wrapper.text()).toContain("1.5");
    expect(wrapper.text()).toContain("25 分钟");
  });

  it("filters rows by knowledge point", async () => {
    const wrapper = mountView();
    await flushPromises();

    await wrapper.find("select.filter-select").setValue("stack");

    expect(wrapper.text()).toContain("栈视频");
    expect(wrapper.text()).not.toContain("绪论导学");
  });

  it("applies focusPoint query as initial filter", async () => {
    route.query = { focusPoint: "stack" };

    const wrapper = mountView();
    await flushPromises();

    expect(wrapper.text()).toContain("栈视频");
    expect(wrapper.text()).not.toContain("绪论导学");

    route.query = {};
  });

  it("creates a resource through the new post API", async () => {
    createCourseResource.mockResolvedValue({ created: true });

    const wrapper = mountView();
    await flushPromises();

    await wrapper.find("button.btn--primary").trigger("click");
    const inputs = wrapper.findAll(".dialog-panel .edit-input");
    await inputs[1].setValue("新资源");
    await inputs[4].setValue("资料库");
    await inputs[5].setValue("https://example.com/new-resource");
    await inputs[6].setValue("2.6");
    await inputs[7].setValue("20");
    await inputs[8].setValue("0.2");
    await inputs[9].setValue("0.8");
    await wrapper.find(".dialog-actions .btn--primary").trigger("click");

    expect(createCourseResource).toHaveBeenCalledWith(
      "data-structures",
      expect.objectContaining({
        title: "新资源",
        importanceWeight: 2.6,
        estimatedMinutes: 20,
        minMastery: 0.2,
        maxMastery: 0.8,
        source: "资料库",
        url: "https://example.com/new-resource",
      }),
      { username: "teacher_demo" },
    );
  });
});
