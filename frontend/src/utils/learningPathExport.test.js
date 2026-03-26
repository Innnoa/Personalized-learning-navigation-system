import { describe, expect, it } from "vitest";

import {
  buildLearningPathExportFileName,
  buildLearningPathExportText,
} from "./learningPathExport";

describe("learningPathExport", () => {
  it("builds readable export text with path summary and resource suggestions", () => {
    const content = buildLearningPathExportText({
      learnerCode: "demo-learner",
      targetCode: "queue",
      targetLabel: "队列",
      availableMinutes: 120,
      generatedAt: new Date(2026, 2, 24, 9, 20, 0),
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 1,
          masteredCount: 0,
          scheduledMinutes: 30,
          totalRequiredMinutes: 60,
          targetReachableWithinBudget: false,
        },
        overallExplanation: {
          summary: "由于目标链路依赖队列，且当前掌握度仍偏低，因此本轮优先安排队列。",
          bullets: [
            "当前目标：队列",
            "本轮安排：1 个节点，优先学习 队列",
          ],
        },
        path: [
          {
            code: "queue",
            name: "队列",
            chapterNo: 3,
            estimatedMinutes: 30,
            masteryPercent: 35,
            status: "scheduled",
            explanation: {
              summary: "由于当前目标直接依赖队列，因此本轮优先安排。",
              labels: ["本轮学习", "高目标相关"],
            },
          },
          {
            code: "queue-circular",
            name: "循环队列",
            chapterNo: 3,
            estimatedMinutes: 30,
            masteryPercent: 10,
            status: "deferred",
            reasonTrace: {
              triggerReasons: ["当前预算不足，先延后到下一轮。"],
            },
          },
        ],
        resourceRecommendations: [
          {
            code: "queue",
            name: "队列",
            status: "scheduled",
            resourceCount: 2,
            primaryResourceTitle: "Hello 算法 · 队列",
            recommendedUsage: "先看概念，再补循环队列。",
            resources: [
              {
                title: "Hello 算法 · 队列",
                type: "article",
                source: "Hello 算法",
                recommendedPhase: "巩固",
              },
            ],
          },
        ],
      },
    });

    expect(content).toContain("学习路径导出结果");
    expect(content).toContain("目标节点：队列（queue）");
    expect(content).toContain("时间预算：120 分钟");
    expect(content).toContain("一-补充、路径总说明");
    expect(content).toContain("由于目标链路依赖队列");
    expect(content).toContain("二、本轮推荐学习");
    expect(content).toContain("推荐理由：由于当前目标直接依赖队列，因此本轮优先安排。");
    expect(content).toContain("二-补充、下一轮建议");
    expect(content).toContain("三、资源建议");
    expect(content).toContain("优先资源：Hello 算法 · 队列");
  });

  it("builds stable export file names from target code", () => {
    const fileName = buildLearningPathExportFileName({
      targetCode: "topological-sort",
      generatedAt: new Date(2026, 2, 24, 9, 20, 0),
    });

    expect(fileName).toBe("learning-path-topological-sort-20260324-0920.txt");
  });

  it("supports detail learning export title, scope label and file prefix", () => {
    const content = buildLearningPathExportText({
      learnerCode: "demo-learner",
      targetCode: "queue-array",
      targetLabel: "顺序队列",
      scopeLabel: "队列二级范围",
      documentTitle: "细化学习路径导出结果",
      availableMinutes: 40,
      generatedAt: new Date(2026, 2, 24, 9, 20, 0),
      planResult: {
        summary: {
          scheduledCount: 1,
          deferredCount: 0,
          masteredCount: 0,
          scheduledMinutes: 20,
          totalRequiredMinutes: 20,
          targetReachableWithinBudget: true,
        },
        path: [
          {
            code: "queue-array",
            name: "顺序队列",
            chapterNo: 4,
            estimatedMinutes: 20,
            masteryPercent: 45,
            status: "scheduled",
            reasonTrace: {
              triggerReasons: ["当前范围内应先掌握顺序队列的基本操作。"],
            },
          },
        ],
      },
    });

    const fileName = buildLearningPathExportFileName({
      targetCode: "queue-array",
      generatedAt: new Date(2026, 2, 24, 9, 20, 0),
      filePrefix: "detail-learning-path",
    });

    expect(content).toContain("细化学习路径导出结果");
    expect(content).toContain("当前范围：队列二级范围");
    expect(content).toContain("目标节点：顺序队列（queue-array）");
    expect(fileName).toBe("detail-learning-path-queue-array-20260324-0920.txt");
  });
});
