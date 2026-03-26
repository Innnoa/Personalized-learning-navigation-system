import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { describe, expect, it, vi } from "vitest";

import LearnerProfileView from "./LearnerProfileView.vue";
import { useNavigationStore } from "../stores/navigationStore";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { generateLearningPath } from "../api/path";

const pushMock = vi.fn();

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock }),
  };
});

vi.mock("../api/learnerProfile", () => ({
  fetchLearnerProfile: vi.fn(),
}));

vi.mock("../api/path", () => ({
  generateLearningPath: vi.fn(),
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
  };
}

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

describe("LearnerProfileView", () => {
  it("opens resource page for a weak knowledge point by generating resource context", async () => {
    fetchLearnerProfile.mockResolvedValue(buildProfilePayload());
    generateLearningPath.mockResolvedValue({
      summary: {
        availableMinutes: 120,
        scheduledCount: 1,
        deferredCount: 0,
      },
      resourceRecommendations: [
        {
          code: "queue",
          name: "队列",
          status: "scheduled",
          resourceCount: 2,
          recommendedUsage: "先看课程视频，再补文本。",
          resources: [],
        },
      ],
    });

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

    const button = wrapper
      .findAll("button")
      .find((item) => item.text().includes("查看推荐资源"));
    expect(button).toBeTruthy();

    await button.trigger("click");
    await flushUi();

    const store = useNavigationStore();
    expect(generateLearningPath).toHaveBeenCalledWith({
      learnerCode: "demo-learner",
      targetCodes: ["queue"],
      availableMinutes: 120,
      masteryByCode: {
        queue: 0.2,
      },
    });
    expect(store.resourceSectionByCode("queue")?.name).toBe("队列");
    expect(pushMock).toHaveBeenCalledWith({
      name: "resource-recommendation",
      params: { code: "queue" },
      query: {
        level: "main",
      },
    });
  });
});
