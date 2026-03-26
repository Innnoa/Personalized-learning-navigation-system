<template>
  <PageLayout
    eyebrow="学习图谱 · 独立页面"
    title="课程知识图谱与学习状态"
    description="本页用于统一浏览课程图谱、学习状态和层级下钻，也支持把节点直接设为当前学习目标。"
  >
    <template #hero-side>
      <article class="hero-note surface-panel">
        <p class="label page-section-label">页面说明</p>
        <h2>适合先讲课程结构，再回首页做路径规划</h2>
        <p>
          图谱会按掌握度着色，并支持双击逐层下钻。选中节点后，可直接设为当前学习目标并跳回首页。
        </p>
      </article>
    </template>

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

const route = useRoute();
const router = useRouter();
const learnerProfile = ref(null);
const learnerProfileError = ref("");

const preferredSelectedCode = computed(() => String(route.query.focus || ""));

async function loadLearnerProfile() {
  learnerProfileError.value = "";

  try {
    learnerProfile.value = await fetchLearnerProfile();
  } catch (error) {
    learnerProfileError.value =
      "未能读取学习者画像。请先启动后端并确认数据库已初始化。";
    console.error(error);
  }
}

async function handleSetTarget(code) {
  if (!code) {
    return;
  }

  await router.push({
    name: "home",
    query: { target: code },
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
