<template>
  <PageLayout
    :eyebrow="pageEyebrow"
    :title="pageTitle"
    :description="pageDescription"
  >
    <section class="home-stack">
      <div v-if="isUnassigned && !learnerProfileLoading" class="unassigned-card">
        <h2>暂未分配课程</h2>
        <p>你尚未被分配到任何课程。请联系教师或管理员为你分配课程后，即可查看个性化学习路径。</p>
      </div>
      <article
        v-else-if="plannerMode === 'detail' && !routeSection && !routeSectionLoading"
        class="card surface-panel state state--warning"
      >
        <p>当前页尚未指定有效的细化范围。</p>
        <p>请先回学习图谱，选择一个可细化节点后再进入统一路径规划页。</p>
      </article>
      <PathPlannerPanel
        v-else-if="plannerMode === 'main'"
        :key="plannerRenderKey"
        :learner-code="resolvedLearnerCode"
        :initial-mastery-by-code="learnerProfile?.masteryByCode || {}"
        :feedback-record-count="learnerProfile?.summary?.feedbackRecordCount || 0"
        :profile-loading="learnerProfileLoading"
        @feedback-saved="handleLearnerProfileUpdated"
        @focus-node="handleGraphFocusRequested"
      />
      <DetailLearningWorkspace
        v-else-if="routeSection"
        :key="detailWorkspaceKey"
        :learner-code="resolvedLearnerCode"
        :mastery-by-code="
          learnerProfile?.graphMasteryByCode || learnerProfile?.masteryByCode || {}
        "
        :section="routeSection"
        compact-mode
        @profile-updated="handleDetailProfileUpdated"
      />
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import PageLayout from "../components/PageLayout.vue";
import DetailLearningWorkspace from "../components/DetailLearningWorkspace.vue";
import PathPlannerPanel from "../components/PathPlannerPanel.vue";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import { useAuthStore } from "../stores/authStore";
import { useNavigationStore } from "../stores/navigationStore";

const router = useRouter();
const route = useRoute();
const authStore = useAuthStore();
const navigationStore = useNavigationStore();
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const plannerRenderKey = ref(0);
const routeSection = ref(null);
const routeSectionLoading = ref(false);
const authLearnerCode = computed(() => String(authStore.linkedLearner?.learnerCode || ""));
const resolvedLearnerCode = computed(
  () => authLearnerCode.value || learnerProfile.value?.learner?.code || "",
);
const plannerMode = computed(() =>
  route.query.scope && String(route.query.scope) !== "root" ? "detail" : "main",
);
const pageEyebrow = computed(() =>
  plannerMode.value === "detail" ? "路径规划 · 二级模式" : "路径规划 · 一级模式",
);
const pageTitle = computed(() =>
  plannerMode.value === "detail"
    ? `${routeSection.value?.name || "当前范围"}路径规划`
    : "课程级路径规划",
);
const pageDescription = computed(() =>
  plannerMode.value === "detail"
    ? '本页用于当前一级节点内部的二级路径规划；目标来自"学习图谱"页，需手动选择可用时间后生成路径。'
    : '本页聚焦课程级目标设置、路径生成、学习反馈与路径调整；课程结构浏览统一放到"学习图谱"页。',
);
const detailWorkspaceKey = computed(
  () => `${routeSection.value?.scopeCode || "empty"}-${resolvedLearnerCode.value}-${plannerRenderKey.value}`,
);

const isUnassigned = computed(() => !resolvedLearnerCode.value);

function handleLearnerProfileUpdated(payload) {
  learnerProfile.value = payload;
}

async function handleDetailProfileUpdated() {
  await loadLearnerProfile();
  plannerRenderKey.value += 1;
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

async function loadRouteSection() {
  if (plannerMode.value !== "detail") {
    routeSection.value = null;
    routeSectionLoading.value = false;
    navigationStore.clearDetailLearningContext();
    return;
  }

  routeSectionLoading.value = true;

  try {
    const payload = await fetchKnowledgeGraph({
      scopeCode: String(route.query.scope || ""),
    });
    const view = payload?.view || {};
    routeSection.value = {
      code: view.parentNodeCode || "",
      name: view.scopeName || String(route.query.scope || ""),
      scopeCode: view.scopeCode || String(route.query.scope || ""),
      scopeLabel: view.scopeName || String(route.query.scope || ""),
      chapterNo: payload?.nodes?.[0]?.chapterNo || 0,
      estimatedMinutes: 0,
      status: "scheduled",
    };
    navigationStore.setPlannerContext({
      scopeCode: routeSection.value.scopeCode,
      targetCode: String(route.query.target || ""),
    });
    navigationStore.setDetailLearningContext({
      learnerCode: resolvedLearnerCode.value || "demo-learner",
      detailLearningSections: [routeSection.value],
      selectedScopeCode: routeSection.value.scopeCode,
      sourceTargetLabel: routeSection.value.name,
      sourcePage: "home",
    });
  } catch (error) {
    routeSection.value = null;
    navigationStore.clearDetailLearningContext();
    console.error(error);
  } finally {
    routeSectionLoading.value = false;
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
  await loadRouteSection();
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

watch(
  () => [route.query.scope, route.query.target],
  async () => {
    await loadRouteSection();
  },
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

.state {
  display: grid;
  gap: 10px;
}

.state--warning {
  background: rgba(255, 244, 221, 0.92);
  color: #8d6311;
}
</style>
