import { describe, expect, it } from "vitest";

import {
  feedbackQuickPresets,
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
});
