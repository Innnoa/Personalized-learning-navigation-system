export const RESOURCE_TIME_BUDGET_THRESHOLDS = Object.freeze({
  standardMinMinutes: 45,
  generousMinMinutes: 90,
});

export function resolveTimeBudgetTier(availableMinutes) {
  const minutes = Number(availableMinutes || 0);
  if (minutes >= RESOURCE_TIME_BUDGET_THRESHOLDS.generousMinMinutes) {
    return "充裕";
  }

  if (minutes >= RESOURCE_TIME_BUDGET_THRESHOLDS.standardMinMinutes) {
    return "标准";
  }

  return "紧凑";
}

export function buildTimeBudgetTierHint(tier) {
  if (tier === "充裕") {
    return "可覆盖主学与补充材料。";
  }

  if (tier === "标准") {
    return "建议优先主学资源，再按需补充。";
  }

  return "建议先聚焦最核心资源。";
}

export function resolveTimeBudgetResourceLimitBonus(availableMinutes) {
  const minutes = Number(availableMinutes || 0);
  if (minutes >= RESOURCE_TIME_BUDGET_THRESHOLDS.generousMinMinutes) {
    return 2;
  }

  if (minutes >= RESOURCE_TIME_BUDGET_THRESHOLDS.standardMinMinutes) {
    return 1;
  }

  return 0;
}
