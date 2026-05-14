import { createPinia, setActivePinia } from "pinia";
import { beforeEach, describe, expect, it, vi } from "vitest";

import { useAuthStore } from "./authStore";

describe("authStore", () => {
  beforeEach(() => {
    window.sessionStorage.clear();
  });

  it("persists authenticated student session, restores it on next init, and clears it on reset", () => {
    setActivePinia(createPinia());
    const store = useAuthStore();

    store.setSession({
      currentUser: {
        id: "student-001",
        username: "alice",
        displayName: "Alice",
      },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: {
        learnerCode: "learner-001",
        learnerName: "Alice",
      },
    });

    expect(store.isAuthenticated).toBe(true);
    expect(store.currentUser).toEqual({
      id: "student-001",
      username: "alice",
      displayName: "Alice",
    });
    expect(store.currentRoles).toEqual(["student"]);
    expect(store.activeRole).toBe("student");
    expect(store.linkedLearner).toEqual({
      learnerCode: "learner-001",
      learnerName: "Alice",
    });

    setActivePinia(createPinia());
    const restoredStore = useAuthStore();

    expect(restoredStore.isAuthenticated).toBe(true);
    expect(restoredStore.currentUser).toEqual({
      id: "student-001",
      username: "alice",
      displayName: "Alice",
    });
    expect(restoredStore.currentRoles).toEqual(["student"]);
    expect(restoredStore.activeRole).toBe("student");
    expect(restoredStore.linkedLearner).toEqual({
      learnerCode: "learner-001",
      learnerName: "Alice",
    });

    restoredStore.clearSession();

    expect(restoredStore.isAuthenticated).toBe(false);
    expect(restoredStore.currentUser).toBeNull();
    expect(restoredStore.currentRoles).toEqual([]);
    expect(restoredStore.activeRole).toBe("");
    expect(restoredStore.linkedLearner).toBeNull();

    setActivePinia(createPinia());
    const resetStore = useAuthStore();

    expect(resetStore.isAuthenticated).toBe(false);
    expect(resetStore.currentUser).toBeNull();
    expect(resetStore.currentRoles).toEqual([]);
    expect(resetStore.activeRole).toBe("");
    expect(resetStore.linkedLearner).toBeNull();
  });

  it("keeps the store authenticated when session persistence fails", () => {
    const setItemSpy = vi
      .spyOn(window.sessionStorage, "setItem")
      .mockImplementation(() => {
        throw new Error("quota exceeded");
      });

    setActivePinia(createPinia());
    const store = useAuthStore();

    expect(() => {
      store.setSession({
        currentUser: {
          id: "student-002",
          username: "bob",
        },
        currentRoles: ["student"],
        activeRole: "student",
        linkedLearner: {
          learnerCode: "learner-002",
        },
      });
    }).not.toThrow();

    expect(store.isAuthenticated).toBe(true);
    expect(store.currentUser).toEqual({
      id: "student-002",
      username: "bob",
    });
    expect(store.currentRoles).toEqual(["student"]);
    expect(store.activeRole).toBe("student");
    expect(store.linkedLearner).toEqual({
      learnerCode: "learner-002",
    });

    setItemSpy.mockRestore();
  });

  it("normalizes the backend auth payload shape returned by the real login API", () => {
    setActivePinia(createPinia());
    const store = useAuthStore();

    store.setSession({
      authenticated: true,
      user: {
        id: 1,
        username: "student_demo",
        displayName: "演示学生账号",
      },
      roles: ["student"],
      activeRole: "student",
      linkedLearner: {
        learnerCode: "demo-learner",
        learnerName: "演示学习者",
      },
    });

    expect(store.isAuthenticated).toBe(true);
    expect(store.currentUser).toEqual({
      id: 1,
      username: "student_demo",
      displayName: "演示学生账号",
    });
    expect(store.currentRoles).toEqual(["student"]);
    expect(store.activeRole).toBe("student");
    expect(store.linkedLearner).toEqual({
      learnerCode: "demo-learner",
      learnerName: "演示学习者",
    });
  });

  it("restores the normalized backend auth payload shape from session storage", () => {
    window.sessionStorage.setItem(
      "plns-auth-session",
      JSON.stringify({
        authenticated: true,
        user: {
          id: 1,
          username: "student_demo",
          displayName: "演示学生账号",
        },
        roles: ["student"],
        activeRole: "student",
        linkedLearner: {
          learnerCode: "demo-learner",
        },
      }),
    );

    setActivePinia(createPinia());
    const store = useAuthStore();

    expect(store.isAuthenticated).toBe(true);
    expect(store.currentUser?.username).toBe("student_demo");
    expect(store.currentRoles).toEqual(["student"]);
    expect(store.activeRole).toBe("student");
  });

  it("falls back to default state and clears broken storage when persisted session JSON is damaged", () => {
    window.sessionStorage.setItem("plns-auth-session", "{broken json");

    setActivePinia(createPinia());
    const store = useAuthStore();

    expect(store.isAuthenticated).toBe(false);
    expect(store.currentUser).toBeNull();
    expect(store.currentRoles).toEqual([]);
    expect(store.activeRole).toBe("");
    expect(store.linkedLearner).toBeNull();
    expect(window.sessionStorage.getItem("plns-auth-session")).toBeNull();
  });
});
