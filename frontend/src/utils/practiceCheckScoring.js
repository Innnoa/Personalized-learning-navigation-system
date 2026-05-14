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

  if (normalizedTotal !== 3) {
    return 20;
  }

  if (normalizedCorrect === 3) {
    return 85;
  }

  if (normalizedCorrect === 2) {
    return 65;
  }

  if (normalizedCorrect === 1) {
    return 40;
  }

  return 20;
}

export function computeWeightedMasteryPercent({ previousMasteryPercent, practiceMasteryPercent }) {
  const previous = clampPercent(previousMasteryPercent);
  const practice = clampPercent(practiceMasteryPercent);

  return Math.round(previous * 0.4 + practice * 0.6);
}
