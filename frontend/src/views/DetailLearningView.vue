<template>
  <PageLayout
    eyebrow="细化路径规划 · 二级路径规划"
    :title="pageTitle"
    :description="pageDescription"
  >
    <section class="detail-learning-layout">
      <article class="card surface-panel detail-branch-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">当前可细化分支</p>
            <h3>只展示当前路径中存在二级图谱的一级节点</h3>
          </div>
          <p class="caption">
            进入本页后可在这些一级节点之间切换，每次只规划当前一个二级范围。
          </p>
        </div>

        <div v-if="availableSections.length === 0" class="empty-tip">
          当前还没有可用的细化学习分支，请先回首页生成学习路径。
        </div>

        <div v-else class="detail-branch-list">
          <button
            v-for="section in availableSections"
            :key="`detail-section-${section.scopeCode}`"
            :data-testid="`detail-section-${section.scopeCode}`"
            type="button"
            class="detail-branch-button"
            :class="{
              'detail-branch-button--active':
                currentSection && currentSection.scopeCode === section.scopeCode,
            }"
            @click="goToSection(section.scopeCode)"
          >
            <strong>{{ section.name }}</strong>
            <span>第{{ section.chapterNo }}章</span>
            <span>{{ section.estimatedMinutes }} 分钟</span>
          </button>
        </div>
      </article>

      <article v-if="!currentSection" class="card surface-panel state state--warning">
        <p>当前页尚未收到首页路径规划的细化分支上下文。</p>
        <p>建议先在首页生成路径，再从“本轮推荐学习”中的一级节点点击“细化学习”。</p>
      </article>

      <DetailLearningWorkspace
        v-else
        :key="workspaceKey"
        :learner-code="detailLearningLearnerCode"
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
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import { fetchLearnerProfile } from "../api/learnerProfile";
import DetailLearningWorkspace from "../components/DetailLearningWorkspace.vue";
import PageLayout from "../components/PageLayout.vue";
import { useNavigationStore } from "../stores/navigationStore";

const route = useRoute();
const router = useRouter();
const navigationStore = useNavigationStore();
const learnerProfile = ref(null);
const learnerProfileError = ref("");
const learnerProfileVersion = ref(0);

const availableSections = computed(() => navigationStore.detailLearningSections || []);
const summary = computed(
  () =>
    navigationStore.detailLearningSummary || {
      sourceTargetLabel: "",
      selectedScopeCode: "",
      availableScopeCount: 0,
      sourcePage: "home",
      generatedAt: "",
    },
);
const requestedScopeCode = computed(() => String(route.query.scope || ""));
const detailLearningLearnerCode = computed(
  () => navigationStore.detailLearningLearnerCode || "demo-learner",
);
const currentSection = computed(() => {
  if (availableSections.value.length === 0) {
    return null;
  }

  return (
    navigationStore.detailLearningSectionByScopeCode(requestedScopeCode.value) ||
    navigationStore.detailLearningSectionByScopeCode(summary.value.selectedScopeCode) ||
    availableSections.value[0]
  );
});
const workspaceKey = computed(
  () =>
    `${currentSection.value?.scopeCode || "empty"}-${detailLearningLearnerCode.value}-${learnerProfileVersion.value}`,
);

const pageTitle = computed(() =>
  currentSection.value ? `${currentSection.value.name}细化学习` : "二级细化学习导航",
);

const pageDescription = computed(() => {
  if (!currentSection.value) {
    return "本页用于承接首页最近一次路径规划中的二级细化节点。若暂无上下文，请先回首页生成路径。";
  }

  return "首页负责一级路径规划；本页负责对应一级节点内部的二级学习顺序、局部反馈与路径变化。";
});

async function ensureCurrentScopeSynced() {
  if (!currentSection.value) {
    return;
  }

  navigationStore.setDetailLearningScope(currentSection.value.scopeCode);

  if (requestedScopeCode.value === currentSection.value.scopeCode) {
    return;
  }

  await router.replace({
    name: "detail-learning",
    query: {
      scope: currentSection.value.scopeCode,
    },
  });
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

async function goToSection(scopeCode) {
  if (!scopeCode) {
    return;
  }

  navigationStore.setDetailLearningScope(scopeCode);
  await router.push({
    name: "detail-learning",
    query: {
      scope: scopeCode,
    },
  });
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
  [availableSections, requestedScopeCode],
  async () => {
    await ensureCurrentScopeSynced();
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

.detail-branch-list {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 12px;
  margin-top: 18px;
}

.detail-branch-button {
  display: grid;
  gap: 6px;
  padding: 14px 16px;
  border-radius: 18px;
  border: 1px solid rgba(12, 106, 113, 0.12);
  background: rgba(255, 255, 255, 0.92);
  color: #21405f;
  text-align: left;
  cursor: pointer;
  transition: transform 0.15s ease, border-color 0.15s ease, box-shadow 0.15s ease;
}

.detail-branch-button:hover {
  transform: translateY(-1px);
}

.detail-branch-button--active {
  border-color: rgba(12, 106, 113, 0.32);
  box-shadow: 0 10px 24px rgba(12, 106, 113, 0.12);
  background: linear-gradient(180deg, rgba(255, 255, 255, 0.96), rgba(238, 248, 247, 0.96));
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
