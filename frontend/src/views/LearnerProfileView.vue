<template>
  <PageLayout
    eyebrow="学习者画像 · 独立页面"
    title="学习者画像与学习记录"
    description="本页单独展示演示学习者的掌握度分布、待补强知识点与最近学习反馈，便于和首页的知识图谱与路径规划分开展示。"
  >
    <template #hero-side>
      <article class="hero-note">
        <p class="label">页面说明</p>
        <h2>适合答辩时单独讲解画像建模</h2>
        <p>
          这里聚焦“学习者当前状态”，包括掌握度统计、最近反馈记录，以及误点后如何通过逐步回退恢复画像。
        </p>
      </article>
    </template>

    <section class="profile-layout">
      <LearnerProfileCard
        :profile="learnerProfile"
        :loading="learnerProfileLoading"
        :error="learnerProfileError"
      />

      <article class="card">
        <p class="label">展示建议</p>
        <h2>适合作为“学习者建模”展示页</h2>
        <ul class="list">
          <li>先说明当前课程固定为“数据结构”，画像用于承接路径规划的默认输入。</li>
          <li>展示平均掌握度、已掌握/进行中/未开始的知识点分布。</li>
          <li>用“当前待补强知识点”说明系统如何发现短板节点。</li>
          <li>用“最近学习记录”说明反馈保存、掌握度变化和逐步回退能力。</li>
          <li>再切回首页，演示画像如何驱动知识图谱和学习路径规划。</li>
        </ul>
      </article>
    </section>
  </PageLayout>
</template>

<script setup>
import { onMounted, ref } from "vue";

import LearnerProfileCard from "../components/LearnerProfileCard.vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchLearnerProfile } from "../api/learnerProfile";

const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const learnerProfileError = ref("");

async function loadLearnerProfile() {
  learnerProfileLoading.value = true;
  learnerProfileError.value = "";

  try {
    learnerProfile.value = await fetchLearnerProfile();
  } catch (error) {
    learnerProfileError.value =
      "未能读取学习者画像，请先启动后端并确认数据库已初始化。";
    console.error(error);
  } finally {
    learnerProfileLoading.value = false;
  }
}

onMounted(async () => {
  await loadLearnerProfile();
});
</script>

<style scoped>
.profile-layout {
  display: grid;
  grid-template-columns: minmax(320px, 1.05fr) minmax(280px, 0.95fr);
  gap: 20px;
  align-items: start;
}

.hero-note,
.card {
  background: rgba(255, 255, 255, 0.82);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
}

.label {
  margin: 0 0 6px;
  font-size: 0.84rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

h2 {
  margin: 0 0 14px;
  font-size: 1.3rem;
}

.hero-note p,
.list {
  margin: 0;
  color: #32404a;
}

.list {
  padding-left: 18px;
  display: grid;
  gap: 10px;
}

@media (max-width: 980px) {
  .profile-layout {
    grid-template-columns: 1fr;
  }
}
</style>
