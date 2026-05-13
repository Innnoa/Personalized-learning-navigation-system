<template>
  <PageLayout
    eyebrow="管理后台 · 用户列表"
    title="用户管理"
    description="展示当前演示环境中的账号与角色信息，保持只读，便于后续扩展管理员能力。"
  >
    <section class="users-stack">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载用户列表...</p>

      <ul v-else class="user-list" data-testid="admin-user-list">
        <li v-for="user in users" :key="user.username" class="user-row">
          <div>
            <p class="user-name">{{ user.username }}</p>
            <p class="user-meta">角色：{{ formatRoles(user.roles) }}</p>
          </div>
        </li>
      </ul>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { fetchAdminUsers } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const usersPayload = ref([]);
const users = computed(() => usersPayload.value);

function formatRoles(roles) {
  return Array.isArray(roles) && roles.length ? roles.join(" / ") : "未分配角色";
}

async function loadUsers() {
  loading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchAdminUsers();
    usersPayload.value = Array.isArray(payload?.users) ? payload.users : [];
  } catch (error) {
    loadError.value = "未能读取用户列表，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadUsers);
</script>

<style scoped>
.users-stack {
  display: grid;
  gap: 18px;
}

.user-list {
  list-style: none;
  margin: 0;
  padding: 0;
  display: grid;
  gap: 14px;
}

.user-row {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.user-name,
.user-meta,
.state-message {
  margin: 0;
}

.user-name {
  font-weight: 700;
  color: #15364a;
}

.user-meta {
  margin-top: 6px;
  color: #51606d;
}

.state-message--error {
  color: #9b3333;
}
</style>
