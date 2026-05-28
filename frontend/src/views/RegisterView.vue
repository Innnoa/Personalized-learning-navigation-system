<template>
  <main class="register-view">
    <section class="register-panel">
      <h1>注册账号</h1>
      <p>{{ isTeacher ? "填写信息后进入教师视图。" : "填写信息后直接进入学生视图。" }}</p>

      <div class="role-toggle">
        <button type="button" :class="{ active: !isTeacher }" @click="isTeacher = false">学生</button>
        <button type="button" :class="{ active: isTeacher }" @click="isTeacher = true">教师</button>
      </div>

      <form class="register-form" @submit.prevent="handleSubmit">
        <label for="username">用户名</label>
        <input id="username" v-model.trim="form.username" type="text" required :disabled="submitting" />

        <label for="password">密码</label>
        <input id="password" v-model="form.password" type="password" required minlength="4" :disabled="submitting" />

        <label for="displayName">显示名</label>
        <input id="displayName" v-model.trim="form.displayName" type="text" :disabled="submitting" />

        <template v-if="isTeacher">
          <label for="employeeCode">工号</label>
          <input id="employeeCode" v-model.trim="form.employeeCode" type="text" placeholder="如: T2024001" :disabled="submitting" />
        </template>
        <template v-else>
          <label for="major">专业</label>
          <input id="major" v-model.trim="form.major" type="text" placeholder="如: 计算机科学" :disabled="submitting" />

          <label for="gradeLabel">年级</label>
          <input id="gradeLabel" v-model.trim="form.gradeLabel" type="text" placeholder="如: 大二" :disabled="submitting" />
        </template>

        <button type="submit" :disabled="submitting">{{ submitting ? "注册中..." : "注册" }}</button>
      </form>

      <p v-if="errorMessage" class="register-error">{{ errorMessage }}</p>

      <p class="register-footer">
        已有账号？<RouterLink :to="{ name: 'login' }">返回登录</RouterLink>
      </p>
    </section>
  </main>
</template>

<script setup>
import { reactive, ref } from "vue";
import { useRouter } from "vue-router";

import { register } from "../api/auth";
import { useAuthStore } from "../stores/authStore";

const router = useRouter();
const authStore = useAuthStore();

const form = reactive({ username: "", password: "", displayName: "", major: "", gradeLabel: "", employeeCode: "" });
const submitting = ref(false);
const errorMessage = ref("");
const isTeacher = ref(false);

async function handleSubmit() {
  if (!form.username || !form.password) {
    errorMessage.value = "用户名和密码不能为空。";
    return;
  }
  if (form.password.length < 4) {
    errorMessage.value = "密码至少 4 个字符。";
    return;
  }
  submitting.value = true;
  errorMessage.value = "";

  try {
    const payload = isTeacher.value
      ? {
          username: form.username,
          password: form.password,
          displayName: form.displayName || form.username,
          role: "teacher",
          employeeCode: form.employeeCode || "",
        }
      : {
          username: form.username,
          password: form.password,
          displayName: form.displayName || form.username,
          major: form.major || "",
          gradeLabel: form.gradeLabel || "",
        };

    const data = await register(payload);
    authStore.setSession(data);
    router.push(isTeacher.value ? { name: "teacher-dashboard" } : { name: "learning-graph" });
  } catch (error) {
    errorMessage.value = error?.response?.data?.detail || "注册失败，请稍后重试。";
  } finally {
    submitting.value = false;
  }
}
</script>

<style scoped>
.register-view {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #f0f5f6;
  padding: 20px;
}

.register-panel {
  width: 100%;
  max-width: 420px;
  padding: 32px;
  background: #ffffff;
  border-radius: 20px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
}

.register-panel h1 {
  margin: 0 0 4px;
  color: #15364a;
  font-size: 1.5rem;
}

.register-panel > p {
  margin: 0 0 16px;
  color: #51606d;
  font-size: 0.9rem;
}

.role-toggle {
  display: flex;
  gap: 0;
  margin-bottom: 16px;
  border: 1px solid #d8e0e6;
  border-radius: 10px;
  overflow: hidden;
}

.role-toggle button {
  flex: 1;
  padding: 8px;
  border: none;
  background: #f8fafb;
  color: #51606d;
  font-weight: 600;
  cursor: pointer;
  font-size: 0.9rem;
  transition: background 0.15s, color 0.15s;
}

.role-toggle button.active {
  background: #0c6a71;
  color: #fff;
}

.register-form {
  display: grid;
  gap: 14px;
}

.register-form label {
  font-size: 0.85rem;
  color: #51606d;
  font-weight: 600;
}

.register-form input {
  padding: 10px 12px;
  border: 1px solid #d8e0e6;
  border-radius: 10px;
  font-size: 0.95rem;
  color: #15364a;
  background: #f8fafb;
  transition: border 0.15s;
  width: 100%;
  box-sizing: border-box;
}

.register-form input:focus {
  border-color: #0c6a71;
  outline: none;
  background: #fff;
}

.register-form button {
  padding: 12px;
  border: none;
  border-radius: 10px;
  background: #0c6a71;
  color: #fff;
  font-size: 1rem;
  font-weight: 700;
  cursor: pointer;
  transition: background 0.15s;
}

.register-form button:hover:not(:disabled) {
  background: #0b5a60;
}

.register-form button:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.register-error {
  margin-top: 12px;
  padding: 10px 14px;
  background: #fce4e4;
  color: #9b3333;
  border-radius: 10px;
  font-size: 0.85rem;
}

.register-footer {
  margin-top: 16px;
  text-align: center;
  font-size: 0.9rem;
  color: #51606d;
}

.register-footer a {
  color: #0c6a71;
  font-weight: 700;
}
</style>
