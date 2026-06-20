import { createPinia, setActivePinia } from "pinia";
import { beforeEach, describe, expect, it } from "vitest";

import router from "./index";
import { useAuthStore } from "../stores/authStore";

async function navigateTo(location) {
  await router.push(location);
  await router.isReady();
}

describe("router role guards", () => {
  beforeEach(async () => {
    window.sessionStorage.clear();
    setActivePinia(createPinia());

    const authStore = useAuthStore();
    authStore.clearSession();

    await router.push("/login");
  });

  it("redirects unauthenticated users to login for protected routes", async () => {
    await navigateTo("/");

    expect(router.currentRoute.value.name).toBe("login");
    expect(router.currentRoute.value.query.redirect).toBe("/");
  });

  it("preserves full teacher route redirect details for unauthenticated users", async () => {
    await navigateTo("/teacher/courses/COS101");

    expect(router.currentRoute.value.name).toBe("login");
    expect(router.currentRoute.value.query.redirect).toBe("/teacher/courses/COS101/overview");
  });

  it("blocks students from teacher-only routes", async () => {
    const authStore = useAuthStore();
    authStore.setSession({
      currentUser: { id: "student-001", username: "student_demo" },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: { learnerCode: "learner-001" },
    });

    await navigateTo("/teacher");

    expect(router.currentRoute.value.name).toBe("learning-graph");
  });

  it("allows teachers to visit teacher routes", async () => {
    const authStore = useAuthStore();
    authStore.setSession({
      currentUser: { id: "teacher-001", username: "teacher_demo" },
      currentRoles: ["teacher"],
      activeRole: "teacher",
    });

    await navigateTo("/teacher/courses");

    expect(router.currentRoute.value.name).toBe("teacher-courses");
  });

  it("blocks teachers from admin-only routes", async () => {
    const authStore = useAuthStore();
    authStore.setSession({
      currentUser: { id: "teacher-001", username: "teacher_demo" },
      currentRoles: ["teacher"],
      activeRole: "teacher",
    });

    await navigateTo("/admin/users");

    expect(router.currentRoute.value.name).toBe("teacher-dashboard");
  });

  it("allows admins to visit admin routes", async () => {
    const authStore = useAuthStore();
    authStore.setSession({
      currentUser: { id: "admin-001", username: "admin_demo" },
      currentRoles: ["admin"],
      activeRole: "admin",
    });

    await navigateTo("/admin/users");

    expect(router.currentRoute.value.name).toBe("admin-users");
  });

  it("redirects legacy detail-learning route to unified planner", async () => {
    const authStore = useAuthStore();
    authStore.setSession({
      currentUser: { id: "student-001", username: "student_demo" },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: { learnerCode: "learner-001" },
    });

    await navigateTo("/detail-learning?scope=queue-detail&target=queue-linked");

    expect(router.currentRoute.value.name).toBe("home");
    expect(router.currentRoute.value.query.scope).toBe("queue-detail");
    expect(router.currentRoute.value.query.target).toBe("queue-linked");
  });
});
