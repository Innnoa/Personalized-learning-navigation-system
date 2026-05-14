import { defineStore } from "pinia";

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
      this.persistSession();
    },
    clearSession() {
      this.currentUser = null;
      this.currentRoles = [];
      this.activeRole = "";
      this.linkedLearner = null;

      clearPersistedSession();
    },
    restoreSession() {
      const nextState = readPersistedSession();

      this.currentUser = nextState.currentUser;
      this.currentRoles = nextState.currentRoles;
      this.activeRole = nextState.activeRole;
      this.linkedLearner = nextState.linkedLearner;
    },
  },
});
