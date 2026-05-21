function toPercentWidth(value) {
  const normalized = Number.isFinite(value) ? Math.max(0, Math.min(100, value)) : 0;
  return `${normalized}%`;
}

function toFinitePercent(value) {
  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return 0;
  }

  return Math.max(0, Math.min(100, numericValue));
}

function toOptionalFinitePercent(value) {
  if (value == null) {
    return null;
  }

  if (typeof value === "string" && value.trim() === "") {
    return null;
  }

  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return null;
  }

  return Math.max(0, Math.min(100, numericValue));
}

function toFiniteCount(value) {
  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return 0;
  }

  return Math.max(0, numericValue);
}

function orderValidTrendPoints(points = []) {
  const indexedPoints = [];
  const unindexedPoints = [];

  points.forEach((point) => {
    const rawIndex = point.index;
    const normalizedIndex = typeof rawIndex === "string" ? rawIndex.trim() : rawIndex;

    if (normalizedIndex !== "" && normalizedIndex != null && Number.isFinite(Number(normalizedIndex))) {
      indexedPoints.push(point);
      return;
    }

    unindexedPoints.push(point);
  });

  indexedPoints.sort((left, right) => Number(left.index) - Number(right.index));

  return [...indexedPoints, ...unindexedPoints];
}

function toPercent(value, total) {
  if (total <= 0) {
    return 0;
  }

  return Math.round((value / total) * 100);
}

export function buildMasteryBarItems(analytics = {}) {
  const items = Array.isArray(analytics.masteryDistribution)
    ? analytics.masteryDistribution
    : [];

  return [...items]
    .map((item) => ({
      ...item,
      masteryPercent: toFinitePercent(item.masteryPercent),
      width: toPercentWidth(toFinitePercent(item.masteryPercent)),
    }))
    .sort((left, right) => right.masteryPercent - left.masteryPercent);
}

export function buildFeedbackTrendPoints(analytics = {}) {
  const items = Array.isArray(analytics.feedbackTrend)
    ? analytics.feedbackTrend
    : [];

  return orderValidTrendPoints(
    items
      .map((item) => {
      const value = toOptionalFinitePercent(item.averageUpdatedMasteryPercent);

      if (value == null) {
        return null;
      }

      return {
        ...item,
        value,
      };
      })
      .filter(Boolean),
  );
}

export function buildFeedbackCompositionItems(analytics = {}) {
  const items = Array.isArray(analytics.feedbackStatusComposition)
    ? analytics.feedbackStatusComposition
    : [];
  const normalizedItems = items.map((item) => ({
    ...item,
    count: toFiniteCount(item.count),
  }));
  const total = normalizedItems.reduce((sum, item) => sum + item.count, 0);

  if (total === 0) {
    return [];
  }

  return normalizedItems.map((item) => ({
    ...item,
    percent: toPercent(item.count, total),
  }));
}

export function buildProgressSegments(summary = {}) {
  const items = [
    {
      key: "mastered",
      label: "已掌握",
      count: toFiniteCount(summary.masteredCount),
    },
    {
      key: "inProgress",
      label: "进行中",
      count: toFiniteCount(summary.inProgressCount),
    },
    {
      key: "notStarted",
      label: "未开始",
      count: toFiniteCount(summary.notStartedCount),
    },
  ];
  const total = items.reduce((sum, item) => sum + item.count, 0);

  if (total === 0) {
    return [];
  }

  return items
    .map((item) => ({
      ...item,
      percent: toPercent(item.count, total),
    }))
    .sort((left, right) => right.count - left.count);
}

export function buildTrendSummary(analytics = {}) {
  const points = buildFeedbackTrendPoints(analytics);

  if (points.length === 0) {
    return null;
  }

  const startValue = points[0].value;
  const latestValue = points[points.length - 1].value;
  const delta = latestValue - startValue;

  let message = "整体掌握度与首次反馈持平";

  if (delta > 0) {
    message = `整体掌握度较首次反馈提升 ${delta} 个百分点`;
  } else if (delta < 0) {
    message = `整体掌握度较首次反馈下降 ${Math.abs(delta)} 个百分点`;
  }

  return {
    startValue,
    latestValue,
    delta,
    direction: delta > 0 ? "up" : delta < 0 ? "down" : "flat",
    message,
  };
}
