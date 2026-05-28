<template>
  <PageLayout
    eyebrow="学习图谱 · 独立页面"
    title="课程知识图谱与学习状态"
    description="本页用于统一浏览课程图谱、学习状态和层级下钻，也支持把节点直接设为当前学习目标。"
  >
    <section class="graph-layout">
      <div v-if="!authLearnerCode" class="unassigned-card">
        <h2>暂未分配课程</h2>
        <p>你尚未被分配到任何课程。请联系教师或管理员为你分配课程后查看学习图谱。</p>
      </div>
      <template v-else>
      <div v-if="isEmptyCourse" class="unassigned-card">
        <h2>该课程暂无知识点</h2>
        <p>当前课程 {{ learnerProfile?.course?.name || '' }} 尚未添加知识点，请等待教师编辑课程内容。</p>
      </div>
      <template v-else>
      <LearnerLearningGraph
        :profile="learnerProfile"
        :preferred-selected-code="preferredSelectedCode"
        @set-target="handleSetTarget"
      />

      <article
        v-if="learnerProfileError"
        class="card surface-panel state state--error page-error-panel"
      >
        {{ learnerProfileError }}
      </article>
      </template>
      </template>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";

import { fetchLearnerProfile } from "../api/learnerProfile";
import LearnerLearningGraph from "../components/LearnerLearningGraph.vue";
import PageLayout from "../components/PageLayout.vue";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();
const learnerProfile = ref(null);
const learnerProfileError = ref("");

const preferredSelectedCode = computed(() => String(route.query.focus || ""));
const authLearnerCode = computed(() => String(authStore.linkedLearner?.learnerCode || ""));
const isEmptyCourse = computed(
  () => learnerProfile.value && learnerProfile.value.summary?.knowledgePointCount === 0,
);

async function loadLearnerProfile() {
  learnerProfileError.value = "";

  if (!authLearnerCode.value) {
    learnerProfileError.value = "暂未分配课程，请等待教师分配后查看。";
    return;
  }

  try {
    learnerProfile.value = await fetchLearnerProfile({ learnerCode: authLearnerCode.value });
  } catch (error) {
    learnerProfileError.value =
      "未能读取学习者画像。请先启动后端并确认数据库已初始化。";
    console.error(error);
  }
}

async function handleSetTarget({ code, scopeCode } = {}) {
  if (!code) {
    return;
  }

  const isRootScope = !scopeCode || scopeCode === "root";

  await router.push({
    name: isRootScope ? "home" : "detail-learning",
    query: isRootScope ? { target: code } : { scope: scopeCode, target: code },
  });
}

onMounted(async () => {
  await loadLearnerProfile();
});
</script>

<style scoped>
.graph-layout {
  display: grid;
  gap: 20px;
}

h2 {
  margin: 0 0 14px;
  font-size: 1.3rem;
}

.hero-note p {
  margin: 0;
}

.hero-note p {
  color: #32404a;
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
  font-size: 1.2rem;
}
</style>
