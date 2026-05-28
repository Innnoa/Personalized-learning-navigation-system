<template>
  <main class="login-view">
    <section class="login-panel">
      <h1>登录</h1>
      <p>使用账号密码或演示账号进入对应角色视图。</p>

      <form class="login-form" @submit.prevent="handleSubmit()">
        <label for="username">用户名</label>
        <input
          id="username"
          v-model.trim="form.username"
          name="username"
          type="text"
          autocomplete="username"
          :disabled="isSubmitting"
        />

        <label for="password">密码</label>
        <input
          id="password"
          v-model="form.password"
          name="password"
          type="password"
          autocomplete="current-password"
          :disabled="isSubmitting"
        />

        <button type="submit" :disabled="isSubmitting">
          {{ isSubmitting ? "登录中..." : "登录" }}
        </button>
      </form>

      <p v-if="errorMessage" class="login-error">{{ errorMessage }}</p>

      <div class="demo-actions">
        <button
          type="button"
          data-testid="demo-student"
          :disabled="isSubmitting"
          @click="handleDemoLogin('student_demo')"
        >
          学生演示
        </button>
        <button
          type="button"
          data-testid="demo-teacher"
          :disabled="isSubmitting"
          @click="handleDemoLogin('teacher_demo')"
        >
          教师演示
        </button>
        <button
          type="button"
          data-testid="demo-admin"
          :disabled="isSubmitting"
          @click="handleDemoLogin('admin_demo')"
        >
          管理员演示
        </button>
      </div>

      <p class="login-footer">
        没有账号？<RouterLink :to="{ name: 'register' }">注册学生/教师账号</RouterLink>
      </p>
    </section>
  </main>
</template>

<script setup>
import { reactive, ref } from "vue";
import { useRoute, useRouter } from "vue-router";

import { login } from "../api/auth";
import { useAuthStore } from "../stores/authStore";

const router = useRouter();
const route = useRoute();
const authStore = useAuthStore();

const form = reactive({
  username: "",
  password: "",
});

const isSubmitting = ref(false);
const errorMessage = ref("");

function resolveTargetRoute(activeRole) {
  if (activeRole === "teacher") {
    return { name: "teacher-dashboard" };
  }

  if (activeRole === "admin") {
    return { name: "admin-dashboard" };
  }

  return { name: "learning-graph" };
}

function resolveRedirectTarget(activeRole) {
  const redirect = Array.isArray(route.query.redirect)
    ? route.query.redirect[0]
    : route.query.redirect;

  if (typeof redirect !== "string" || !redirect.startsWith("/")) {
    return resolveTargetRoute(activeRole);
  }

  const resolvedRoute = router.resolve(redirect);
  const requiredRole = resolvedRoute.meta?.roleScope;

  if (!resolvedRoute.matched.length) {
    return resolveTargetRoute(activeRole);
  }

  if (requiredRole && requiredRole !== activeRole) {
    return resolveTargetRoute(activeRole);
  }

  return redirect;
}

async function handleSubmit(payload = form) {
  isSubmitting.value = true;
  errorMessage.value = "";

  try {
    const session = await login({
      username: payload.username,
      password: payload.password,
    });

    authStore.setSession(session);
    await router.push(resolveRedirectTarget(authStore.activeRole));
  } catch (error) {
    errorMessage.value = "登录失败，请检查账号或密码。";
  } finally {
    isSubmitting.value = false;
  }
}

function handleDemoLogin(username) {
  return handleSubmit({
    username,
    password: "demo123",
  });
}
</script>

<style scoped>
.login-view {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 2rem;
}

.login-panel {
  width: min(100%, 24rem);
  max-width: 100%;
  box-sizing: border-box;
  display: grid;
  gap: 1rem;
  padding: 1.5rem;
  border: 1px solid #d9d9d9;
  border-radius: 0.75rem;
  background: #ffffff;
  overflow: hidden;
}

.login-form {
  display: grid;
  gap: 0.75rem;
}

.login-form input,
.login-form button,
.demo-actions button {
  width: 100%;
  box-sizing: border-box;
  padding: 0.75rem;
  border-radius: 0.5rem;
  border: 1px solid #c7c7c7;
}

.login-form button,
.demo-actions button {
  cursor: pointer;
}

.demo-actions {
  display: grid;
  gap: 0.5rem;
}

.login-error {
  margin: 0;
  color: #c53030;
}

.demo-credentials {
  padding: 0.875rem 1rem;
  border-radius: 0.5rem;
  background: #f6f8fa;
  border: 1px solid #e2e8f0;
}

.demo-credentials-title {
  margin: 0 0 0.5rem;
  font-weight: 600;
}

.demo-credentials ul {
  margin: 0;
  padding-left: 1.25rem;
  color: #4a5568;
}

.login-footer {
  margin-top: 16px;
  text-align: center;
  font-size: 0.9rem;
  color: #51606d;
}

.login-footer a {
  color: #0c6a71;
  font-weight: 700;
}
</style>
