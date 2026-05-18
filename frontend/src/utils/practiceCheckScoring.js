function clampPercent(value) {
  const numeric = Number(value);

  if (!Number.isFinite(numeric)) {
    return 0;
  }

  return Math.max(0, Math.min(100, numeric));
}

export function mapCorrectCountToPracticeMastery({ correctCount, totalCount }) {
  const normalizedTotal = Math.max(0, Number(totalCount) || 0);
  const normalizedCorrect = Math.max(0, Math.min(normalizedTotal, Number(correctCount) || 0));

  if (normalizedTotal <= 0) {
    return 20;
  }

  const correctRate = normalizedCorrect / normalizedTotal;

  if (correctRate >= 1) {
    return 90;
  }

  if (correctRate >= 0.8) {
    return 75;
  }

  if (correctRate >= 0.6) {
    return 60;
  }

  if (correctRate >= 0.4) {
    return 40;
  }

  return 20;
}

export function resolvePracticeCompletionStatus({ correctCount, totalCount }) {
  const normalizedTotal = Math.max(0, Number(totalCount) || 0);
  const normalizedCorrect = Math.max(0, Math.min(normalizedTotal, Number(correctCount) || 0));

  if (normalizedTotal > 0 && normalizedCorrect >= normalizedTotal) {
    return "completed";
  }

  if (normalizedCorrect <= 0) {
    return "blocked";
  }

  return "partial";
}

export function computeWeightedMasteryPercent({ previousMasteryPercent, practiceMasteryPercent }) {
  const previous = clampPercent(previousMasteryPercent);
  const practice = clampPercent(practiceMasteryPercent);

  return Math.round(previous * 0.4 + practice * 0.6);
}
