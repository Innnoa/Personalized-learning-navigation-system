<template>
  <section class="stats-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载学情数据...</p>

    <template v-else>
      <div class="summary-grid">
        <article class="summary-card">
          <p class="summary-label">学生总数</p>
          <strong class="summary-value">{{ stats.studentCount }}</strong>
        </article>
        <article class="summary-card">
          <p class="summary-label">平均掌握度</p>
          <strong class="summary-value">{{ masteryPercent(stats.avgMastery) }}</strong>
        </article>
      </div>

      <article class="distribution-card">
        <h3>掌握度分布</h3>
        <div class="distribution-bars">
          <div class="dist-row">
            <span class="dist-label">较高 (70-100%)</span>
            <div class="dist-track">
              <div
                class="dist-fill dist-fill--high"
                :style="{ width: barWidth('high') }"
              ></div>
            </div>
            <strong class="dist-count">{{ dist.high.count }}</strong>
          </div>
          <div class="dist-row">
            <span class="dist-label">中等 (35-70%)</span>
            <div class="dist-track">
              <div
                class="dist-fill dist-fill--mid"
                :style="{ width: barWidth('mid') }"
              ></div>
            </div>
            <strong class="dist-count">{{ dist.mid.count }}</strong>
          </div>
          <div class="dist-row">
            <span class="dist-label">较低 (0-35%)</span>
            <div class="dist-track">
              <div
                class="dist-fill dist-fill--low"
                :style="{ width: barWidth('low') }"
              ></div>
            </div>
            <strong class="dist-count">{{ dist.low.count }}</strong>
          </div>
        </div>
      </article>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchTeacherCourseStats } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const statsData = ref(null);

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const stats = computed(() => statsData.value || { studentCount: 0, avgMastery: 0, masteryDistribution: { low: { count: 0 }, mid: { count: 0 }, high: { count: 0 } } });
const dist = computed(() => stats.value.masteryDistribution || { low: { count: 0 }, mid: { count: 0 }, high: { count: 0 } });

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

function barWidth(tier) {
  const total = (dist.value.low?.count || 0) + (dist.value.mid?.count || 0) + (dist.value.high?.count || 0);
  if (total === 0) return "0%";
  return Math.round(((dist.value[tier]?.count || 0) / total) * 100) + "%";
}

async function loadStats() {
  loading.value = true;
  loadError.value = "";
  try {
    statsData.value = await fetchTeacherCourseStats(courseCode.value, {
      username: teacherUsername.value,
    });
  } catch (error) {
    loadError.value = "未能读取课程统计数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

onMounted(loadStats);
</script>

<style scoped>
.stats-layout {
  display: grid;
  gap: 18px;
}

.summary-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
}

.summary-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.summary-label {
  margin: 0;
  color: #51606d;
}

.summary-value {
  display: block;
  margin-top: 10px;
  font-size: 1.5rem;
  color: #15364a;
}

.distribution-card {
  padding: 20px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.distribution-card h3 {
  margin: 0 0 16px;
  color: #15364a;
}

.distribution-bars {
  display: grid;
  gap: 14px;
}

.dist-row {
  display: flex;
  align-items: center;
  gap: 12px;
}

.dist-label {
  flex-shrink: 0;
  width: 130px;
  color: #51606d;
  font-size: 0.9rem;
}

.dist-track {
  flex: 1;
  height: 18px;
  background: #eef7f8;
  border-radius: 9px;
  overflow: hidden;
}

.dist-fill {
  height: 100%;
  border-radius: 9px;
  transition: width 0.3s;
}

.dist-fill--high {
  background: #0c6a71;
}

.dist-fill--mid {
  background: #4a9ea3;
}

.dist-fill--low {
  background: #d0ccb5;
}

.dist-count {
  flex-shrink: 0;
  width: 30px;
  text-align: right;
  color: #15364a;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}
</style>
