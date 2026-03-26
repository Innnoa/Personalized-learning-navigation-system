function clampPercent(value) {
  return Math.max(0, Math.min(100, Math.round(Number(value || 0))));
}

function resolveBaseMasteryPercent(item) {
  return clampPercent(item?.masteryPercent || 0);
}

export const feedbackQuickPresets = [
  {
    key: "completed-mastered",
    label: "已完成并掌握",
    tone: "completed",
  },
  {
    key: "partial-review",
    label: "看过但不熟",
    tone: "partial",
  },
  {
    key: "blocked-unfinished",
    label: "未完成",
    tone: "blocked",
  },
];

export function resolveFeedbackQuickPresetDraft(item, presetKey) {
  const masteryPercent = resolveBaseMasteryPercent(item);

  if (presetKey === "completed-mastered") {
    return {
      completionStatus: "completed",
      selfRatedMastery: Math.max(masteryPercent, 90),
    };
  }

  if (presetKey === "partial-review") {
    return {
      completionStatus: "partial",
      selfRatedMastery: Math.max(Math.min(masteryPercent + 20, 75), 55),
    };
  }

  if (presetKey === "blocked-unfinished") {
    return {
      completionStatus: "blocked",
      selfRatedMastery: Math.min(Math.max(masteryPercent, 15), 35),
    };
  }

  throw new Error(`未知的反馈快捷预设：${presetKey}`);
}
