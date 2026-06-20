import { createPinia, setActivePinia } from "pinia";
import { mount } from "@vue/test-utils";
import { beforeEach, describe, expect, it, vi } from "vitest";

import { useAuthStore } from "../stores/authStore";
import LoginView from "./LoginView.vue";

const pushMock = vi.fn();
const loginMock = vi.fn();
const resolveMock = vi.fn();
const routeMock = {
  query: {},
};

vi.mock("vue-router", async () => {
  const actual = await vi.importActual("vue-router");
  return {
    ...actual,
    useRouter: () => ({ push: pushMock, resolve: resolveMock }),
    useRoute: () => routeMock,
  };
});

vi.mock("../api/auth", () => ({
  login: (...args) => loginMock(...args),
}));

async function flushUi() {
  await Promise.resolve();
  await Promise.resolve();
}

function mountView() {
  const pinia = createPinia();
  setActivePinia(pinia);

  return {
    wrapper: mount(LoginView, {
      global: {
        plugins: [pinia],
      },
    }),
    authStore: useAuthStore(),
  };
}

describe("LoginView", () => {
  beforeEach(() => {
    window.sessionStorage.clear();
    pushMock.mockReset();
    loginMock.mockReset();
    resolveMock.mockReset();
    resolveMock.mockImplementation((target) => {
      if (target.startsWith("/teacher")) {
        return {
          matched: [{ path: "/teacher" }],
          meta: { roleScope: "teacher" },
        };
      }

      if (target.startsWith("/admin")) {
        return {
          matched: [{ path: "/admin" }],
          meta: { roleScope: "admin" },
        };
      }

      if (target.startsWith("/")) {
        return {
          matched: [{ path: "/" }],
          meta: { roleScope: "student" },
        };
      }

      return {
        matched: [],
        meta: {},
      };
    });
    routeMock.query = {};
  });

  it("submits username and password, updates auth store, and routes students to learning graph", async () => {
    loginMock.mockResolvedValue({
      currentUser: {
        id: "student-001",
        username: "student_demo",
        displayName: "Student Demo",
      },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: {
        learnerCode: "learner-001",
      },
    });

    const { wrapper, authStore } = mountView();

    await wrapper.get("#username").setValue("student_demo");
    await wrapper.get("#password").setValue("student-pass");
    await wrapper.get("form").trigger("submit.prevent");
    await flushUi();

    expect(loginMock).toHaveBeenCalledWith({
      username: "student_demo",
      password: "student-pass",
    });
    expect(authStore.currentUser).toEqual({
      id: "student-001",
      username: "student_demo",
      displayName: "Student Demo",
    });
    expect(authStore.currentRoles).toEqual(["student"]);
    expect(authStore.activeRole).toBe("student");
    expect(pushMock).toHaveBeenCalledWith({ name: "learning-graph" });
  });

  it("uses the demo shortcut to log in as the student demo account", async () => {
    loginMock.mockResolvedValue({
      currentUser: {
        id: "student-001",
        username: "student_demo",
      },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: {
        learnerCode: "learner-001",
      },
    });

    const { wrapper } = mountView();

    await wrapper.get("[data-testid='demo-student']").trigger("click");
    await flushUi();

    expect(loginMock).toHaveBeenCalledWith({
      username: "student_demo",
      password: "demo123",
    });
  });

  it("shows the demo login entry points on the login page", async () => {
    const { wrapper } = mountView();

    expect(wrapper.text()).toContain("使用账号密码或演示账号进入对应角色视图");
    expect(wrapper.text()).toContain("学生演示");
    expect(wrapper.text()).toContain("教师演示");
    expect(wrapper.text()).toContain("管理员演示");
  });

  it("routes teachers to the teacher dashboard after login", async () => {
    loginMock.mockResolvedValue({
      currentUser: {
        id: "teacher-001",
        username: "teacher_demo",
      },
      currentRoles: ["teacher"],
      activeRole: "teacher",
    });

    const { wrapper, authStore } = mountView();

    await wrapper.get("#username").setValue("teacher_demo");
    await wrapper.get("#password").setValue("teacher-pass");
    await wrapper.get("form").trigger("submit.prevent");
    await flushUi();

    expect(authStore.activeRole).toBe("teacher");
    expect(pushMock).toHaveBeenCalledWith({ name: "teacher-dashboard" });
  });

  it("honors a protected teacher redirect after teacher login", async () => {
    routeMock.query = {
      redirect: "/teacher/courses/COS101",
    };

    loginMock.mockResolvedValue({
      currentUser: {
        id: "teacher-001",
        username: "teacher_demo",
      },
      currentRoles: ["teacher"],
      activeRole: "teacher",
    });

    const { wrapper } = mountView();

    await wrapper.get("#username").setValue("teacher_demo");
    await wrapper.get("#password").setValue("teacher-pass");
    await wrapper.get("form").trigger("submit.prevent");
    await flushUi();

    expect(pushMock).toHaveBeenCalledWith("/teacher/courses/COS101");
  });

  it("honors a protected student redirect after student login", async () => {
    routeMock.query = {
      redirect: "/learning-graph?focus=queue",
    };

    loginMock.mockResolvedValue({
      currentUser: {
        id: "student-001",
        username: "student_demo",
      },
      currentRoles: ["student"],
      activeRole: "student",
      linkedLearner: {
        learnerCode: "learner-001",
      },
    });

    const { wrapper } = mountView();

    await wrapper.get("#username").setValue("student_demo");
    await wrapper.get("#password").setValue("student-pass");
    await wrapper.get("form").trigger("submit.prevent");
    await flushUi();

    expect(pushMock).toHaveBeenCalledWith("/learning-graph?focus=queue");
  });

  it("falls back to the role default when redirect is invalid", async () => {
    routeMock.query = {
      redirect: "https://malicious.example/teacher",
    };

    loginMock.mockResolvedValue({
      currentUser: {
        id: "teacher-001",
        username: "teacher_demo",
      },
      currentRoles: ["teacher"],
      activeRole: "teacher",
    });

    const { wrapper } = mountView();

    await wrapper.get("#username").setValue("teacher_demo");
    await wrapper.get("#password").setValue("teacher-pass");
    await wrapper.get("form").trigger("submit.prevent");
    await flushUi();

    expect(pushMock).toHaveBeenCalledWith({ name: "teacher-dashboard" });
  });
});
