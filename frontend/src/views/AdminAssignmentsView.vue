<template>
  <PageLayout
    eyebrow="管理后台 · 授课分配"
    title="指派管理"
    description="展示教师与课程的当前分配关系，保持只读，方便管理员核对演示数据。"
  >
    <section class="assignments-stack">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载授课分配...</p>

      <ul v-else class="assignment-list" data-testid="admin-assignment-list">
        <li
          v-for="assignment in assignments"
          :key="`${assignment.teacherUsername}-${assignment.courseCode}`"
          class="assignment-row"
        >
          <p class="assignment-title">{{ assignment.teacherUsername }} → {{ assignment.courseCode }}</p>
          <p class="assignment-meta">{{ assignment.courseName || "未命名课程" }}</p>
        </li>
      </ul>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { fetchAdminAssignments } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const assignmentsPayload = ref([]);
const assignments = computed(() => assignmentsPayload.value);

async function loadAssignments() {
  loading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchAdminAssignments();
    assignmentsPayload.value = Array.isArray(payload?.assignments) ? payload.assignments : [];
  } catch (error) {
    loadError.value = "未能读取授课分配，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadAssignments);
</script>

<style scoped>
.assignments-stack {
  display: grid;
  gap: 18px;
}

.assignment-list {
  list-style: none;
  margin: 0;
  padding: 0;
  display: grid;
  gap: 14px;
}

.assignment-row {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.assignment-title,
.assignment-meta,
.state-message {
  margin: 0;
}

.assignment-title {
  font-weight: 700;
  color: #15364a;
}

.assignment-meta {
  margin-top: 6px;
  color: #51606d;
}

.state-message--error {
  color: #9b3333;
}
</style>
