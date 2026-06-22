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
              <p class="course-meta">负责教师：{{ course.teacherDisplayName || course.teacherUsername || teacherUsername }}</p>
            </div>
            <div class="count-pill">
              <strong>{{ course.knowledgePointCount }}</strong>
              <span>知识点</span>
            </div>
          </div>
          <div class="status-row">
            <span class="status-label">当前维护状态</span>
            <strong class="status-badge" :class="statusClass">{{ course.maintenanceStatus }}</strong>
          </div>
          <p class="course-description">{{ course.description }}</p>
          <p class="course-audience">适用对象：{{ course.targetAudience }}</p>
        </article>

        <section class="metric-grid">
          <article v-for="item in metricCards" :key="item.label" class="metric-card">
            <p class="metric-label">{{ item.label }}</p>
            <strong class="metric-value">{{ item.value }}</strong>
            <p class="metric-note">{{ item.note }}</p>
          </article>
        </section>

        <section class="detail-grid">
          <article class="detail-card">
            <div class="section-head">
              <h3>内容覆盖</h3>
              <span class="section-tip">看清哪些维护面已补齐</span>
            </div>
            <div class="coverage-list">
              <div class="coverage-row">
                <span>资源覆盖率</span>
                <strong>{{ resourceCoverageRate }}</strong>
              </div>
              <div class="coverage-row">
                <span>题库覆盖率</span>
                <strong>{{ questionCoverageRate }}</strong>
              </div>
              <div class="coverage-row">
                <span>未覆盖知识点</span>
                <strong>{{ course.uncoveredPointCount }}</strong>
              </div>
            </div>
          </article>

          <article class="detail-card">
            <div class="section-head">
              <h3>覆盖缺口</h3>
              <span class="section-tip">直接点名哪些知识点还没补齐</span>
            </div>
            <div v-if="uncoveredPoints.length === 0" class="empty-note">当前没有覆盖缺口。</div>
            <ul v-else class="gap-list">
              <li v-for="point in uncoveredPoints" :key="point.code" class="gap-item">
                <strong>{{ point.name }}</strong>
                <span>{{ point.code }}</span>
                <small>{{ formatMissingCoverage(point.missingCoverage) }}</small>
                <button type="button" class="gap-action" @click="goToCoverageGap(point)">
                  {{ point.missingCoverage.includes("resource") ? "去补资源" : "去补题库" }}
                </button>
              </li>
            </ul>
          </article>

        </section>

        <article class="table-card">
          <div class="section-head">
            <h3>章节分布</h3>
            <span class="section-tip">按章节查看知识点承载量</span>
          </div>
          <table class="chapter-table">
            <thead>
              <tr>
                <th>章节</th>
                <th>章节名称</th>
                <th>知识点数</th>
              </tr>
            </thead>
            <tbody>
              <tr v-for="chapter in chapterStats" :key="chapter.chapterNo">
                <td>第 {{ chapter.chapterNo }} 章</td>
                <td>{{ chapter.chapterName }}</td>
                <td>
                  <div class="chapter-progress">
                    <div class="chapter-bar-track">
                      <div class="chapter-bar" :style="{ width: chapterBarWidth(chapter) }" />
                    </div>
                    <span>{{ chapter.knowledgePointCount }}</span>
                  </div>
                </td>
              </tr>
            </tbody>
          </table>
        </article>
      </template>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";

import { fetchTeacherCourseOverview } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();
const loading = ref(true);
const loadError = ref("");
const coursePayload = ref(null);

const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const courseCode = computed(() => String(route.params?.courseCode || ""));
const course = computed(() => coursePayload.value?.course || null);
const chapterStats = computed(() => coursePayload.value?.chapterStats || []);
const uncoveredPoints = computed(() => coursePayload.value?.uncoveredPoints || []);

function numberOrZero(value) {
  const parsed = Number(value);
  return Number.isFinite(parsed) ? parsed : 0;
}

const metricCards = computed(() => {
  if (!course.value) return [];
  return [
    { label: "知识点", value: numberOrZero(course.value.knowledgePointCount), note: "课程主图谱规模" },
    { label: "依赖关系", value: numberOrZero(course.value.dependencyCount), note: "知识前置连接数" },
    { label: "学习资源", value: numberOrZero(course.value.resourceCount), note: "教师资源总量" },
    { label: "题库", value: numberOrZero(course.value.questionBankCount), note: "当前题库数量" },
    { label: "题目", value: numberOrZero(course.value.questionCount), note: "课程可用题目总量" },
  ];
});

const resourceCoverageRate = computed(() => {
  const total = numberOrZero(course.value?.knowledgePointCount);
  const covered = numberOrZero(course.value?.resourceCoveredPointCount);
  if (!total) return "0%";
  return `${Math.round((covered / total) * 100)}%`;
});

const questionCoverageRate = computed(() => {
  const total = numberOrZero(course.value?.knowledgePointCount);
  const covered = numberOrZero(course.value?.questionCoveredPointCount);
  if (!total) return "0%";
  return `${Math.round((covered / total) * 100)}%`;
});

const maxChapterPointCount = computed(() =>
  Math.max(1, ...chapterStats.value.map((item) => Number(item.knowledgePointCount || 0))),
);

const statusClass = computed(() => {
  const status = course.value?.maintenanceStatus || "";
  if (status === "建设完整") return "status-badge--ok";
  if (status === "待补依赖") return "status-badge--warn";
  return "status-badge--alert";
});

function chapterBarWidth(chapter) {
  return `${Math.round((Number(chapter.knowledgePointCount || 0) / maxChapterPointCount.value) * 100)}%`;
}

function formatMissingCoverage(items = []) {
  return items.map((item) => {
    if (item === "resource") return "缺学习资源";
    if (item === "question_bank") return "缺题库覆盖";
    return item;
  }).join(" / ");
}

function goToCoverageGap(point) {
  if (point.missingCoverage.includes("resource")) {
    router.push({
      name: "teacher-course-resources",
      params: { courseCode: courseCode.value },
      query: { focusPoint: point.code },
    });
    return;
  }

  router.push({
    name: "teacher-course-questions",
    params: { courseCode: courseCode.value },
    query: { focusPoint: point.code },
  });
}

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
.detail-card,
.metric-card,
.table-card {
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
.course-meta,
.course-description,
.course-audience,
.state-message {
  margin: 0;
  color: #51606d;
}

.overview-card h2,
.detail-card h3,
.table-card h3 {
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

.status-row {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-top: 14px;
}

.status-label {
  color: #607380;
  font-size: 0.9rem;
}

.status-badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  font-size: 0.9rem;
}

.status-badge--ok {
  background: #eaf7ee;
  color: #1d7a3f;
}

.status-badge--warn {
  background: #fff5e7;
  color: #9b6a12;
}

.status-badge--alert {
  background: #fdecec;
  color: #a33d3d;
}

.course-meta {
  margin-top: 10px;
  font-size: 0.92rem;
}

.state-message--error {
  color: #9b3333;
}

.metric-grid {
  display: grid;
  gap: 14px;
  grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
}

.metric-label,
.metric-note,
.section-tip {
  margin: 0;
  color: #607380;
}

.metric-value {
  display: block;
  margin-top: 8px;
  font-size: 1.8rem;
  color: #15364a;
}

.metric-note {
  margin-top: 8px;
  font-size: 0.88rem;
  line-height: 1.5;
}

.detail-grid {
  display: grid;
  gap: 18px;
  grid-template-columns: repeat(2, minmax(0, 1fr));
}

.section-head {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  gap: 12px;
}

.coverage-list,
.gap-list {
  margin-top: 16px;
}

.coverage-row {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  padding: 12px 0;
  border-bottom: 1px solid #eef2f5;
  color: #51606d;
}

.coverage-row strong {
  color: #0c6a71;
}

.gap-list {
  list-style: none;
  padding: 0;
  display: grid;
  gap: 10px;
}

.gap-item {
  display: grid;
  gap: 4px;
  padding: 12px 14px;
  border: 1px solid #e7edf2;
  border-radius: 12px;
  background: #fbfdfe;
}

.gap-item span,
.gap-item small,
.empty-note {
  color: #607380;
}

.gap-action {
  justify-self: start;
  margin-top: 4px;
  padding: 6px 10px;
  border: 1px solid #c8d8e0;
  border-radius: 8px;
  background: #fff;
  color: #0c6a71;
  cursor: pointer;
  font-weight: 600;
}

.chapter-table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 16px;
}

.chapter-table th,
.chapter-table td {
  padding: 12px 10px;
  border-bottom: 1px solid #eef2f5;
  text-align: left;
}

.chapter-table th {
  color: #607380;
  background: #f7fbfd;
}

.chapter-progress {
  display: flex;
  align-items: center;
  gap: 10px;
}

.chapter-bar-track {
  width: 160px;
  height: 10px;
  border-radius: 999px;
  background: #e8f0f4;
  overflow: hidden;
}

.chapter-bar {
  height: 100%;
  border-radius: 999px;
  background: linear-gradient(90deg, #0c6a71, #58a2aa);
}

@media (max-width: 720px) {
  .overview-header {
    flex-direction: column;
  }

  .detail-grid {
    grid-template-columns: 1fr;
  }
}
</style>
