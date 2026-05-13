import { describe, expect, it } from "vitest";

import {
  buildFeedbackCompositionItems,
  buildFeedbackTrendPoints,
  buildMasteryBarItems,
} from "./profileCharts";

describe("profileCharts helpers", () => {
  const analytics = {
    masteryDistribution: [
      {
        code: "queue",
        name: "队列",
        chapterNo: 4,
        masteryPercent: 20,
      },
      {
        code: "stack",
        name: "栈",
        chapterNo: 4,
        masteryPercent: 80,
      },
      {
        code: "graph",
        name: "图",
        chapterNo: 6,
        masteryPercent: 55,
      },
    ],
    feedbackTrend: [
      {
        index: 1,
        label: "反馈1",
        knowledgePointCode: "queue",
        averageUpdatedMasteryPercent: 35,
      },
      {
        index: 2,
        label: "反馈2",
        knowledgePointCode: "stack",
        averageUpdatedMasteryPercent: 50,
      },
      {
        index: 3,
        label: "反馈3",
        knowledgePointCode: "graph",
        averageUpdatedMasteryPercent: 65,
      },
    ],
    feedbackStatusComposition: [
      {
        status: "completed",
        label: "已完成",
        count: 3,
      },
      {
        status: "partial",
        label: "部分完成",
        count: 2,
      },
      {
        status: "blocked",
        label: "学习受阻",
        count: 1,
      },
    ],
  };

  it("buildMasteryBarItems sorts mastery distribution and maps widths", () => {
    expect(buildMasteryBarItems(analytics)).toEqual([
      {
        code: "stack",
        name: "栈",
        chapterNo: 4,
        masteryPercent: 80,
        width: "80%",
      },
      {
        code: "graph",
        name: "图",
        chapterNo: 6,
        masteryPercent: 55,
        width: "55%",
      },
      {
        code: "queue",
        name: "队列",
        chapterNo: 4,
        masteryPercent: 20,
        width: "20%",
      },
    ]);
  });

  it("buildFeedbackTrendPoints maps average updated mastery percent to value", () => {
    expect(buildFeedbackTrendPoints(analytics)).toEqual([
      {
        index: 1,
        label: "反馈1",
        knowledgePointCode: "queue",
        averageUpdatedMasteryPercent: 35,
        value: 35,
      },
      {
        index: 2,
        label: "反馈2",
        knowledgePointCode: "stack",
        averageUpdatedMasteryPercent: 50,
        value: 50,
      },
      {
        index: 3,
        label: "反馈3",
        knowledgePointCode: "graph",
        averageUpdatedMasteryPercent: 65,
        value: 65,
      },
    ]);
  });

  it("buildFeedbackCompositionItems computes item percent values", () => {
    expect(buildFeedbackCompositionItems(analytics)).toEqual([
      {
        status: "completed",
        label: "已完成",
        count: 3,
        percent: 50,
      },
      {
        status: "partial",
        label: "部分完成",
        count: 2,
        percent: 33,
      },
      {
        status: "blocked",
        label: "学习受阻",
        count: 1,
        percent: 17,
      },
    ]);
  });

  it("returns empty arrays for empty analytics input", () => {
    expect(buildMasteryBarItems()).toEqual([]);
    expect(buildFeedbackTrendPoints()).toEqual([]);
    expect(buildFeedbackCompositionItems()).toEqual([]);
  });

  it("skips decorative all-zero activity composition charts", () => {
    expect(
      buildFeedbackCompositionItems({
        feedbackStatusComposition: [
          { status: "completed", label: "已完成", count: 0 },
          { status: "partial", label: "部分完成", count: 0 },
          { status: "blocked", label: "学习受阻", count: 0 },
        ],
      }),
    ).toEqual([]);
  });

  it("normalizes invalid mastery and trend numeric inputs", () => {
    expect(
      buildMasteryBarItems({
        masteryDistribution: [
          { code: "invalid", name: "无效", masteryPercent: undefined },
          { code: "text", name: "文本", masteryPercent: "42" },
          { code: "null", name: "空值", masteryPercent: null },
          { code: "high", name: "过高", masteryPercent: 120 },
        ],
      }),
    ).toEqual([
      {
        code: "high",
        name: "过高",
        masteryPercent: 100,
        width: "100%",
      },
      {
        code: "text",
        name: "文本",
        masteryPercent: 42,
        width: "42%",
      },
      {
        code: "invalid",
        name: "无效",
        masteryPercent: 0,
        width: "0%",
      },
      {
        code: "null",
        name: "空值",
        masteryPercent: 0,
        width: "0%",
      },
    ]);

    expect(
      buildFeedbackTrendPoints({
        feedbackTrend: [
          {
            index: 1,
            label: "反馈1",
            knowledgePointCode: "queue",
            averageUpdatedMasteryPercent: "55",
          },
          {
            index: 2,
            label: "反馈2",
            knowledgePointCode: "stack",
            averageUpdatedMasteryPercent: undefined,
          },
        ],
      }),
    ).toEqual([
      {
        index: 1,
        label: "反馈1",
        knowledgePointCode: "queue",
        averageUpdatedMasteryPercent: "55",
        value: 55,
      },
      {
        index: 2,
        label: "反馈2",
        knowledgePointCode: "stack",
        averageUpdatedMasteryPercent: undefined,
        value: 0,
      },
    ]);
  });

  it("does not mutate input analytics objects", () => {
    const input = {
      masteryDistribution: [{ code: "queue", name: "队列", masteryPercent: 20 }],
      feedbackTrend: [
        {
          index: 1,
          label: "反馈1",
          knowledgePointCode: "queue",
          averageUpdatedMasteryPercent: 35,
        },
      ],
      feedbackStatusComposition: [
        { status: "completed", label: "已完成", count: 1 },
      ],
    };
    const snapshot = structuredClone(input);

    buildMasteryBarItems(input);
    buildFeedbackTrendPoints(input);
    buildFeedbackCompositionItems(input);

    expect(input).toEqual(snapshot);
  });
});
