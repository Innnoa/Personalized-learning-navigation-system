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
            :class="getQuestionReportClass(question.id)"
          >
            <p class="question-number">{{ getQuestionNumberLabel(question.id, questionIndex) }}</p>
            <p class="question-prompt">{{ question.prompt }}</p>
            <div class="question-options">
              <label
                v-for="(option, optionIndex) in question.options"
                :key="`${question.id}-${optionIndex}`"
                class="option-item"
                :class="getOptionReportClass(question.id, optionIndex)"
              >
                <input
                  :name="question.id"
                  :value="optionIndex"
                  :checked="answers[question.id] === optionIndex"
                  type="radio"
                  :disabled="showReport || submitting"
                  @change="selectAnswer(question.id, optionIndex)"
                />
                <span>{{ option }}</span>
              </label>
            </div>

            <div v-if="showReport" class="question-report-block">
              <p class="question-report-line">
                你的答案：{{ getSelectedOptionLabel(question.id) }}
              </p>
              <p v-if="!isQuestionCorrect(question.id)" class="question-report-line question-report-line--answer">
                正确答案：{{ getCorrectOptionLabel(question.id) }}
              </p>
              <p
                v-if="!isQuestionCorrect(question.id) && question.explanation"
                class="question-report-line question-report-line--reason"
              >
                理由：{{ question.explanation }}
              </p>
            </div>
          </article>

          <p v-if="submitError" class="submit-message error-message">{{ submitError }}</p>
          <p v-if="submitSuccessMessage" class="submit-message success-message">
            {{ submitSuccessMessage }}
          </p>

          <article v-if="reportSummary" class="report-summary-card">
            <div class="section-headline">
              <div>
                <p class="label page-section-label">批改报告</p>
                <h3>本次练习结果摘要</h3>
              </div>
            </div>

            <dl class="report-summary-grid">
              <div class="report-summary-item">
                <dt>答题结果</dt>
                <dd>答对 {{ reportSummary.correctCount }} / {{ reportSummary.totalCount }} 题</dd>
              </div>
              <div class="report-summary-item">
                <dt>提交前掌握度</dt>
                <dd>{{ reportSummary.previousMasteryPercent }}%</dd>
              </div>
              <div class="report-summary-item">
                <dt>练习掌握度</dt>
                <dd>{{ reportSummary.practiceMasteryPercent }}%</dd>
              </div>
              <div class="report-summary-item">
                <dt>加权后掌握度</dt>
                <dd>{{ reportSummary.weightedMasteryPercent }}%</dd>
              </div>
              <div class="report-summary-item">
                <dt>掌握度变化</dt>
                <dd>{{ formatMasteryDelta(reportSummary.masteryDeltaPercent) }}</dd>
              </div>
            </dl>
          </article>

          <div class="action-row">
            <button v-if="!showReport" type="submit" class="primary-button" :disabled="submitting">
              {{ submitting ? "提交中..." : "提交练习结果" }}
            </button>
            <button
              type="button"
              class="ghost-button"
              @click="goHome"
            >
              {{ showReport ? "返回首页并刷新路径" : "返回首页" }}
            </button>
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
  getPracticeCheckAnswerKey,
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
const reportSummary = ref(null);

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
  getPracticeCheckQuestionSet(practiceCheckContext.value.targetCode, {
    previousMasteryPercent: practiceCheckContext.value.previousMasteryPercent,
  }) || [],
);

const hasSupportedQuestions = computed(() => questionSet.value.length > 0);
const resolvedQuestionTargetCode = computed(() =>
  resolvePracticeQuestionTargetCode(practiceCheckContext.value.targetCode),
);
const answerKey = computed(() =>
  getPracticeCheckAnswerKey(resolvedQuestionTargetCode.value, {
    previousMasteryPercent: practiceCheckContext.value.previousMasteryPercent,
  }),
);
const showReport = computed(() => Boolean(reportSummary.value));

function selectAnswer(questionId, optionIndex) {
  if (showReport.value) {
    return;
  }

  answers[questionId] = optionIndex;
  submitError.value = "";
  submitSuccessMessage.value = "";
}

function hasAnsweredAllQuestions() {
  return questionSet.value.every((question) => Number.isInteger(answers[question.id]));
}

function countCorrectAnswers() {
  return questionSet.value.reduce((count, question) => {
    if (answers[question.id] === answerKey.value[question.id]) {
      return count + 1;
    }

    return count;
  }, 0);
}

function isQuestionCorrect(questionId) {
  return answers[questionId] === answerKey.value[questionId];
}

function getSelectedOptionLabel(questionId) {
  const question = questionSet.value.find((item) => item.id === questionId);
  const selectedOptionIndex = answers[questionId];

  if (!question || !Number.isInteger(selectedOptionIndex)) {
    return "未作答";
  }

  return question.options[selectedOptionIndex] || "未作答";
}

function getCorrectOptionLabel(questionId) {
  const question = questionSet.value.find((item) => item.id === questionId);
  const correctOptionIndex = answerKey.value[questionId];

  if (!question || !Number.isInteger(correctOptionIndex)) {
    return "未配置";
  }

  return question.options[correctOptionIndex] || "未配置";
}

function getQuestionReportClass(questionId) {
  if (!showReport.value) {
    return "";
  }

  return isQuestionCorrect(questionId)
    ? "question-item--correct"
    : "question-item--incorrect";
}

function getQuestionNumberLabel(questionId, questionIndex) {
  const baseLabel = `第 ${questionIndex + 1} 题`;

  if (!showReport.value) {
    return baseLabel;
  }

  return `${baseLabel} · ${isQuestionCorrect(questionId) ? "回答正确" : "回答错误"}`;
}

function getOptionReportClass(questionId, optionIndex) {
  if (!showReport.value) {
    return "";
  }

  const classNames = [];
  if (answers[questionId] === optionIndex) {
    classNames.push("option-item--selected");
  }
  if (answerKey.value[questionId] === optionIndex) {
    classNames.push("option-item--correct");
  }

  return classNames.join(" ");
}

function formatMasteryDelta(deltaPercent) {
  const roundedDelta = Math.round(Number(deltaPercent) || 0);

  if (roundedDelta > 0) {
    return `+${roundedDelta}%`;
  }

  return `${roundedDelta}%`;
}

async function submitPractice() {
  submitError.value = "";
  submitSuccessMessage.value = "";
  reportSummary.value = null;

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
  const correctCount = countCorrectAnswers();

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
    reportSummary.value = {
      correctCount,
      totalCount: questionSet.value.length,
      previousMasteryPercent: Math.round(
        Number(practiceCheckContext.value.previousMasteryPercent) || 0,
      ),
      practiceMasteryPercent,
      weightedMasteryPercent,
      masteryDeltaPercent:
        weightedMasteryPercent -
        Math.round(Number(practiceCheckContext.value.previousMasteryPercent) || 0),
    };
  } catch (error) {
    submitError.value = "练习结果未成功写入，请重试。";
    console.error(error);
  } finally {
    submitting.value = false;
  }
}

async function goHome() {
  const query = {};
  if (practiceCheckContext.value.scopeCode && practiceCheckContext.value.scopeCode !== "root") {
    query.scope = practiceCheckContext.value.scopeCode;
  }
  if (practiceCheckContext.value.targetCode) {
    query.target = practiceCheckContext.value.targetCode;
  }

  await router.push({
    name: "home",
    query,
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

.question-item--correct {
  border: 1px solid rgba(23, 107, 57, 0.22);
  background: rgba(23, 107, 57, 0.08);
}

.question-item--incorrect {
  border: 1px solid rgba(180, 35, 24, 0.2);
  background: rgba(180, 35, 24, 0.06);
}

.question-number {
  margin: 0 0 8px;
  color: #0c5960;
  font-weight: 700;
}

.question-result-label {
  color: #32404a;
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

.option-item--selected {
  font-weight: 700;
}

.option-item--correct {
  color: #176b39;
}

.question-report-block {
  display: grid;
  gap: 6px;
  margin-top: 12px;
  padding-top: 12px;
  border-top: 1px dashed rgba(12, 106, 113, 0.18);
}

.question-report-line {
  margin: 0;
  color: #32404a;
  line-height: 1.6;
}

.question-report-line--answer,
.question-report-line--reason {
  color: #17303d;
}

.report-summary-card {
  display: grid;
  gap: 14px;
  padding: 18px;
  border-radius: 16px;
  background: rgba(12, 106, 113, 0.08);
}

.report-summary-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
  gap: 12px;
}

.report-summary-item {
  display: grid;
  gap: 6px;
}

.report-summary-item dt,
.report-summary-item dd {
  margin: 0;
}

.report-summary-item dt {
  color: #5f6c7a;
  font-size: 0.92rem;
}

.report-summary-item dd {
  color: #17303d;
  font-weight: 700;
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
