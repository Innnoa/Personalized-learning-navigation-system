<template>
  <PageLayout
    eyebrow="管理后台"
    title="后台概览"
    description="当前阶段提供只读后台总览，便于快速查看用户、课程与授课分配的基础规模。"
  >
    <section class="dashboard-stack">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载后台概览...</p>

      <div v-else class="summary-grid">
        <article class="summary-card">
          <p class="summary-label">用户总数</p>
          <strong class="summary-value">{{ users.length }}</strong>
        </article>
        <article class="summary-card">
          <p class="summary-label">课程数量</p>
          <strong class="summary-value">{{ courses.length }}</strong>
        </article>
        <article class="summary-card">
          <p class="summary-label">授课分配</p>
          <strong class="summary-value">{{ assignments.length }}</strong>
        </article>
      </div>

      <div v-if="!loading && !loadError" class="quick-links">
        <RouterLink class="quick-link" :to="{ name: 'admin-users' }">查看用户列表</RouterLink>
        <RouterLink class="quick-link" :to="{ name: 'admin-courses' }">查看课程列表</RouterLink>
        <RouterLink class="quick-link" :to="{ name: 'admin-assignments' }">查看分配列表</RouterLink>
      </div>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import {
  fetchAdminAssignments,
  fetchAdminCourses,
  fetchAdminUsers,
} from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const payload = ref({ users: [], courses: [], assignments: [] });

const users = computed(() => payload.value.users || []);
const courses = computed(() => payload.value.courses || []);
const assignments = computed(() => payload.value.assignments || []);

async function loadDashboard() {
  loading.value = true;
  loadError.value = "";

  try {
    const [usersPayload, coursesPayload, assignmentsPayload] = await Promise.all([
      fetchAdminUsers(),
      fetchAdminCourses(),
      fetchAdminAssignments(),
    ]);

    payload.value = {
      users: Array.isArray(usersPayload?.users) ? usersPayload.users : [],
      courses: Array.isArray(coursesPayload?.courses) ? coursesPayload.courses : [],
      assignments: Array.isArray(assignmentsPayload?.assignments)
        ? assignmentsPayload.assignments
        : [],
    };
  } catch (error) {
    loadError.value = "未能读取后台概览，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadDashboard);
</script>

<style scoped>
.dashboard-stack {
  display: grid;
  gap: 18px;
}

.summary-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
}

.summary-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.summary-label,
.state-message {
  margin: 0;
  color: #51606d;
}

.summary-value {
  display: block;
  margin-top: 10px;
  font-size: 1.5rem;
  color: #15364a;
}

.quick-links {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
}

.quick-link {
  color: #0c6a71;
  font-weight: 700;
  text-decoration: none;
}

.state-message--error {
  color: #9b3333;
}
</style>
