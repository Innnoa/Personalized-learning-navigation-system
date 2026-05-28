<template>
  <PageLayout
    eyebrow="教师工作台 · 课程列表"
    title="课程管理"
    description="展示教师当前已分配课程，保持只读列表形态，便于后续逐步接入维护能力。"
    role-scope="teacher"
  >
    <section class="courses-stack">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载课程列表...</p>

      <ul v-else class="course-list" data-testid="teacher-course-list">
        <li v-for="course in courses" :key="course.courseCode" class="course-row">
          <div>
            <p class="course-name">{{ course.courseName }}</p>
            <p class="course-meta">{{ course.courseCode }} · {{ course.targetAudience }}</p>
          </div>
          <RouterLink
            class="course-link"
            :to="{ name: 'teacher-course-overview', params: { courseCode: course.courseCode } }"
          >
            打开课程概览
          </RouterLink>
        </li>
      </ul>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { fetchTeacherCourses } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const authStore = useAuthStore();
const loading = ref(true);
const loadError = ref("");
const teacherCourses = ref([]);
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const courses = computed(() => teacherCourses.value);

async function loadCourses() {
  loading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchTeacherCourses({
      username: teacherUsername.value,
    });
    teacherCourses.value = Array.isArray(payload?.courses) ? payload.courses : [];
  } catch (error) {
    loadError.value = "未能读取教师课程列表，请稍后重试。";
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

.course-meta {
  margin-top: 6px;
  color: #51606d;
}

.course-link {
  color: #0c6a71;
  font-weight: 700;
  text-decoration: none;
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
