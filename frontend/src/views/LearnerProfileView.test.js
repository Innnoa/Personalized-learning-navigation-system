import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { describe, expect, it, vi } from "vitest";

import LearnerProfileView from "./LearnerProfileView.vue";
import ProfileActivityComposition from "../components/ProfileActivityComposition.vue";
import ProfileMasteryTrendChart from "../components/ProfileMasteryTrendChart.vue";
import ProfileProgressOverview from "../components/ProfileProgressOverview.vue";
import { useAuthStore } from "../stores/authStore";
import { fetchLearnerProfile } from "../api/learnerProfile";

vi.mock("../api/learnerProfile", () => ({
  fetchLearnerProfile: vi.fn(),
}));

function buildProfilePayload() {
  return {
    learner: {
      code: "demo-learner",
      name: "演示学习者",
      major: "计算机科学与技术",
      gradeLabel: "2022级",
    },
    course: {
      name: "数据结构",
    },
    summary: {
      averageMasteryPercent: 42,
      masteredCount: 2,
      inProgressCount: 4,
      notStartedCount: 8,
      feedbackRecordCount: 1,
      resourceViewRecordCount: 0,
      knowledgePointCount: 14,
    },
    masteryByCode: {
      queue: 0.2,
    },
    masteryItems: [
      {
        code: "queue",
        name: "队列",
        chapterNo: 4,
        chapterName: "栈与队列",
        masteryPercent: 20,
      },
    ],
    recentFeedbackItems: [],
    recentResourceViewItems: [],
    analytics: {
      masteryDistribution: [
        {
          code: "queue",
          name: "队列",
          chapterNo: 4,
          masteryPercent: 20,
        },
      ],
      feedbackTrend: [
        {
          index: 1,
          label: "反馈1",
          knowledgePointCode: "queue",
          averageUpdatedMasteryPercent: 20,
        },
      ],
      feedbackStatusComposition: [
        {
          status: "completed",
          label: "已完成",
          count: 1,
        },
        {
          status: "partial",
          label: "部分完成",
          count: 0,
        },
        {
          status: "blocked",
          label: "学习受阻",
          count: 0,
        },
      ],
    },
  };
}

function buildZeroActivityProfilePayload() {
  return {
    ...buildProfilePayload(),
    analytics: {
      masteryDistribution: [
        {
          code: "queue",
          name: "队列",
          chapterNo: 4,
          masteryPercent: 20,
        },
      ],
      feedbackTrend: [
        {
          index: 1,
          label: "反馈1",
          knowledgePointCode: "queue",
          averageUpdatedMasteryPercent: 20,
        },
      ],
      feedbackStatusComposition: [
        {
          status: "completed",
          label: "已完成",
          count: 0,
        },
        {
          status: "partial",
          label: "部分完成",
          count: 0,
        },
        {
          status: "blocked",
          label: "学习受阻",
          count: 0,
        },
      ],
    },
  };
}

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView(options = {}) {
  const pinia = createPinia();
  setActivePinia(pinia);

  const authStore = useAuthStore();
  if (options.session) {
    authStore.setSession(options.session);
  }

  return mount(LearnerProfileView, {
    global: {
      plugins: [pinia],
      stubs: {
        PageLayout: {
          template: "<div><slot name='hero-side' /><slot /></div>",
        },
      },
    },
  });
}

describe("LearnerProfileView", () => {
  it("renders the narrative profile layout with spotlight sections", async () => {
    fetchLearnerProfile.mockResolvedValue(buildProfilePayload());

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(LearnerProfileView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            template: "<div><slot name='hero-side' /><slot /></div>",
          },
        },
      },
    });

    await flushUi();

    expect(wrapper.text()).toContain("进度总览");
    expect(wrapper.text()).toContain("聚焦观察");
    expect(wrapper.text()).toContain("最近学习记录");
    expect(wrapper.text()).toContain("最近资源行为");
    expect(wrapper.text()).toContain("支撑证据");
    expect(wrapper.find(".profile-narrative").exists()).toBe(true);
    expect(wrapper.find(".profile-spotlight-grid").exists()).toBe(true);
    expect(wrapper.findComponent(ProfileProgressOverview).exists()).toBe(true);
    expect(wrapper.findComponent(ProfileMasteryTrendChart).exists()).toBe(true);
    expect(wrapper.findComponent(ProfileActivityComposition).exists()).toBe(true);
    expect(wrapper.text()).not.toContain("薄弱点排行");
    expect(wrapper.text()).not.toContain("优先补强知识点");
    expect(wrapper.text()).not.toContain("查看推荐资源");
  });

  it("renders spotlight empty state instead of zero activity composition bars", async () => {
    fetchLearnerProfile.mockResolvedValue(buildZeroActivityProfilePayload());

    const pinia = createPinia();
    setActivePinia(pinia);

    const wrapper = mount(LearnerProfileView, {
      global: {
        plugins: [pinia],
        stubs: {
          PageLayout: {
            template: "<div><slot name='hero-side' /><slot /></div>",
          },
        },
      },
    });

    await flushUi();

    expect(wrapper.text()).toContain("聚焦观察");
    expect(wrapper.text()).toContain("暂无学习活动数据。");
    expect(wrapper.text()).not.toContain("0 条 · 0%");
  });
});
