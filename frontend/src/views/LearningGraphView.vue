<template>
  <PageLayout
    eyebrow="学习图谱 · 独立页面"
    title="课程知识图谱与学习状态"
    description="本页用于统一浏览课程图谱、学习状态和层级下钻，也支持把节点直接设为当前学习目标。"
  >
    <section class="graph-layout">
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

async function loadLearnerProfile() {
  learnerProfileError.value = "";

  try {
    learnerProfile.value = authLearnerCode.value
      ? await fetchLearnerProfile({ learnerCode: authLearnerCode.value })
      : await fetchLearnerProfile();
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
</style>
