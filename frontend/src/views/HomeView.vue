<template>
  <PageLayout
    eyebrow="主图路径规划 · 首页"
    title="个性化学习路径规划"
    description='首页聚焦目标设置、路径生成、学习反馈与路径调整；课程结构浏览统一放到"学习图谱"页。'
  >
    <section class="home-stack">
      <PathPlannerPanel
        :key="plannerRenderKey"
        :learner-code="resolvedLearnerCode"
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
import { computed, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import PageLayout from "../components/PageLayout.vue";
import PathPlannerPanel from "../components/PathPlannerPanel.vue";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { useAuthStore } from "../stores/authStore";

const router = useRouter();
const route = useRoute();
const authStore = useAuthStore();
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const plannerRenderKey = ref(0);
const authLearnerCode = computed(() => String(authStore.linkedLearner?.learnerCode || ""));
const resolvedLearnerCode = computed(
  () => authLearnerCode.value || learnerProfile.value?.learner?.code || "demo-learner",
);

function handleLearnerProfileUpdated(payload) {
  learnerProfile.value = payload;
}

async function loadLearnerProfile() {
  learnerProfileLoading.value = true;

  try {
    learnerProfile.value = authLearnerCode.value
      ? await fetchLearnerProfile({ learnerCode: authLearnerCode.value })
      : await fetchLearnerProfile();
  } catch (error) {
    console.error(error);
  } finally {
    learnerProfileLoading.value = false;
  }
}

async function refreshLearnerProfileAfterPracticeUpdate() {
  const cleanedQuery = { ...route.query };
  delete cleanedQuery.practiceUpdated;

  await loadLearnerProfile();
  plannerRenderKey.value += 1;

  await router.replace({
    name: route.name,
    query: cleanedQuery,
  });
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
  await loadLearnerProfile();
});

watch(
  () => route.query.practiceUpdated,
  async (practiceUpdated) => {
    if (practiceUpdated !== "1") {
      return;
    }

    await refreshLearnerProfileAfterPracticeUpdate();
  },
  { immediate: true },
);
</script>

<style scoped>
.home-stack {
  display: grid;
  gap: 20px;
}
</style>
