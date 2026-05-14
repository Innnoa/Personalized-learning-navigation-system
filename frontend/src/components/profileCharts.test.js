import { describe, expect, it } from "vitest";

import {
  buildProgressSegments,
  buildFeedbackCompositionItems,
  buildFeedbackTrendPoints,
  buildTrendSummary,
  buildMasteryBarItems,
  buildWeakPointRankingItems,
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
  const profile = {
    summary: {
      masteredCount: 2,
      inProgressCount: 4,
      notStartedCount: 2,
    },
    masteryItems: [
      {
        code: "graph",
        name: "图",
        chapterNo: 6,
        chapterName: "图",
        masteryPercent: 55,
      },
      {
        code: "queue",
        name: "队列",
        chapterNo: 4,
        chapterName: "栈与队列",
        masteryPercent: 20,
      },
      {
        code: "stack",
        name: "栈",
        chapterNo: 4,
        chapterName: "栈与队列",
        masteryPercent: 80,
      },
      {
        code: "tree",
        name: "树",
        chapterNo: 5,
        chapterName: "树与二叉树",
        masteryPercent: 10,
      },
      {
        code: "sort",
        name: "排序",
        chapterNo: 8,
        chapterName: "排序",
        masteryPercent: 90,
      },
      {
        code: "search",
        name: "查找",
        chapterNo: 7,
        chapterName: "查找",
        masteryPercent: "35",
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

  it("buildFeedbackTrendPoints maps measured mastery values and excludes missing points", () => {
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

  it("buildFeedbackTrendPoints applies the same mixed indexed and unindexed ordering used by trend summary", () => {
    expect(
      buildFeedbackTrendPoints({
        feedbackTrend: [
          { label: "未编号1", averageUpdatedMasteryPercent: 90 },
          { index: 3, label: "反馈3", averageUpdatedMasteryPercent: 80 },
          { index: 1, label: "反馈1", averageUpdatedMasteryPercent: 40 },
          { label: "未编号2", averageUpdatedMasteryPercent: 20 },
        ],
      }),
    ).toEqual([
      { index: 1, label: "反馈1", averageUpdatedMasteryPercent: 40, value: 40 },
      { index: 3, label: "反馈3", averageUpdatedMasteryPercent: 80, value: 80 },
      { label: "未编号1", averageUpdatedMasteryPercent: 90, value: 90 },
      { label: "未编号2", averageUpdatedMasteryPercent: 20, value: 20 },
    ]);
  });

  it("buildFeedbackTrendPoints treats blank-string indexes as unindexed instead of zero", () => {
    expect(
      buildFeedbackTrendPoints({
        feedbackTrend: [
          { index: "", label: "空白编号", averageUpdatedMasteryPercent: 90 },
          { index: 2, label: "反馈2", averageUpdatedMasteryPercent: 80 },
          { index: 1, label: "反馈1", averageUpdatedMasteryPercent: 40 },
          { label: "未编号", averageUpdatedMasteryPercent: 20 },
        ],
      }),
    ).toEqual([
      { index: 1, label: "反馈1", averageUpdatedMasteryPercent: 40, value: 40 },
      { index: 2, label: "反馈2", averageUpdatedMasteryPercent: 80, value: 80 },
      { index: "", label: "空白编号", averageUpdatedMasteryPercent: 90, value: 90 },
      { label: "未编号", averageUpdatedMasteryPercent: 20, value: 20 },
    ]);
  });

  it("buildFeedbackTrendPoints treats null and whitespace-only indexes as unindexed", () => {
    expect(
      buildFeedbackTrendPoints({
        feedbackTrend: [
          { index: null, label: "空值编号", averageUpdatedMasteryPercent: 90 },
          { index: "   ", label: "空白符编号", averageUpdatedMasteryPercent: 70 },
          { index: 2, label: "反馈2", averageUpdatedMasteryPercent: 80 },
          { index: 1, label: "反馈1", averageUpdatedMasteryPercent: 40 },
        ],
      }),
    ).toEqual([
      { index: 1, label: "反馈1", averageUpdatedMasteryPercent: 40, value: 40 },
      { index: 2, label: "反馈2", averageUpdatedMasteryPercent: 80, value: 80 },
      { index: null, label: "空值编号", averageUpdatedMasteryPercent: 90, value: 90 },
      { index: "   ", label: "空白符编号", averageUpdatedMasteryPercent: 70, value: 70 },
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

  it("buildProgressSegments converts summary counts into ranked progress segments", () => {
    expect(buildProgressSegments(profile.summary)).toEqual([
      {
        key: "inProgress",
        label: "进行中",
        count: 4,
        percent: 50,
      },
      {
        key: "mastered",
        label: "已掌握",
        count: 2,
        percent: 25,
      },
      {
        key: "notStarted",
        label: "未开始",
        count: 2,
        percent: 25,
      },
    ]);
  });

  it("buildTrendSummary derives message-oriented trend summary from first to last feedback point", () => {
    expect(buildTrendSummary(analytics)).toEqual({
      startValue: 35,
      latestValue: 65,
      delta: 30,
      direction: "up",
      message: "整体掌握度较首次反馈提升 30 个百分点",
    });

    expect(
      buildTrendSummary({
        feedbackTrend: [{ averageUpdatedMasteryPercent: 40 }],
      }),
    ).toEqual({
      startValue: 40,
      latestValue: 40,
      delta: 0,
      direction: "flat",
      message: "整体掌握度与首次反馈持平",
    });

    expect(
      buildTrendSummary({
        feedbackTrend: [
          { averageUpdatedMasteryPercent: 70 },
          { averageUpdatedMasteryPercent: 55 },
          { averageUpdatedMasteryPercent: 45 },
        ],
      }),
    ).toEqual({
      startValue: 70,
      latestValue: 45,
      delta: -25,
      direction: "down",
      message: "整体掌握度较首次反馈下降 25 个百分点",
    });

    expect(
      buildTrendSummary({
        feedbackTrend: [
          { index: 3, averageUpdatedMasteryPercent: 45 },
          { index: 1, averageUpdatedMasteryPercent: 70 },
          { index: 2, averageUpdatedMasteryPercent: 55 },
        ],
      }),
    ).toEqual({
      startValue: 70,
      latestValue: 45,
      delta: -25,
      direction: "down",
      message: "整体掌握度较首次反馈下降 25 个百分点",
    });

    expect(
      buildTrendSummary({
        feedbackTrend: [
          { averageUpdatedMasteryPercent: 90 },
          { index: 3, averageUpdatedMasteryPercent: 80 },
          { index: 1, averageUpdatedMasteryPercent: 40 },
          { averageUpdatedMasteryPercent: 20 },
        ],
      }),
    ).toEqual({
      startValue: 40,
      latestValue: 20,
      delta: -20,
      direction: "down",
      message: "整体掌握度较首次反馈下降 20 个百分点",
    });
  });

  it("buildTrendSummary ignores missing or invalid endpoint mastery values", () => {
    expect(
      buildTrendSummary({
        feedbackTrend: [
          { index: 1, averageUpdatedMasteryPercent: undefined },
          { index: 2, averageUpdatedMasteryPercent: 40 },
          { index: 3, averageUpdatedMasteryPercent: 80 },
          { index: 4, averageUpdatedMasteryPercent: null },
        ],
      }),
    ).toEqual({
      startValue: 40,
      latestValue: 80,
      delta: 40,
      direction: "up",
      message: "整体掌握度较首次反馈提升 40 个百分点",
    });

    expect(
      buildTrendSummary({
        feedbackTrend: [
          { index: 1, averageUpdatedMasteryPercent: undefined },
          { index: 2, averageUpdatedMasteryPercent: "NaN" },
        ],
      }),
    ).toEqual(null);
  });

  it("treats whitespace-only averageUpdatedMasteryPercent values as missing", () => {
    expect(
      buildFeedbackTrendPoints({
        feedbackTrend: [
          { index: 1, label: "反馈1", averageUpdatedMasteryPercent: "   " },
          { index: 2, label: "反馈2", averageUpdatedMasteryPercent: 40 },
          { index: 3, label: "反馈3", averageUpdatedMasteryPercent: "\t" },
        ],
      }),
    ).toEqual([
      { index: 2, label: "反馈2", averageUpdatedMasteryPercent: 40, value: 40 },
    ]);

    expect(
      buildTrendSummary({
        feedbackTrend: [
          { index: 1, averageUpdatedMasteryPercent: "   " },
          { index: 2, averageUpdatedMasteryPercent: 40 },
          { index: 3, averageUpdatedMasteryPercent: "\t" },
        ],
      }),
    ).toEqual({
      startValue: 40,
      latestValue: 40,
      delta: 0,
      direction: "flat",
      message: "整体掌握度与首次反馈持平",
    });
  });

  it("buildWeakPointRankingItems ranks weak points by mastery percent", () => {
    expect(buildWeakPointRankingItems(profile.masteryItems)).toEqual([
      {
        code: "tree",
        name: "树",
        chapterNo: 5,
        chapterName: "树与二叉树",
        masteryPercent: 10,
      },
      {
        code: "queue",
        name: "队列",
        chapterNo: 4,
        chapterName: "栈与队列",
        masteryPercent: 20,
      },
      {
        code: "search",
        name: "查找",
        chapterNo: 7,
        chapterName: "查找",
        masteryPercent: 35,
      },
      {
        code: "graph",
        name: "图",
        chapterNo: 6,
        chapterName: "图",
        masteryPercent: 55,
      },
      {
        code: "stack",
        name: "栈",
        chapterNo: 4,
        chapterName: "栈与队列",
        masteryPercent: 80,
      },
    ]);
  });

  it("buildWeakPointRankingItems skips invalid, missing, or empty-string mastery values", () => {
    expect(
      buildWeakPointRankingItems([
        { code: "missing", name: "未测", masteryPercent: undefined },
        { code: "nan", name: "无效", masteryPercent: "NaN" },
        { code: "null", name: "空值", masteryPercent: null },
        { code: "empty", name: "空字符串", masteryPercent: "" },
        { code: "valid-low", name: "薄弱", masteryPercent: "35" },
        { code: "valid-high", name: "较好", masteryPercent: 88 },
      ]),
    ).toEqual([
      { code: "valid-low", name: "薄弱", masteryPercent: 35 },
    ]);
  });

  it("buildWeakPointRankingItems treats whitespace-only mastery values as missing", () => {
    expect(
      buildWeakPointRankingItems([
        { code: "spaces", name: "空白", masteryPercent: "   " },
        { code: "tab", name: "制表符", masteryPercent: "\t" },
        { code: "valid-low", name: "薄弱", masteryPercent: "35" },
      ]),
    ).toEqual([
      { code: "valid-low", name: "薄弱", masteryPercent: 35 },
    ]);
  });

  it("returns empty arrays for empty analytics input", () => {
    expect(buildMasteryBarItems()).toEqual([]);
    expect(buildFeedbackTrendPoints()).toEqual([]);
    expect(buildFeedbackCompositionItems()).toEqual([]);
    expect(buildProgressSegments()).toEqual([]);
    expect(buildWeakPointRankingItems()).toEqual([]);
    expect(buildTrendSummary()).toEqual(null);
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

  it("normalizes invalid mastery inputs and excludes missing trend values", () => {
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
