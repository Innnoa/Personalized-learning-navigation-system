<template>
  <PageLayout
    eyebrow="学习者画像 · 独立页面"
    title="学习者画像与学习记录"
    description="本页展示学习者的掌握度分布、最近学习反馈与最近资源行为。"
  >
    <template #hero-side>
      <div class="hero-side">
        <div v-if="authStore.currentUser?.username === 'student_demo'" class="demo-reset-section">
          <button
            type="button"
            class="demo-reset-button"
            :disabled="resettingDemo"
            @click="handleResetDemo"
          >
            {{ resettingDemo ? "正在恢复演示状态..." : "恢复演示初始状态" }}
          </button>
          <p class="demo-reset-note">
            清空学习记录与资源行为，并恢复默认掌握度，便于重新演示。
          </p>
          <p v-if="resetMessage" class="demo-reset-message demo-reset-message--ok">
            {{ resetMessage }}
          </p>
          <p v-if="resetError" class="demo-reset-message demo-reset-message--error">
            {{ resetError }}
          </p>
        </div>
      </div>
    </template>

    <section class="profile-layout">
      <LearnerProfileCard
        :profile="learnerProfile"
        :loading="learnerProfileLoading"
        :error="learnerProfileError"
      />
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";

import { resetDemoState } from "../api/demo";
import LearnerProfileCard from "../components/LearnerProfileCard.vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchLearnerProfile } from "../api/learnerProfile";
import { useAuthStore } from "../stores/authStore";
import { useNavigationStore } from "../stores/navigationStore";

const authStore = useAuthStore();
const navigationStore = useNavigationStore();
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const learnerProfileError = ref("");
const resettingDemo = ref(false);
const resetMessage = ref("");
const resetError = ref("");
const authLearnerCode = computed(() => String(authStore.linkedLearner?.learnerCode || ""));

async function loadLearnerProfile() {
  learnerProfileLoading.value = true;
  learnerProfileError.value = "";

  if (!authLearnerCode.value) {
    learnerProfileError.value = "暂未分配课程，请等待教师分配后查看。";
    learnerProfileLoading.value = false;
    return;
  }

  try {
    learnerProfile.value = await fetchLearnerProfile({ learnerCode: authLearnerCode.value });
  } catch (error) {
    learnerProfileError.value =
      error?.response?.data?.detail ||
      "未能读取学习者画像。请确认后端已启动且数据库已初始化。";
    console.error(error);
  } finally {
    learnerProfileLoading.value = false;
  }
}

async function handleResetDemo() {
  resettingDemo.value = true;
  resetMessage.value = "";
  resetError.value = "";

  try {
    const payload = await resetDemoState();
    learnerProfile.value = payload;
    navigationStore.clearResourceRecommendationContext();
    navigationStore.clearLearningGraphViewState();
    navigationStore.clearDetailLearningContext();
    resetMessage.value =
      `已恢复演示初始状态，清空 ${payload.resetSummary?.clearedFeedbackRecordCount || 0} 条反馈记录与 ${payload.resetSummary?.clearedResourceViewRecordCount || 0} 条资源行为。`;
  } catch (error) {
    resetError.value =
      error?.response?.data?.detail ||
      "恢复演示初始状态失败。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    resettingDemo.value = false;
  }
}

onMounted(loadLearnerProfile);
</script>

<style scoped>
.hero-side {
  display: grid;
  gap: 18px;
}

.profile-layout {
  display: grid;
  gap: 18px;
}

h2 {
  margin: 0 0 14px;
  font-size: 1.3rem;
}

.hero-note p {
  margin: 0;
  color: #32404a;
}

.demo-reset-section {
  display: grid;
  gap: 10px;
}

.demo-reset-button {
  border: 0;
  border-radius: 999px;
  padding: 11px 16px;
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  font-weight: 700;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease;
}

.demo-reset-button:hover:not(:disabled) {
  transform: translateY(-1px);
}

.demo-reset-button:disabled {
  opacity: 0.7;
  cursor: wait;
}

.demo-reset-note,
.demo-reset-message {
  margin: 0;
  line-height: 1.6;
}

.demo-reset-note {
  color: #586674;
}

.demo-reset-message--ok {
  color: #176b39;
}

.demo-reset-message--error {
  color: #9b3333;
}
</style>
