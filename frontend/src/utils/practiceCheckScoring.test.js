import { describe, expect, it } from "vitest";

import {
  mapCorrectCountToPracticeMastery,
  computeWeightedMasteryPercent,
} from "./practiceCheckScoring";

describe("practiceCheckScoring", () => {
  it("maps three fixed demo questions to the agreed mastery buckets", () => {
    expect(mapCorrectCountToPracticeMastery({ correctCount: 3, totalCount: 3 })).toBe(85);
    expect(mapCorrectCountToPracticeMastery({ correctCount: 2, totalCount: 3 })).toBe(65);
    expect(mapCorrectCountToPracticeMastery({ correctCount: 1, totalCount: 3 })).toBe(40);
    expect(mapCorrectCountToPracticeMastery({ correctCount: 0, totalCount: 3 })).toBe(20);
  });

  it("weights previous mastery at 40% and practice mastery at 60%", () => {
    expect(
      computeWeightedMasteryPercent({ previousMasteryPercent: 50, practiceMasteryPercent: 85 }),
    ).toBe(71);
    expect(
      computeWeightedMasteryPercent({ previousMasteryPercent: 70, practiceMasteryPercent: 40 }),
    ).toBe(52);
  });

  it("clamps invalid percents into the 0 to 100 range before weighting", () => {
    expect(
      computeWeightedMasteryPercent({ previousMasteryPercent: -20, practiceMasteryPercent: 120 }),
    ).toBe(60);
    expect(
      computeWeightedMasteryPercent({ previousMasteryPercent: Number.NaN, practiceMasteryPercent: Infinity }),
    ).toBe(0);
  });
});
