<template>
  <PageLayout
    eyebrow="首页 · 路径规划"
    title="个性化学习路径规划"
    description="首页聚焦目标设置、路径生成、学习反馈与路径调整；课程结构浏览统一放到“学习图谱”页。"
  >
    <template #hero-side>
      <div class="hero-side">
        <HealthStatusCard :health="health" :error="healthError">
          <template #actions>
            <button
              type="button"
              class="demo-reset-button"
              :disabled="resettingDemo"
              @click="handleResetDemo"
            >
              {{ resettingDemo ? "正在恢复演示状态..." : "恢复演示初始状态" }}
            </button>
            <p class="demo-reset-note">
              清空学习记录与资源行为，并恢复默认掌握度，便于重新演示。
            </p>
            <p v-if="resetMessage" class="demo-reset-message demo-reset-message--ok">
              {{ resetMessage }}
            </p>
            <p v-if="resetError" class="demo-reset-message demo-reset-message--error">
              {{ resetError }}
            </p>
          </template>
        </HealthStatusCard>
      </div>
    </template>

    <section class="home-stack">
      <PathPlannerPanel
        :key="plannerRenderKey"
        :learner-code="learnerProfile?.learner?.code || 'demo-learner'"
        :initial-mastery-by-code="learnerProfile?.masteryByCode || {}"
        :feedback-record-count="learnerProfile?.summary?.feedbackRecordCount || 0"
        :profile-loading="learnerProfileLoading"
        @feedback-saved="handleLearnerProfileUpdated"
        @focus-node="handleGraphFocusRequested"
      />
    </section>
  </PageLayout>
</template>

<script setup>
import { onMounted, ref } from "vue";
import { useRouter } from "vue-router";

import { resetDemoState } from "../api/demo";
import HealthStatusCard from "../components/HealthStatusCard.vue";
import PageLayout from "../components/PageLayout.vue";
import PathPlannerPanel from "../components/PathPlannerPanel.vue";
import { fetchHealth } from "../api/health";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { useNavigationStore } from "../stores/navigationStore";

const router = useRouter();
const navigationStore = useNavigationStore();
const health = ref(null);
const healthError = ref("");
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const plannerRenderKey = ref(0);
const resettingDemo = ref(false);
const resetMessage = ref("");
const resetError = ref("");

function handleLearnerProfileUpdated(payload) {
  learnerProfile.value = payload;
}

async function handleGraphFocusRequested(code) {
  if (!code) {
    return;
  }

  await router.push({
    name: "learning-graph",
    query: { focus: code },
  });
}

async function handleResetDemo() {
  resettingDemo.value = true;
  resetMessage.value = "";
  resetError.value = "";

  try {
    const payload = await resetDemoState();
    learnerProfile.value = payload;
    navigationStore.clearResourceRecommendationContext();
    navigationStore.clearLearningGraphViewState();
    navigationStore.clearDetailLearningContext();
    plannerRenderKey.value += 1;
    resetMessage.value =
      `已恢复演示初始状态，清空 ${payload.resetSummary?.clearedFeedbackRecordCount || 0} 条反馈记录与 ${payload.resetSummary?.clearedResourceViewRecordCount || 0} 条资源行为。`;
    await router.replace({
      name: "home",
    });
  } catch (error) {
    resetError.value =
      error?.response?.data?.detail ||
      "恢复演示初始状态失败。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    resettingDemo.value = false;
  }
}

onMounted(async () => {
  const [healthResult, learnerResult] = await Promise.allSettled([
    fetchHealth(),
    fetchLearnerProfile(),
  ]);

  if (healthResult.status === "fulfilled") {
    health.value = healthResult.value;
  } else {
    healthError.value =
      "未能连接后端。请先启动 Drogon 服务；若在 WSL 中运行，也请检查当前代理配置。";
    console.error(healthResult.reason);
  }

  if (learnerResult.status === "fulfilled") {
    learnerProfile.value = learnerResult.value;
  } else {
    console.error(learnerResult.reason);
  }

  learnerProfileLoading.value = false;
});
</script>

<style scoped>
.hero-side {
  display: grid;
  gap: 18px;
}

.home-stack {
  display: grid;
  gap: 20px;
}

.demo-reset-button {
  border: 0;
  border-radius: 999px;
  padding: 11px 16px;
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  font-weight: 700;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease;
}

.demo-reset-button:hover:not(:disabled) {
  transform: translateY(-1px);
}

.demo-reset-button:disabled {
  opacity: 0.7;
  cursor: wait;
}

.demo-reset-note,
.demo-reset-message {
  margin: 0;
  line-height: 1.6;
}

.demo-reset-note {
  color: #586674;
}

.demo-reset-message--ok {
  color: #176b39;
}

.demo-reset-message--error {
  color: #9b3333;
}
</style>
