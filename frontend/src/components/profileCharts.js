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

function toFiniteCount(value) {
  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return 0;
  }

  return Math.max(0, numericValue);
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

  return items.map((item) => ({
    ...item,
    value: toFinitePercent(item.averageUpdatedMasteryPercent),
  }));
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
    percent: Math.round((item.count / total) * 100),
  }));
}
