<template>
  <PageLayout
    eyebrow="细化路径规划 · 二级路径规划"
    :title="pageTitle"
    :description="pageDescription"
  >
    <section class="detail-learning-layout">
      <div v-if="!authLearnerCode" class="unassigned-card">
        <h2>暂未分配课程</h2>
        <p>你尚未被分配到任何课程。请联系教师或管理员为你分配课程后查看细化路径规划。</p>
      </div>
      <template v-else>
      <article v-if="!currentSection" class="card surface-panel state state--warning">
        <p>当前页尚未指定可细化范围。</p>
        <p>请先进入学习图谱，再从带有细化图谱的节点进入本页。</p>
      </article>

      <DetailLearningWorkspace
        v-else
        :key="workspaceKey"
        :learner-code="detailLearningLearnerCode"
        :course-code="learnerProfile?.course?.code || authStore.linkedLearner?.courseCode || ''"
        :mastery-by-code="
          learnerProfile?.graphMasteryByCode || learnerProfile?.masteryByCode || {}
        "
        :section="currentSection"
        @profile-updated="handleProfileUpdated"
      />

      <article
        v-if="learnerProfileError"
        class="card surface-panel state state--error page-error-panel"
      >
        {{ learnerProfileError }}
      </article>
      </template>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import { fetchLearnerProfile } from "../api/learnerProfile";
import DetailLearningWorkspace from "../components/DetailLearningWorkspace.vue";
import PageLayout from "../components/PageLayout.vue";
import { useAuthStore } from "../stores/authStore";
import { useNavigationStore } from "../stores/navigationStore";
import { fetchKnowledgeGraph } from "../api/knowledgeGraph";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();
const navigationStore = useNavigationStore();
const learnerProfile = ref(null);
const learnerProfileError = ref("");
const learnerProfileVersion = ref(0);
const routeSection = ref(null);
const routeSectionError = ref("");
const authLearnerCode = computed(() => String(authStore.linkedLearner?.learnerCode || ""));
const requestedScopeCode = computed(() => String(route.query.scope || ""));
const detailLearningLearnerCode = computed(
  () => authLearnerCode.value || "demo-learner",
);
const currentSection = computed(() => routeSection.value);
const workspaceKey = computed(
  () =>
    `${currentSection.value?.scopeCode || "empty"}-${detailLearningLearnerCode.value}-${learnerProfileVersion.value}`,
);

const pageTitle = computed(() =>
  currentSection.value ? `${currentSection.value.name}细化学习` : "二级细化学习导航",
);

const pageDescription = computed(() => {
  if (!currentSection.value) {
    return "本页用于承接学习图谱中的细化节点。若暂无范围参数，请先回学习图谱选择可细化节点。";
  }

  return "学习图谱负责进入当前细化范围；本页负责该一级节点内部的二级学习顺序、局部反馈与路径变化。";
});

async function loadRouteSection(scopeCode) {
  if (!scopeCode) {
    routeSection.value = null;
    routeSectionError.value = "";
    return;
  }

  routeSectionError.value = "";

  try {
    const params = { scopeCode };
    const courseCode = String(
      learnerProfile.value?.course?.code || authStore.linkedLearner?.courseCode || "",
    );
    if (courseCode) {
      params.courseCode = courseCode;
    }
    const payload = await fetchKnowledgeGraph(params);
    const view = payload?.view || {};
    routeSection.value = {
      code: view.parentNodeCode || "",
      name: view.scopeName || scopeCode,
      scopeCode: view.scopeCode || scopeCode,
      scopeLabel: view.scopeName || scopeCode,
      chapterNo: payload?.nodes?.[0]?.chapterNo || 0,
      estimatedMinutes: 0,
      status: "scheduled",
    };
  } catch (error) {
    routeSection.value = null;
    routeSectionError.value = "未能读取当前细化范围。请检查后端服务或图谱配置。";
    console.error(error);
  }
}

async function loadLearnerProfile() {
  learnerProfileError.value = "";

  try {
    learnerProfile.value = await fetchLearnerProfile({
      learnerCode: detailLearningLearnerCode.value,
    });
  } catch (error) {
    learnerProfileError.value =
      "未能读取学习者画像。细化图谱仍可展示，但默认掌握度可能与最近演示状态不完全一致。";
    console.error(error);
  } finally {
    learnerProfileVersion.value += 1;
  }
}

async function goHome() {
  await router.push({
    name: "home",
  });
}

async function goToLearningGraph() {
  if (!currentSection.value?.code) {
    return;
  }

  await router.push({
    name: "learning-graph",
    query: {
      focus: currentSection.value.code,
    },
  });
}

async function handleProfileUpdated() {
  await loadLearnerProfile();
}

watch(
  requestedScopeCode,
  async () => {
    await loadRouteSection(requestedScopeCode.value);
  },
  {
    immediate: true,
  },
);

watch(
  detailLearningLearnerCode,
  async (learnerCode, previousLearnerCode) => {
    if (!learnerCode || learnerCode === previousLearnerCode) {
      return;
    }

    await loadLearnerProfile();
  },
);

watch(
  () => learnerProfile.value?.course?.code || authStore.linkedLearner?.courseCode || "",
  async (courseCode, previousCourseCode) => {
    if (!courseCode || courseCode === previousCourseCode || !requestedScopeCode.value) {
      return;
    }

    await loadRouteSection(requestedScopeCode.value);
  },
);

onMounted(async () => {
  await loadLearnerProfile();
});
</script>

<style scoped>
.detail-learning-layout {
  display: grid;
  gap: 20px;
}

h2,
h3 {
  margin: 0;
}

.hero-note-copy,
.caption,
.empty-tip,
.state p {
  margin: 0;
  color: #5f6c7a;
  line-height: 1.7;
}

.hero-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  margin-top: 18px;
}

.ghost-button {
  border: 0;
  border-radius: 999px;
  padding: 11px 16px;
  background: rgba(12, 106, 113, 0.08);
  color: #0c5960;
  font-weight: 700;
  cursor: pointer;
  transition: transform 0.15s ease, background 0.15s ease;
}

.ghost-button:hover {
  transform: translateY(-1px);
}

.section-headline {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  gap: 18px;
}

.caption {
  max-width: 420px;
  font-size: 0.92rem;
}

.state {
  display: grid;
  gap: 10px;
}

.state--warning {
  background: rgba(255, 244, 221, 0.92);
  color: #8d6311;
}

@media (max-width: 900px) {
  .section-headline {
    flex-direction: column;
  }
}
</style>
