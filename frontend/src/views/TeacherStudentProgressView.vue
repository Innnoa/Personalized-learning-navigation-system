<template>
  <PageLayout
    eyebrow="教师工作台 · 学生详情"
    :title="learner.name || '学生详情'"
    description="查看学生掌握度、最近学习反馈与学习记录。"
    role-scope="teacher"
  >
    <nav class="breadcrumb">
      <RouterLink
        :to="{ name: 'teacher-course-students', params: { courseCode } }"
        class="breadcrumb-link"
      >
        ← 返回学生列表
      </RouterLink>
    </nav>

    <section class="progress-layout">
      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载学生数据...</p>

      <template v-else>
        <article class="info-card">
          <div class="info-row">
            <span class="info-label">姓名</span>
            <strong>{{ learner.name }}</strong>
          </div>
          <div class="info-row">
            <span class="info-label">专业</span>
            <span>{{ learner.major }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">年级</span>
            <span>{{ learner.gradeLabel }}</span>
          </div>
          <div class="info-row">
            <span class="info-label">课程</span>
            <span>{{ learner.courseName }}</span>
          </div>
        </article>

        <article class="mastery-card">
          <h3>掌握度明细</h3>
          <div class="mastery-list">
            <div
              v-for="(score, code) in masteryByCode"
              :key="code"
              class="mastery-row"
              data-testid="mastery-row"
            >
              <span class="mastery-code">{{ code }}</span>
              <div class="mastery-bar">
                <div class="mastery-bar__track">
                  <div
                    class="mastery-bar__fill"
                    :style="{ width: masteryPercent(score) }"
                  ></div>
                </div>
                <span class="mastery-bar__value">{{ masteryPercent(score) }}</span>
              </div>
            </div>
          </div>
          <p v-if="Object.keys(masteryByCode).length === 0" class="state-message">
            暂无掌握度数据。
          </p>
        </article>

        <article class="feedback-card">
          <h3>最近学习反馈</h3>
          <div v-if="recentFeedback.length > 0" class="feedback-list">
            <div
              v-for="(fb, idx) in recentFeedback"
              :key="idx"
              class="feedback-row"
              data-testid="feedback-row"
            >
              <div class="feedback-header">
                <strong>{{ fb.nodeName }}</strong>
                <span class="feedback-status" :class="statusClass(fb.completionStatus)">
                  {{ statusLabel(fb.completionStatus) }}
                </span>
              </div>
              <div class="feedback-detail">
                掌握度 {{ masteryPercent(fb.previousMastery) }} → {{ masteryPercent(fb.updatedMastery) }}
              </div>
              <p class="feedback-time">{{ fb.recordedAt }}</p>
            </div>
          </div>
          <p v-else class="state-message">暂无学习反馈记录。</p>
        </article>
      </template>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { fetchTeacherStudentProgress } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const props = defineProps({
  courseCode: { type: String, required: true },
  learnerCode: { type: String, required: true },
});

const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const progressData = ref(null);

const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const learner = computed(() => progressData.value?.learner || {});
const masteryByCode = computed(() => progressData.value?.masteryByCode || {});
const recentFeedback = computed(() => progressData.value?.recentFeedback || []);

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

function statusLabel(status) {
  const map = { completed: "已完成", partial: "部分完成", blocked: "学习受阻" };
  return map[status] || status;
}

function statusClass(status) {
  const map = { completed: "status--completed", partial: "status--partial", blocked: "status--blocked" };
  return map[status] || "";
}

async function loadProgress() {
  loading.value = true;
  loadError.value = "";
  try {
    progressData.value = await fetchTeacherStudentProgress(props.learnerCode, {
      username: teacherUsername.value,
      courseCode: props.courseCode,
    });
  } catch (error) {
    loadError.value = "未能读取学生进度数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadProgress);
</script>

<style scoped>
.progress-layout {
  display: grid;
  gap: 18px;
}

.breadcrumb {
  margin-bottom: 12px;
}

.breadcrumb-link {
  color: #0c6a71;
  font-weight: 600;
  text-decoration: none;
}

.info-card,
.mastery-card,
.feedback-card {
  padding: 20px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.info-card h3,
.mastery-card h3,
.feedback-card h3 {
  margin: 0 0 14px;
  color: #15364a;
}

.info-row {
  display: flex;
  gap: 12px;
  padding: 6px 0;
  color: #15364a;
}

.info-label {
  width: 60px;
  color: #51606d;
  flex-shrink: 0;
}

.mastery-list {
  display: grid;
  gap: 10px;
}

.mastery-row {
  display: flex;
  align-items: center;
  gap: 12px;
}

.mastery-code {
  width: 160px;
  font-family: monospace;
  color: #51606d;
  font-size: 0.85rem;
}

.mastery-bar {
  flex: 1;
  display: flex;
  align-items: center;
  gap: 10px;
}

.mastery-bar__track {
  flex: 1;
  height: 10px;
  background: #eef7f8;
  border-radius: 5px;
  overflow: hidden;
}

.mastery-bar__fill {
  height: 100%;
  background: #0c6a71;
  border-radius: 5px;
}

.mastery-bar__value {
  color: #0c6a71;
  font-weight: 700;
  font-size: 0.85rem;
  width: 40px;
  text-align: right;
}

.feedback-list {
  display: grid;
  gap: 12px;
}

.feedback-row {
  padding: 12px;
  border: 1px solid #d8e0e6;
  border-radius: 10px;
  background: #f8fafb;
}

.feedback-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.feedback-status {
  font-size: 0.8rem;
  padding: 2px 8px;
  border-radius: 6px;
  font-weight: 600;
}

.status--completed {
  background: #e0f2e9;
  color: #1a7a3a;
}

.status--partial {
  background: #fef3d0;
  color: #8a6d1b;
}

.status--blocked {
  background: #fce4e4;
  color: #9b3333;
}

.feedback-detail {
  margin-top: 6px;
  color: #51606d;
  font-size: 0.85rem;
}

.feedback-time {
  margin: 4px 0 0;
  color: #8896a0;
  font-size: 0.8rem;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}
</style>
