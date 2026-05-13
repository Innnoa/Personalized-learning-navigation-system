import { mount } from "@vue/test-utils";
import { describe, expect, it, vi } from "vitest";

import ProfileActivityComposition from "./ProfileActivityComposition.vue";
import ProfileMasteryTrendChart from "./ProfileMasteryTrendChart.vue";
import ProfileProgressOverview from "./ProfileProgressOverview.vue";
import ProfileWeakPointRanking from "./ProfileWeakPointRanking.vue";

const getPropValidator = (component, propName) => component.props?.[propName]?.validator
  ?? component.__vccOpts?.props?.[propName]?.validator;

describe("Profile progress visualization components", () => {
  it("renders progress overview summary and ranked segments", () => {
    const wrapper = mount(ProfileProgressOverview, {
      props: {
        averageMasteryPercent: 68,
        segments: [
          {
            key: "in-progress",
            label: "进行中",
            count: 4,
            percent: 50,
          },
          {
            key: "mastered",
            label: "已掌握",
            count: 3,
            percent: 38,
          },
        ],
      },
    });

    expect(wrapper.text()).toContain("68%");
    expect(wrapper.text()).toContain("进行中");
    expect(wrapper.text()).toContain("4 个知识点 · 50%");
    expect(wrapper.text()).toContain("已掌握");
    expect(wrapper.text()).toContain("3 个知识点 · 38%");
  });

  it("renders mastery trend chart points and trend summary", () => {
    const wrapper = mount(ProfileMasteryTrendChart, {
      props: {
        points: [
          {
            key: "trend-1",
            label: "反馈1",
            value: 40,
          },
          {
            key: "trend-2",
            label: "反馈2",
            value: 55,
          },
          {
            key: "trend-3",
            label: "反馈3",
            value: 70,
          },
        ],
        trendSummary: {
          direction: "up",
          message: "整体掌握度较首次反馈提升 30 个百分点",
        },
      },
    });

    expect(wrapper.find("svg").exists()).toBe(true);
    expect(wrapper.text()).toContain("反馈1");
    expect(wrapper.text()).toContain("40%");
    expect(wrapper.text()).toContain("70%");
    expect(wrapper.text()).toContain("整体掌握度较首次反馈提升 30 个百分点");
  });

  it("keeps chart rendering stable without point ids and clamps plotted values into bounds", async () => {
    const warnSpy = vi.spyOn(console, "warn").mockImplementation(() => {});

    const wrapper = mount(ProfileMasteryTrendChart, {
      props: {
        points: [
          {
            label: "重复反馈",
            value: -15,
          },
          {
            label: "重复反馈",
            value: 140,
          },
        ],
      },
    });

    await wrapper.setProps({
      points: [
        {
          label: "新增反馈",
          value: 80,
        },
        {
          label: "重复反馈",
          value: 50,
        },
        {
          label: "重复反馈",
          value: -15,
        },
        {
          label: "重复反馈",
          value: 140,
        },
      ],
    });

    const points = wrapper.findAll("circle");
    const renderedPoints = wrapper.findAll(".point-item");

    expect(points).toHaveLength(4);
    expect(points[2].attributes("cy")).toBe("100");
    expect(points[3].attributes("cy")).toBe("20");
    expect(renderedPoints).toHaveLength(4);
    expect(renderedPoints.map((pointWrapper) => pointWrapper.text())).toEqual([
      "新增反馈80%",
      "重复反馈50%",
      "重复反馈0%",
      "重复反馈100%",
    ]);
    expect(renderedPoints[2].text()).toContain("0%");
    expect(renderedPoints[3].text()).toContain("100%");
    expect(wrapper.text()).not.toContain("-15%");
    expect(wrapper.text()).not.toContain("140%");
    expect(
      warnSpy.mock.calls.some(([message]) => String(message).includes("Duplicate keys")),
    ).toBe(false);

    warnSpy.mockRestore();
  });

  it("filters malformed trend points instead of rendering misleading placeholders", () => {
    const warnSpy = vi.spyOn(console, "warn").mockImplementation(() => {});

    const wrapper = mount(ProfileMasteryTrendChart, {
      props: {
        points: [
          null,
          undefined,
          "invalid",
          {
            label: "   ",
            value: 45,
          },
          {
            label: "无效数值",
            value: Number.NaN,
          },
          {
            label: "无穷大",
            value: Number.POSITIVE_INFINITY,
          },
          {
            label: "有效反馈",
            value: 60,
          },
        ],
      },
    });

    const renderedPoints = wrapper.findAll(".point-item");

    expect(wrapper.find("svg").exists()).toBe(true);
    expect(renderedPoints).toHaveLength(1);
    expect(renderedPoints[0].text()).toContain("有效反馈");
    expect(renderedPoints[0].text()).toContain("60%");
    expect(wrapper.text()).not.toContain("无效数值");
    expect(wrapper.text()).not.toContain("无穷大");
    expect(
      warnSpy.mock.calls.some(([message]) => String(message).includes("Duplicate keys")),
    ).toBe(false);

    warnSpy.mockRestore();
  });

  it("renders feedback activity composition percentages", () => {
    const wrapper = mount(ProfileActivityComposition, {
      props: {
        items: [
          {
            status: "completed",
            label: "已完成",
            count: 4,
            percent: 67,
          },
          {
            status: "partial",
            label: "部分完成",
            count: 2,
            percent: 33,
          },
        ],
      },
    });

    expect(wrapper.text()).toContain("已完成");
    expect(wrapper.text()).toContain("4 条");
    expect(wrapper.text()).toContain("67%");
    expect(wrapper.text()).toContain("部分完成");
    expect(wrapper.text()).toContain("33%");
  });

  it("clamps percentage-based bar widths for overview and activity charts", () => {
    const overview = mount(ProfileProgressOverview, {
      props: {
        averageMasteryPercent: 68,
        segments: [
          {
            key: "negative",
            label: "待开始",
            count: 1,
            percent: -20,
          },
          {
            key: "overflow",
            label: "已掌握",
            count: 3,
            percent: 140,
          },
        ],
      },
    });

    const activity = mount(ProfileActivityComposition, {
      props: {
        items: [
          {
            status: "negative",
            label: "未提交",
            count: 1,
            percent: -5,
          },
          {
            status: "overflow",
            label: "已完成",
            count: 4,
            percent: 150,
          },
        ],
      },
    });

    expect(overview.findAll(".bar-fill")[0].element.style.width).toBe("0%");
    expect(overview.findAll(".bar-fill")[1].element.style.width).toBe("100%");
    expect(activity.findAll(".bar-fill")[0].element.style.width).toBe("0%");
    expect(activity.findAll(".bar-fill")[1].element.style.width).toBe("100%");
    expect(overview.text()).toContain("1 个知识点 · 0%");
    expect(overview.text()).toContain("3 个知识点 · 100%");
    expect(overview.text()).not.toContain("-20%");
    expect(overview.text()).not.toContain("140%");
    expect(activity.text()).toContain("1 条 · 0%");
    expect(activity.text()).toContain("4 条 · 100%");
    expect(activity.text()).not.toContain("-5%");
    expect(activity.text()).not.toContain("150%");
  });

  it("filters malformed entries with missing essential display fields", () => {
    const warnSpy = vi.spyOn(console, "warn").mockImplementation(() => {});

    const overview = mount(ProfileProgressOverview, {
      props: {
        averageMasteryPercent: 68,
        segments: [
          null,
          {
            key: "blank-label",
            label: "   ",
            count: 2,
            percent: 30,
          },
          {
            key: "valid-segment",
            label: "进行中",
            count: 1,
            percent: 70,
          },
        ],
      },
    });

    const activity = mount(ProfileActivityComposition, {
      props: {
        items: [
          undefined,
          {
            status: "blank-label",
            label: "",
            count: 2,
            percent: 40,
          },
          {
            status: "valid-item",
            label: "已完成",
            count: 3,
            percent: 60,
          },
        ],
      },
    });

    const weakPoints = mount(ProfileWeakPointRanking, {
      props: {
        items: [
          null,
          {
            code: "blank-name",
            name: "   ",
            masteryPercent: 18,
          },
          {
            code: "valid-weak-point",
            name: "图",
            masteryPercent: 42,
          },
        ],
      },
    });

    expect(overview.findAll(".segment-item")).toHaveLength(1);
    expect(overview.text()).toContain("进行中");
    expect(overview.text()).not.toContain("未命名分段");

    expect(activity.findAll(".composition-item")).toHaveLength(1);
    expect(activity.text()).toContain("已完成");
    expect(activity.text()).not.toContain("未命名活动");

    expect(weakPoints.findAll(".weak-item")).toHaveLength(1);
    expect(weakPoints.text()).toContain("图");
    expect(weakPoints.text()).not.toContain("未命名知识点");

    warnSpy.mockRestore();
  });

  it("drops malformed overview, activity, and weak point rows instead of coercing numeric fields", () => {
    const overview = mount(ProfileProgressOverview, {
      props: {
        averageMasteryPercent: 68,
        segments: [
          {
            key: "string-count",
            label: "字符串数量",
            count: "2",
            percent: 40,
          },
          {
            key: "nan-percent",
            label: "无效占比",
            count: 3,
            percent: Number.NaN,
          },
          {
            key: "valid-segment",
            label: "进行中",
            count: 1,
            percent: 70,
          },
        ],
      },
    });

    const activity = mount(ProfileActivityComposition, {
      props: {
        items: [
          {
            status: "string-percent",
            label: "字符串占比",
            count: 2,
            percent: "40",
          },
          {
            status: "invalid-count",
            label: "无效数量",
            count: Number.POSITIVE_INFINITY,
            percent: 10,
          },
          {
            status: "valid-item",
            label: "已完成",
            count: 3,
            percent: 60,
          },
        ],
      },
    });

    const weakPoints = mount(ProfileWeakPointRanking, {
      props: {
        items: [
          {
            code: "string-mastery",
            name: "字符串掌握度",
            masteryPercent: "42",
          },
          {
            code: "nan-mastery",
            name: "无效掌握度",
            masteryPercent: Number.NaN,
          },
          {
            code: "valid-weak-point",
            name: "图",
            masteryPercent: 42,
          },
        ],
      },
    });

    expect(overview.findAll(".segment-item")).toHaveLength(1);
    expect(overview.findAll(".segment-item").map((itemWrapper) => itemWrapper.text())).toEqual([
      "进行中1 个知识点 · 70%",
    ]);
    expect(overview.text()).toContain("进行中");
    expect(overview.text()).not.toContain("字符串数量");
    expect(overview.text()).not.toContain("无效占比");

    expect(activity.findAll(".composition-item")).toHaveLength(1);
    expect(activity.findAll(".composition-item").map((itemWrapper) => itemWrapper.text())).toEqual([
      "已完成3 条 · 60%",
    ]);
    expect(activity.text()).toContain("已完成");
    expect(activity.text()).not.toContain("字符串占比");
    expect(activity.text()).not.toContain("无效数量");

    expect(weakPoints.findAll(".weak-item")).toHaveLength(1);
    expect(weakPoints.findAll(".weak-item").map((itemWrapper) => itemWrapper.text())).toEqual([
      "图章节信息待补充42% 查看推荐资源",
    ]);
    expect(weakPoints.text()).toContain("图");
    expect(weakPoints.text()).not.toContain("字符串掌握度");
    expect(weakPoints.text()).not.toContain("无效掌握度");
  });

  it("keeps duplicate list rows stable when prepending unrelated items", async () => {
    const warnSpy = vi.spyOn(console, "warn").mockImplementation(() => {});

    const overview = mount(ProfileProgressOverview, {
      props: {
        averageMasteryPercent: 68,
        segments: [
          {
            label: "重复分段",
            count: 2,
            percent: 30,
          },
          {
            label: "进行中",
            count: 1,
            percent: 70,
          },
        ],
      },
    });

    await overview.setProps({
      segments: [
        {
          label: "新增分段",
          count: 3,
          percent: 20,
        },
        {
          label: "重复分段",
          count: 2,
          percent: 30,
        },
        {
          label: "进行中",
          count: 1,
          percent: 70,
        },
      ],
    });

    const activity = mount(ProfileActivityComposition, {
      props: {
        items: [
          {
            label: "重复活动",
            count: 2,
            percent: 40,
          },
          {
            label: "已完成",
            count: 3,
            percent: 60,
          },
        ],
      },
    });

    await activity.setProps({
      items: [
        {
          label: "新增活动",
          count: 1,
          percent: 10,
        },
        {
          label: "重复活动",
          count: 2,
          percent: 40,
        },
        {
          label: "已完成",
          count: 3,
          percent: 60,
        },
      ],
    });

    const weakPoints = mount(ProfileWeakPointRanking, {
      props: {
        items: [
          {
            name: "重复知识点",
            masteryPercent: 18,
          },
          {
            name: "图",
            masteryPercent: 42,
          },
        ],
      },
    });

    await weakPoints.setProps({
      items: [
        {
          name: "新增知识点",
          masteryPercent: 12,
        },
        {
          name: "重复知识点",
          masteryPercent: 18,
        },
        {
          name: "图",
          masteryPercent: 42,
        },
      ],
    });

    expect(overview.findAll(".segment-item").map((itemWrapper) => itemWrapper.text())).toEqual([
      "新增分段3 个知识点 · 20%",
      "重复分段2 个知识点 · 30%",
      "进行中1 个知识点 · 70%",
    ]);
    expect(activity.findAll(".composition-item").map((itemWrapper) => itemWrapper.text())).toEqual([
      "新增活动1 条 · 10%",
      "重复活动2 条 · 40%",
      "已完成3 条 · 60%",
    ]);
    expect(weakPoints.findAll(".weak-item").map((itemWrapper) => itemWrapper.text())).toEqual([
      "新增知识点章节信息待补充12% 查看推荐资源",
      "重复知识点章节信息待补充18% 查看推荐资源",
      "图章节信息待补充42% 查看推荐资源",
    ]);

    expect(
      warnSpy.mock.calls.some(([message]) => String(message).includes("Duplicate keys")),
    ).toBe(false);

    warnSpy.mockRestore();
  });

  it("treats null array props as empty states across all visualization components", () => {
    const overview = mount(ProfileProgressOverview, {
      props: {
        averageMasteryPercent: 68,
        segments: null,
      },
    });

    const trend = mount(ProfileMasteryTrendChart, {
      props: {
        points: null,
      },
    });

    const activity = mount(ProfileActivityComposition, {
      props: {
        items: null,
      },
    });

    const weakPoints = mount(ProfileWeakPointRanking, {
      props: {
        items: null,
      },
    });

    expect(overview.text()).toContain("暂无可展示的进度分布。");
    expect(trend.text()).toContain("暂无掌握度趋势数据。");
    expect(activity.text()).toContain("暂无学习活动数据。");
    expect(weakPoints.text()).toContain("当前没有需要重点补强的知识点。");
  });

  it("normalizes unsupported trend directions to a stable summary style", () => {
    const wrapper = mount(ProfileMasteryTrendChart, {
      props: {
        points: [
          {
            key: "trend-1",
            label: "反馈1",
            value: 40,
          },
        ],
        trendSummary: {
          direction: "sideways",
          message: "趋势保持稳定",
        },
      },
    });

    const summary = wrapper.find(".summary");

    expect(summary.exists()).toBe(true);
    expect(summary.classes()).toContain("summary--flat");
    expect(summary.classes()).not.toContain("summary--sideways");
    expect(summary.text()).toContain("趋势保持稳定");
  });

  it("does not render a trend summary block for blank or malformed summary messages", async () => {
    const wrapper = mount(ProfileMasteryTrendChart, {
      props: {
        points: [
          {
            key: "trend-1",
            label: "反馈1",
            value: 40,
          },
        ],
        trendSummary: {
          direction: "up",
          message: "   ",
        },
      },
    });

    expect(wrapper.find(".summary").exists()).toBe(false);

    await wrapper.setProps({
      trendSummary: {
        direction: "down",
        message: 123,
      },
    });

    expect(wrapper.find(".summary").exists()).toBe(false);
    expect(wrapper.text()).not.toContain("123");
  });

  it("documents explicit prop validator contracts for display-ready items", () => {
    const overviewSegmentsValidator = getPropValidator(ProfileProgressOverview, "segments");
    const activityItemsValidator = getPropValidator(ProfileActivityComposition, "items");
    const trendPointsValidator = getPropValidator(ProfileMasteryTrendChart, "points");
    const weakPointItemsValidator = getPropValidator(ProfileWeakPointRanking, "items");

    expect(overviewSegmentsValidator(null)).toBe(true);
    expect(overviewSegmentsValidator([
      { label: "进行中", count: 2, percent: 40 },
    ])).toBe(true);
    expect(overviewSegmentsValidator([
      { label: "", count: 2, percent: 40 },
    ])).toBe(false);

    expect(activityItemsValidator(null)).toBe(true);
    expect(activityItemsValidator([
      { label: "已完成", count: 3, percent: 60 },
    ])).toBe(true);
    expect(activityItemsValidator([
      { label: "已完成", count: Number.NaN, percent: 60 },
    ])).toBe(false);

    expect(trendPointsValidator(null)).toBe(true);
    expect(trendPointsValidator([
      { label: "反馈1", value: 40 },
    ])).toBe(true);
    expect(trendPointsValidator([
      { label: "   ", value: 40 },
    ])).toBe(false);

    expect(weakPointItemsValidator(null)).toBe(true);
    expect(weakPointItemsValidator([
      { name: "图", masteryPercent: 42 },
    ])).toBe(true);
    expect(weakPointItemsValidator([
      { name: "图", masteryPercent: Number.POSITIVE_INFINITY },
    ])).toBe(false);
  });

  it("renders weak points in provided order and emits resource event on click", async () => {
    const wrapper = mount(ProfileWeakPointRanking, {
      props: {
        items: [
          {
            code: "graph",
            name: "图",
            chapterNo: 6,
            chapterName: "图",
            masteryPercent: 91,
          },
          {
            code: "queue",
            name: "队列",
            chapterNo: 4,
            chapterName: "栈与队列",
            masteryPercent: 18,
          },
          {
            code: "sort",
            name: "排序",
            chapterNo: 8,
            chapterName: "排序",
            masteryPercent: 42,
          },
        ],
      },
    });

    const rows = wrapper.findAll(".weak-item");
    expect(rows).toHaveLength(3);
    expect(wrapper.text()).toContain("按传入顺序展示");
    expect(rows[0].text()).toContain("图");
    expect(rows[1].text()).toContain("队列");
    expect(rows[1].text()).toContain("18%");
    expect(rows[2].text()).toContain("排序");

    await rows[1].find("button").trigger("click");

    expect(wrapper.emitted("open-resource")).toEqual([
      [
        {
          code: "queue",
        },
      ],
    ]);
  });

  it("validates the open-resource payload contract", () => {
    const wrapper = mount(ProfileWeakPointRanking, {
      props: {
        items: [],
      },
    });

    const emitValidator = wrapper.vm.$options.emits["open-resource"];

    expect(emitValidator({ code: "queue" })).toBe(true);
    expect(emitValidator("queue")).toBe(false);
    expect(emitValidator({})).toBe(false);
  });

  it("renders available chapter details instead of collapsing partial chapter info", () => {
    const wrapper = mount(ProfileWeakPointRanking, {
      props: {
        items: [
          {
            code: "hash",
            name: "哈希",
            chapterNo: 5,
            masteryPercent: 0,
          },
          {
            code: "tree",
            name: "树",
            chapterName: "树结构",
            masteryPercent: 0,
          },
          {
            code: "graph",
            name: "图",
            masteryPercent: 0,
          },
        ],
      },
    });

    const rows = wrapper.findAll(".weak-item");

    expect(rows[0].text()).toContain("第5章");
    expect(rows[0].text()).not.toContain("章节信息待补充");
    expect(rows[1].text()).toContain("树结构");
    expect(rows[1].text()).not.toContain("章节信息待补充");
    expect(rows[2].text()).toContain("章节信息待补充");
    expect(wrapper.text()).toContain("0%");
    expect(wrapper.text()).not.toContain("undefined");
  });

  it("does not emit open-resource when weak point code is missing", async () => {
    const wrapper = mount(ProfileWeakPointRanking, {
      props: {
        items: [
          {
            name: "未编码知识点",
            masteryPercent: 15,
          },
        ],
      },
    });

    await wrapper.find("button").trigger("click");

    expect(wrapper.emitted("open-resource")).toBeUndefined();
  });
});
