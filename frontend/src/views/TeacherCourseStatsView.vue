<template>
  <section class="stats-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading && !hasLearningStatusContent" class="state-message">正在加载学情数据...</p>

    <template v-else>
      <div class="learning-status-grid">
        <section class="summary-panel">
          <div class="section-head">
            <h3>班级学情总览</h3>
            <span class="section-tip">先看整体，再判断是否需要逐个跟进</span>
          </div>

          <template v-if="hasStudentStats">
            <div class="summary-grid">
              <article class="summary-card">
                <p class="summary-label">学生总数</p>
                <div class="summary-card__main">
                  <strong class="summary-value">{{ studentStats.studentCount }}</strong>
                  <span class="summary-accent">班级在学人数</span>
                </div>
              </article>
              <article class="summary-card">
                <p class="summary-label">平均掌握度</p>
                <div class="summary-card__main">
                  <strong class="summary-value">{{ masteryPercent(studentStats.avgMastery) }}</strong>
                  <span class="summary-accent">整体状态中位偏稳</span>
                </div>
              </article>
            </div>

            <article class="distribution-card">
              <div class="distribution-head">
                <p class="summary-label">掌握度分布</p>
                <span class="distribution-total">共 {{ studentStats.studentCount }} 人</span>
              </div>
              <div class="distribution-list">
                <div v-for="item in studentDistribution" :key="item.key" class="distribution-row">
                  <div class="distribution-meta">
                    <span class="distribution-name">{{ item.label }}</span>
                    <span class="distribution-sub">{{ distributionHint(item.key) }}</span>
                  </div>
                  <div class="distribution-figure">
                    <strong>{{ item.count }}</strong>
                    <span>人</span>
                  </div>
                </div>
              </div>
            </article>
          </template>
          <p v-else-if="learningStatusLoading" class="learning-status-note">正在加载摘要数据...</p>
          <p v-else class="learning-status-note learning-status-note--error">
            {{ statsError || "未能读取班级学情，请稍后重试。" }}
          </p>
        </section>

        <section class="priority-panel">
          <div class="section-head">
            <h3>重点关注学生</h3>
            <span class="section-tip">低掌握度优先，其次参考最近活动</span>
          </div>

          <p v-if="!hasStudentList && learningStatusLoading" class="learning-status-note">正在加载学生列表...</p>
          <p v-else-if="!hasStudentList" class="learning-status-note learning-status-note--error">
            {{ studentsError || "未能读取班级学情，请稍后重试。" }}
          </p>
          <div v-else-if="priorityStudents.length === 0" class="empty-note">
            暂无学生学情数据。
          </div>
          <ul v-else class="priority-list">
            <li
              v-for="student in priorityStudents"
              :key="student.learnerCode"
              class="priority-item"
              data-testid="priority-student"
            >
              <div class="priority-rank">
                <span class="priority-rank__value">{{ riskLevel(student.overallMastery) }}</span>
              </div>
              <div class="priority-student-main">
                <div class="priority-row priority-row--top">
                  <strong class="priority-name">{{ student.learnerName || student.learnerCode }}</strong>
                  <span class="priority-score">掌握度 {{ masteryPercent(student.overallMastery) }}</span>
                </div>
                <div class="priority-row priority-row--bottom">
                  <span class="priority-major">{{ student.major }} · {{ student.gradeLabel }}</span>
                  <span class="priority-activity">{{ lastActivityLabel(student.lastActivityAt) }}</span>
                </div>
              </div>
              <button type="button" class="priority-action" @click="goToStudentProgress(student)">
                查看进展
              </button>
            </li>
          </ul>
        </section>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";

import { fetchTeacherCourseStats, fetchTeacherStudents } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const learningStatusLoading = ref(true);
const statsPayload = ref(null);
const studentsPayload = ref(null);
const statsError = ref("");
const studentsError = ref("");

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const hasStudentStats = computed(() => Boolean(statsPayload.value));
const hasStudentList = computed(() => Boolean(studentsPayload.value));
const hasLearningStatusContent = computed(() => hasStudentStats.value || hasStudentList.value);
const studentStats = computed(() => ({
  studentCount: numberOrZero(statsPayload.value?.studentCount),
  avgMastery: numberOrZero(statsPayload.value?.avgMastery),
}));
const studentDistribution = computed(() => {
  const distribution = statsPayload.value?.masteryDistribution || {};
  return [
    { key: "low", label: "较低（0-35%）", count: numberOrZero(distribution.low?.count) },
    { key: "mid", label: "中等（35-70%）", count: numberOrZero(distribution.mid?.count) },
    { key: "high", label: "较高（70-100%）", count: numberOrZero(distribution.high?.count) },
  ];
});
const priorityStudents = computed(() =>
  [...(studentsPayload.value?.students || [])].sort((left, right) => {
    const masteryGap = numberOrZero(left.overallMastery) - numberOrZero(right.overallMastery);
    if (masteryGap !== 0) return masteryGap;

    const activityGap = parseActivityRank(left.lastActivityAt) - parseActivityRank(right.lastActivityAt);
    if (activityGap !== 0) return activityGap;

    return String(left.learnerCode || "").localeCompare(String(right.learnerCode || ""));
  }).slice(0, 5),
);

function numberOrZero(value) {
  const parsed = Number(value);
  return Number.isFinite(parsed) ? parsed : 0;
}

function masteryPercent(value) {
  return `${Math.round(numberOrZero(value) * 100)}%`;
}

function parseActivityRank(value) {
  if (!value) return Number.NEGATIVE_INFINITY;
  const parsed = Date.parse(value);
  return Number.isNaN(parsed) ? Number.NEGATIVE_INFINITY : parsed;
}

function lastActivityLabel(value) {
  if (!value) return "最近活动：暂无";
  return Number.isNaN(Date.parse(value)) ? "最近活动：记录异常" : `最近活动：${value}`;
}

function distributionHint(key) {
  if (key === "low") return "优先关注";
  if (key === "mid") return "继续跟进";
  return "状态稳定";
}

function riskLevel(value) {
  const score = numberOrZero(value);
  if (score < 0.35) return "高";
  if (score < 0.7) return "中";
  return "稳";
}

function goToStudentProgress(student) {
  router.push({
    name: "teacher-student-progress",
    params: {
      courseCode: courseCode.value,
      learnerCode: student.learnerCode,
    },
  });
}

async function loadStats() {
  loading.value = true;
  loadError.value = "";
  learningStatusLoading.value = true;
  statsPayload.value = null;
  studentsPayload.value = null;
  statsError.value = "";
  studentsError.value = "";

  const statsPromise = fetchTeacherCourseStats(courseCode.value, {
    username: teacherUsername.value,
  });
  const studentsPromise = fetchTeacherStudents(courseCode.value, {
    username: teacherUsername.value,
  });

  const [statsResult, studentsResult] = await Promise.allSettled([statsPromise, studentsPromise]);

  if (statsResult.status === "fulfilled") {
    statsPayload.value = statsResult.value;
  } else {
    statsError.value = "未能读取班级学情，请稍后重试。";
    console.error(statsResult.reason);
  }

  if (studentsResult.status === "fulfilled") {
    studentsPayload.value = studentsResult.value;
  } else {
    studentsError.value = "未能读取班级学情，请稍后重试。";
    console.error(studentsResult.reason);
  }

  if (!statsPayload.value && !studentsPayload.value) {
    loadError.value = "未能读取班级学情，请稍后重试。";
  }

  learningStatusLoading.value = false;
  loading.value = false;
}

onMounted(loadStats);
</script>

<style scoped>
.stats-layout {
  display: grid;
  gap: 18px;
}

.learning-status-grid {
  display: grid;
  gap: 18px;
  grid-template-columns: minmax(0, 1.05fr) minmax(0, 1.3fr);
}

.summary-panel,
.priority-panel,
.summary-card,
.distribution-card,
.priority-item {
  padding: 20px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.summary-panel,
.priority-panel {
  display: grid;
  gap: 16px;
}

.section-head {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  gap: 12px;
}

.section-head h3,
.summary-label,
.learning-status-note {
  margin: 0;
}

.summary-label,
.section-tip,
.learning-status-note,
.priority-student-main span,
.priority-student-meta {
  color: #607380;
}

.summary-grid {
  display: grid;
  gap: 14px;
  grid-template-columns: repeat(2, minmax(0, 1fr));
}

.summary-card,
.distribution-card,
.priority-item {
  padding: 14px 16px;
  border: 1px solid #e7edf2;
  border-radius: 14px;
  background: #fbfdfe;
}

.summary-value {
  display: block;
  font-size: 2rem;
  color: #15364a;
  line-height: 1;
}

.summary-card__main {
  display: grid;
  gap: 10px;
  margin-top: 14px;
}

.summary-accent,
.distribution-total {
  color: #7b8c96;
  font-size: 0.88rem;
}

.distribution-head {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  gap: 12px;
}

.distribution-list,
.priority-list {
  display: grid;
  gap: 10px;
}

.distribution-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
  color: #51606d;
  padding: 4px 0;
}

.distribution-row + .distribution-row {
  padding-top: 10px;
  border-top: 1px solid #eef2f5;
}

.distribution-meta,
.distribution-figure {
  display: grid;
  gap: 2px;
}

.distribution-sub,
.distribution-figure span {
  color: #8b98a1;
  font-size: 0.84rem;
}

.distribution-figure {
  justify-items: end;
}

.distribution-figure strong {
  font-size: 1.05rem;
  color: #15364a;
}

.priority-list {
  list-style: none;
  padding: 0;
}

.priority-item {
  display: grid;
  grid-template-columns: auto minmax(0, 1fr) auto;
  align-items: center;
  gap: 14px 16px;
}

.priority-rank {
  display: grid;
  place-items: center;
  width: 44px;
  height: 44px;
  border-radius: 14px;
  background: linear-gradient(180deg, #eaf6f7, #d6ecee);
  color: #0c6a71;
  font-weight: 700;
  flex-shrink: 0;
}

.priority-rank__value {
  font-size: 0.95rem;
}

.priority-student-main {
  min-width: 0;
  display: grid;
  gap: 8px;
}

.priority-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  min-width: 0;
}

.priority-name,
.priority-major,
.priority-activity {
  min-width: 0;
}

.priority-name {
  color: #15364a;
  font-size: 1rem;
}

.priority-major,
.priority-activity {
  color: #607380;
  font-size: 0.92rem;
}

.priority-major {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  padding-right: 12px;
}

.priority-score {
  color: #0c6a71;
  font-weight: 700;
  white-space: nowrap;
}

.priority-activity {
  white-space: nowrap;
  text-align: right;
}

.priority-action {
  grid-column: 3;
  align-self: center;
  padding: 8px 12px;
  border: 1px solid #c8d8e0;
  border-radius: 10px;
  background: #fff;
  color: #0c6a71;
  cursor: pointer;
  font-weight: 600;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error,
.learning-status-note--error {
  color: #9b3333;
}

.empty-note {
  color: #607380;
}

@media (max-width: 720px) {
  .learning-status-grid,
  .summary-grid {
    grid-template-columns: 1fr;
  }

  .distribution-head,
  .priority-item {
    grid-template-columns: 1fr;
  }

  .priority-rank {
    width: 40px;
    height: 40px;
  }

  .priority-row {
    flex-direction: column;
    align-items: flex-start;
  }

  .priority-major,
  .priority-activity {
    white-space: normal;
    text-align: left;
    padding-right: 0;
  }

  .priority-action {
    grid-column: auto;
    grid-row: auto;
    justify-self: start;
  }
}
</style>
