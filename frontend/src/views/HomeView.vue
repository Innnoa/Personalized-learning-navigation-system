<template>
  <PageLayout
    eyebrow="主图路径规划 · 首页"
    title="个性化学习路径规划"
    description='首页聚焦目标设置、路径生成、学习反馈与路径调整；课程结构浏览统一放到"学习图谱"页。'
  >
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

import PageLayout from "../components/PageLayout.vue";
import PathPlannerPanel from "../components/PathPlannerPanel.vue";
import { fetchLearnerProfile } from "../api/learnerProfile";

const router = useRouter();
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const plannerRenderKey = ref(0);

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

onMounted(async () => {
  try {
    learnerProfile.value = await fetchLearnerProfile();
  } catch (error) {
    console.error(error);
  } finally {
    learnerProfileLoading.value = false;
  }
});
</script>

<style scoped>
.home-stack {
  display: grid;
  gap: 20px;
}
</style>
