import { describe, expect, it } from "vitest";

import {
  buildTimeBudgetTierHint,
  resolveTimeBudgetResourceLimitBonus,
  resolveTimeBudgetTier,
  RESOURCE_TIME_BUDGET_THRESHOLDS,
} from "./resourceRecommendationPolicy";

describe("resourceRecommendationPolicy", () => {
  it("exposes shared time budget thresholds", () => {
    expect(RESOURCE_TIME_BUDGET_THRESHOLDS).toEqual({
      standardMinMinutes: 45,
      generousMinMinutes: 90,
    });
  });

  it("resolves budget tier by unified thresholds", () => {
    expect(resolveTimeBudgetTier(20)).toBe("紧凑");
    expect(resolveTimeBudgetTier(60)).toBe("标准");
    expect(resolveTimeBudgetTier(120)).toBe("充裕");
  });

  it("resolves display bonus by unified thresholds", () => {
    expect(resolveTimeBudgetResourceLimitBonus(20)).toBe(0);
    expect(resolveTimeBudgetResourceLimitBonus(60)).toBe(1);
    expect(resolveTimeBudgetResourceLimitBonus(120)).toBe(2);
  });

  it("maps tier to consistent hint copy", () => {
    expect(buildTimeBudgetTierHint("紧凑")).toContain("先聚焦最核心资源");
    expect(buildTimeBudgetTierHint("标准")).toContain("优先主学资源");
    expect(buildTimeBudgetTierHint("充裕")).toContain("主学与补充材料");
  });
});
