<template>
  <section class="planner-fragment">
    <article class="card planner-core-card">
      <div class="card-head">
        <div>
          <p class="label">路径规划</p>
          <h2>个性化学习导航</h2>
        </div>
        <p class="caption">输入目标、时间和掌握度，生成当前轮次学习路径</p>
      </div>

      <div v-if="optionsLoading" class="state state--loading">
        正在读取知识点选项...
      </div>

      <div v-else-if="optionsError" class="state state--error">
        {{ optionsError }}
      </div>

      <template v-else>
        <div class="preset-row">
          <button type="button" class="ghost-button" @click="applyPreset('beginner')">
            零基础
          </button>
          <button
            type="button"
            class="ghost-button"
            @click="applyPreset('linear-ready')"
          >
            线性结构已掌握
          </button>
          <button type="button" class="ghost-button" @click="applyPreset('graph-sprint')">
            图算法冲刺
          </button>
        </div>

        <form class="planner-form" @submit.prevent="submitPlan">
          <label class="field">
            <span>学习目标</span>
            <select v-model="selectedTargetCode">
              <option v-for="node in knowledgePoints" :key="node.code" :value="node.code">
                {{ node.label }}
              </option>
            </select>
          </label>

          <label class="field">
            <span>可用时间</span>
            <select v-model.number="availableMinutes">
              <option v-for="minutes in minuteOptions" :key="minutes" :value="minutes">
                {{ minutes }} 分钟
              </option>
            </select>
          </label>

          <div class="field field--wide">
            <div class="field-head">
              <span>掌握度细项</span>
              <button type="button" class="ghost-button" @click="toggleEditor">
                {{ showMasteryEditor ? "收起" : "展开" }}
              </button>
            </div>
            <p class="field-hint">
              默认使用预设掌握度。展开后可对每个知识点单独调节 0% 到 100%。
            </p>
          </div>

          <div v-if="showMasteryEditor" class="mastery-grid">
            <label
              v-for="node in knowledgePoints"
              :key="node.code"
              class="mastery-item"
            >
              <span>{{ node.label }}</span>
              <div class="slider-row">
                <input
                  v-model.number="masteryPercentByCode[node.code]"
                  type="range"
                  min="0"
                  max="100"
                  step="5"
                />
                <strong>{{ masteryPercentByCode[node.code] ?? 0 }}%</strong>
              </div>
            </label>
          </div>

          <button class="submit-button" :disabled="planning">
            {{ planning ? "正在生成路径..." : "生成学习路径" }}
          </button>
        </form>

        <div v-if="planError" class="state state--error state--result">
          {{ planError }}
        </div>

        <template v-if="planResult">
          <dl class="summary-grid">
            <div>
              <dt>目标</dt>
              <dd>{{ selectedTargetLabel }}</dd>
            </div>
            <div>
              <dt>本轮安排</dt>
              <dd>{{ planResult.summary.scheduledCount }} 个节点</dd>
            </div>
            <div>
              <dt>延后节点</dt>
              <dd>{{ planResult.summary.deferredCount }} 个</dd>
            </div>
            <div>
              <dt>已掌握</dt>
              <dd>{{ planResult.summary.masteredCount }} 个</dd>
            </div>
            <div>
              <dt>本轮预计</dt>
              <dd>{{ planResult.summary.scheduledMinutes }} 分钟</dd>
            </div>
            <div>
              <dt>全部所需</dt>
              <dd>{{ planResult.summary.totalRequiredMinutes }} 分钟</dd>
            </div>
          </dl>

          <p class="result-message" :class="reachableClass">
            {{ reachableMessage }}
          </p>

          <article v-if="overallExplanation" class="overall-explanation-card">
            <div class="section-headline">
              <div>
                <p class="label">路径总说明</p>
                <h3>本次路径为什么这样安排</h3>
              </div>
              <p class="caption">
                该说明直接聚合规划结果与推荐依据，方便答辩时先讲“整条路径的逻辑”。
              </p>
            </div>

            <ul
              v-if="(overallExplanation.labels || []).length > 0"
              class="overall-explanation-tags"
            >
              <li
                v-for="label in overallExplanation.labels || []"
                :key="`overall-${label}`"
                class="overall-explanation-tag"
              >
                {{ label }}
              </li>
            </ul>

            <p class="overall-explanation-summary">
              {{ overallExplanation.summary }}
            </p>

            <ul
              v-if="(overallExplanation.bullets || []).length > 0"
              class="overall-explanation-list"
            >
              <li
                v-for="(bullet, index) in overallExplanation.bullets || []"
                :key="`overall-bullet-${index}`"
              >
                {{ bullet }}
              </li>
            </ul>
          </article>

          <div class="result-action-row">
            <button
              type="button"
              class="ghost-button"
              data-testid="export-path-button"
              @click="exportCurrentPlan"
            >
              导出本次学习路径
            </button>
            <p class="result-action-note">
              导出为文本文件，包含目标、时间预算、当前路径、推荐理由摘要与资源建议。
            </p>
          </div>

          <div v-if="exportError" class="state state--error state--result">
            {{ exportError }}
          </div>

          <div class="result-grid">
            <article class="result-card">
            <h3>本轮推荐学习</h3>
            <ol class="path-list">
              <li v-for="item in scheduledItems" :key="item.code" class="path-item">
                <div class="path-item-head">
                  <strong>{{ item.name }}</strong>
                  <div class="path-item-actions">
                    <span>{{ item.estimatedMinutes }} 分钟</span>
                    <button
                      type="button"
                      class="detail-toggle"
                      @click="emit('focus-node', item.code)"
                    >
                      前往学习图谱
                    </button>
                    <button
                      v-if="hasDetailLearningSection(item.code)"
                      :data-testid="`detail-learning-${item.code}`"
                      type="button"
                      class="detail-toggle"
                      @click="openDetailLearningSection(item.code)"
                    >
                      细化学习
                    </button>
                    <button
                      v-if="hasResourceSection(item.code)"
                      type="button"
                      class="detail-toggle"
                      @click="openResourceRecommendationSection(item.code)"
                    >
                      查看推荐资源
                    </button>
                    <button
                      type="button"
                      class="detail-toggle"
                      @click="toggleExplanation(item.code)"
                    >
                      {{ isExplanationExpanded(item.code) ? "收起理由" : "展开理由" }}
                    </button>
                  </div>
                </div>
                <p class="path-item-meta">
                  第{{ item.chapterNo }}章 · 掌握度 {{ item.masteryPercent }}%
                </p>
                <ul
                  v-if="getExplanationLabels(item).length > 0"
                  class="explanation-tags"
                >
                  <li
                    v-for="label in getExplanationLabels(item)"
                    :key="`${item.code}-${label}`"
                    class="explanation-tag"
                  >
                    {{ label }}
                  </li>
                </ul>
                <p class="path-item-reason">
                  {{ getExplanationSummary(item) }}
                </p>
                <div
                  v-if="isExplanationExpanded(item.code)"
                  class="explanation-detail"
                >
                  <ul class="explanation-list">
                    <li
                      v-for="(bullet, index) in getExplanationBullets(item)"
                      :key="`${item.code}-bullet-${index}`"
                    >
                      {{ bullet }}
                    </li>
                  </ul>
                  <dl class="explanation-metrics">
                    <div>
                      <dt>相关度</dt>
                      <dd>{{ Math.round(item.reasonTrace.relevanceScore * 100) }}%</dd>
                    </div>
                    <div>
                      <dt>重要度</dt>
                      <dd>{{ Math.round(item.reasonTrace.importanceScore * 100) }}%</dd>
                    </div>
                    <div>
                      <dt>时间占比</dt>
                      <dd>{{ Math.round(item.reasonTrace.timeCostPenalty * 100) }}%</dd>
                    </div>
                  </dl>
                </div>
              </li>
            </ol>
            <p v-if="scheduledItems.length === 0" class="empty-tip">
              当前时间预算内没有新的学习节点，可尝试提高时间预算或降低掌握度预设。
            </p>
          </article>

          <article class="result-card">
            <h3>下一轮建议</h3>
            <ul class="path-list plain-list">
              <li v-for="item in deferredItems" :key="item.code" class="path-item">
                <div class="path-item-head">
                  <strong>{{ item.name }}</strong>
                  <div class="path-item-actions">
                    <span>{{ item.estimatedMinutes }} 分钟</span>
                    <button
                      type="button"
                      class="detail-toggle"
                      @click="emit('focus-node', item.code)"
                    >
                      前往学习图谱
                    </button>
                    <button
                      v-if="hasResourceSection(item.code)"
                      type="button"
                      class="detail-toggle"
                      @click="openResourceRecommendationSection(item.code)"
                    >
                      查看推荐资源
                    </button>
                    <button
                      type="button"
                      class="detail-toggle"
                      @click="toggleExplanation(item.code)"
                    >
                      {{ isExplanationExpanded(item.code) ? "收起理由" : "展开理由" }}
                    </button>
                  </div>
                </div>
                <p class="path-item-meta">
                  第{{ item.chapterNo }}章 · 目标相关度
                  {{ Math.round(item.reasonTrace.relevanceScore * 100) }}%
                </p>
                <ul
                  v-if="getExplanationLabels(item).length > 0"
                  class="explanation-tags"
                >
                  <li
                    v-for="label in getExplanationLabels(item)"
                    :key="`${item.code}-${label}`"
                    class="explanation-tag"
                  >
                    {{ label }}
                  </li>
                </ul>
                <p class="path-item-reason">
                  {{ getExplanationSummary(item) }}
                </p>
                <div
                  v-if="isExplanationExpanded(item.code)"
                  class="explanation-detail"
                >
                  <ul class="explanation-list">
                    <li
                      v-for="(bullet, index) in getExplanationBullets(item)"
                      :key="`${item.code}-bullet-${index}`"
                    >
                      {{ bullet }}
                    </li>
                  </ul>
                  <dl class="explanation-metrics">
                    <div>
                      <dt>相关度</dt>
                      <dd>{{ Math.round(item.reasonTrace.relevanceScore * 100) }}%</dd>
                    </div>
                    <div>
                      <dt>重要度</dt>
                      <dd>{{ Math.round(item.reasonTrace.importanceScore * 100) }}%</dd>
                    </div>
                    <div>
                      <dt>时间占比</dt>
                      <dd>{{ Math.round(item.reasonTrace.timeCostPenalty * 100) }}%</dd>
                    </div>
                  </dl>
                </div>
              </li>
            </ul>
            <p v-if="deferredItems.length === 0" class="empty-tip">
              当前时间预算已覆盖本次目标链路。
            </p>
          </article>
          </div>
        </template>
      </template>
    </article>

    <template v-if="planResult">
      <section class="planner-extension-stack planner-extension-card">
        <article class="card planner-extension-intro">
          <div class="section-headline">
            <div>
              <p class="label">扩展区</p>
          <h3>练习检验与路径变化</h3>
            </div>
            <p class="caption">
          首屏聚焦“图谱 + 路径”，资源直接从上方路径项进入；下方主要用于进入练习检验，并观察路径如何变化。
            </p>
          </div>
          <ul class="planner-extension-chips">
            <li class="planner-extension-chip">练习检验</li>
            <li class="planner-extension-chip">操作摘要</li>
            <li class="planner-extension-chip">路径变化</li>
          </ul>
        </article>

        <div class="planner-extension-columns">
          <div class="planner-extension-main">
            <section class="card planner-section-card feedback-panel">
              <div class="section-headline">
                <div>
                  <p class="label">练习检验</p>
                  <h3>通过练习检验客观更新掌握度</h3>
                </div>
                <div class="feedback-head-side">
                  <p class="caption">不再支持主观填写掌握度；请直接进入练习检验，由系统根据答题结果自动更新掌握度并重新规划。</p>
                  <button
                    type="button"
                    class="ghost-button"
                    :disabled="rollingBack || !canRollbackFeedback"
                    @click="rollbackLatestAdjustment"
                  >
                    {{
                      rollingBack
                        ? "正在回退..."
                        : canRollbackFeedback
                          ? "撤销最近一次反馈"
                          : "暂无可回退记录"
                    }}
                  </button>
                </div>
              </div>

              <div v-if="scheduledItems.length === 0" class="empty-tip feedback-empty">
                当前没有待学习节点，因此无需调整路径。
              </div>

              <div v-else class="feedback-practice-entry-list">
                <article
                  v-for="item in scheduledItems"
                  :key="item.code"
                  class="feedback-item"
                >
                  <div class="feedback-item-head">
                    <div>
                      <strong>{{ item.name }}</strong>
                      <p class="path-item-meta">
                        第{{ item.chapterNo }}章 · 当前掌握度 {{ item.masteryPercent }}%
                      </p>
                    </div>
                    <span class="feedback-badge">{{ item.estimatedMinutes }} 分钟</span>
                  </div>
                  <p class="path-item-reason">
                    当前节点将通过练习检验结果自动判断掌握度，并据此更新推荐路径。
                  </p>
                  <div class="feedback-item-actions">
                    <button
                      :data-testid="`practice-check-${item.code}`"
                      type="button"
                      class="ghost-button"
                      @click="goToPracticeCheck(item)"
                    >
                      进入练习检验
                    </button>
                  </div>
                </article>
              </div>

              <div v-if="rollbackError" class="state state--error state--result">
                {{ rollbackError }}
              </div>
            </section>
          </div>

          <aside class="planner-extension-side">
            <article
              v-if="rollbackSummary"
              class="card planner-section-card planner-section-card--summary"
            >
              <div class="section-headline">
                <div>
                  <p class="label">操作摘要</p>
                  <h3>最近一次回退</h3>
                </div>
                <p class="caption">用于快速查看最近一次回退对掌握度的影响。</p>
              </div>

              <template v-if="rollbackSummary">
                <dl class="summary-grid feedback-summary">
                  <div>
                    <dt>回退节点</dt>
                    <dd>{{ rollbackSummary.feedbackItemCount }} 个</dd>
                  </div>
                  <div>
                    <dt>撤销完成</dt>
                    <dd>{{ rollbackSummary.completedCount }} 个</dd>
                  </div>
                  <div>
                    <dt>撤销部分完成</dt>
                    <dd>{{ rollbackSummary.partialCount }} 个</dd>
                  </div>
                  <div>
                    <dt>撤销受阻</dt>
                    <dd>{{ rollbackSummary.blockedCount }} 个</dd>
                  </div>
                </dl>

                <article class="result-card result-card--wide">
                  <h3>最近一次回退说明</h3>
                  <ul class="path-list plain-list">
                    <li
                      v-for="item in rollbackItems"
                      :key="`${item.code}-${item.recordedAt}`"
                      class="path-item"
                    >
                      <div class="path-item-head">
                        <strong>{{ resolveKnowledgePointLabel(item.code) }}</strong>
                        <span>
                          {{ Math.round(item.rolledBackFromMastery * 100) }}% ->
                          {{ Math.round(item.restoredMastery * 100) }}%
                        </span>
                      </div>
                      <p class="path-item-meta">
                        已撤销反馈：{{ adjustmentStatusLabelMap[item.completionStatus] }}
                      </p>
                      <p class="path-item-reason">
                        已将该节点恢复到上一次提交前的掌握度水平，可继续重新判断后再提交。
                      </p>
                    </li>
                  </ul>
                </article>
              </template>
            </article>
          </aside>
        </div>

        <article
          v-if="pathComparison"
          class="card planner-section-card planner-section-card--comparison"
        >
          <div class="section-headline">
            <div>
              <p class="label">路径变化</p>
              <h3>{{ comparisonChangeTitle }}</h3>
            </div>
            <p class="caption">用于观察最近一次练习结果写回或回退后，本轮学习安排如何变化。</p>
          </div>

          <dl class="summary-grid comparison-summary">
            <div>
              <dt>{{ comparisonBeforeCountLabel }}</dt>
              <dd>{{ pathComparison.summary.beforeScheduledCount }} 个</dd>
            </div>
            <div>
              <dt>{{ comparisonAfterCountLabel }}</dt>
              <dd>{{ pathComparison.summary.afterScheduledCount }} 个</dd>
            </div>
            <div>
              <dt>转为已掌握</dt>
              <dd>{{ pathComparison.summary.promotedToMasteredCount }} 个</dd>
            </div>
            <div>
              <dt>提前到本轮</dt>
              <dd>{{ pathComparison.summary.movedIntoCurrentRoundCount }} 个</dd>
            </div>
            <div>
              <dt>顺延到下轮</dt>
              <dd>{{ pathComparison.summary.deferredToNextRoundCount }} 个</dd>
            </div>
            <div>
              <dt>顺序调整</dt>
              <dd>{{ pathComparison.summary.reorderedCount }} 个</dd>
            </div>
          </dl>

          <div class="result-grid comparison-grid">
            <article class="result-card">
              <h3>{{ comparisonBeforeTitle }}</h3>
              <ol class="path-list">
                <li
                  v-for="item in pathComparison.beforeScheduledItems"
                  :key="`before-${item.code}`"
                  class="path-item"
                >
                  <div class="path-item-head">
                    <strong>{{ item.name }}</strong>
                    <span>{{ item.estimatedMinutes }} 分钟</span>
                  </div>
                  <p class="path-item-meta">
                    第{{ item.chapterNo }}章 · 掌握度 {{ item.masteryPercent }}%
                  </p>
                </li>
              </ol>
              <p
                v-if="pathComparison.beforeScheduledItems.length === 0"
                class="empty-tip"
              >
                {{ comparisonBeforeEmptyTip }}
              </p>
            </article>

            <article class="result-card">
              <h3>{{ comparisonAfterTitle }}</h3>
              <ol class="path-list">
                <li
                  v-for="item in pathComparison.afterScheduledItems"
                  :key="`after-${item.code}`"
                  class="path-item"
                >
                  <div class="path-item-head">
                    <strong>{{ item.name }}</strong>
                    <span>{{ item.estimatedMinutes }} 分钟</span>
                  </div>
                  <p class="path-item-meta">
                    第{{ item.chapterNo }}章 · 掌握度 {{ item.masteryPercent }}%
                  </p>
                </li>
              </ol>
              <p
                v-if="pathComparison.afterScheduledItems.length === 0"
                class="empty-tip"
              >
                {{ comparisonAfterEmptyTip }}
              </p>
            </article>
          </div>

          <article class="result-card result-card--wide">
            <h3>关键变化明细</h3>
            <ul
              v-if="pathComparison.changeItems.length > 0"
              class="path-list plain-list"
            >
              <li
                v-for="item in pathComparison.changeItems"
                :key="`change-${item.code}`"
                class="path-item"
              >
                <div class="path-item-head">
                  <strong>{{ item.name }}</strong>
                  <span
                    class="change-badge"
                    :class="changeBadgeClass(item.changeType)"
                  >
                    {{ item.changeLabel }}
                  </span>
                </div>
                <p class="path-item-meta">
                  状态：{{ item.beforeStatusLabel }} -> {{ item.afterStatusLabel }}
                </p>
                <p class="path-item-reason">
                  {{ item.changeDescription }}
                </p>
              </li>
            </ul>
            <p v-else class="empty-tip">
              {{ comparisonNoChangeTip }}
            </p>
          </article>
        </article>
      </section>
    </template>

  </section>
</template>

<script setup>
import { computed, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import {
  rollbackLatestLearningFeedback,
} from "../api/feedback";
import { generateLearningPath } from "../api/path";
import { useNavigationStore } from "../stores/navigationStore";
import {
  normalizeFeedbackDraft,
} from "../utils/feedbackQuickPreset";
import { downloadLearningPathExport } from "../utils/learningPathExport";

const props = defineProps({
  learnerCode: {
    type: String,
    default: "",
  },
  externalTargetCode: {
    type: String,
    default: "",
  },
  initialMasteryByCode: {
    type: Object,
    default: () => ({}),
  },
  profileLoading: {
    type: Boolean,
    default: false,
  },
  feedbackRecordCount: {
    type: Number,
    default: 0,
  },
});

const emit = defineEmits([
  "feedback-saved",
  "focus-node",
]);
const route = useRoute();
const router = useRouter();
const navigationStore = useNavigationStore();

const minuteOptions = [60, 90, 120, 150, 180, 240];
const adjustmentStatusLabelMap = {
  completed: "已完成",
  partial: "部分完成",
  blocked: "学习受阻",
};
const planStatusLabelMap = {
  mastered: "已掌握",
  scheduled: "本轮学习",
  deferred: "顺延到下轮",
  "not-in-path": "未进入路径",
};
const optionsLoading = ref(true);
const optionsError = ref("");
const knowledgePoints = ref([]);
const selectedTargetCode = ref("");
const availableMinutes = ref(120);
const showMasteryEditor = ref(false);
const masteryPercentByCode = ref({});
const planning = ref(false);
const planError = ref("");
const exportError = ref("");
const planResult = ref(null);
const rollingBack = ref(false);
const rollbackError = ref("");
const rollbackSummary = ref(null);
const rollbackItems = ref([]);
const pathComparison = ref(null);
const pathComparisonMode = ref("adjust");
const optionsReady = ref(false);
const initialPlanInitialized = ref(false);
const expandedExplanationByCode = ref({});
const externalTargetCode = computed(() => String(props.externalTargetCode || ""));
const routeTargetCode = computed(() => String(route.query.target || ""));
const routeScopeCode = computed(() => String(route.query.scope || "root"));
const plannerDraftStorageKey = computed(
  () => `plns-draft-${props.learnerCode || "demo"}-${routeScopeCode.value}`,
);
const plannerPlanStorageKey = computed(
  () => `plns-plan-${props.learnerCode || "demo"}-${routeScopeCode.value}`,
);

const selectedTargetLabel = computed(() => {
  const target = knowledgePoints.value.find(
    (node) => node.code === selectedTargetCode.value,
  );
  return target?.label || "未选择";
});

const scheduledItems = computed(() =>
  (planResult.value?.path || []).filter((item) => item.status === "scheduled"),
);

const deferredItems = computed(() =>
  (planResult.value?.path || []).filter((item) => item.status === "deferred"),
);

const resourceRecommendations = computed(
  () => planResult.value?.resourceRecommendations || [],
);
const overallExplanation = computed(
  () => planResult.value?.overallExplanation || null,
);
const knowledgePointMetaByCode = computed(
  () =>
    new Map(
      knowledgePoints.value.map((node) => [
        node.code,
        {
          code: node.code,
          label: node.label,
          chapterNo: node.chapterNo || 0,
          detailScopeCode: node.detailScopeCode || "",
        },
      ]),
    ),
);

const canRollbackFeedback = computed(() => props.feedbackRecordCount > 0);

const reachableClass = computed(() =>
  planResult.value?.summary?.targetReachableWithinBudget
    ? "result-message--ok"
    : "result-message--warn",
);

const reachableMessage = computed(() => {
  if (!planResult.value) {
    return "";
  }

  if (planResult.value.summary.targetReachableWithinBudget) {
    return "当前时间预算足以覆盖目标链路，本轮路径可直接作为演示结果。";
  }

  return "当前时间预算不足以完成目标链路，系统已把超出的节点顺延到下一轮。";
});

const comparisonBeforeCountLabel = computed(() =>
  pathComparisonMode.value === "rollback" ? "回退前本轮" : "调整前本轮",
);

const comparisonAfterCountLabel = computed(() =>
  pathComparisonMode.value === "rollback" ? "回退后本轮" : "调整后本轮",
);

const comparisonBeforeTitle = computed(() =>
  pathComparisonMode.value === "rollback" ? "回退前本轮安排" : "调整前本轮安排",
);

const comparisonAfterTitle = computed(() =>
  pathComparisonMode.value === "rollback" ? "回退后本轮安排" : "调整后本轮安排",
);

const comparisonBeforeEmptyTip = computed(() =>
  pathComparisonMode.value === "rollback"
    ? "回退前当前轮次没有待学习节点。"
    : "回退前当前轮次没有待学习节点。",
);

const comparisonAfterEmptyTip = computed(() =>
  pathComparisonMode.value === "rollback"
    ? "回退后当前轮次暂时没有新的待学习节点。"
    : "回退后当前轮次暂时没有新的待学习节点。",
);

const comparisonChangeTitle = computed(() =>
  "回退后路径变化",
);

const comparisonNoChangeTip = computed(() =>
  pathComparisonMode.value === "rollback"
    ? "本次回退主要恢复了掌握度，路径状态没有出现明显变化。"
    : "本次回退主要恢复了掌握度，路径状态没有出现明显变化。",
);

function setAllMastery(percent) {
  masteryPercentByCode.value = Object.fromEntries(
    knowledgePoints.value.map((node) => [node.code, percent]),
  );
}

function applyPreset(preset) {
  setAllMastery(0);

  if (preset === "linear-ready") {
    [
      "ds-intro",
      "algorithm-analysis",
      "linear-list",
      "sequence-list",
      "linked-list",
      "stack",
      "queue",
    ].forEach((code) => {
      masteryPercentByCode.value[code] = 80;
    });
  }

  if (preset === "graph-sprint") {
    [
      "ds-intro",
      "algorithm-analysis",
      "linear-list",
      "sequence-list",
      "linked-list",
      "stack",
      "queue",
      "string",
      "tree-basic",
      "binary-tree-traversal",
    ].forEach((code) => {
      masteryPercentByCode.value[code] = 85;
    });
    masteryPercentByCode.value["graph-basic"] = 55;
    masteryPercentByCode.value["topological-sort"] = 15;
  }
}

function toggleEditor() {
  showMasteryEditor.value = !showMasteryEditor.value;
}

function applyMasterySnapshot(snapshot) {
  masteryPercentByCode.value = Object.fromEntries(
    knowledgePoints.value.map((node) => [
      node.code,
      Math.round(Number(snapshot?.[node.code] ?? 0) * 100),
    ]),
  );
}

function buildMasteryPayload() {
  return Object.fromEntries(
    Object.entries(masteryPercentByCode.value).map(([code, percent]) => [
      code,
      Number(percent || 0) / 100,
    ]),
  );
}

function resolveKnowledgePointLabel(code) {
  return knowledgePoints.value.find((node) => node.code === code)?.label || code;
}

function clonePlanSnapshot(plan) {
  if (!plan) {
    return null;
  }

  return JSON.parse(JSON.stringify(plan));
}

function formatPlanStatus(status) {
  return planStatusLabelMap[status] || status;
}

function buildPathComparison(beforePlan, afterPlan) {
  if (!beforePlan || !afterPlan) {
    return null;
  }

  const beforePath = beforePlan.path || [];
  const afterPath = afterPlan.path || [];
  const beforeScheduledItems = beforePath.filter((item) => item.status === "scheduled");
  const afterScheduledItems = afterPath.filter((item) => item.status === "scheduled");
  const beforeByCode = new Map(beforePath.map((item) => [item.code, item]));
  const afterByCode = new Map(afterPath.map((item) => [item.code, item]));
  const beforeScheduledOrder = Object.fromEntries(
    beforeScheduledItems.map((item, index) => [item.code, index + 1]),
  );
  const afterScheduledOrder = Object.fromEntries(
    afterScheduledItems.map((item, index) => [item.code, index + 1]),
  );
  const allCodes = [
    ...new Set([
      ...beforePath.map((item) => item.code),
      ...afterPath.map((item) => item.code),
    ]),
  ];

  const changeItems = [];
  let promotedToMasteredCount = 0;
  let movedIntoCurrentRoundCount = 0;
  let deferredToNextRoundCount = 0;
  let reorderedCount = 0;

  for (const code of allCodes) {
    const beforeItem = beforeByCode.get(code) || null;
    const afterItem = afterByCode.get(code) || null;
    const beforeStatus = beforeItem?.status || "not-in-path";
    const afterStatus = afterItem?.status || "not-in-path";
    const beforeOrder = beforeScheduledOrder[code] || null;
    const afterOrder = afterScheduledOrder[code] || null;

    let changeType = "";
    let changeLabel = "";
    let changeDescription = "";

    if (beforeStatus !== "mastered" && afterStatus === "mastered") {
      promotedToMasteredCount += 1;
      changeType = "mastered";
      changeLabel = "转为已掌握";
      changeDescription = `掌握度由 ${beforeItem?.masteryPercent ?? 0}% 提升到 ${afterItem?.masteryPercent ?? 0}% ，后续路径可转向新的后继节点。`;
    } else if (beforeStatus !== "scheduled" && afterStatus === "scheduled") {
      movedIntoCurrentRoundCount += 1;
      changeType = "advanced";
      changeLabel = "提前到本轮";
      changeDescription = `该节点原本处于“${formatPlanStatus(beforeStatus)}”，现在进入本轮安排，当前顺序为第 ${afterOrder} 位。`;
    } else if (beforeStatus === "scheduled" && afterStatus !== "scheduled") {
      deferredToNextRoundCount += 1;
      changeType = "deferred";
      changeLabel = "移出本轮";
      changeDescription = `该节点由本轮第 ${beforeOrder} 位调整为“${formatPlanStatus(afterStatus)}”，本轮会优先处理更紧急的节点。`;
    } else if (
      beforeStatus === "scheduled" &&
      afterStatus === "scheduled" &&
      beforeOrder !== afterOrder
    ) {
      reorderedCount += 1;
      changeType = "reordered";
      changeLabel = "顺序调整";
      changeDescription = `该节点在本轮中的顺序由第 ${beforeOrder} 位调整为第 ${afterOrder} 位。`;
    }

    if (!changeType) {
      continue;
    }

    changeItems.push({
      code,
      name: afterItem?.name || beforeItem?.name || resolveKnowledgePointLabel(code),
      changeType,
      changeLabel,
      beforeStatusLabel: formatPlanStatus(beforeStatus),
      afterStatusLabel: formatPlanStatus(afterStatus),
      changeDescription,
    });
  }

  const changePriority = {
    mastered: 1,
    advanced: 2,
    deferred: 3,
    reordered: 4,
  };
  changeItems.sort(
    (left, right) =>
      (changePriority[left.changeType] || 99) -
      (changePriority[right.changeType] || 99),
  );

  return {
    summary: {
      beforeScheduledCount: beforeScheduledItems.length,
      afterScheduledCount: afterScheduledItems.length,
      promotedToMasteredCount,
      movedIntoCurrentRoundCount,
      deferredToNextRoundCount,
      reorderedCount,
    },
    beforeScheduledItems,
    afterScheduledItems,
    changeItems,
  };
}

function getExplanationSummary(item) {
  if (item?.explanation?.summary) {
    return item.explanation.summary;
  }

  return item?.reasonTrace?.triggerReasons?.join(" ") || "暂无推荐理由说明。";
}

function getExplanationLabels(item) {
  return item?.explanation?.labels || [];
}

function getExplanationBullets(item) {
  return item?.explanation?.bullets || item?.reasonTrace?.triggerReasons || [];
}

function findResourceSectionByCode(code) {
  return resourceRecommendations.value.find((section) => section.code === code) || null;
}

function hasResourceSection(code) {
  return Boolean(findResourceSectionByCode(code));
}

function buildDetailLearningSections(currentPlan = planResult.value) {
  if (!currentPlan) {
    return [];
  }

  const seenCodes = new Set();

  return (currentPlan.path || [])
    .map((item) => {
      if (!item?.code || seenCodes.has(item.code)) {
        return null;
      }

      const metadata = knowledgePointMetaByCode.value.get(item.code);
      if (!metadata?.detailScopeCode) {
        return null;
      }

      seenCodes.add(item.code);
      return {
        code: item.code,
        name: item.name,
        scopeCode: metadata.detailScopeCode,
        scopeLabel: item.name,
        chapterNo: item.chapterNo || metadata.chapterNo || 0,
        masteryPercent: item.masteryPercent,
        estimatedMinutes: item.estimatedMinutes,
        status: item.status,
      };
    })
    .filter(Boolean);
}

function hasDetailLearningSection(code) {
  return buildDetailLearningSections().some((item) => item.code === code);
}

function syncDetailLearningContext(selectedCode = "") {
  const detailLearningSections = buildDetailLearningSections();
  if (detailLearningSections.length === 0) {
    navigationStore.clearDetailLearningContext();
    return null;
  }

  const selectedSection =
    detailLearningSections.find((item) => item.code === selectedCode) ||
    detailLearningSections.find(
      (item) =>
        item.scopeCode === navigationStore.detailLearningSummary?.selectedScopeCode,
    ) ||
    detailLearningSections[0];

  navigationStore.setDetailLearningContext({
    learnerCode: props.learnerCode,
    detailLearningSections,
    selectedScopeCode: selectedSection?.scopeCode || "",
    sourceTargetLabel: selectedTargetLabel.value,
    sourcePage: "home",
  });

  return selectedSection || null;
}

function syncResourceRecommendationContext(currentPlan = planResult.value) {
  if (!currentPlan) {
    navigationStore.clearResourceRecommendationContext("main");
    return;
  }

  navigationStore.setResourceRecommendationContext({
    learnerCode: props.learnerCode,
    resourceRecommendationSections: currentPlan.resourceRecommendations || [],
    contextMode: "main",
    sourcePage: "home",
    targetLabel: selectedTargetLabel.value,
    availableMinutes: currentPlan.summary?.availableMinutes || availableMinutes.value,
    scheduledCount: currentPlan.summary?.scheduledCount || 0,
    deferredCount: currentPlan.summary?.deferredCount || 0,
    scopeCode: "root",
    scopeLabel: "课程主线",
    parentNodeCode: "",
    parentNodeLabel: "",
  });
}

function openResourceRecommendationSection(code) {
  const section = findResourceSectionByCode(code);
  if (!section) {
    return;
  }

  syncResourceRecommendationContext(planResult.value);
  router.push({
    name: "resource-recommendation",
    params: { code: section.code },
    query: {
      level: "main",
    },
  });
}

function openDetailLearningSection(code) {
  const selectedSection = syncDetailLearningContext(code);
  if (!selectedSection) {
    return;
  }

  router.push({
    name: "home",
    query: {
      scope: selectedSection.scopeCode,
      target: code,
    },
  });
}

function toggleExplanation(code) {
  expandedExplanationByCode.value = {
    ...expandedExplanationByCode.value,
    [code]: !expandedExplanationByCode.value[code],
  };
}

function isExplanationExpanded(code) {
  return Boolean(expandedExplanationByCode.value[code]);
}

function changeBadgeClass(changeType) {
  return {
    "change-badge--mastered": changeType === "mastered",
    "change-badge--advanced": changeType === "advanced",
    "change-badge--deferred": changeType === "deferred",
    "change-badge--reordered": changeType === "reordered",
  };
}

function clearOperationOutputs() {
  rollbackError.value = "";
  rollbackSummary.value = null;
  rollbackItems.value = [];
  pathComparison.value = null;
}

function goToPracticeCheck(targetItem = null) {
  const resolvedTargetItem =
    targetItem ||
    scheduledItems.value.find((item) => item.code === selectedTargetCode.value) ||
    scheduledItems.value[0] ||
    null;
  if (!resolvedTargetItem) {
    return;
  }

  navigationStore.setPracticeCheckContext({
    learnerCode: props.learnerCode,
    sourcePage: "home",
    targetCode: resolvedTargetItem.code,
    targetName: resolvedTargetItem.name,
    scopeCode: "root",
    scopeLabel: "课程主图",
    previousMasteryPercent: Math.round(Number(resolvedTargetItem.masteryPercent) || 0),
    completionStatus: "completed",
    notes: "",
  });
  router.push({ name: "practice-check" });
}

function exportCurrentPlan() {
  if (!planResult.value) {
    return;
  }

  exportError.value = "";

  try {
    downloadLearningPathExport({
      learnerCode: props.learnerCode,
      targetCode: selectedTargetCode.value,
      targetLabel: selectedTargetLabel.value,
      availableMinutes: availableMinutes.value,
      planResult: planResult.value,
    });
  } catch (error) {
    exportError.value =
      error?.message || "学习路径导出失败，请稍后重试或检查浏览器下载权限。";
    console.error(error);
  }
}

function resolveRequestedTargetCode(preferredCode = "") {
  const candidateCode = preferredCode || externalTargetCode.value || routeTargetCode.value;
  if (!candidateCode) {
    return "";
  }

  const exists = knowledgePoints.value.some(
    (node) => node.code === candidateCode,
  );
  return exists ? candidateCode : "";
}

function applyRequestedTargetCode(preferredCode = "") {
  const requestedTargetCode = resolveRequestedTargetCode(preferredCode);
  if (!requestedTargetCode) {
    return false;
  }

  selectedTargetCode.value = requestedTargetCode;
  return true;
}

function buildPlannerDraftSnapshot() {
  return {
    selectedTargetCode: selectedTargetCode.value,
    availableMinutes: availableMinutes.value,
    showMasteryEditor: showMasteryEditor.value,
    masteryPercentByCode: { ...masteryPercentByCode.value },
  };
}

function restorePlannerDraftSnapshot(snapshot = {}) {
  if (!snapshot || typeof snapshot !== "object") {
    return;
  }

  if (
    snapshot.selectedTargetCode &&
    knowledgePoints.value.some((node) => node.code === snapshot.selectedTargetCode)
  ) {
    selectedTargetCode.value = snapshot.selectedTargetCode;
  }

  if (minuteOptions.includes(Number(snapshot.availableMinutes))) {
    availableMinutes.value = Number(snapshot.availableMinutes);
  }

  showMasteryEditor.value = Boolean(snapshot.showMasteryEditor);

  if (snapshot.masteryPercentByCode && typeof snapshot.masteryPercentByCode === "object") {
    masteryPercentByCode.value = Object.fromEntries(
      knowledgePoints.value.map((node) => [
        node.code,
        Math.round(Number(snapshot.masteryPercentByCode[node.code] ?? masteryPercentByCode.value[node.code] ?? 0)),
      ]),
    );
  }
}

function persistPlannerDraftSnapshot() {
  if (!props.learnerCode) {
    return;
  }

  try {
    window.sessionStorage.setItem(
      plannerDraftStorageKey.value,
      JSON.stringify(buildPlannerDraftSnapshot()),
    );
  } catch {}
}

async function loadKnowledgePoints() {
  optionsLoading.value = true;
  optionsError.value = "";

  try {
    const payload = await fetchKnowledgeGraph();
    knowledgePoints.value = payload.nodes;
    applyRequestedTargetCode();
  } catch (error) {
    optionsError.value =
      "未能读取知识点选项。请先启动后端并确认数据库已初始化。";
    console.error(error);
  } finally {
    optionsLoading.value = false;
  }
}

function syncPlannerContext() {
  navigationStore.setPlannerContext({
    scopeCode: routeScopeCode.value,
    targetCode: selectedTargetCode.value,
  });
}

async function initializePlannerFromProfile() {
  if (!optionsReady.value || initialPlanInitialized.value || props.profileLoading) {
    return;
  }

  if (Object.keys(props.initialMasteryByCode || {}).length > 0) {
    applyMasterySnapshot(props.initialMasteryByCode);
  } else {
    applyPreset("beginner");
  }

  applyRequestedTargetCode();
  initialPlanInitialized.value = true;
}

async function submitPlan() {
  planning.value = true;
  planError.value = "";
  exportError.value = "";
  clearOperationOutputs();
  expandedExplanationByCode.value = {};

  try {
    planResult.value = await generateLearningPath({
      learnerCode: props.learnerCode,
      targetCodes: [selectedTargetCode.value],
      availableMinutes: availableMinutes.value,
      masteryByCode: buildMasteryPayload(),
    });
    // Persist plan result
    if (props.learnerCode) {
      try {
        window.sessionStorage.setItem(
          plannerPlanStorageKey.value,
          JSON.stringify({
            planResult: planResult.value,
            availableMinutes: availableMinutes.value,
            selectedTargetCode: selectedTargetCode.value,
          }),
        );
      } catch {}
    }
    syncResourceRecommendationContext(planResult.value);
    syncDetailLearningContext();
  } catch (error) {
    planError.value =
      error?.response?.data?.detail ||
      "学习路径生成失败。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    planning.value = false;
  }
}

async function rollbackLatestAdjustment() {
  if (!canRollbackFeedback.value || !planResult.value) {
    return;
  }

  rollingBack.value = true;
  exportError.value = "";
  clearOperationOutputs();
  const beforePlanSnapshot = clonePlanSnapshot(planResult.value);

  try {
    const payload = await rollbackLatestLearningFeedback({
      learnerCode: props.learnerCode,
    });

    const updatedMasteryByCode = payload.masteryByCode || {};
    applyMasterySnapshot(updatedMasteryByCode);
    rollbackSummary.value = payload.rollbackSummary || null;
    rollbackItems.value = payload.rollbackItems || [];
    expandedExplanationByCode.value = {};
    emit("feedback-saved", payload);

    const nextPlanResult = await generateLearningPath({
      learnerCode: props.learnerCode,
      targetCodes: [selectedTargetCode.value],
      availableMinutes: availableMinutes.value,
      masteryByCode: updatedMasteryByCode,
    });
    planResult.value = nextPlanResult;
    syncResourceRecommendationContext(nextPlanResult);
    syncDetailLearningContext();
    pathComparisonMode.value = "rollback";
    pathComparison.value = buildPathComparison(beforePlanSnapshot, nextPlanResult);
  } catch (error) {
    rollbackError.value =
      error?.response?.data?.detail ||
      "学习反馈回退失败。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    rollingBack.value = false;
  }
}

watch(
  () => props.profileLoading,
  async (loading) => {
    if (!loading) {
      await initializePlannerFromProfile();
    }
  },
);

watch(
  () => props.initialMasteryByCode,
  (snapshot) => {
    if (!optionsReady.value || Object.keys(snapshot || {}).length === 0) {
      return;
    }

    applyMasterySnapshot(snapshot);
  },
  { deep: true },
);

watch(
  routeTargetCode,
  async (targetCode) => {
    if (!targetCode || !optionsReady.value) {
      return;
    }

    applyRequestedTargetCode();
  },
);

watch(
  externalTargetCode,
  async (targetCode) => {
    if (!targetCode || !optionsReady.value) {
      return;
    }

    applyRequestedTargetCode(targetCode);
  },
);

onMounted(async () => {
  await loadKnowledgePoints();
  optionsReady.value = true;

  if (props.learnerCode) {
    try {
      const draftRaw = window.sessionStorage.getItem(plannerDraftStorageKey.value);
      if (draftRaw) {
        restorePlannerDraftSnapshot(JSON.parse(draftRaw));
      }
    } catch {}
  }

  // Restore saved plan
  if (props.learnerCode) {
    try {
      const raw = window.sessionStorage.getItem(plannerPlanStorageKey.value);
      if (raw) {
        const cached = JSON.parse(raw);
        if (cached.planResult) {
          planResult.value = cached.planResult;
          availableMinutes.value = cached.availableMinutes || 60;
          selectedTargetCode.value = cached.selectedTargetCode || selectedTargetCode.value;
          syncResourceRecommendationContext(cached.planResult);
          syncDetailLearningContext();
        }
      }
    } catch {}
  }

  if (!optionsError.value) {
    await initializePlannerFromProfile();
  }
});

// Persist target selection
watch(selectedTargetCode, (val) => {
  if (val && props.learnerCode) {
    try {
      persistPlannerDraftSnapshot();
    } catch {}
  }
  if (val) {
    syncPlannerContext();
  }
});

watch(
  [availableMinutes, showMasteryEditor, masteryPercentByCode],
  () => {
    persistPlannerDraftSnapshot();
  },
  {
    deep: true,
  },
);

watch(
  routeScopeCode,
  () => {
    if (selectedTargetCode.value) {
      syncPlannerContext();
    }
  },
);
</script>

<style scoped>
.planner-fragment {
  display: contents;
}

.card {
  background: var(--panel-surface);
  border: var(--panel-border);
  border-radius: 24px;
  padding: 22px;
  box-shadow: var(--panel-shadow);
  contain: paint;
}

.planner-core-card {
  grid-column: 1 / -1;
  align-self: start;
}

.planner-extension-card {
  grid-column: 1 / -1;
}

.planner-extension-stack {
  display: grid;
  gap: 18px;
  align-items: start;
}

.planner-extension-intro,
.planner-section-card--comparison {
  grid-column: 1 / -1;
}

.planner-extension-columns {
  display: grid;
  grid-template-columns: minmax(0, 1.6fr) minmax(280px, 0.9fr);
  gap: 18px;
  align-items: start;
}

.planner-extension-main,
.planner-extension-side {
  display: grid;
  gap: 18px;
  align-content: start;
  min-width: 0;
}

.planner-extension-side {
  position: sticky;
  top: 24px;
  align-self: start;
  max-height: calc(100vh - 24px);
  overflow: auto;
  padding-right: 4px;
  scrollbar-gutter: stable;
}

.planner-extension-intro {
  position: relative;
  overflow: hidden;
  background:
    radial-gradient(circle at top right, rgba(12, 106, 113, 0.12), transparent 28%),
    linear-gradient(135deg, rgba(247, 250, 249, 0.96), rgba(255, 255, 255, 0.92));
}

.planner-extension-intro::after {
  content: "";
  position: absolute;
  inset: auto -32px -44px auto;
  width: 180px;
  height: 180px;
  border-radius: 50%;
  background: rgba(12, 106, 113, 0.06);
  pointer-events: none;
}

.planner-section-card {
  display: grid;
  gap: 18px;
  min-height: 100%;
}

.section-headline {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: start;
}

.planner-extension-intro .caption {
  max-width: 520px;
  line-height: 1.6;
}

.planner-extension-chips {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin: 18px 0 0;
  padding: 0;
  list-style: none;
}

.planner-extension-chip {
  display: inline-flex;
  align-items: center;
  padding: 7px 12px;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.76);
  border: 1px solid rgba(12, 106, 113, 0.12);
  color: #0c5960;
  font-size: 0.84rem;
  font-weight: 700;
}

.card-head {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: baseline;
}

.label {
  margin: 0 0 6px;
  font-size: 0.84rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

h2,
h3 {
  margin: 0;
}

.caption {
  margin: 0;
  color: #66727d;
  font-size: 0.9rem;
}

.preset-row {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-top: 18px;
}

.ghost-button,
.submit-button {
  border: none;
  border-radius: 999px;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease;
}

.ghost-button {
  padding: 9px 14px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-weight: 600;
}

.submit-button {
  padding: 12px 18px;
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  font-weight: 700;
}

.ghost-button:hover,
.submit-button:hover {
  transform: translateY(-1px);
}

.submit-button:disabled {
  cursor: wait;
  opacity: 0.7;
}

.planner-form {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px;
  margin-top: 18px;
}

.field {
  display: grid;
  gap: 8px;
}

.field span {
  font-weight: 700;
  color: #24323b;
}

.field select {
  border: 1px solid rgba(22, 32, 42, 0.12);
  border-radius: 14px;
  padding: 12px 14px;
  background: rgba(255, 255, 255, 0.92);
}

.field--wide {
  grid-column: 1 / -1;
}

.field-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
}

.field-hint {
  margin: 0;
  color: #66727d;
  font-size: 0.92rem;
}

.mastery-grid {
  grid-column: 1 / -1;
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
}

.mastery-item {
  display: grid;
  gap: 8px;
  padding: 14px;
  border-radius: 18px;
  background: rgba(244, 248, 247, 0.86);
}

.mastery-item span {
  font-weight: 600;
  color: #24323b;
}

.slider-row {
  display: grid;
  grid-template-columns: minmax(0, 1fr) auto;
  gap: 12px;
  align-items: center;
}

.summary-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
  margin-top: 22px;
}

.planner-section-card--summary .summary-grid {
  grid-template-columns: repeat(2, minmax(0, 1fr));
  margin-top: 0;
}

.summary-grid div {
  padding: 14px;
  border-radius: 18px;
  background: rgba(244, 248, 247, 0.86);
}

dt {
  font-size: 0.82rem;
  color: #6d7781;
}

dd {
  margin: 4px 0 0;
  font-weight: 700;
  color: #24323b;
}

.result-message {
  margin: 18px 0 0;
  padding: 14px 16px;
  border-radius: 16px;
  font-weight: 600;
}

.result-message--ok {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.result-message--warn {
  background: rgba(186, 93, 23, 0.12);
  color: #8a4d10;
}

.overall-explanation-card {
  margin-top: 18px;
  border-radius: 22px;
  background:
    linear-gradient(135deg, rgba(12, 106, 113, 0.08), rgba(255, 255, 255, 0.9)),
    rgba(247, 250, 249, 0.9);
  padding: 18px;
  contain: layout paint;
}

.overall-explanation-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 16px 0 0;
  padding: 0;
  list-style: none;
}

.overall-explanation-tag {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.8);
  border: 1px solid rgba(12, 106, 113, 0.12);
  color: #0c5960;
  font-size: 0.82rem;
  font-weight: 700;
}

.overall-explanation-summary {
  margin: 14px 0 0;
  color: #2d3d46;
  line-height: 1.7;
  font-weight: 600;
}

.overall-explanation-list {
  display: grid;
  gap: 8px;
  margin: 14px 0 0;
  padding-left: 18px;
  color: #44515c;
  line-height: 1.55;
}

.result-action-row {
  display: flex;
  flex-wrap: wrap;
  gap: 12px 16px;
  align-items: center;
  margin-top: 16px;
}

.result-action-note {
  margin: 0;
  color: #66727d;
  font-size: 0.92rem;
  line-height: 1.5;
}

.result-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px;
  margin-top: 18px;
}

.result-card {
  border-radius: 22px;
  background: rgba(247, 250, 249, 0.86);
  padding: 18px;
  contain: layout paint;
}

.result-card--wide {
  margin-top: 18px;
}

.path-list {
  display: grid;
  gap: 12px;
  margin: 16px 0 0;
  padding-left: 20px;
}

.plain-list {
  padding-left: 0;
  list-style: none;
}

.path-item {
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.9);
  padding: 14px;
  contain: layout paint;
}

.path-item-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
}

.path-item-actions {
  display: flex;
  align-items: center;
  gap: 10px;
  flex-wrap: wrap;
}

.path-item-meta,
.path-item-reason,
.empty-tip {
  margin: 8px 0 0;
  color: #44515c;
}

.explanation-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 10px 0 0;
  padding: 0;
  list-style: none;
}

.explanation-tag {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-size: 0.82rem;
  font-weight: 700;
}

.path-item-reason {
  font-size: 0.94rem;
  line-height: 1.5;
}

.detail-toggle {
  border: none;
  border-radius: 999px;
  padding: 6px 12px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-weight: 700;
  cursor: pointer;
}

.explanation-detail {
  margin-top: 12px;
  padding-top: 12px;
  border-top: 1px solid rgba(22, 32, 42, 0.08);
}

.explanation-list {
  display: grid;
  gap: 8px;
  margin: 0;
  padding-left: 18px;
  color: #44515c;
}

.explanation-metrics {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 10px;
  margin-top: 12px;
}

.explanation-metrics div {
  padding: 10px 12px;
  border-radius: 14px;
  background: rgba(244, 248, 247, 0.86);
}

.state {
  margin-top: 18px;
  border-radius: 18px;
  padding: 18px;
  font-weight: 600;
}

.state--loading {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.state--error {
  background: rgba(176, 47, 47, 0.1);
  color: #8d2323;
}

.state--result {
  margin-top: 22px;
}

.feedback-panel {
  padding-top: 22px;
}

.feedback-head,
.feedback-item-head {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  align-items: start;
}

.feedback-head-side {
  display: grid;
  gap: 10px;
  justify-items: end;
}

.feedback-form {
  display: grid;
  gap: 14px;
  margin-top: 18px;
}

.feedback-item {
  border-radius: 20px;
  background: rgba(247, 250, 249, 0.86);
  padding: 16px;
  contain: layout paint;
}

@supports (content-visibility: auto) {
  .planner-extension-intro,
  .planner-section-card--comparison,
  .overall-explanation-card,
  .result-card,
  .feedback-item,
  .path-item {
    content-visibility: auto;
  }

  .planner-extension-intro,
  .planner-section-card--comparison,
  .overall-explanation-card,
  .result-card {
    contain-intrinsic-size: 260px;
  }

  .feedback-item,
  .path-item {
  contain-intrinsic-size: 140px;
  }
}

.feedback-quick-actions {
  display: grid;
  gap: 10px;
  margin-top: 14px;
  padding: 12px 14px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.92);
}

.feedback-quick-actions__label {
  color: #53616c;
  font-size: 0.84rem;
  font-weight: 700;
}

.feedback-quick-actions__buttons {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.feedback-preset-button {
  border: 1px solid rgba(22, 32, 42, 0.1);
  background: rgba(244, 248, 247, 0.96);
  color: #214861;
  font-weight: 700;
  padding: 8px 12px;
}

.feedback-preset-button--completed {
  background: rgba(28, 113, 58, 0.08);
  border-color: rgba(28, 113, 58, 0.16);
  color: #14542d;
}

.feedback-preset-button--partial {
  background: rgba(12, 106, 113, 0.08);
  border-color: rgba(12, 106, 113, 0.14);
  color: #0c5960;
}

.feedback-preset-button--blocked {
  background: rgba(186, 93, 23, 0.08);
  border-color: rgba(186, 93, 23, 0.16);
  color: #8a4d10;
}

.feedback-fields {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 14px;
  margin-top: 14px;
}

.feedback-badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-size: 0.9rem;
  font-weight: 700;
}

.feedback-summary {
  margin-top: 18px;
}

.comparison-summary {
  margin-top: 18px;
}

.comparison-grid {
  margin-top: 18px;
}

.feedback-empty {
  margin-top: 18px;
}

.change-badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  font-size: 0.84rem;
  font-weight: 700;
}

.change-badge--mastered {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.change-badge--advanced {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.change-badge--deferred {
  background: rgba(186, 93, 23, 0.12);
  color: #8a4d10;
}

.change-badge--reordered {
  background: rgba(55, 82, 129, 0.12);
  color: #27456f;
}

@media (max-width: 980px) {
  .planner-core-card,
  .planner-extension-card {
    grid-column: 1 / -1;
  }

  .planner-extension-columns,
  .planner-extension-stack {
    grid-template-columns: 1fr;
  }

  .planner-extension-side {
    position: static;
    top: auto;
    max-height: none;
    overflow: visible;
    padding-right: 0;
  }
  .planner-form,
  .mastery-grid,
  .summary-grid,
  .result-grid,
  .feedback-fields,
  .explanation-metrics {
    grid-template-columns: 1fr;
  }

  .card-head,
  .section-headline,
  .field-head,
  .result-action-row,
  .path-item-head,
  .feedback-head,
  .feedback-item-head {
    flex-direction: column;
    align-items: start;
  }

  .feedback-head-side {
    justify-items: start;
  }

  .feedback-quick-actions__buttons {
    width: 100%;
  }
}
</style>
