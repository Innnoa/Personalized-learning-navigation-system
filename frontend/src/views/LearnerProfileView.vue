<template>
  <PageLayout
    eyebrow="学习者画像 · 独立页面"
    title="学习者画像与学习记录"
    description="本页展示学习者的掌握度分布、待补强知识点、最近学习反馈与最近资源行为。"
  >
    <template #hero-side>
      <article class="hero-note surface-panel">
        <p class="label page-section-label">页面说明</p>
        <h2>适合单独讲解画像建模与学习轨迹</h2>
        <p>
          这里聚焦“学习者当前状态”，包括掌握度统计、薄弱点发现、最近反馈记录与最近资源行为。
        </p>
      </article>
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
import LearnerProfileCard from "../components/LearnerProfileCard.vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { useNavigationStore } from "../stores/navigationStore";

const RESOURCE_PREVIEW_MINUTES = 120;
const router = useRouter();
const navigationStore = useNavigationStore();
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const learnerProfileError = ref("");
const resourcePreviewCode = ref("");
const resourcePreviewError = ref("");

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
  await loadLearnerProfile();
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
</script>

<style scoped>
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
</style>
