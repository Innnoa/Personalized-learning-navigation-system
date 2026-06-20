import { defineStore } from "pinia";
import { useNavigationStore } from "./navigationStore";

const AUTH_SESSION_STORAGE_KEY = "plns-auth-session";

function normalizeObject(value) {
  return value && typeof value === "object" && !Array.isArray(value) ? value : null;
}

function buildDefaultAuthState() {
  return {
    currentUser: null,
    currentRoles: [],
    activeRole: "",
    linkedLearner: null,
    linkedLearners: [],
    activeLearnerCode: "",
  };
}

function normalizeSession(payload = {}) {
  const nextState = buildDefaultAuthState();
  const currentUser = normalizeObject(payload.currentUser) || normalizeObject(payload.user);
  const linkedLearner = normalizeObject(payload.linkedLearner);
  const currentRoles = Array.isArray(payload.currentRoles)
    ? payload.currentRoles.filter((item) => String(item || ""))
    : Array.isArray(payload.roles)
      ? payload.roles.filter((item) => String(item || ""))
    : [];
  const activeRole = String(payload.activeRole || "");

  nextState.currentUser = currentUser;
  nextState.currentRoles = currentRoles;
  nextState.activeRole = currentRoles.includes(activeRole)
    ? activeRole
    : currentRoles[0] || "";
  nextState.linkedLearner = linkedLearner;
  nextState.linkedLearners = Array.isArray(payload.linkedLearners) ? payload.linkedLearners : [];
  if (nextState.linkedLearners.length === 0 && nextState.linkedLearner) {
    nextState.linkedLearners = [{
      learnerId: nextState.linkedLearner.learnerId,
      learnerCode: nextState.linkedLearner.learnerCode,
      learnerName: nextState.linkedLearner.learnerName,
      courseCode: nextState.linkedLearner.courseCode || "",
      courseName: nextState.linkedLearner.courseName || "",
    }];
  }
  nextState.activeLearnerCode = linkedLearner?.learnerCode || "";

  if (!nextState.currentUser) {
    return buildDefaultAuthState();
  }

  return nextState;
}

function readPersistedSession() {
  if (typeof window === "undefined") {
    return buildDefaultAuthState();
  }

  try {
    const raw = window.sessionStorage.getItem(AUTH_SESSION_STORAGE_KEY);
    if (!raw) {
      return buildDefaultAuthState();
    }

    return normalizeSession(JSON.parse(raw));
  } catch (error) {
    if (typeof window !== "undefined") {
      window.sessionStorage.removeItem(AUTH_SESSION_STORAGE_KEY);
    }
    console.warn("读取登录会话失败，将回退到默认状态。", error);
    return buildDefaultAuthState();
  }
}

function clearPersistedSession() {
  if (typeof window === "undefined") {
    return;
  }

  try {
    window.sessionStorage.removeItem(AUTH_SESSION_STORAGE_KEY);
  } catch (error) {
    console.warn("清理登录会话缓存失败，将仅保留内存状态。", error);
  }
}

function clearNavStorage() {
  if (typeof window === "undefined") return;
  try {
    window.sessionStorage.removeItem("plns-resource-context");
    window.sessionStorage.removeItem("plns-learning-graph-context");
    window.sessionStorage.removeItem("plns-detail-learning-context");
    window.sessionStorage.removeItem("plns-practice-check-context");
    window.sessionStorage.removeItem("plns-planner-context");
    Object.keys(window.sessionStorage)
      .filter((key) => key.startsWith("plns-plan-") || key.startsWith("plns-target-") || key.startsWith("plns-draft-"))
      .forEach((key) => window.sessionStorage.removeItem(key));
  } catch {}
}

export const useAuthStore = defineStore("auth", {
  state: () => ({
    ...readPersistedSession(),
  }),
  getters: {
    isAuthenticated: (state) => Boolean(state.currentUser),
  },
  actions: {
    persistSession() {
      if (typeof window === "undefined") {
        return;
      }

      try {
        window.sessionStorage.setItem(
          AUTH_SESSION_STORAGE_KEY,
          JSON.stringify({
            currentUser: this.currentUser,
            currentRoles: this.currentRoles,
            activeRole: this.activeRole,
            linkedLearner: this.linkedLearner,
            linkedLearners: this.linkedLearners,
            activeLearnerCode: this.activeLearnerCode,
          }),
        );
      } catch (error) {
        console.warn("写入登录会话缓存失败，将仅保留内存状态。", error);
      }
    },
    setSession(payload = {}) {
      const nextState = normalizeSession(payload);

      this.currentUser = nextState.currentUser;
      this.currentRoles = nextState.currentRoles;
      this.activeRole = nextState.activeRole;
      this.linkedLearner = nextState.linkedLearner;
      this.linkedLearners = nextState.linkedLearners;
      this.activeLearnerCode = nextState.activeLearnerCode;
      this.persistSession();

      // Sync navigation store
      const navStore = useNavigationStore();
      if (nextState.activeRole === "student") {
        navStore.activeLearnerCode = nextState.linkedLearner?.learnerCode || "";
      }
      // Clear stale learner context for new session
      navStore.detailLearningSections = [];
      navStore.resourceRecommendationMainSections = [];
      navStore.resourceRecommendationDetailSections = [];
      navStore.resourceRecommendationSections = [];
      clearNavStorage();
    },
    switchLearner(learnerCode) {
      const match = this.linkedLearners.find((l) => l.learnerCode === learnerCode);
      if (match) {
        this.activeLearnerCode = learnerCode;
        this.linkedLearner = match;
        this.persistSession();
        const navStore = useNavigationStore();
        navStore.activeLearnerCode = learnerCode;
        navStore.detailLearningSections = [];
        navStore.resourceRecommendationMainSections = [];
        navStore.resourceRecommendationDetailSections = [];
        navStore.resourceRecommendationSections = [];
      }
    },
    clearSession() {
      this.currentUser = null;
      this.currentRoles = [];
      this.activeRole = "";
      this.linkedLearner = null;
      this.linkedLearners = [];
      this.activeLearnerCode = "";

      clearPersistedSession();
      const navStore = useNavigationStore();
      navStore.activeLearnerCode = "";
      navStore.detailLearningSections = [];
      navStore.resourceRecommendationSnapshot = null;
      clearNavStorage();
    },
    restoreSession() {
      const nextState = readPersistedSession();

      this.currentUser = nextState.currentUser;
      this.currentRoles = nextState.currentRoles;
      this.activeRole = nextState.activeRole;
      this.linkedLearner = nextState.linkedLearner;
      this.linkedLearners = nextState.linkedLearners;
      this.activeLearnerCode = nextState.activeLearnerCode;

      const navStore = useNavigationStore();
      if (nextState.activeRole === "student") {
        navStore.activeLearnerCode = nextState.linkedLearner?.learnerCode || "";
      }
      navStore.detailLearningSections = [];
      navStore.resourceRecommendationSnapshot = null;
      clearNavStorage();
    },
  },
});
