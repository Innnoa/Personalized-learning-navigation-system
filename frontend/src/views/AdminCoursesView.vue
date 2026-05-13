<template>
  <PageLayout
    eyebrow="管理后台 · 课程列表"
    title="课程总览"
    description="展示系统内课程与知识点规模，当前仅作为只读后台列表使用。"
  >
    <section class="courses-stack">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载课程列表...</p>

      <ul v-else class="course-list" data-testid="admin-course-list">
        <li v-for="course in courses" :key="course.courseCode" class="course-row">
          <div>
            <p class="course-name">{{ course.courseName }}</p>
            <p class="course-meta">{{ course.courseCode }} · {{ course.targetAudience }}</p>
          </div>
          <div class="course-count">{{ course.knowledgePointCount }} knowledge points</div>
        </li>
      </ul>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { fetchAdminCourses } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const coursesPayload = ref([]);
const courses = computed(() => coursesPayload.value);

async function loadCourses() {
  loading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchAdminCourses();
    coursesPayload.value = Array.isArray(payload?.courses) ? payload.courses : [];
  } catch (error) {
    loadError.value = "未能读取课程列表，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadCourses);
</script>

<style scoped>
.courses-stack {
  display: grid;
  gap: 18px;
}

.course-list {
  list-style: none;
  margin: 0;
  padding: 0;
  display: grid;
  gap: 14px;
}

.course-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 16px;
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.course-name,
.course-meta,
.state-message {
  margin: 0;
}

.course-name {
  font-weight: 700;
  color: #15364a;
}

.course-meta,
.course-count {
  margin-top: 6px;
  color: #51606d;
}

.course-count {
  white-space: nowrap;
}

.state-message--error {
  color: #9b3333;
}

@media (max-width: 720px) {
  .course-row {
    align-items: start;
    flex-direction: column;
  }
}
</style>
