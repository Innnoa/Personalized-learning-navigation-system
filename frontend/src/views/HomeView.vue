<template>
  <PageLayout
    eyebrow="主图路径规划 · 首页"
    title="个性化学习路径规划"
    description='首页聚焦目标设置、路径生成、学习反馈与路径调整；课程结构浏览统一放到"学习图谱"页。'
  >
    <section class="home-stack">
      <div v-if="isUnassigned && !learnerProfileLoading" class="unassigned-card">
        <h2>暂未分配课程</h2>
        <p>你尚未被分配到任何课程。请联系教师或管理员为你分配课程后，即可查看个性化学习路径。</p>
      </div>
      <PathPlannerPanel
        v-else
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
  () => authLearnerCode.value || learnerProfile.value?.learner?.code || "",
);

const isUnassigned = computed(() => !resolvedLearnerCode.value);

function handleLearnerProfileUpdated(payload) {
  learnerProfile.value = payload;
}

async function loadLearnerProfile() {
  learnerProfileLoading.value = true;

  if (!authLearnerCode.value) {
    learnerProfileLoading.value = false;
    return;
  }

  try {
    learnerProfile.value = await fetchLearnerProfile({ learnerCode: authLearnerCode.value });
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

.unassigned-card {
  padding: 32px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #fff8f0;
  text-align: center;
}

.unassigned-card h2 {
  margin: 0 0 8px;
  color: #8a6d1b;
}

.unassigned-card p {
  margin: 0;
  color: #51606d;
  font-size: 0.95rem;
}
</style>
