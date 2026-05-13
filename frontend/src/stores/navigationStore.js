import { defineStore } from "pinia";

const RESOURCE_CONTEXT_STORAGE_KEY = "plns-resource-context";
const LEARNING_GRAPH_CONTEXT_STORAGE_KEY = "plns-learning-graph-context";
const DETAIL_LEARNING_CONTEXT_STORAGE_KEY = "plns-detail-learning-context";
const PRACTICE_CHECK_CONTEXT_STORAGE_KEY = "plns-practice-check-context";

function buildDefaultResourceSummary() {
  return {
    contextMode: "main",
    sourcePage: "home",
    targetLabel: "",
    availableMinutes: 0,
    scheduledCount: 0,
    deferredCount: 0,
    scopeCode: "root",
    scopeLabel: "",
    parentNodeCode: "",
    parentNodeLabel: "",
    generatedAt: "",
  };
}

function buildDefaultPersistedResourceContext() {
  return {
    activeLearnerCode: "demo-learner",
    resourceRecommendationSections: [],
    resourceRecommendationSummary: buildDefaultResourceSummary(),
    resourceRecommendationMainSections: [],
    resourceRecommendationMainSummary: buildDefaultResourceSummary(),
    resourceRecommendationDetailSections: [],
    resourceRecommendationDetailSummary: {
      ...buildDefaultResourceSummary(),
      contextMode: "detail",
    },
  };
}

function buildDefaultLearningGraphViewState() {
  return {
    currentScopeCode: "root",
    selectedNodeCode: "",
    lastFocusedNodeCode: "",
    lastFocusedScopeCode: "root",
    updatedAt: "",
  };
}

function buildDefaultDetailLearningSummary() {
  return {
    sourceTargetLabel: "",
    selectedScopeCode: "",
    availableScopeCount: 0,
    sourcePage: "home",
    generatedAt: "",
  };
}

function buildDefaultDetailLearningViewState() {
  return {
    targetNodeCode: "",
    availableMinutes: 40,
    showMasteryEditor: false,
    masteryPercentByCode: {},
    masteryOverridePercentByCode: {},
    feedbackDraftByCode: {},
    planResult: null,
    adjustmentSummary: null,
    adjustmentItems: [],
    pathComparison: null,
    updatedAt: "",
  };
}

function buildDefaultPracticeCheckContext() {
  return {
    learnerCode: "demo-learner",
    sourcePage: "home",
    targetCode: "",
    targetName: "",
    scopeCode: "",
    scopeLabel: "",
    feedbackBatchId: "",
    feedbackItemCount: 0,
    generatedAt: "",
  };
}

function normalizeObjectRecord(value) {
  return value && typeof value === "object" && !Array.isArray(value) ? value : {};
}

function normalizeDetailLearningViewState(value) {
  const nextState = buildDefaultDetailLearningViewState();
  const payload = normalizeObjectRecord(value);
  const availableMinutes = Number(payload.availableMinutes);

  nextState.targetNodeCode = String(payload.targetNodeCode || "");
  nextState.availableMinutes =
    Number.isFinite(availableMinutes) && availableMinutes > 0
      ? availableMinutes
      : nextState.availableMinutes;
  nextState.showMasteryEditor = Boolean(payload.showMasteryEditor);
  nextState.masteryPercentByCode = normalizeObjectRecord(payload.masteryPercentByCode);
  nextState.masteryOverridePercentByCode = normalizeObjectRecord(
    payload.masteryOverridePercentByCode,
  );
  nextState.feedbackDraftByCode = normalizeObjectRecord(payload.feedbackDraftByCode);
  nextState.planResult = normalizeObjectRecord(payload.planResult);
  nextState.adjustmentSummary = normalizeObjectRecord(payload.adjustmentSummary);
  nextState.adjustmentItems = Array.isArray(payload.adjustmentItems)
    ? payload.adjustmentItems
    : [];
  nextState.pathComparison = normalizeObjectRecord(payload.pathComparison);
  nextState.updatedAt = String(payload.updatedAt || "");

  if (Object.keys(nextState.planResult).length === 0) {
    nextState.planResult = null;
  }

  if (Object.keys(nextState.adjustmentSummary).length === 0) {
    nextState.adjustmentSummary = null;
  }

  if (Object.keys(nextState.pathComparison).length === 0) {
    nextState.pathComparison = null;
  }

  return nextState;
}

function filterDetailLearningViewStates(viewStates, scopeCodes = []) {
  const source = normalizeObjectRecord(viewStates);
  const allowedScopeCodes = new Set(
    Array.isArray(scopeCodes) ? scopeCodes.filter((item) => String(item || "")) : [],
  );

  return Object.fromEntries(
    Object.entries(source)
      .filter(([scopeCode]) => {
        if (allowedScopeCodes.size === 0) {
          return false;
        }

        return allowedScopeCodes.has(scopeCode);
      })
      .map(([scopeCode, state]) => [scopeCode, normalizeDetailLearningViewState(state)]),
  );
}

function normalizeResourceSummary(value, contextMode = "main") {
  return {
    ...buildDefaultResourceSummary(),
    ...normalizeObjectRecord(value),
    contextMode,
  };
}

function normalizeResourceSections(value) {
  return Array.isArray(value) ? value : [];
}

function readPersistedResourceContext() {
  if (typeof window === "undefined") {
    return buildDefaultPersistedResourceContext();
  }

  try {
    const raw = window.sessionStorage.getItem(RESOURCE_CONTEXT_STORAGE_KEY);
    if (!raw) {
      return buildDefaultPersistedResourceContext();
    }

    const parsed = JSON.parse(raw);
    const nextState = buildDefaultPersistedResourceContext();

    nextState.activeLearnerCode = parsed.activeLearnerCode || "demo-learner";

    if (
      "resourceRecommendationMainSections" in parsed ||
      "resourceRecommendationDetailSections" in parsed
    ) {
      nextState.resourceRecommendationMainSections = normalizeResourceSections(
        parsed.resourceRecommendationMainSections,
      );
      nextState.resourceRecommendationMainSummary = normalizeResourceSummary(
        parsed.resourceRecommendationMainSummary,
        "main",
      );
      nextState.resourceRecommendationDetailSections = normalizeResourceSections(
        parsed.resourceRecommendationDetailSections,
      );
      nextState.resourceRecommendationDetailSummary = normalizeResourceSummary(
        parsed.resourceRecommendationDetailSummary,
        "detail",
      );
      nextState.resourceRecommendationSections = normalizeResourceSections(
        parsed.resourceRecommendationSections,
      );
      nextState.resourceRecommendationSummary = normalizeResourceSummary(
        parsed.resourceRecommendationSummary,
        parsed.resourceRecommendationSummary?.contextMode || "main",
      );
      return nextState;
    }

    nextState.resourceRecommendationSections = normalizeResourceSections(
      parsed.resourceRecommendationSections,
    );
    nextState.resourceRecommendationSummary = normalizeResourceSummary(
      parsed.resourceRecommendationSummary,
      parsed.resourceRecommendationSummary?.contextMode || "main",
    );

    if (nextState.resourceRecommendationSummary.contextMode === "detail") {
      nextState.resourceRecommendationDetailSections =
        nextState.resourceRecommendationSections;
      nextState.resourceRecommendationDetailSummary =
        nextState.resourceRecommendationSummary;
    } else {
      nextState.resourceRecommendationMainSections =
        nextState.resourceRecommendationSections;
      nextState.resourceRecommendationMainSummary =
        nextState.resourceRecommendationSummary;
    }

    return nextState;
  } catch (error) {
    console.warn("读取资源推荐上下文失败，将回退到默认状态。", error);
    return buildDefaultPersistedResourceContext();
  }
}

function readPersistedLearningGraphContext() {
  if (typeof window === "undefined") {
    return {
      learningGraphViewState: buildDefaultLearningGraphViewState(),
    };
  }

  try {
    const raw = window.sessionStorage.getItem(LEARNING_GRAPH_CONTEXT_STORAGE_KEY);
    if (!raw) {
      return {
        learningGraphViewState: buildDefaultLearningGraphViewState(),
      };
    }

    const parsed = JSON.parse(raw);
    return {
      learningGraphViewState: {
        ...buildDefaultLearningGraphViewState(),
        ...(parsed.learningGraphViewState || {}),
      },
    };
  } catch (error) {
    console.warn("读取学习图谱状态失败，将回退到默认状态。", error);
    return {
      learningGraphViewState: buildDefaultLearningGraphViewState(),
    };
  }
}

function readPersistedDetailLearningContext() {
  if (typeof window === "undefined") {
    return {
      detailLearningLearnerCode: "demo-learner",
      detailLearningSections: [],
      detailLearningSummary: buildDefaultDetailLearningSummary(),
      detailLearningViewStates: {},
    };
  }

  try {
    const raw = window.sessionStorage.getItem(DETAIL_LEARNING_CONTEXT_STORAGE_KEY);
    if (!raw) {
      return {
        detailLearningLearnerCode: "demo-learner",
        detailLearningSections: [],
        detailLearningSummary: buildDefaultDetailLearningSummary(),
        detailLearningViewStates: {},
      };
    }

    const parsed = JSON.parse(raw);
    const detailLearningSections = Array.isArray(parsed.detailLearningSections)
      ? parsed.detailLearningSections
      : [];
    return {
      detailLearningLearnerCode: parsed.detailLearningLearnerCode || "demo-learner",
      detailLearningSections,
      detailLearningSummary: {
        ...buildDefaultDetailLearningSummary(),
        ...(parsed.detailLearningSummary || {}),
      },
      detailLearningViewStates: filterDetailLearningViewStates(
        parsed.detailLearningViewStates,
        detailLearningSections.map((item) => item.scopeCode),
      ),
    };
  } catch (error) {
    console.warn("读取细化学习上下文失败，将回退到默认状态。", error);
    return {
      detailLearningLearnerCode: "demo-learner",
      detailLearningSections: [],
      detailLearningSummary: buildDefaultDetailLearningSummary(),
      detailLearningViewStates: {},
    };
  }
}

function readPersistedPracticeCheckContext() {
  if (typeof window === "undefined") {
    return {
      practiceCheckContext: buildDefaultPracticeCheckContext(),
    };
  }

  try {
    const raw = window.sessionStorage.getItem(PRACTICE_CHECK_CONTEXT_STORAGE_KEY);
    if (!raw) {
      return {
        practiceCheckContext: buildDefaultPracticeCheckContext(),
      };
    }

    const parsed = JSON.parse(raw);
    return {
      practiceCheckContext: {
        ...buildDefaultPracticeCheckContext(),
        ...normalizeObjectRecord(parsed.practiceCheckContext),
      },
    };
  } catch (error) {
    console.warn("读取练习检测上下文失败，将回退到默认状态。", error);
    return {
      practiceCheckContext: buildDefaultPracticeCheckContext(),
    };
  }
}

export const useNavigationStore = defineStore("navigation", {
  state: () => ({
    ...readPersistedResourceContext(),
    ...readPersistedLearningGraphContext(),
    ...readPersistedDetailLearningContext(),
    ...readPersistedPracticeCheckContext(),
  }),
  getters: {
    resourceSectionByCode: (state) => (code) =>
      state.resourceRecommendationSections.find((item) => item.code === code) ||
      null,
    resourceRecommendationSectionsByMode: (state) => (mode = "main") =>
      mode === "detail"
        ? state.resourceRecommendationDetailSections
        : state.resourceRecommendationMainSections,
    resourceRecommendationSummaryByMode: (state) => (mode = "main") =>
      mode === "detail"
        ? state.resourceRecommendationDetailSummary
        : state.resourceRecommendationMainSummary,
    resourceSectionByModeAndCode: (state) => (mode = "main", code = "") => {
      const sections =
        mode === "detail"
          ? state.resourceRecommendationDetailSections
          : state.resourceRecommendationMainSections;
      return sections.find((item) => item.code === code) || null;
    },
    detailLearningSectionByScopeCode: (state) => (scopeCode) =>
      state.detailLearningSections.find((item) => item.scopeCode === scopeCode) ||
      null,
    detailLearningViewStateByScopeCode: (state) => (scopeCode) =>
      state.detailLearningViewStates[String(scopeCode || "")] || null,
  },
  actions: {
    persistResourceContext() {
      if (typeof window === "undefined") {
        return;
      }

      window.sessionStorage.setItem(
        RESOURCE_CONTEXT_STORAGE_KEY,
        JSON.stringify({
          activeLearnerCode: this.activeLearnerCode,
          resourceRecommendationSections: this.resourceRecommendationSections,
          resourceRecommendationSummary: this.resourceRecommendationSummary,
          resourceRecommendationMainSections:
            this.resourceRecommendationMainSections,
          resourceRecommendationMainSummary:
            this.resourceRecommendationMainSummary,
          resourceRecommendationDetailSections:
            this.resourceRecommendationDetailSections,
          resourceRecommendationDetailSummary:
            this.resourceRecommendationDetailSummary,
        }),
      );
    },
    setActiveResourceRecommendationMode(contextMode = "main") {
      const nextMode = contextMode === "detail" ? "detail" : "main";
      const nextSections =
        nextMode === "detail"
          ? this.resourceRecommendationDetailSections
          : this.resourceRecommendationMainSections;
      const nextSummary =
        nextMode === "detail"
          ? this.resourceRecommendationDetailSummary
          : this.resourceRecommendationMainSummary;

      if (!Array.isArray(nextSections) || nextSections.length === 0) {
        return;
      }

      this.resourceRecommendationSections = nextSections;
      this.resourceRecommendationSummary = nextSummary;
      this.persistResourceContext();
    },
    persistLearningGraphViewState() {
      if (typeof window === "undefined") {
        return;
      }

      window.sessionStorage.setItem(
        LEARNING_GRAPH_CONTEXT_STORAGE_KEY,
        JSON.stringify({
          learningGraphViewState: this.learningGraphViewState,
        }),
      );
    },
    persistDetailLearningContext() {
      if (typeof window === "undefined") {
        return;
      }

      window.sessionStorage.setItem(
        DETAIL_LEARNING_CONTEXT_STORAGE_KEY,
        JSON.stringify({
          detailLearningLearnerCode: this.detailLearningLearnerCode,
          detailLearningSections: this.detailLearningSections,
          detailLearningSummary: this.detailLearningSummary,
          detailLearningViewStates: this.detailLearningViewStates,
        }),
      );
    },
    persistPracticeCheckContext() {
      if (typeof window === "undefined") {
        return;
      }

      window.sessionStorage.setItem(
        PRACTICE_CHECK_CONTEXT_STORAGE_KEY,
        JSON.stringify({
          practiceCheckContext: this.practiceCheckContext,
        }),
      );
    },
    setResourceRecommendationContext({
      learnerCode = "demo-learner",
      resourceRecommendationSections = [],
      contextMode = "main",
      sourcePage = "home",
      targetLabel = "",
      availableMinutes = 0,
      scheduledCount = 0,
      deferredCount = 0,
      scopeCode = "root",
      scopeLabel = "",
      parentNodeCode = "",
      parentNodeLabel = "",
    }) {
      this.activeLearnerCode = learnerCode || "demo-learner";
      const normalizedSections = normalizeResourceSections(
        resourceRecommendationSections,
      );
      const nextSummary = {
        contextMode,
        sourcePage,
        targetLabel,
        availableMinutes,
        scheduledCount,
        deferredCount,
        scopeCode,
        scopeLabel,
        parentNodeCode,
        parentNodeLabel,
        generatedAt: new Date().toISOString(),
      };
      if (contextMode === "detail") {
        this.resourceRecommendationDetailSections = normalizedSections;
        this.resourceRecommendationDetailSummary = nextSummary;
      } else {
        this.resourceRecommendationMainSections = normalizedSections;
        this.resourceRecommendationMainSummary = nextSummary;
      }
      this.resourceRecommendationSections = normalizedSections;
      this.resourceRecommendationSummary = nextSummary;
      this.persistResourceContext();
    },
    clearResourceRecommendationContext(contextMode = "") {
      const normalizedMode =
        contextMode === "detail" || contextMode === "main" ? contextMode : "";

      if (!normalizedMode) {
        this.activeLearnerCode = "demo-learner";
        this.resourceRecommendationSections = [];
        this.resourceRecommendationSummary = buildDefaultResourceSummary();
        this.resourceRecommendationMainSections = [];
        this.resourceRecommendationMainSummary = buildDefaultResourceSummary();
        this.resourceRecommendationDetailSections = [];
        this.resourceRecommendationDetailSummary = {
          ...buildDefaultResourceSummary(),
          contextMode: "detail",
        };
        this.persistResourceContext();
        return;
      }

      if (normalizedMode === "detail") {
        this.resourceRecommendationDetailSections = [];
        this.resourceRecommendationDetailSummary = {
          ...buildDefaultResourceSummary(),
          contextMode: "detail",
        };
      } else {
        this.resourceRecommendationMainSections = [];
        this.resourceRecommendationMainSummary = buildDefaultResourceSummary();
      }

      if (this.resourceRecommendationSummary.contextMode === normalizedMode) {
        const fallbackMode =
          normalizedMode === "detail" &&
          this.resourceRecommendationMainSections.length > 0
            ? "main"
            : normalizedMode === "main" &&
                this.resourceRecommendationDetailSections.length > 0
              ? "detail"
              : "";

        if (fallbackMode) {
          this.resourceRecommendationSections =
            fallbackMode === "detail"
              ? this.resourceRecommendationDetailSections
              : this.resourceRecommendationMainSections;
          this.resourceRecommendationSummary =
            fallbackMode === "detail"
              ? this.resourceRecommendationDetailSummary
              : this.resourceRecommendationMainSummary;
        } else {
          this.resourceRecommendationSections = [];
          this.resourceRecommendationSummary = buildDefaultResourceSummary();
        }
      }

      this.persistResourceContext();
    },
    clearAllResourceRecommendationContexts() {
      this.activeLearnerCode = "demo-learner";
      this.resourceRecommendationSections = [];
      this.resourceRecommendationSummary = buildDefaultResourceSummary();
      this.resourceRecommendationMainSections = [];
      this.resourceRecommendationMainSummary = buildDefaultResourceSummary();
      this.resourceRecommendationDetailSections = [];
      this.resourceRecommendationDetailSummary = {
        ...buildDefaultResourceSummary(),
        contextMode: "detail",
      };
      this.persistResourceContext();
    },
    setDetailLearningContext({
      learnerCode = "demo-learner",
      detailLearningSections = [],
      selectedScopeCode = "",
      sourceTargetLabel = "",
      sourcePage = "home",
    }) {
      const normalizedSections = Array.isArray(detailLearningSections)
        ? detailLearningSections
        : [];
      const fallbackScopeCode = normalizedSections[0]?.scopeCode || "";
      const nextSelectedScopeCode = normalizedSections.some(
        (item) => item.scopeCode === selectedScopeCode,
      )
        ? selectedScopeCode
        : fallbackScopeCode;

      this.detailLearningLearnerCode = learnerCode || "demo-learner";
      this.detailLearningSections = normalizedSections;
      this.detailLearningViewStates = filterDetailLearningViewStates(
        this.detailLearningViewStates,
        normalizedSections.map((item) => item.scopeCode),
      );
      this.detailLearningSummary = {
        sourceTargetLabel,
        selectedScopeCode: nextSelectedScopeCode,
        availableScopeCount: normalizedSections.length,
        sourcePage,
        generatedAt: new Date().toISOString(),
      };
      this.persistDetailLearningContext();
    },
    setDetailLearningViewState(scopeCode = "", payload = {}) {
      const normalizedScopeCode = String(scopeCode || "");
      if (
        !normalizedScopeCode ||
        !this.detailLearningSections.some((item) => item.scopeCode === normalizedScopeCode)
      ) {
        return;
      }

      this.detailLearningViewStates = {
        ...this.detailLearningViewStates,
        [normalizedScopeCode]: normalizeDetailLearningViewState({
          ...this.detailLearningViewStates[normalizedScopeCode],
          ...payload,
          updatedAt: new Date().toISOString(),
        }),
      };
      this.persistDetailLearningContext();
    },
    clearDetailLearningViewState(scopeCode = "") {
      const normalizedScopeCode = String(scopeCode || "");
      if (!normalizedScopeCode) {
        this.detailLearningViewStates = {};
        this.persistDetailLearningContext();
        return;
      }

      if (!this.detailLearningViewStates[normalizedScopeCode]) {
        return;
      }

      const nextViewStates = { ...this.detailLearningViewStates };
      delete nextViewStates[normalizedScopeCode];
      this.detailLearningViewStates = nextViewStates;
      this.persistDetailLearningContext();
    },
    setDetailLearningScope(scopeCode = "") {
      const normalizedScopeCode = String(scopeCode || "");
      if (
        normalizedScopeCode &&
        !this.detailLearningSections.some((item) => item.scopeCode === normalizedScopeCode)
      ) {
        return;
      }

      this.detailLearningSummary = {
        ...this.detailLearningSummary,
        selectedScopeCode: normalizedScopeCode,
        updatedAt: new Date().toISOString(),
      };
      this.persistDetailLearningContext();
    },
    clearDetailLearningContext() {
      this.detailLearningLearnerCode = "demo-learner";
      this.detailLearningSections = [];
      this.detailLearningSummary = buildDefaultDetailLearningSummary();
      this.detailLearningViewStates = {};
      this.persistDetailLearningContext();
    },
    setPracticeCheckContext(payload = {}) {
      const nextContext = {
        ...this.practiceCheckContext,
        ...normalizeObjectRecord(payload),
        learnerCode: String(payload.learnerCode || this.practiceCheckContext.learnerCode || "demo-learner"),
        sourcePage: String(payload.sourcePage || this.practiceCheckContext.sourcePage || "home"),
        targetCode: String(payload.targetCode || ""),
        targetName: String(payload.targetName || ""),
        scopeCode: String(payload.scopeCode || ""),
        scopeLabel: String(payload.scopeLabel || ""),
        feedbackBatchId: String(payload.feedbackBatchId || ""),
        feedbackItemCount: Number.isFinite(Number(payload.feedbackItemCount))
          ? Number(payload.feedbackItemCount)
          : 0,
        generatedAt: new Date().toISOString(),
      };

      this.practiceCheckContext = nextContext;
      this.persistPracticeCheckContext();
    },
    clearPracticeCheckContext() {
      this.practiceCheckContext = buildDefaultPracticeCheckContext();
      this.persistPracticeCheckContext();
    },
    setLearningGraphViewState(payload = {}) {
      const nextState = {
        ...this.learningGraphViewState,
      };

      if ("currentScopeCode" in payload) {
        nextState.currentScopeCode = payload.currentScopeCode || "root";
      }

      if ("selectedNodeCode" in payload) {
        nextState.selectedNodeCode = payload.selectedNodeCode || "";
      }

      if ("lastFocusedNodeCode" in payload) {
        nextState.lastFocusedNodeCode = payload.lastFocusedNodeCode || "";
      }

      if ("lastFocusedScopeCode" in payload) {
        nextState.lastFocusedScopeCode = payload.lastFocusedScopeCode || "root";
      }

      nextState.updatedAt = new Date().toISOString();
      this.learningGraphViewState = nextState;
      this.persistLearningGraphViewState();
    },
    clearLearningGraphViewState() {
      this.learningGraphViewState = buildDefaultLearningGraphViewState();
      this.persistLearningGraphViewState();
    },
  },
});
