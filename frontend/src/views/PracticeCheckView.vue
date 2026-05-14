<template>
  <PageLayout
    eyebrow="练习检测 · 提交页"
    :title="pageTitle"
    description="完成固定练习题后即可提交本次练习结果，并将加权掌握度写回学习反馈。"
  >
    <section class="practice-check-layout">
      <article class="card surface-panel intro-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">练习说明</p>
            <h2>{{ pageHeading }}</h2>
          </div>
          <p class="caption">
            请选择每道题的答案。系统会根据答题正确情况与先前掌握度计算新的加权掌握度，并提交到学习反馈。
          </p>
        </div>

        <p class="intro-note">
          需完成全部题目后才能提交；当前页面仅支持已配置固定题目的知识点。
        </p>
      </article>

      <article class="card surface-panel context-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">流转上下文</p>
            <h3>当前练习检测信息</h3>
          </div>
        </div>

        <dl class="context-grid">
          <div class="context-item">
            <dt>当前知识点</dt>
            <dd>{{ practiceCheckContext.targetName || "未指定" }}</dd>
          </div>
          <div class="context-item">
            <dt>知识点编码</dt>
            <dd>{{ practiceCheckContext.targetCode || "未指定" }}</dd>
          </div>
          <div class="context-item">
            <dt>当前范围</dt>
            <dd>{{ practiceScopeText }}</dd>
          </div>
          <div class="context-item">
            <dt>学习者编码</dt>
            <dd>{{ practiceCheckContext.learnerCode || "demo-learner" }}</dd>
          </div>
          <div class="context-item">
            <dt>来源页面</dt>
            <dd>{{ practiceCheckContext.sourcePage || "home" }}</dd>
          </div>
          <div class="context-item">
            <dt>反馈批次 ID</dt>
            <dd>{{ practiceCheckContext.feedbackBatchId || "未生成" }}</dd>
          </div>
          <div class="context-item">
            <dt>反馈条目数</dt>
            <dd>{{ practiceCheckContext.feedbackItemCount ?? 0 }}</dd>
          </div>
        </dl>
      </article>

      <article class="card surface-panel question-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">固定练习题</p>
            <h3>请完成以下题目后提交</h3>
          </div>
        </div>

        <form v-if="hasSupportedQuestions" class="question-list" @submit.prevent="submitPractice">
          <article
            v-for="(question, questionIndex) in questionSet"
            :key="question.id"
            class="question-item"
          >
            <p class="question-number">第 {{ questionIndex + 1 }} 题</p>
            <p class="question-prompt">{{ question.prompt }}</p>
            <div class="question-options">
              <label
                v-for="(option, optionIndex) in question.options"
                :key="`${question.id}-${optionIndex}`"
                class="option-item"
              >
                <input
                  :name="question.id"
                  :value="optionIndex"
                  :checked="answers[question.id] === optionIndex"
                  type="radio"
                  @change="selectAnswer(question.id, optionIndex)"
                />
                <span>{{ option }}</span>
              </label>
            </div>
          </article>

          <p v-if="submitError" class="submit-message error-message">{{ submitError }}</p>
          <p v-if="submitSuccessMessage" class="submit-message success-message">
            {{ submitSuccessMessage }}
          </p>

          <div class="action-row">
            <button type="submit" class="primary-button" :disabled="submitting">
              {{ submitting ? "提交中..." : "提交练习结果" }}
            </button>
            <button type="button" class="ghost-button" @click="goHome">返回首页</button>
          </div>
        </form>
        <p v-else class="question-placeholder">该知识点练习题待补充</p>
      </article>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, reactive, ref } from "vue";
import { useRouter } from "vue-router";

import { submitLearningFeedback } from "../api/feedback";
import PageLayout from "../components/PageLayout.vue";
import { useNavigationStore } from "../stores/navigationStore";
import {
  getPracticeCheckQuestionSet,
  resolvePracticeQuestionTargetCode,
} from "../utils/practiceCheckQuestions";
import {
  computeWeightedMasteryPercent,
  mapCorrectCountToPracticeMastery,
  resolvePracticeCompletionStatus,
} from "../utils/practiceCheckScoring";

const router = useRouter();
const navigationStore = useNavigationStore();
const answers = reactive({});
const submitError = ref("");
const submitSuccessMessage = ref("");
const submitting = ref(false);

const CORRECT_OPTION_INDEX_BY_TARGET = {
  queue: {
    "queue-1": 1,
    "queue-2": 0,
    "queue-3": 2,
  },
};

const practiceCheckContext = computed(
  () => navigationStore.practiceCheckContext || {},
);

const pageTitle = computed(() =>
  practiceCheckContext.value.targetName
    ? `${practiceCheckContext.value.targetName}练习检验`
    : "练习检验"
);

const pageHeading = computed(() =>
  practiceCheckContext.value.targetName
    ? `请完成 ${practiceCheckContext.value.targetName} 固定练习题`
    : "请完成固定练习题"
);

const practiceScopeText = computed(() => {
  const scopeLabel = practiceCheckContext.value.scopeLabel || "";
  const scopeCode = practiceCheckContext.value.scopeCode || "";

  if (scopeLabel && scopeCode) {
    return `${scopeLabel}（${scopeCode}）`;
  }

  return scopeLabel || scopeCode || "未指定";
});

const questionSet = computed(() =>
  getPracticeCheckQuestionSet(practiceCheckContext.value.targetCode) || [],
);

const hasSupportedQuestions = computed(() => questionSet.value.length > 0);
const resolvedQuestionTargetCode = computed(() =>
  resolvePracticeQuestionTargetCode(practiceCheckContext.value.targetCode),
);

function selectAnswer(questionId, optionIndex) {
  answers[questionId] = optionIndex;
  submitError.value = "";
  submitSuccessMessage.value = "";
}

function hasAnsweredAllQuestions() {
  return questionSet.value.every((question) => Number.isInteger(answers[question.id]));
}

function countCorrectAnswers() {
  const answerKey = CORRECT_OPTION_INDEX_BY_TARGET[resolvedQuestionTargetCode.value] || {};

  return questionSet.value.reduce((count, question) => {
    if (answers[question.id] === answerKey[question.id]) {
      return count + 1;
    }

    return count;
  }, 0);
}

async function submitPractice() {
  submitError.value = "";
  submitSuccessMessage.value = "";

  if (!hasAnsweredAllQuestions()) {
    submitError.value = "请先完成全部题目再提交。";
    return;
  }

  const practiceMasteryPercent = mapCorrectCountToPracticeMastery({
    correctCount: countCorrectAnswers(),
    totalCount: questionSet.value.length,
  });
  const completionStatus = resolvePracticeCompletionStatus({
    correctCount: countCorrectAnswers(),
    totalCount: questionSet.value.length,
  });
  const weightedMasteryPercent = computeWeightedMasteryPercent({
    previousMasteryPercent: practiceCheckContext.value.previousMasteryPercent,
    practiceMasteryPercent,
  });

  submitting.value = true;

  try {
    await submitLearningFeedback({
      learnerCode: practiceCheckContext.value.learnerCode || "demo-learner",
      masteryByCode: {
        [practiceCheckContext.value.targetCode]:
          Number(practiceCheckContext.value.previousMasteryPercent || 0) / 100,
      },
        feedbackItems: [
          {
            code: practiceCheckContext.value.targetCode,
            completionStatus,
            selfRatedMastery: weightedMasteryPercent / 100,
          },
        ],
    });

    submitSuccessMessage.value = "练习结果已提交。";
    await router.push({
      name: "home",
      query: { practiceUpdated: "1" },
    });
  } catch (error) {
    submitError.value = "练习结果未成功写入，请重试。";
    console.error(error);
  } finally {
    submitting.value = false;
  }
}

async function goHome() {
  await router.push({
    name: "home",
  });
}
</script>

<style scoped>
.practice-check-layout {
  display: grid;
  gap: 20px;
}

h2,
h3,
.caption,
.intro-note,
.context-item dt,
.context-item dd {
  margin: 0;
}

.caption,
.intro-note {
  color: #5f6c7a;
  line-height: 1.7;
}

.context-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 14px;
}

.context-item {
  display: grid;
  gap: 6px;
  padding: 14px 16px;
  border-radius: 16px;
  background: rgba(12, 106, 113, 0.06);
}

.context-item dt {
  color: #5f6c7a;
  font-size: 0.92rem;
}

.context-item dd {
  color: #17303d;
  font-size: 1rem;
  font-weight: 700;
  line-height: 1.5;
}

.action-row {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
}

.question-list {
  display: grid;
  gap: 16px;
}

.question-item {
  padding: 16px;
  border-radius: 16px;
  background: rgba(12, 106, 113, 0.05);
}

.question-number {
  margin: 0 0 8px;
  color: #0c5960;
  font-weight: 700;
}

.question-prompt {
  margin: 0 0 10px;
  color: #17303d;
  font-weight: 700;
}

.question-options {
  display: grid;
  gap: 10px;
}

.option-item {
  display: flex;
  gap: 10px;
  align-items: flex-start;
  color: #5f6c7a;
  line-height: 1.6;
}

.question-placeholder {
  margin: 0;
  color: #5f6c7a;
}

.submit-message {
  margin: 0;
  font-weight: 600;
}

.error-message {
  color: #b42318;
}

.success-message {
  color: #0c5960;
}

.primary-button {
  border: 0;
  border-radius: 999px;
  padding: 11px 18px;
  background: #0c5960;
  color: #ffffff;
  font-weight: 700;
  cursor: pointer;
}

.primary-button:disabled {
  opacity: 0.7;
  cursor: wait;
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
  background: rgba(12, 106, 113, 0.14);
}
</style>
