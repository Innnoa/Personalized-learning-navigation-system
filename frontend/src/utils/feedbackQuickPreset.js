function clampPercent(value) {
  return Math.max(0, Math.min(100, Math.round(Number(value || 0))));
}

function resolveBaseMasteryPercent(item) {
  return clampPercent(item?.masteryPercent || 0);
}

const FEEDBACK_MASTERY_RANGE_BY_STATUS = {
  completed: {
    min: 80,
    max: 100,
  },
  partial: {
    min: 40,
    max: 80,
  },
  blocked: {
    min: 0,
    max: 35,
  },
};

function resolveCompletionStatus(completionStatus) {
  if (completionStatus === "partial" || completionStatus === "blocked") {
    return completionStatus;
  }

  return "completed";
}

export function getFeedbackMasteryRange(completionStatus = "completed") {
  return FEEDBACK_MASTERY_RANGE_BY_STATUS[resolveCompletionStatus(completionStatus)];
}

export function clampFeedbackMasteryForStatus(
  completionStatus = "completed",
  value = 0,
) {
  const range = getFeedbackMasteryRange(completionStatus);
  return Math.max(range.min, Math.min(range.max, clampPercent(value)));
}

export function resolveFeedbackStatusDraft(item, completionStatus = "completed") {
  const status = resolveCompletionStatus(completionStatus);
  const masteryPercent = resolveBaseMasteryPercent(item);

  if (status === "completed") {
    return {
      completionStatus: status,
      selfRatedMastery: clampFeedbackMasteryForStatus(status, Math.max(masteryPercent, 90)),
    };
  }

  if (status === "partial") {
    return {
      completionStatus: status,
      selfRatedMastery: clampFeedbackMasteryForStatus(
        status,
        Math.max(Math.min(masteryPercent + 20, 75), 55),
      ),
    };
  }

  return {
    completionStatus: status,
    selfRatedMastery: clampFeedbackMasteryForStatus(
      status,
      Math.min(Math.max(masteryPercent, 15), 35),
    ),
  };
}

export function normalizeFeedbackDraft(item, draft = {}) {
  const status = resolveCompletionStatus(draft?.completionStatus);
  const fallbackDraft = resolveFeedbackStatusDraft(item, status);
  const nextMastery =
    draft?.selfRatedMastery === undefined || draft?.selfRatedMastery === null
      ? fallbackDraft.selfRatedMastery
      : clampFeedbackMasteryForStatus(status, draft.selfRatedMastery);

  return {
    completionStatus: status,
    selfRatedMastery: nextMastery,
  };
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
  if (presetKey === "completed-mastered") {
    return resolveFeedbackStatusDraft(item, "completed");
  }

  if (presetKey === "partial-review") {
    return resolveFeedbackStatusDraft(item, "partial");
  }

  if (presetKey === "blocked-unfinished") {
    return resolveFeedbackStatusDraft(item, "blocked");
  }

  throw new Error(`未知的反馈快捷预设：${presetKey}`);
}
