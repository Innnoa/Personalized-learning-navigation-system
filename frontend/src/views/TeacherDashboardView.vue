<template>
  <PageLayout
    eyebrow="教师工作台"
    title="课程维护概览"
    description="当前阶段仅提供只读概览，帮助教师快速查看已分配课程与后续维护入口。"
  >
    <section class="dashboard-stack">
      <div class="summary-grid">
        <article class="summary-card">
          <p class="summary-label">已分配课程</p>
          <strong class="summary-value">{{ courses.length }}</strong>
        </article>
        <article class="summary-card">
          <p class="summary-label">维护入口</p>
          <strong class="summary-value">course overview</strong>
        </article>
      </div>

      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载教师课程...</p>

      <div v-else class="course-grid">
        <article
          v-for="course in courses"
          :key="course.courseCode"
          class="course-card"
          data-testid="course-summary-card"
        >
          <div class="course-card__header">
            <h2>{{ course.courseName }}</h2>
            <span class="course-code">{{ course.courseCode }}</span>
          </div>
          <p class="course-audience">{{ course.targetAudience || "未填写适用对象" }}</p>
          <RouterLink
            class="course-link"
            :to="{ name: 'teacher-course-detail', params: { courseCode: course.courseCode } }"
          >
            查看 maintenance-entry
          </RouterLink>
        </article>
      </div>
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
    loadError.value = "未能读取教师课程概览，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadCourses);
</script>

<style scoped>
.dashboard-stack {
  display: grid;
  gap: 18px;
}

.summary-grid,
.course-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
}

.summary-card,
.course-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.summary-label,
.course-audience,
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

.course-card__header {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: baseline;
}

.course-card__header h2 {
  margin: 0;
  font-size: 1.1rem;
}

.course-code {
  color: #0c6a71;
  font-weight: 700;
}

.course-link {
  display: inline-flex;
  margin-top: 14px;
  color: #0c6a71;
  font-weight: 700;
  text-decoration: none;
}

.state-message--error {
  color: #9b3333;
}
</style>
