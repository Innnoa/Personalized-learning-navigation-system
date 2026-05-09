<template>
  <PageLayout
    eyebrow="学习者画像 · 独立页面"
    title="学习者画像与学习记录"
    description="本页展示学习者的掌握度分布、待补强知识点、最近学习反馈与最近资源行为。"
  >
    <template #hero-side>
      <div class="hero-side">
        <HealthStatusCard :health="health" :error="healthError" />
        <div class="demo-reset-section">
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
        </div>
      </div>
    </template>

    <section class="profile-layout">
      <LearnerProfileCard
        :profile="learnerProfile"
        :loading="learnerProfileLoading"
        :error="learnerProfileError"
        :resource-preview-code="resourcePreviewCode"
        :resource-preview-error="resourcePreviewError"
        @open-resource="handleOpenWeakItemResource"
      />
    </section>
  </PageLayout>
</template>

<script setup>
import { onMounted, ref } from "vue";
import { useRouter } from "vue-router";

import { generateLearningPath } from "../api/path";
import { resetDemoState } from "../api/demo";
import HealthStatusCard from "../components/HealthStatusCard.vue";
import LearnerProfileCard from "../components/LearnerProfileCard.vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchHealth } from "../api/health";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { useNavigationStore } from "../stores/navigationStore";

const RESOURCE_PREVIEW_MINUTES = 120;
const router = useRouter();
const navigationStore = useNavigationStore();
const health = ref(null);
const healthError = ref("");
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const learnerProfileError = ref("");
const resourcePreviewCode = ref("");
const resourcePreviewError = ref("");
const resettingDemo = ref(false);
const resetMessage = ref("");
const resetError = ref("");

async function loadLearnerProfile() {
  learnerProfileLoading.value = true;
  learnerProfileError.value = "";

  try {
    learnerProfile.value = await fetchLearnerProfile();
  } catch (error) {
    learnerProfileError.value =
      "未能读取学习者画像。请先启动后端并确认数据库已初始化。";
    console.error(error);
  } finally {
    learnerProfileLoading.value = false;
  }
}

onMounted(async () => {
  const [healthResult, learnerResult] = await Promise.allSettled([
    fetchHealth(),
    loadLearnerProfile(),
  ]);

  if (healthResult.status === "fulfilled") {
    health.value = healthResult.value;
  } else {
    healthError.value =
      "未能连接后端。请先启动 Drogon 服务；若在 WSL 中运行，也请检查当前代理配置。";
    console.error(healthResult.reason);
  }
});

async function handleOpenWeakItemResource(item) {
  if (!learnerProfile.value || !item?.code) {
    return;
  }

  resourcePreviewCode.value = item.code;
  resourcePreviewError.value = "";

  try {
    const learnerCode = learnerProfile.value.learner?.code || "demo-learner";
    const masteryByCode = learnerProfile.value.masteryByCode || {};
    const payload = await generateLearningPath({
      learnerCode,
      targetCodes: [item.code],
      availableMinutes: RESOURCE_PREVIEW_MINUTES,
      masteryByCode,
    });

    const resourceSections = payload.resourceRecommendations || [];
    if (resourceSections.length === 0) {
      navigationStore.clearResourceRecommendationContext("main");
      resourcePreviewError.value =
        "当前节点暂无可用资源快照。请先回首页生成学习路径后再查看。";
      return;
    }

    navigationStore.setResourceRecommendationContext({
      learnerCode,
      resourceRecommendationSections: resourceSections,
      targetLabel: item.name,
      availableMinutes:
        payload.summary?.availableMinutes || RESOURCE_PREVIEW_MINUTES,
      scheduledCount: payload.summary?.scheduledCount || 0,
      deferredCount: payload.summary?.deferredCount || 0,
    });

    const targetSection =
      resourceSections.find((section) => section.code === item.code) ||
      resourceSections[0];

    await router.push({
      name: "resource-recommendation",
      params: { code: targetSection.code },
      query: {
        level: "main",
      },
    });
  } catch (error) {
    resourcePreviewError.value =
      error?.response?.data?.detail ||
      "未能准备该节点资源。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    resourcePreviewCode.value = "";
  }
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
    resetMessage.value =
      `已恢复演示初始状态，清空 ${payload.resetSummary?.clearedFeedbackRecordCount || 0} 条反馈记录与 ${payload.resetSummary?.clearedResourceViewRecordCount || 0} 条资源行为。`;
  } catch (error) {
    resetError.value =
      error?.response?.data?.detail ||
      "恢复演示初始状态失败。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    resettingDemo.value = false;
  }
}
</script>

<style scoped>
.hero-side {
  display: grid;
  gap: 18px;
}

.profile-layout {
  display: grid;
  gap: 18px;
}

h2 {
  margin: 0 0 14px;
  font-size: 1.3rem;
}

.hero-note p {
  margin: 0;
  color: #32404a;
}

.demo-reset-section {
  display: grid;
  gap: 10px;
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
