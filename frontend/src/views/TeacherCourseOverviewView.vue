<template>
  <section class="overview-stack">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载课程概览...</p>

      <template v-else-if="course">
        <article class="overview-card">
          <div class="overview-header">
            <div>
              <p class="course-code">{{ course.courseCode }}</p>
              <h2>{{ course.courseName }}</h2>
            </div>
            <div class="count-pill">
              <strong>{{ course.knowledgePointCount }}</strong>
              <span>knowledge points</span>
            </div>
          </div>
          <p class="course-description">{{ course.description }}</p>
          <p class="course-audience">适用对象：{{ course.targetAudience }}</p>
        </article>

        <article class="hint-card">
          <h3>maintenance-entry hints</h3>
          <ul>
            <li>优先核对 data-structures 课程知识点数量与名称是否完整。</li>
            <li>后续维护入口建议从知识点、依赖关系和资源映射三类信息展开。</li>
            <li>当前阶段保持只读，避免直接在前端提供编辑动作。</li>
          </ul>
        </article>
      </template>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchTeacherCourseOverview } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();
const loading = ref(true);
const loadError = ref("");
const coursePayload = ref(null);

const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const courseCode = computed(() => String(route.params?.courseCode || ""));
const course = computed(() => coursePayload.value?.course || null);

async function loadCourseOverview() {
  loading.value = true;
  loadError.value = "";

  try {
    coursePayload.value = await fetchTeacherCourseOverview({
      username: teacherUsername.value,
      courseCode: courseCode.value,
    });
  } catch (error) {
    loadError.value = "未能读取课程概览，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadCourseOverview);
</script>

<style scoped>
.overview-stack {
  display: grid;
  gap: 18px;
}

.overview-card,
.hint-card {
  padding: 20px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.overview-header {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: start;
}

.course-code,
.course-description,
.course-audience,
.state-message,
.hint-card ul {
  margin: 0;
  color: #51606d;
}

.overview-card h2,
.hint-card h3 {
  margin: 8px 0 0;
}

.count-pill {
  min-width: 124px;
  padding: 12px 14px;
  border-radius: 14px;
  background: #eef7f8;
  text-align: center;
  color: #0c6a71;
}

.count-pill strong {
  display: block;
  font-size: 1.6rem;
}

.course-description,
.course-audience {
  margin-top: 14px;
  line-height: 1.6;
}

.hint-card ul {
  padding-left: 20px;
  margin-top: 12px;
  line-height: 1.7;
}

.state-message--error {
  color: #9b3333;
}

@media (max-width: 720px) {
  .overview-header {
    flex-direction: column;
  }
}
</style>
