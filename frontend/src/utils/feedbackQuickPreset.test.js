import { describe, expect, it } from "vitest";

import {
  clampFeedbackMasteryForStatus,
  feedbackQuickPresets,
  getFeedbackMasteryRange,
  normalizeFeedbackDraft,
  resolveFeedbackQuickPresetDraft,
} from "./feedbackQuickPreset";

describe("feedbackQuickPreset", () => {
  it("exposes three demo-oriented quick presets", () => {
    expect(feedbackQuickPresets.map((item) => item.key)).toEqual([
      "completed-mastered",
      "partial-review",
      "blocked-unfinished",
    ]);
  });

  it("resolves completed preset to a mastered draft", () => {
    expect(
      resolveFeedbackQuickPresetDraft(
        { masteryPercent: 35 },
        "completed-mastered",
      ),
    ).toEqual({
      completionStatus: "completed",
      selfRatedMastery: 90,
    });
  });

  it("resolves partial preset to an in-progress draft", () => {
    expect(
      resolveFeedbackQuickPresetDraft(
        { masteryPercent: 45 },
        "partial-review",
      ),
    ).toEqual({
      completionStatus: "partial",
      selfRatedMastery: 65,
    });
  });

  it("resolves blocked preset to a low-confidence draft", () => {
    expect(
      resolveFeedbackQuickPresetDraft(
        { masteryPercent: 70 },
        "blocked-unfinished",
      ),
    ).toEqual({
      completionStatus: "blocked",
      selfRatedMastery: 35,
    });
  });

  it("returns mastery ranges by completion status", () => {
    expect(getFeedbackMasteryRange("completed")).toEqual({ min: 80, max: 100 });
    expect(getFeedbackMasteryRange("partial")).toEqual({ min: 40, max: 80 });
    expect(getFeedbackMasteryRange("blocked")).toEqual({ min: 0, max: 35 });
  });

  it("clamps mastery to the active completion-status range", () => {
    expect(clampFeedbackMasteryForStatus("completed", 65)).toBe(80);
    expect(clampFeedbackMasteryForStatus("partial", 95)).toBe(80);
    expect(clampFeedbackMasteryForStatus("blocked", 60)).toBe(35);
  });

  it("normalizes a draft to the current completion-status range", () => {
    expect(
      normalizeFeedbackDraft(
        { masteryPercent: 20 },
        { completionStatus: "completed", selfRatedMastery: 30 },
      ),
    ).toEqual({
      completionStatus: "completed",
      selfRatedMastery: 80,
    });

    expect(
      normalizeFeedbackDraft(
        { masteryPercent: 70 },
        { completionStatus: "blocked", selfRatedMastery: 55 },
      ),
    ).toEqual({
      completionStatus: "blocked",
      selfRatedMastery: 35,
    });
  });
});
