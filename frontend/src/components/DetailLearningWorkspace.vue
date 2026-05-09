<template>
  <section class="detail-workspace">
    <article class="card detail-graph-card">
      <div class="card-head">
        <div>
          <p class="label">细化图谱</p>
          <h2>{{ scopeTitle }}</h2>
        </div>
        <p class="caption">
          当前页面只展示二级范围图谱。节点颜色表示当前掌握度，图谱固定展示，点击节点可切换当前学习目标。
        </p>
      </div>

      <div v-if="graphLoading" class="state state--loading">
        正在读取当前二级图谱...
      </div>

      <div v-else-if="graphError" class="state state--error">
        {{ graphError }}
      </div>

      <template v-else-if="currentScopeGraph.nodes.length > 0">
        <div class="scope-toolbar">
          <div class="breadcrumb-row">
            <span
              v-for="item in breadcrumbs"
              :key="`detail-breadcrumb-${item.scopeCode}`"
              class="breadcrumb-button"
              :class="{
                'breadcrumb-button--active': item.scopeCode === currentScopeCode,
              }"
            >
              {{ item.label }}
            </span>
          </div>

          <div class="scope-note">
            <p class="scope-label">当前范围</p>
            <strong>{{ currentScopeName }}</strong>
            <span>{{ resolveScopeLevelLabel(currentScopeLevel) }}</span>
          </div>
        </div>

        <dl class="summary-grid">
          <div>
            <dt>当前层节点</dt>
            <dd>{{ currentScopeGraph.nodes.length }} 个</dd>
          </div>
          <div>
            <dt>当前层路径</dt>
            <dd>{{ currentScopeGraph.edges.length }} 条</dd>
          </div>
          <div>
            <dt>已学习节点</dt>
            <dd>{{ learnedNodeCount }} 个</dd>
          </div>
          <div>
            <dt>未学习节点</dt>
            <dd>{{ unlearnedNodeCount }} 个</dd>
          </div>
        </dl>

        <p class="rule-note">
          当前细化图谱使用与主学习图谱一致的浏览方式：点击节点查看详情，并同步为下方细化路径规划目标；本页不再继续下钻三级节点。
        </p>

        <div class="legend-row">
          <span class="legend-chip legend-chip--progress-low">浅粉：0% - 35%</span>
          <span class="legend-chip legend-chip--progress-mid">中粉：36% - 70%</span>
          <span class="legend-chip legend-chip--progress-high">深粉：71% - 100%</span>
          <span class="legend-chip legend-chip--selected">描边强调：当前选中节点</span>
          <span class="legend-chip legend-chip--context">描边增强：上下关联节点</span>
        </div>

        <div class="graph-layout">
          <div ref="graphRef" class="graph-canvas"></div>
        </div>

        <aside class="detail-panel">
          <template v-if="selectedNode">
            <div class="detail-head">
              <div>
                <p class="label label--detail">节点详情</p>
                <h3>{{ selectedNode.label }}</h3>
              </div>
              <span
                class="mastery-badge"
                :class="{
                  'mastery-badge--mastered': selectedNode.masteryState === 'mastered',
                  'mastery-badge--progress': selectedNode.masteryState === 'in-progress',
                  'mastery-badge--idle': selectedNode.masteryState === 'not-started',
                }"
              >
                {{ resolveMasteryStateLabel(selectedNode.masteryState) }}
              </span>
            </div>

            <dl class="detail-grid">
              <div>
                <dt>章节</dt>
                <dd>第{{ selectedNode.chapterNo }}章</dd>
              </div>
              <div>
                <dt>掌握度</dt>
                <dd>{{ selectedNode.masteryPercent }}%</dd>
              </div>
              <div>
                <dt>图谱层级</dt>
                <dd>{{ resolveScopeLevelLabel(currentScopeLevel) }}</dd>
              </div>
              <div>
                <dt>所在范围</dt>
                <dd>{{ currentScopeName }}</dd>
              </div>
              <div>
                <dt>类型</dt>
                <dd>{{ resolveNodeTypeLabel(selectedNode.nodeType) }}</dd>
              </div>
              <div>
                <dt>预计学习</dt>
                <dd>{{ selectedNode.estimatedMinutes }} 分钟</dd>
              </div>
            </dl>

            <p class="detail-description">
              {{ selectedNode.description || "当前节点暂未补充说明。" }}
            </p>

            <div class="relation-grid">
              <section class="relation-section">
                <div class="relation-head">
                  <h4>直接前置节点</h4>
                  <span>{{ directPrerequisites.length }} 个</span>
                </div>
                <ul v-if="directPrerequisites.length > 0" class="relation-list">
                  <li
                    v-for="item in directPrerequisites"
                    :key="`${selectedNode.code}-detail-pre-${item.code}`"
                    class="relation-item"
                  >
                    <div class="relation-main">
                      <strong>{{ item.label }}</strong>
                      <span
                        class="relation-state"
                        :class="`relation-state--${item.masteryState}`"
                      >
                        {{ resolveMasteryStateLabel(item.masteryState) }}
                      </span>
                    </div>
                    <p>掌握度 {{ item.masteryPercent }}%</p>
                  </li>
                </ul>
                <p v-else class="empty-tip">该节点当前没有直接前置节点。</p>
              </section>

              <section class="relation-section">
                <div class="relation-head">
                  <h4>直接后继节点</h4>
                  <span>{{ directDependents.length }} 个</span>
                </div>
                <ul v-if="directDependents.length > 0" class="relation-list">
                  <li
                    v-for="item in directDependents"
                    :key="`${selectedNode.code}-detail-next-${item.code}`"
                    class="relation-item"
                  >
                    <div class="relation-main">
                      <strong>{{ item.label }}</strong>
                      <span
                        class="relation-state"
                        :class="`relation-state--${item.masteryState}`"
                      >
                        {{ resolveMasteryStateLabel(item.masteryState) }}
                      </span>
                    </div>
                    <p>掌握度 {{ item.masteryPercent }}%</p>
                  </li>
                </ul>
                <p v-else class="empty-tip">该节点当前没有直接后继节点。</p>
              </section>
            </div>
          </template>

          <div v-else class="empty-tip empty-tip--panel">
            点击上方任一节点后，可在这里查看它的掌握度、上下关联结构，以及当前细化学习范围中的位置。
          </div>
        </aside>
      </template>

      <div v-else class="empty-tip empty-tip--standalone">
        当前范围暂时没有可展示的细化图谱节点。
      </div>
    </article>

    <article class="card detail-planner-card">
      <div class="card-head">
        <div>
          <p class="label">细化路径规划</p>
          <h2>{{ scopePlannerTitle }}</h2>
        </div>
        <p class="caption">
          路径规划只在当前二级范围内进行，用来回答“这个一级节点内部应该先学什么”。
        </p>
      </div>

      <div v-if="graphLoading" class="state state--loading">
        正在准备当前范围路径规划...
      </div>

      <div v-else-if="graphError" class="state state--error">
        {{ graphError }}
      </div>

      <template v-else-if="currentScopeGraph.nodes.length > 0">
        <form class="detail-path-form" @submit.prevent="submitDetailPlan">
          <label class="field">
            <span>当前范围目标节点</span>
            <select v-model="detailTargetCode">
              <option
                v-for="node in detailTargetOptions"
                :key="`detail-target-${node.code}`"
                :value="node.code"
              >
                {{ node.label }}
              </option>
            </select>
          </label>

          <label class="field">
            <span>本范围可用时间</span>
            <select v-model.number="detailAvailableMinutes">
              <option
                v-for="minutes in detailMinuteOptions"
                :key="`detail-minute-${minutes}`"
                :value="minutes"
              >
                {{ minutes }} 分钟
              </option>
            </select>
          </label>

          <div class="field field--wide">
            <div class="field-head">
              <span>二级节点掌握度</span>
              <button
                type="button"
                class="ghost-button"
                @click="toggleDetailMasteryEditor"
              >
                {{ showDetailMasteryEditor ? "收起" : "展开" }}
              </button>
            </div>
            <p class="field-hint">
              默认会读取当前学习者画像中的掌握度。展开后可在当前范围内临时调整并重新生成局部路径。
            </p>
          </div>

          <div v-if="showDetailMasteryEditor" class="detail-mastery-grid">
            <label
              v-for="node in detailTargetOptions"
              :key="`detail-mastery-${node.code}`"
              class="detail-mastery-item"
            >
              <span>{{ node.label }}</span>
              <div class="slider-row">
                <input
                  v-model.number="detailMasteryPercentByCode[node.code]"
                  type="range"
                  min="0"
                  max="100"
                  step="5"
                />
                <strong>{{ detailMasteryPercentByCode[node.code] ?? 0 }}%</strong>
              </div>
            </label>
          </div>

          <button class="submit-button" :disabled="detailPlanning">
            {{ detailPlanning ? "正在生成本范围学习路径..." : "生成本范围学习路径" }}
          </button>
        </form>

        <div v-if="detailPlanError" class="state state--error state--result">
          {{ detailPlanError }}
        </div>

        <template v-if="detailPlanResult">
          <dl class="detail-summary-grid">
            <div>
              <dt>当前目标</dt>
              <dd>{{ detailTargetLabel }}</dd>
            </div>
            <div>
              <dt>本轮安排</dt>
              <dd>{{ detailPlanResult.summary.scheduledCount }} 个</dd>
            </div>
            <div>
              <dt>下一轮建议</dt>
              <dd>{{ detailPlanResult.summary.deferredCount }} 个</dd>
            </div>
            <div>
              <dt>已掌握</dt>
              <dd>{{ detailPlanResult.summary.masteredCount }} 个</dd>
            </div>
            <div>
              <dt>本轮预计</dt>
              <dd>{{ detailPlanResult.summary.scheduledMinutes }} 分钟</dd>
            </div>
            <div>
              <dt>全部所需</dt>
              <dd>{{ detailPlanResult.summary.totalRequiredMinutes }} 分钟</dd>
            </div>
          </dl>

          <p class="detail-result-message" :class="detailReachableClass">
            {{ detailReachableMessage }}
          </p>

          <div class="detail-result-action-row">
            <button
              type="button"
              class="ghost-button"
              data-testid="detail-export-path-button"
              @click="exportDetailPlan"
            >
              导出当前细化路径
            </button>
            <p class="detail-result-action-note">
              导出为文本文件，包含当前二级范围、目标节点、本轮安排、下一轮建议与资源建议。
            </p>
          </div>

          <div v-if="detailExportError" class="state state--error state--result">
            {{ detailExportError }}
          </div>

          <div class="detail-path-result-grid">
            <article class="detail-path-card">
              <h3>本轮推荐学习</h3>
              <ol class="detail-path-list">
                <li
                  v-for="item in detailScheduledItems"
                  :key="`detail-scheduled-${item.code}`"
                  class="detail-path-item"
                >
                  <div class="detail-path-item-head">
                    <strong>{{ item.name }}</strong>
                    <div class="detail-path-item-actions">
                      <span>{{ item.estimatedMinutes }} 分钟</span>
                      <button
                        v-if="hasDetailResourceSection(item.code)"
                        type="button"
                        :data-testid="`detail-resource-${item.code}`"
                        class="ghost-button detail-path-action-button"
                        @click="openDetailResourceRecommendationSection(item.code)"
                      >
                        查看推荐资源
                      </button>
                    </div>
                  </div>
                  <p class="detail-path-item-meta">
                    难度 {{ item.difficultyLevel }}/5 · 掌握度 {{ item.masteryPercent }}%
                  </p>
                  <p class="detail-path-item-reason">
                    {{ getExplanationSummary(item) }}
                  </p>
                </li>
              </ol>
              <p v-if="detailScheduledItems.length === 0" class="empty-tip">
                当前时间预算内没有新的细化节点需要安排。
              </p>
            </article>

            <article class="detail-path-card">
              <h3>下一轮建议</h3>
              <ul class="detail-path-list detail-path-list--plain">
                <li
                  v-for="item in detailDeferredItems"
                  :key="`detail-deferred-${item.code}`"
                  class="detail-path-item"
                >
                  <div class="detail-path-item-head">
                    <strong>{{ item.name }}</strong>
                    <div class="detail-path-item-actions">
                      <span>{{ item.estimatedMinutes }} 分钟</span>
                      <button
                        v-if="hasDetailResourceSection(item.code)"
                        type="button"
                        :data-testid="`detail-resource-${item.code}`"
                        class="ghost-button detail-path-action-button"
                        @click="openDetailResourceRecommendationSection(item.code)"
                      >
                        查看推荐资源
                      </button>
                    </div>
                  </div>
                  <p class="detail-path-item-meta">
                    难度 {{ item.difficultyLevel }}/5 · 目标相关度
                    {{ Math.round(item.reasonTrace.relevanceScore * 100) }}%
                  </p>
                  <p class="detail-path-item-reason">
                    {{ getExplanationSummary(item) }}
                  </p>
                </li>
              </ul>
              <p v-if="detailDeferredItems.length === 0" class="empty-tip">
                当前时间预算已覆盖本范围目标链路。
              </p>
            </article>
          </div>

          <article class="detail-feedback-panel">
            <div class="section-headline">
              <div>
                <p class="label">学习反馈</p>
                <h3>根据本范围学习情况重新规划</h3>
              </div>
              <p class="caption">
                只对“本轮推荐学习”中的细化节点提交反馈，系统会即时重算当前二级路径。
              </p>
            </div>

            <div v-if="detailScheduledItems.length === 0" class="empty-tip">
              当前没有待学习的细化节点，因此无需提交局部反馈。
            </div>

            <form
              v-else
              class="detail-feedback-form"
              @submit.prevent="submitDetailAdjustment"
            >
              <article
                v-for="item in detailScheduledItems"
                :key="`detail-feedback-${item.code}`"
                class="detail-feedback-item"
              >
                <div class="detail-path-item-head">
                  <div>
                    <strong>{{ item.name }}</strong>
                    <p class="detail-path-item-meta">
                      当前掌握度 {{ item.masteryPercent }}% · 预计 {{ item.estimatedMinutes }} 分钟
                    </p>
                  </div>
                </div>

                <div class="detail-feedback-quick-actions">
                  <span class="detail-feedback-quick-actions__label">快捷录入</span>
                  <div class="detail-feedback-quick-actions__buttons">
                    <button
                      v-for="preset in feedbackQuickPresets"
                      :key="`${item.code}-${preset.key}`"
                      type="button"
                      class="detail-feedback-preset-button"
                      :class="`detail-feedback-preset-button--${preset.tone}`"
                      @click="applyDetailFeedbackQuickPreset(item, preset.key)"
                    >
                      {{ preset.label }}
                    </button>
                  </div>
                </div>

                <div class="detail-feedback-fields">
                  <label class="field">
                    <span>完成情况</span>
                    <select
                      v-model="detailFeedbackDraftByCode[item.code].completionStatus"
                      @change="handleDetailFeedbackStatusChange(item)"
                    >
                      <option value="completed">已完成</option>
                      <option value="partial">部分完成</option>
                      <option value="blocked">学习受阻</option>
                    </select>
                  </label>

                  <label class="field">
                    <span>学习后掌握度</span>
                    <div class="slider-row">
                      <input
                        :key="`${item.code}-${detailFeedbackDraftByCode[item.code].completionStatus}`"
                        v-model.number="detailFeedbackDraftByCode[item.code].selfRatedMastery"
                        type="range"
                        :min="getDetailFeedbackMasteryRangeForCode(item.code).min"
                        :max="getDetailFeedbackMasteryRangeForCode(item.code).max"
                        step="5"
                      />
                      <strong>{{ detailFeedbackDraftByCode[item.code].selfRatedMastery }}%</strong>
                    </div>
                    <p class="field-hint field-hint--feedback">
                      当前可调区间：{{ getDetailFeedbackMasteryRangeLabel(item.code) }}
                    </p>
                  </label>
                </div>
              </article>

              <button class="submit-button" :disabled="detailAdjusting">
                {{ detailAdjusting ? "正在根据反馈调整..." : "提交本范围反馈并重算" }}
              </button>
            </form>

            <div
              v-if="detailAdjustmentError"
              class="state state--error state--result"
            >
              {{ detailAdjustmentError }}
            </div>
          </article>

          <article v-if="detailAdjustmentSummary" class="detail-adjustment-panel">
            <div class="section-headline">
              <div>
                <p class="label">操作摘要</p>
                <h3>最近一次局部反馈结果</h3>
              </div>
              <p class="caption">用于快速查看本范围学习反馈对掌握度的影响。</p>
            </div>

            <dl class="detail-summary-grid detail-summary-grid--compact">
              <div>
                <dt>反馈节点</dt>
                <dd>{{ detailAdjustmentSummary.feedbackItemCount }} 个</dd>
              </div>
              <div>
                <dt>已完成</dt>
                <dd>{{ detailAdjustmentSummary.completedCount }} 个</dd>
              </div>
              <div>
                <dt>部分完成</dt>
                <dd>{{ detailAdjustmentSummary.partialCount }} 个</dd>
              </div>
              <div>
                <dt>学习受阻</dt>
                <dd>{{ detailAdjustmentSummary.blockedCount }} 个</dd>
              </div>
            </dl>

            <ul class="detail-adjustment-list">
              <li
                v-for="item in detailAdjustmentItems"
                :key="`detail-adjustment-${item.code}`"
                class="detail-path-item"
              >
                <div class="detail-path-item-head">
                  <strong>{{ resolveGraphNodeLabel(item.code) }}</strong>
                  <span>
                    {{ Math.round(item.previousMastery * 100) }}% ->
                    {{ Math.round(item.updatedMastery * 100) }}%
                  </span>
                </div>
                <p class="detail-path-item-meta">
                  反馈结果：{{ detailAdjustmentStatusLabelMap[item.completionStatus] }}
                </p>
                <p class="detail-path-item-reason">
                  {{ item.adjustmentReasons.join(" ") }}
                </p>
              </li>
            </ul>
          </article>

          <article v-if="detailPathComparison" class="detail-comparison-panel">
            <div class="section-headline">
              <div>
                <p class="label">路径变化</p>
                <h3>局部路径前后对比</h3>
              </div>
              <p class="caption">
                用于观察本次局部反馈后，当前二级范围内的本轮学习安排如何变化。
              </p>
            </div>

            <dl class="detail-summary-grid detail-summary-grid--compact">
              <div>
                <dt>调整前本轮</dt>
                <dd>{{ detailPathComparison.summary.beforeScheduledCount }} 个</dd>
              </div>
              <div>
                <dt>调整后本轮</dt>
                <dd>{{ detailPathComparison.summary.afterScheduledCount }} 个</dd>
              </div>
              <div>
                <dt>转为已掌握</dt>
                <dd>{{ detailPathComparison.summary.promotedToMasteredCount }} 个</dd>
              </div>
              <div>
                <dt>提前到本轮</dt>
                <dd>{{ detailPathComparison.summary.movedIntoCurrentRoundCount }} 个</dd>
              </div>
              <div>
                <dt>移出本轮</dt>
                <dd>{{ detailPathComparison.summary.deferredToNextRoundCount }} 个</dd>
              </div>
              <div>
                <dt>顺序调整</dt>
                <dd>{{ detailPathComparison.summary.reorderedCount }} 个</dd>
              </div>
            </dl>

            <div class="detail-path-result-grid">
              <article class="detail-path-card">
                <h3>调整前本轮安排</h3>
                <ol class="detail-path-list">
                  <li
                    v-for="item in detailPathComparison.beforeScheduledItems"
                    :key="`detail-before-${item.code}`"
                    class="detail-path-item"
                  >
                    <div class="detail-path-item-head">
                      <strong>{{ item.name }}</strong>
                      <span>{{ item.estimatedMinutes }} 分钟</span>
                    </div>
                    <p class="detail-path-item-meta">掌握度 {{ item.masteryPercent }}%</p>
                  </li>
                </ol>
              </article>

              <article class="detail-path-card">
                <h3>调整后本轮安排</h3>
                <ol class="detail-path-list">
                  <li
                    v-for="item in detailPathComparison.afterScheduledItems"
                    :key="`detail-after-${item.code}`"
                    class="detail-path-item"
                  >
                    <div class="detail-path-item-head">
                      <strong>{{ item.name }}</strong>
                      <span>{{ item.estimatedMinutes }} 分钟</span>
                    </div>
                    <p class="detail-path-item-meta">掌握度 {{ item.masteryPercent }}%</p>
                  </li>
                </ol>
              </article>
            </div>

            <ul
              v-if="detailPathComparison.changeItems.length > 0"
              class="detail-adjustment-list"
            >
              <li
                v-for="item in detailPathComparison.changeItems"
                :key="`detail-change-${item.code}`"
                class="detail-path-item"
              >
                <div class="detail-path-item-head">
                  <strong>{{ item.name }}</strong>
                  <span class="change-badge" :class="changeBadgeClass(item.changeType)">
                    {{ item.changeLabel }}
                  </span>
                </div>
                <p class="detail-path-item-meta">
                  状态：{{ item.beforeStatusLabel }} -> {{ item.afterStatusLabel }}
                </p>
                <p class="detail-path-item-reason">
                  {{ item.changeDescription }}
                </p>
              </li>
            </ul>
            <p v-else class="empty-tip">
              本次局部反馈没有改变当前二级范围的本轮安排顺序。
            </p>
          </article>
        </template>
      </template>

      <div v-else class="empty-tip empty-tip--standalone">
        当前范围暂时没有可用于路径规划的细化节点。
      </div>
    </article>
  </section>
</template>

<script setup>
import { computed, nextTick, onBeforeUnmount, ref, watch } from "vue";
import { useRouter } from "vue-router";

import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import { adjustDetailLearningPath, generateDetailLearningPath } from "../api/path";
import { loadCytoscape } from "../graph/loadCytoscape";
import { useNavigationStore } from "../stores/navigationStore";
import {
  clampFeedbackMasteryForStatus,
  feedbackQuickPresets,
  getFeedbackMasteryRange,
  normalizeFeedbackDraft,
  resolveFeedbackQuickPresetDraft,
  resolveFeedbackStatusDraft,
} from "../utils/feedbackQuickPreset";
import { downloadLearningPathExport } from "../utils/learningPathExport";

const props = defineProps({
  learnerCode: {
    type: String,
    default: "demo-learner",
  },
  masteryByCode: {
    type: Object,
    default: () => ({}),
  },
  section: {
    type: Object,
    default: null,
  },
});
const emit = defineEmits(["profile-updated"]);

const router = useRouter();
const navigationStore = useNavigationStore();
const graphRef = ref(null);
const graphLoading = ref(true);
const graphError = ref("");
const graphData = ref(null);
const detailMinuteOptions = [20, 30, 40, 60, 90, 120];
const detailPlanning = ref(false);
const detailPlanError = ref("");
const detailPlanResult = ref(null);
const detailAvailableMinutes = ref(40);
const detailTargetCode = ref("");
const showDetailMasteryEditor = ref(false);
const detailMasteryPercentByCode = ref({});
const detailFeedbackDraftByCode = ref({});
const detailAdjusting = ref(false);
const detailAdjustmentError = ref("");
const detailAdjustmentSummary = ref(null);
const detailAdjustmentItems = ref([]);
const detailPathComparison = ref(null);
const detailExportError = ref("");
const loadedScopeCode = ref("");
let cyInstance = null;

const detailAdjustmentStatusLabelMap = {
  completed: "已完成",
  partial: "部分完成",
  blocked: "学习受阻",
};

const nodeTypeLabelMap = {
  foundation: "基础概念",
  structure: "核心结构",
  implementation: "实现方法",
  algorithm: "算法方法",
  application: "应用场景",
  abstraction: "抽象建模",
};

const scopeTitle = computed(() => {
  if (graphData.value?.view?.scopeName) {
    return `${graphData.value.view.scopeName}二级图谱`;
  }

  return props.section?.scopeLabel
    ? `${props.section.scopeLabel}二级图谱`
    : "二级细化图谱";
});

const scopePlannerTitle = computed(() => {
  if (graphData.value?.view?.scopeName) {
    return `${graphData.value.view.scopeName}学习导航`;
  }

  return props.section?.scopeLabel
    ? `${props.section.scopeLabel}学习导航`
    : "二级节点学习导航";
});

const breadcrumbs = computed(() => graphData.value?.breadcrumbs || []);
const currentScopeCode = computed(() => graphData.value?.view?.scopeCode || "");
const currentScopeName = computed(() => graphData.value?.view?.scopeName || "当前细化范围");
const currentScopeLevel = computed(() => resolveScopeLevel(graphData.value));

const detailTargetOptions = computed(() => {
  const nodes = [...(graphData.value?.nodes || [])];
  const edges = graphData.value?.edges || [];
  const outdegreeByCode = Object.fromEntries(nodes.map((node) => [node.code, 0]));

  edges.forEach((edge) => {
    if (Object.hasOwn(outdegreeByCode, edge.source)) {
      outdegreeByCode[edge.source] += 1;
    }
  });

  nodes.sort((left, right) => {
    if ((outdegreeByCode[left.code] === 0) !== (outdegreeByCode[right.code] === 0)) {
      return outdegreeByCode[left.code] === 0 ? -1 : 1;
    }

    if ((left.displayOrder || 0) !== (right.displayOrder || 0)) {
      return (left.displayOrder || 0) - (right.displayOrder || 0);
    }

    return (left.label || left.code).localeCompare(right.label || right.code);
  });

  return nodes;
});

const currentScopeGraph = computed(() =>
  normalizePayload(graphData.value, detailMasteryPercentByCode.value),
);

const learnedNodeCount = computed(
  () => currentScopeGraph.value.nodes.filter((node) => node.masteryPercent > 0).length,
);

const unlearnedNodeCount = computed(
  () => currentScopeGraph.value.nodes.filter((node) => node.masteryPercent <= 0).length,
);

const selectedNode = computed(() => {
  if (!detailTargetCode.value) {
    return null;
  }

  return currentScopeGraph.value.nodeMap.get(detailTargetCode.value) || null;
});

const directPrerequisites = computed(() => {
  if (!selectedNode.value) {
    return [];
  }

  const prerequisiteCodes = currentScopeGraph.value.edges
    .filter((edge) => edge.target === selectedNode.value.code)
    .map((edge) => edge.source);

  return prerequisiteCodes
    .map((code) => currentScopeGraph.value.nodeMap.get(code))
    .filter(Boolean)
    .sort(sortNodes);
});

const directDependents = computed(() => {
  if (!selectedNode.value) {
    return [];
  }

  const dependentCodes = currentScopeGraph.value.edges
    .filter((edge) => edge.source === selectedNode.value.code)
    .map((edge) => edge.target);

  return dependentCodes
    .map((code) => currentScopeGraph.value.nodeMap.get(code))
    .filter(Boolean)
    .sort(sortNodes);
});

const detailScheduledItems = computed(() =>
  (detailPlanResult.value?.path || []).filter((item) => item.status === "scheduled"),
);

const detailDeferredItems = computed(() =>
  (detailPlanResult.value?.path || []).filter((item) => item.status === "deferred"),
);
const detailResourceRecommendations = computed(
  () => detailPlanResult.value?.resourceRecommendations || [],
);

const detailTargetLabel = computed(
  () =>
    detailTargetOptions.value.find((node) => node.code === detailTargetCode.value)?.label ||
    "未选择",
);

const detailReachableClass = computed(() =>
  detailPlanResult.value?.summary?.targetReachableWithinBudget
    ? "detail-result-message--ok"
    : "detail-result-message--warn",
);

const detailReachableMessage = computed(() => {
  if (!detailPlanResult.value) {
    return "";
  }

  if (detailPlanResult.value.summary.targetReachableWithinBudget) {
    return "当前时间预算足以覆盖本范围目标链路，可直接作为该一级节点内部的执行计划。";
  }

  return "当前时间预算不足以完成本范围目标链路，系统已把部分细化节点顺延到下一轮。";
});

function sortNodes(left, right) {
  if ((left.chapterNo || 0) !== (right.chapterNo || 0)) {
    return (left.chapterNo || 0) - (right.chapterNo || 0);
  }

  if ((left.displayOrder || 0) !== (right.displayOrder || 0)) {
    return (left.displayOrder || 0) - (right.displayOrder || 0);
  }

  return (left.label || left.code).localeCompare(right.label || right.code);
}

function resolveMasteryState(masteryPercent) {
  if (masteryPercent >= 85) {
    return "mastered";
  }

  if (masteryPercent >= 61) {
    return "familiar";
  }

  if (masteryPercent >= 31) {
    return "learning";
  }

  if (masteryPercent > 0) {
    return "beginner";
  }

  return "not-started";
}

function resolveMasteryStateLabel(masteryState) {
  const labelMap = {
    mastered: "已掌握",
    familiar: "基本掌握",
    learning: "学习中",
    beginner: "初步了解",
    "not-started": "未学习",
  };

  return labelMap[masteryState] || "未学习";
}

function resolveNodeTypeLabel(nodeType) {
  return nodeTypeLabelMap[nodeType] || nodeType || "知识点";
}

function resolveScopeLevelLabel(level) {
  if (level === 1) {
    return "一级主图";
  }

  if (level === 2) {
    return "二级细化图";
  }

  return "三级细化图";
}

function resolveScopeLevel(payload) {
  return Math.min(payload?.breadcrumbs?.length || (payload?.view?.isRoot ? 1 : 2), 3);
}

function clampMasteryPercent(value) {
  return Math.max(0, Math.min(100, Math.round(Number(value || 0))));
}

function normalizeProfileMastery(value) {
  const numericValue = Number(value || 0);
  if (numericValue <= 1) {
    return clampMasteryPercent(numericValue * 100);
  }

  return clampMasteryPercent(numericValue);
}

function interpolateChannel(start, end, ratio) {
  return Math.round(start + (end - start) * ratio);
}

function interpolateColor(start, end, ratio) {
  return `rgb(${interpolateChannel(start[0], end[0], ratio)}, ${interpolateChannel(
    start[1],
    end[1],
    ratio,
  )}, ${interpolateChannel(start[2], end[2], ratio)})`;
}

function resolveMasteryNodeColor(percent) {
  const masteryPercent = clampMasteryPercent(percent);

  if (masteryPercent <= 35) {
    return interpolateColor([255, 246, 250], [246, 217, 230], masteryPercent / 35);
  }

  if (masteryPercent <= 70) {
    return interpolateColor([246, 217, 230], [235, 181, 204], (masteryPercent - 35) / 35);
  }

  return interpolateColor([235, 181, 204], [220, 144, 176], (masteryPercent - 70) / 30);
}

function resolveMasteryBorderColor(percent) {
  const masteryPercent = clampMasteryPercent(percent);

  if (masteryPercent <= 35) {
    return interpolateColor([238, 206, 220], [227, 172, 194], masteryPercent / 35);
  }

  if (masteryPercent <= 70) {
    return interpolateColor([227, 172, 194], [211, 136, 167], (masteryPercent - 35) / 35);
  }

  return interpolateColor([211, 136, 167], [190, 104, 139], (masteryPercent - 70) / 30);
}

function resolveMasteryTextColor() {
  return "#63374a";
}

function resolveNodeTextColor(percent) {
  return resolveMasteryTextColor();
}

function buildNodeProgressBadgeDataUri(percent) {
  const masteryPercent = clampMasteryPercent(percent);
  const svg = `
    <svg xmlns="http://www.w3.org/2000/svg" width="38" height="18" viewBox="0 0 38 18">
      <rect x="0.75" y="0.75" width="36.5" height="16.5" rx="8.25" fill="rgba(255,255,255,0.88)" stroke="rgba(160,97,127,0.28)" stroke-width="1.5"/>
      <text x="19" y="12.2" text-anchor="middle" font-family="sans-serif" font-size="10" font-weight="700" fill="#7b4760">${masteryPercent}%</text>
    </svg>
  `;

  return `data:image/svg+xml;utf8,${encodeURIComponent(svg)}`;
}

function normalizePayload(payload, masteryPercentByCode) {
  if (!payload?.nodes?.length) {
    return {
      nodes: [],
      edges: [],
      nodeMap: new Map(),
    };
  }

  const nodes = payload.nodes
    .map((node) => {
      const masteryPercent = clampMasteryPercent(masteryPercentByCode?.[node.code] || 0);
      return {
        ...node,
        masteryState: resolveMasteryState(masteryPercent),
        masteryPercent,
        isLearned: masteryPercent > 0 ? 1 : 0,
        nodeColor: resolveMasteryNodeColor(masteryPercent),
        borderColor: resolveMasteryBorderColor(masteryPercent),
        textColor: resolveNodeTextColor(masteryPercent),
        progressBadgeSvg: buildNodeProgressBadgeDataUri(masteryPercent),
      };
    })
    .sort((left, right) => {
      if ((left.displayOrder || 0) !== (right.displayOrder || 0)) {
        return (left.displayOrder || 0) - (right.displayOrder || 0);
      }

      return (left.label || left.code).localeCompare(right.label || right.code);
    });
  const nodeMap = new Map(nodes.map((node) => [node.code, node]));

  return {
    nodes,
    edges: (payload.edges || []).map((edge) => ({
      ...edge,
      isLearned:
        nodeMap.get(edge.source)?.isLearned === 1 &&
        nodeMap.get(edge.target)?.isLearned === 1
          ? 1
          : 0,
    })),
    nodeMap,
  };
}

function buildElements(payload) {
  const nodes = payload.nodes.map((node) => ({
    data: {
      id: node.code,
      label: node.label,
      hasDetailGraph: node.hasDetailGraph ? 1 : 0,
      isLearned: node.isLearned,
      nodeColor: node.nodeColor,
      borderColor: node.borderColor,
      textColor: node.textColor,
      progressBadgeSvg: node.progressBadgeSvg,
    },
  }));

  const edges = payload.edges.map((edge) => ({
    data: {
      id: edge.id,
      source: edge.source,
      target: edge.target,
      isLearned: edge.isLearned,
    },
  }));

  return [...nodes, ...edges];
}

function buildGraphLayout(payload) {
  if (payload.edges.length > 0) {
    return {
      name: "breadthfirst",
      directed: true,
      padding: 36,
      spacingFactor: 1.25,
      avoidOverlap: true,
      animate: false,
      nodeDimensionsIncludeLabels: true,
      fit: false,
    };
  }

  return {
    name: "grid",
    padding: 36,
    avoidOverlap: true,
    animate: false,
    fit: false,
  };
}

function buildInitialGraphLayout() {
  return {
    name: "preset",
    fit: false,
  };
}

function waitForViewportStable() {
  return new Promise((resolve) => {
    if (
      typeof window !== "undefined" &&
      typeof window.requestAnimationFrame === "function"
    ) {
      window.requestAnimationFrame(() => {
        window.requestAnimationFrame(resolve);
      });
      return;
    }

    setTimeout(resolve, 0);
  });
}

async function runGraphLayout(payload) {
  if (!cyInstance) {
    return;
  }

  await nextTick();
  await waitForViewportStable();
  cyInstance.resize();

  await new Promise((resolve) => {
    const handleLayoutStop = () => {
      cyInstance?.off("layoutstop", handleLayoutStop);
      resolve();
    };

    cyInstance.one("layoutstop", handleLayoutStop);
    cyInstance.layout(buildGraphLayout(payload)).run();
  });

  cyInstance.resize();
  cyInstance.fit(cyInstance.elements(), 40);
}

function refreshGraphState() {
  if (!cyInstance) {
    return;
  }

  cyInstance.elements().removeClass("is-selected is-context is-context-edge is-dimmed");

  if (!detailTargetCode.value) {
    return;
  }

  const node = cyInstance.getElementById(detailTargetCode.value);
  if (!node || node.empty()) {
    return;
  }

  const contextNodes = node
    .predecessors()
    .nodes()
    .union(node.successors().nodes())
    .union(node);
  const contextEdges = node
    .predecessors()
    .edges()
    .union(node.successors().edges())
    .union(node.connectedEdges());

  cyInstance.elements().addClass("is-dimmed");
  contextNodes.removeClass("is-dimmed").addClass("is-context");
  contextEdges.removeClass("is-dimmed").addClass("is-context-edge");
  node.removeClass("is-context");
  node.removeClass("is-dimmed");
  node.addClass("is-selected");
}

async function renderGraph(payload) {
  if (!graphRef.value) {
    return;
  }

  if (cyInstance) {
    cyInstance.destroy();
    cyInstance = null;
  }

  if (!payload?.nodes?.length) {
    return;
  }

  const cytoscape = await loadCytoscape();

  cyInstance = cytoscape({
    container: graphRef.value,
    elements: buildElements(payload),
    autoungrabify: true,
    boxSelectionEnabled: false,
    userZoomingEnabled: false,
    userPanningEnabled: false,
    layout: buildInitialGraphLayout(),
    style: [
      {
        selector: "node",
        style: {
          shape: "round-rectangle",
          label: "data(label)",
          "background-color": "data(nodeColor)",
          color: "data(textColor)",
          "font-size": "10.5px",
          "font-weight": 600,
          "text-valign": "center",
          "text-halign": "center",
          width: 124,
          height: 90,
          "text-wrap": "wrap",
          "text-max-width": 102,
          "border-width": 3,
          "border-style": "dashed",
          "border-color": "data(borderColor)",
          "background-image": "data(progressBadgeSvg)",
          "background-image-containment": "over",
          "background-width": "38px",
          "background-height": "18px",
          "background-fit": "none",
          "background-repeat": "no-repeat",
          "background-position-x": "90%",
          "background-position-y": "90%",
          "overlay-opacity": 0,
          cursor: "pointer",
        },
      },
      {
        selector: "node[hasDetailGraph = 1]",
        style: {
          "border-style": "dashed",
        },
      },
      {
        selector: "edge",
        style: {
          width: 2.8,
          "line-color": "#d6a6bb",
          "target-arrow-color": "#d6a6bb",
          "target-arrow-shape": "triangle",
          "curve-style": "bezier",
          opacity: 0.86,
          "overlay-opacity": 0,
        },
      },
      {
        selector: "edge[isLearned = 1]",
        style: {
          "line-color": "#c87b9d",
          "target-arrow-color": "#c87b9d",
        },
      },
      {
        selector: "node.is-context",
        style: {
          "border-width": 5,
          "border-style": "dashed",
          "border-color": "#2f5fae",
          opacity: 1,
        },
      },
      {
        selector: "edge.is-context-edge",
        style: {
          width: 4,
          opacity: 1,
        },
      },
      {
        selector: "node.is-selected",
        style: {
          "border-width": 5,
          "border-style": "dashed",
          "border-color": "#a64d72",
          opacity: 1,
          "shadow-blur": 16,
          "shadow-color": "#d995b0",
          "shadow-opacity": 0.48,
          "shadow-offset-x": 0,
          "shadow-offset-y": 0,
          "z-index": 999,
        },
      },
      {
        selector: "node.is-dimmed",
        style: {
          opacity: 0.52,
          "text-opacity": 0.92,
        },
      },
      {
        selector: "edge.is-dimmed",
        style: {
          opacity: 0.18,
        },
      },
    ],
  });

  cyInstance.on("tap", "node", (event) => {
    detailTargetCode.value = event.target.id();
  });

  await runGraphLayout(payload);
  refreshGraphState();
}

function resolveDefaultDetailTargetCode(nodes = [], edges = []) {
  if (!Array.isArray(nodes) || nodes.length === 0) {
    return "";
  }

  const indegreeByCode = Object.fromEntries(nodes.map((node) => [node.code, 0]));
  edges.forEach((edge) => {
    if (Object.hasOwn(indegreeByCode, edge.target)) {
      indegreeByCode[edge.target] += 1;
    }
  });

  const sortedNodes = [...nodes].sort((left, right) => {
    if ((left.displayOrder || 0) !== (right.displayOrder || 0)) {
      return (left.displayOrder || 0) - (right.displayOrder || 0);
    }

    return (left.label || left.code).localeCompare(right.label || right.code);
  });
  const topLevelNodes = sortedNodes.filter((node) => indegreeByCode[node.code] === 0);

  return topLevelNodes[0]?.code || sortedNodes[0]?.code || "";
}

function createDetailMasterySnapshot(nodes = []) {
  return Object.fromEntries(
    nodes.map((node) => [
      node.code,
      normalizeProfileMastery(props.masteryByCode?.[node.code]),
    ]),
  );
}

function createDetailMasteryOverrideSnapshot(nodes = [], masteryPercentByCode = {}) {
  const baselineSnapshot = createDetailMasterySnapshot(nodes);

  return Object.fromEntries(
    nodes
      .map((node) => {
        const currentPercent = clampMasteryPercent(masteryPercentByCode?.[node.code]);
        const baselinePercent = clampMasteryPercent(baselineSnapshot[node.code]);

        if (currentPercent === baselinePercent) {
          return null;
        }

        return [node.code, currentPercent];
      })
      .filter(Boolean),
  );
}

function buildDetailMasteryPayload() {
  return Object.fromEntries(
    Object.entries(detailMasteryPercentByCode.value).map(([code, percent]) => [
      code,
      Number(percent || 0) / 100,
    ]),
  );
}

function clearDetailOperationOutputs() {
  detailAdjustmentError.value = "";
  detailAdjustmentSummary.value = null;
  detailAdjustmentItems.value = [];
  detailPathComparison.value = null;
}

function normalizeDetailAvailableMinutes(value) {
  const numericValue = Number(value || 0);
  if (detailMinuteOptions.includes(numericValue)) {
    return numericValue;
  }

  return 40;
}

function normalizeDetailMasteryPercentMap(
  nodes = [],
  overrideSnapshot = {},
  legacySnapshot = {},
) {
  const baselineSnapshot = createDetailMasterySnapshot(nodes);
  const fallbackSnapshot = Object.values(baselineSnapshot).some((value) => value > 0)
    ? baselineSnapshot
    : Object.fromEntries(
        nodes.map((node) => [
          node.code,
          clampMasteryPercent(legacySnapshot?.[node.code]),
        ]),
      );
  const normalizedSnapshot = { ...fallbackSnapshot };

  nodes.forEach((node) => {
    if (!Object.hasOwn(overrideSnapshot || {}, node.code)) {
      return;
    }

    normalizedSnapshot[node.code] = clampMasteryPercent(overrideSnapshot[node.code]);
  });

  return normalizedSnapshot;
}

function normalizeDetailFeedbackDrafts(nodes = [], drafts = {}) {
  const nodeByCode = new Map(nodes.map((node) => [node.code, node]));
  const allowedCodes = new Set(nodes.map((node) => node.code));
  return Object.fromEntries(
    Object.entries(drafts || {})
      .filter(([code]) => allowedCodes.has(code))
      .map(([code, value]) => [
        code,
        normalizeFeedbackDraft(nodeByCode.get(code), value),
      ]),
  );
}

function hasRememberedDetailPlanResult(viewState) {
  return Array.isArray(viewState?.planResult?.path) && viewState.planResult.path.length > 0;
}

function restoreScopeState(payload, viewState = null) {
  const nodes = payload.nodes || [];
  const edges = payload.edges || [];
  const nodeCodeSet = new Set(nodes.map((node) => node.code));
  const defaultTargetCode = resolveDefaultDetailTargetCode(nodes, edges);
  const rememberedTargetCode = String(viewState?.targetNodeCode || "");
  const nextTargetCode = nodeCodeSet.has(rememberedTargetCode)
    ? rememberedTargetCode
    : defaultTargetCode;

  detailAvailableMinutes.value = normalizeDetailAvailableMinutes(viewState?.availableMinutes);
  detailTargetCode.value = nextTargetCode;
  detailMasteryPercentByCode.value = normalizeDetailMasteryPercentMap(
    nodes,
    viewState?.masteryOverridePercentByCode || {},
    viewState?.masteryPercentByCode || {},
  );
  showDetailMasteryEditor.value = Boolean(viewState?.showMasteryEditor);
  detailPlanResult.value = hasRememberedDetailPlanResult(viewState)
    ? viewState.planResult
    : null;
  detailPlanError.value = "";
  detailFeedbackDraftByCode.value = normalizeDetailFeedbackDrafts(
    nodes,
    viewState?.feedbackDraftByCode || {},
  );
  detailAdjustmentSummary.value = viewState?.adjustmentSummary || null;
  detailAdjustmentItems.value = Array.isArray(viewState?.adjustmentItems)
    ? viewState.adjustmentItems
    : [];
  detailPathComparison.value = viewState?.pathComparison || null;
  detailExportError.value = "";
  detailAdjustmentError.value = "";

  if (detailPlanResult.value && Object.keys(detailFeedbackDraftByCode.value).length === 0) {
    syncDetailFeedbackDrafts();
  }
}

function clonePlanSnapshot(plan) {
  if (!plan) {
    return null;
  }

  return JSON.parse(JSON.stringify(plan));
}

function formatPlanStatus(status) {
  if (status === "mastered") {
    return "已掌握";
  }

  if (status === "scheduled") {
    return "本轮学习";
  }

  if (status === "deferred") {
    return "顺延到下轮";
  }

  return "未进入路径";
}

function buildDetailPathComparison(beforePlan, afterPlan) {
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
    ...new Set([...beforePath.map((item) => item.code), ...afterPath.map((item) => item.code)]),
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
      changeDescription = `该节点由本轮第 ${beforeOrder} 位调整为“${formatPlanStatus(afterStatus)}”。`;
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
      name: afterItem?.name || beforeItem?.name || code,
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

function syncDetailFeedbackDrafts() {
  detailFeedbackDraftByCode.value = Object.fromEntries(
    detailScheduledItems.value.map((item) => [
      item.code,
      resolveFeedbackStatusDraft(item, "completed"),
    ]),
  );
}

function applyDetailFeedbackQuickPreset(item, presetKey) {
  detailFeedbackDraftByCode.value = {
    ...detailFeedbackDraftByCode.value,
    [item.code]: resolveFeedbackQuickPresetDraft(item, presetKey),
  };
}

function getDetailFeedbackMasteryRangeForCode(code) {
  const completionStatus =
    detailFeedbackDraftByCode.value[code]?.completionStatus || "completed";
  return getFeedbackMasteryRange(completionStatus);
}

function getDetailFeedbackMasteryRangeLabel(code) {
  const range = getDetailFeedbackMasteryRangeForCode(code);
  return `${range.min}% - ${range.max}%`;
}

function handleDetailFeedbackStatusChange(item) {
  const currentDraft = detailFeedbackDraftByCode.value[item.code] || {};
  detailFeedbackDraftByCode.value = {
    ...detailFeedbackDraftByCode.value,
    [item.code]: normalizeFeedbackDraft(item, currentDraft),
  };
}

function buildDetailFeedbackItems() {
  return detailScheduledItems.value.map((item) => ({
    code: item.code,
    completionStatus:
      normalizeFeedbackDraft(item, detailFeedbackDraftByCode.value[item.code]).completionStatus,
    selfRatedMastery:
      clampFeedbackMasteryForStatus(
        normalizeFeedbackDraft(item, detailFeedbackDraftByCode.value[item.code]).completionStatus,
        detailFeedbackDraftByCode.value[item.code]?.selfRatedMastery || 0,
      ) / 100,
  }));
}

function applyDetailMasterySnapshot(snapshot) {
  detailMasteryPercentByCode.value = Object.fromEntries(
    detailTargetOptions.value.map((node) => [
      node.code,
      clampMasteryPercent(Number(snapshot?.[node.code] || 0) * 100),
    ]),
  );
}

function toggleDetailMasteryEditor() {
  showDetailMasteryEditor.value = !showDetailMasteryEditor.value;
}

function getExplanationSummary(item) {
  if (item?.explanation?.summary) {
    return item.explanation.summary;
  }

  return item?.reasonTrace?.triggerReasons?.join(" ") || "暂无推荐理由说明。";
}

function resolveGraphNodeLabel(code) {
  return graphData.value?.nodes?.find((node) => node.code === code)?.label || code;
}

function changeBadgeClass(changeType) {
  return {
    "change-badge--mastered": changeType === "mastered",
    "change-badge--advanced": changeType === "advanced",
    "change-badge--deferred": changeType === "deferred",
    "change-badge--reordered": changeType === "reordered",
  };
}

function buildRememberedDetailViewState() {
  const nodes = graphData.value?.nodes || [];

  return {
    targetNodeCode: detailTargetCode.value,
    availableMinutes: detailAvailableMinutes.value,
    showMasteryEditor: showDetailMasteryEditor.value,
    masteryPercentByCode: { ...detailMasteryPercentByCode.value },
    masteryOverridePercentByCode: createDetailMasteryOverrideSnapshot(
      nodes,
      detailMasteryPercentByCode.value,
    ),
    feedbackDraftByCode: JSON.parse(JSON.stringify(detailFeedbackDraftByCode.value || {})),
    planResult: clonePlanSnapshot(detailPlanResult.value),
    adjustmentSummary: detailAdjustmentSummary.value
      ? JSON.parse(JSON.stringify(detailAdjustmentSummary.value))
      : null,
    adjustmentItems: clonePlanSnapshot(detailAdjustmentItems.value) || [],
    pathComparison: clonePlanSnapshot(detailPathComparison.value),
  };
}

function findDetailResourceSectionByCode(code) {
  return detailResourceRecommendations.value.find((section) => section.code === code) || null;
}

function hasDetailResourceSection(code) {
  return Boolean(findDetailResourceSectionByCode(code));
}

function syncDetailResourceRecommendationContext(currentPlan = detailPlanResult.value) {
  if (!currentPlan) {
    navigationStore.clearResourceRecommendationContext("detail");
    return;
  }

  const resourceRecommendationSections = (currentPlan.resourceRecommendations || []).map(
    (section) => ({
      ...section,
      recordingKnowledgePointCode: props.section?.code || "",
      scopeCode: props.section?.scopeCode || currentScopeCode.value || "",
      scopeLabel: props.section?.scopeLabel || currentScopeName.value || "",
      parentNodeCode: props.section?.code || "",
      parentNodeLabel: props.section?.name || props.section?.scopeLabel || "",
    }),
  );

  navigationStore.setResourceRecommendationContext({
    learnerCode: props.learnerCode,
    resourceRecommendationSections,
    contextMode: "detail",
    sourcePage: "detail-learning",
    targetLabel: detailTargetLabel.value,
    availableMinutes: currentPlan.summary?.availableMinutes || detailAvailableMinutes.value,
    scheduledCount: currentPlan.summary?.scheduledCount || 0,
    deferredCount: currentPlan.summary?.deferredCount || 0,
    scopeCode: props.section?.scopeCode || currentScopeCode.value || "",
    scopeLabel: props.section?.scopeLabel || currentScopeName.value || "",
    parentNodeCode: props.section?.code || "",
    parentNodeLabel: props.section?.name || props.section?.scopeLabel || "",
  });
}

function openDetailResourceRecommendationSection(code) {
  const section = findDetailResourceSectionByCode(code);
  if (!section) {
    return;
  }

  syncDetailResourceRecommendationContext(detailPlanResult.value);
  router.push({
    name: "resource-recommendation",
    params: { code: section.code },
    query: {
      level: "detail",
    },
  });
}

function persistCurrentScopeViewState() {
  if (
    !props.section?.scopeCode ||
    graphLoading.value ||
    graphError.value ||
    loadedScopeCode.value !== props.section.scopeCode
  ) {
    return;
  }

  navigationStore.setDetailLearningViewState(
    props.section.scopeCode,
    buildRememberedDetailViewState(),
  );
}

function exportDetailPlan() {
  if (!detailPlanResult.value) {
    return;
  }

  detailExportError.value = "";

  try {
    downloadLearningPathExport({
      learnerCode: props.learnerCode,
      targetCode: detailTargetCode.value,
      targetLabel: detailTargetLabel.value,
      availableMinutes: detailAvailableMinutes.value,
      planResult: detailPlanResult.value,
      scopeLabel: currentScopeName.value,
      documentTitle: "细化学习路径导出结果",
      filePrefix: "detail-learning-path",
    });
  } catch (error) {
    detailExportError.value =
      error?.message || "细化学习路径导出失败，请稍后重试或检查浏览器下载权限。";
    console.error(error);
  }
}

async function submitDetailPlan() {
  if (!props.section?.scopeCode) {
    return;
  }

  detailPlanning.value = true;
  detailPlanError.value = "";
  detailExportError.value = "";
  clearDetailOperationOutputs();

  try {
    const targetNodeCode =
      detailTargetCode.value ||
      resolveDefaultDetailTargetCode(graphData.value?.nodes || [], graphData.value?.edges || []);
    detailTargetCode.value = targetNodeCode;

    const payload = await generateDetailLearningPath({
      learnerCode: props.learnerCode,
      scopeCode: props.section.scopeCode,
      targetNodeCode,
      availableMinutes: detailAvailableMinutes.value,
      masteryByCode: buildDetailMasteryPayload(),
    });
    detailPlanResult.value = payload;
    syncDetailFeedbackDrafts();
    refreshGraphState();
  } catch (error) {
    detailPlanError.value =
      error?.response?.data?.detail ||
      "本范围学习路径生成失败。请稍后重试或检查后端日志。";
    detailPlanResult.value = null;
    refreshGraphState();
    console.error(error);
  } finally {
    detailPlanning.value = false;
  }
}

async function submitDetailAdjustment() {
  if (!props.section?.scopeCode || detailScheduledItems.value.length === 0) {
    return;
  }

  detailAdjusting.value = true;
  detailAdjustmentError.value = "";
  detailExportError.value = "";
  const beforePlanSnapshot = clonePlanSnapshot(detailPlanResult.value);

  try {
    const payload = await adjustDetailLearningPath({
      learnerCode: props.learnerCode,
      scopeCode: props.section.scopeCode,
      targetNodeCode: detailTargetCode.value,
      availableMinutes: detailAvailableMinutes.value,
      masteryByCode: buildDetailMasteryPayload(),
      feedbackItems: buildDetailFeedbackItems(),
    });

    applyDetailMasterySnapshot(payload.updatedMasteryByCode || {});
    detailAdjustmentSummary.value = payload.feedbackSummary || null;
    detailAdjustmentItems.value = payload.adjustments || [];
    detailPlanResult.value = payload;
    detailPathComparison.value = buildDetailPathComparison(beforePlanSnapshot, payload);
    syncDetailFeedbackDrafts();
    emit("profile-updated");
    refreshGraphState();
  } catch (error) {
    detailAdjustmentError.value =
      error?.response?.data?.detail ||
      "本范围学习反馈调整失败。请稍后重试或检查后端日志。";
    console.error(error);
  } finally {
    detailAdjusting.value = false;
  }
}

async function loadScopeGraph(scopeCode) {
  if (!scopeCode) {
    loadedScopeCode.value = "";
    graphLoading.value = false;
    graphError.value = "";
    graphData.value = null;
    detailPlanResult.value = null;
    detailExportError.value = "";
    return;
  }

  graphLoading.value = true;
  loadedScopeCode.value = "";
  graphError.value = "";
  detailPlanError.value = "";

  try {
    const payload = await fetchKnowledgeGraph({ scopeCode });
    const viewState = navigationStore.detailLearningViewStateByScopeCode(scopeCode);
    graphData.value = payload;
    restoreScopeState(payload, viewState);
    loadedScopeCode.value = scopeCode;
    graphLoading.value = false;
    await nextTick();
    await renderGraph(currentScopeGraph.value);

    if (!hasRememberedDetailPlanResult(viewState)) {
      await submitDetailPlan();
    } else {
      refreshGraphState();
    }
  } catch (error) {
    loadedScopeCode.value = "";
    graphLoading.value = false;
    graphError.value =
      "未能读取当前二级图谱。请先启动后端并确认数据库已初始化。";
    console.error(error);
  }
}

watch(
  () => props.section?.scopeCode,
  async (scopeCode) => {
    await loadScopeGraph(String(scopeCode || ""));
  },
  {
    immediate: true,
  },
);

watch(
  currentScopeGraph,
  async (payload) => {
    if (graphLoading.value || graphError.value || !payload.nodes.length) {
      return;
    }

    await nextTick();
    await renderGraph(payload);
  },
  {
    deep: true,
  },
);

watch(
  detailTargetCode,
  () => {
    refreshGraphState();
  },
);

watch(
  [
    () => props.section?.scopeCode,
    detailTargetCode,
    detailAvailableMinutes,
    showDetailMasteryEditor,
    detailMasteryPercentByCode,
    detailFeedbackDraftByCode,
    detailPlanResult,
    detailAdjustmentSummary,
    detailAdjustmentItems,
    detailPathComparison,
  ],
  () => {
    persistCurrentScopeViewState();
  },
  {
    deep: true,
  },
);

onBeforeUnmount(() => {
  if (cyInstance) {
    cyInstance.destroy();
    cyInstance = null;
  }
});
</script>

<style scoped>
.detail-workspace {
  display: grid;
  gap: 18px;
}

.card {
  background: var(--panel-surface);
  border: var(--panel-border);
  border-radius: 24px;
  padding: 22px;
  box-shadow: var(--panel-shadow);
  contain: paint;
}

.card-head,
.section-headline {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  gap: 18px;
}

.label {
  margin: 0 0 6px;
  font-size: 0.82rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

.label--detail {
  color: #6a7283;
}

h2,
h3,
h4 {
  margin: 0;
  color: #183042;
}

h2 {
  font-size: 1.28rem;
}

h3 {
  font-size: 1.1rem;
}

h4 {
  font-size: 0.98rem;
}

.caption {
  margin: 0;
  max-width: 460px;
  color: #516170;
  line-height: 1.65;
}

.state,
.empty-tip {
  border-radius: 18px;
  padding: 18px;
  color: #43515f;
}

.state {
  background: #eef5ff;
}

.state--error {
  background: #fff2f2;
  color: #9b3333;
}

.scope-toolbar {
  display: grid;
  gap: 14px;
  margin-bottom: 16px;
}

.breadcrumb-row {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.breadcrumb-button {
  border: 1px solid #d5dce5;
  background: #f4f7fb;
  color: #4f6071;
  border-radius: 999px;
  padding: 8px 14px;
  font-size: 0.88rem;
  font-weight: 600;
}

.breadcrumb-button--active {
  background: #e7f0ff;
  border-color: #bfd4fb;
  color: #2459b4;
}

.scope-note {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 10px;
  color: #526172;
}

.scope-note strong {
  color: #183042;
}

.scope-label {
  margin: 0;
  color: #6a7682;
}

.summary-grid {
  margin: 0 0 14px;
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
}

.detail-summary-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 12px;
  margin: 18px 0 0;
}

.summary-grid div,
.detail-grid div,
.detail-summary-grid div {
  padding: 14px 16px;
  border-radius: 18px;
  background: #f5f8fb;
  border: 1px solid rgba(103, 124, 144, 0.16);
}

.summary-grid dt,
.detail-grid dt,
.detail-summary-grid dt {
  margin: 0 0 6px;
  font-size: 0.85rem;
  color: #667381;
}

.summary-grid dd,
.detail-grid dd,
.detail-summary-grid dd {
  margin: 0;
  color: #173042;
  font-weight: 700;
}

.rule-note {
  margin: 0 0 16px;
  color: #4d5d6c;
  line-height: 1.7;
}

.legend-row {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-bottom: 18px;
}

.legend-chip {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 8px 14px;
  border-radius: 999px;
  font-size: 0.88rem;
  font-weight: 600;
  border: 1px solid transparent;
}

.legend-chip::before {
  content: "";
  width: 10px;
  height: 10px;
  border-radius: 999px;
  background: currentColor;
}

.legend-chip--progress-low {
  color: #b56c8f;
  background: #fdeef4;
  border-color: #f2cfdd;
}

.legend-chip--progress-mid {
  color: #a45379;
  background: #f5d7e4;
  border-color: #e8bace;
}

.legend-chip--progress-high {
  color: #8d4567;
  background: #ebb6cc;
  border-color: #dc93b4;
}

.legend-chip--selected {
  color: #a64d72;
  background: #fff4f8;
  border-color: #e8bdd0;
}

.legend-chip--context {
  color: #375d95;
  background: #eef4ff;
  border-color: #d5e2fb;
}

.graph-layout {
  margin-bottom: 18px;
}

.graph-canvas {
  width: 100%;
  min-height: 760px;
  border-radius: 24px;
  background:
    radial-gradient(circle at top, rgba(131, 153, 181, 0.1), transparent 38%),
    linear-gradient(180deg, rgba(247, 249, 252, 0.96), rgba(237, 242, 248, 0.9));
  border: 1px solid rgba(99, 119, 141, 0.16);
}

.detail-panel {
  display: grid;
  gap: 16px;
  padding: 18px;
  border-radius: 24px;
  background: #fbfcfe;
  border: 1px solid rgba(100, 120, 140, 0.14);
}

.detail-head,
.relation-head,
.relation-main {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 12px;
}

.mastery-badge,
.relation-state {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  border-radius: 999px;
  padding: 6px 12px;
  font-size: 0.82rem;
  font-weight: 700;
}

.mastery-badge--mastered,
.relation-state--mastered {
  color: #ffffff;
  background: #3f7fe8;
}

.mastery-badge--progress,
.relation-state--familiar {
  color: #1a6b3a;
  background: #d4edda;
}

.relation-state--learning {
  color: #856404;
  background: #fff3cd;
}

.relation-state--beginner {
  color: #0c5460;
  background: #d1ecf1;
}

.mastery-badge--idle,
.relation-state--not-started {
  color: #586778;
  background: #e5eaef;
}

.detail-grid {
  margin: 0;
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
}

.detail-description {
  margin: 0;
  line-height: 1.75;
  color: #4d5d6c;
}

.relation-section {
  display: grid;
  gap: 10px;
}

.relation-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px;
}

.relation-head span {
  color: #607080;
  font-size: 0.88rem;
}

.relation-list {
  list-style: none;
  margin: 0;
  padding: 0;
  display: grid;
  gap: 10px;
}

.relation-item {
  padding: 12px 14px;
  border-radius: 16px;
  background: #f4f7fb;
  border: 1px solid rgba(103, 124, 144, 0.14);
}

.relation-item strong {
  color: #183042;
}

.relation-item p {
  margin: 6px 0 0;
  color: #5b6a79;
}

.empty-tip--panel,
.empty-tip--standalone {
  background: #f5f8fb;
  border: 1px solid rgba(100, 120, 140, 0.14);
}

.state--result {
  margin-top: 16px;
}

.empty-tip--standalone {
  margin-top: 18px;
}

.detail-path-form,
.detail-feedback-form {
  display: grid;
  gap: 16px;
  margin-top: 18px;
}

.field {
  display: grid;
  gap: 8px;
}

.field--wide {
  grid-column: 1 / -1;
}

.field span,
.field-head span {
  font-weight: 700;
  color: #21405f;
}

.field select,
.field input[type="range"] {
  width: 100%;
}

.field select {
  border: 1px solid rgba(103, 124, 144, 0.16);
  border-radius: 14px;
  padding: 12px 14px;
  background: #f4f7fb;
  color: #21405f;
  font-size: 0.96rem;
}

.field-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}

.field-hint {
  margin: 0;
  color: #667381;
  font-size: 0.9rem;
  line-height: 1.6;
}

.slider-row {
  display: grid;
  grid-template-columns: minmax(0, 1fr) auto;
  align-items: center;
  gap: 12px;
}

.detail-mastery-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 12px;
}

.detail-mastery-item,
.detail-feedback-item {
  display: grid;
  gap: 12px;
  padding: 14px 16px;
  border-radius: 18px;
  background: #f4f7fb;
  border: 1px solid rgba(103, 124, 144, 0.14);
}

.submit-button,
.ghost-button,
.detail-feedback-preset-button {
  border: 0;
  border-radius: 999px;
  padding: 11px 16px;
  font-weight: 700;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease, background 0.15s ease;
}

.submit-button {
  background: linear-gradient(135deg, #c7668f, #d995b0);
  color: #ffffff;
}

.submit-button:hover:not(:disabled),
.ghost-button:hover:not(:disabled),
.detail-feedback-preset-button:hover:not(:disabled) {
  transform: translateY(-1px);
}

.submit-button:disabled,
.ghost-button:disabled,
.detail-feedback-preset-button:disabled {
  opacity: 0.72;
  cursor: wait;
}

.ghost-button {
  background: #f4f7fb;
  color: #4f6071;
  border: 1px solid #d5dce5;
}

.detail-summary-grid--compact {
  margin-top: 16px;
}

.detail-result-message {
  margin: 18px 0 0;
  padding: 14px 16px;
  border-radius: 18px;
  font-weight: 600;
}

.detail-result-message--ok {
  background: rgba(214, 246, 225, 0.9);
  color: #176b39;
}

.detail-result-message--warn {
  background: rgba(255, 244, 221, 0.92);
  color: #9c6d10;
}

.detail-result-action-row {
  display: grid;
  gap: 8px;
  margin-top: 16px;
}

.detail-result-action-note {
  margin: 0;
  color: #607080;
  line-height: 1.6;
}

.detail-path-result-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 18px;
  margin-top: 18px;
}

.detail-path-card,
.detail-feedback-panel,
.detail-adjustment-panel,
.detail-comparison-panel {
  display: grid;
  gap: 16px;
  margin-top: 18px;
  padding: 18px;
  border-radius: 22px;
  background: #fbfcfe;
  border: 1px solid rgba(100, 120, 140, 0.14);
}

.detail-path-card h3 {
  font-size: 1rem;
}

.detail-path-list,
.detail-adjustment-list {
  margin: 0;
  padding: 0;
  list-style: none;
  display: grid;
  gap: 12px;
}

.detail-path-item {
  padding: 14px 16px;
  border-radius: 18px;
  background: #f4f7fb;
  border: 1px solid rgba(103, 124, 144, 0.14);
}

.detail-path-item-head {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  align-items: flex-start;
}

.detail-path-item-actions {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  align-items: center;
  gap: 10px;
}

.detail-path-action-button {
  padding: 8px 14px;
}

.detail-path-item-meta,
.detail-path-item-reason {
  margin: 8px 0 0;
  color: #5d6b79;
  line-height: 1.6;
}

.detail-feedback-quick-actions {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: center;
  flex-wrap: wrap;
}

.detail-feedback-quick-actions__label {
  color: #607080;
  font-weight: 700;
}

.detail-feedback-quick-actions__buttons {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.detail-feedback-preset-button {
  background: #eef4ff;
  color: #375d95;
  border: 1px solid #d5e2fb;
}

.detail-feedback-preset-button--positive {
  background: rgba(214, 246, 225, 0.92);
  color: #176b39;
}

.detail-feedback-preset-button--neutral {
  background: rgba(255, 244, 221, 0.94);
  color: #8d6311;
}

.detail-feedback-preset-button--warn {
  background: rgba(255, 232, 232, 0.94);
  color: #9b3333;
}

.detail-feedback-fields {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 14px;
}

.change-badge {
  display: inline-flex;
  align-items: center;
  border-radius: 999px;
  padding: 6px 10px;
  font-size: 0.82rem;
  font-weight: 700;
}

.change-badge--mastered {
  background: rgba(214, 246, 225, 0.92);
  color: #176b39;
}

.change-badge--advanced {
  background: rgba(225, 240, 255, 0.92);
  color: #2459b4;
}

.change-badge--deferred {
  background: rgba(255, 232, 232, 0.94);
  color: #9b3333;
}

.change-badge--reordered {
  background: rgba(243, 236, 255, 0.92);
  color: #7c4bb3;
}

.state--loading {
  background: rgba(236, 246, 247, 0.92);
  color: #0c5960;
}

@media (max-width: 980px) {
  .card-head,
  .section-headline,
  .detail-head,
  .relation-head,
  .relation-main,
  .detail-path-item-head,
  .detail-feedback-quick-actions {
    flex-direction: column;
  }

  .summary-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }

  .detail-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }

  .relation-grid,
  .detail-path-result-grid,
  .detail-feedback-fields {
    grid-template-columns: 1fr;
  }

  .graph-canvas {
    min-height: 680px;
  }

  .detail-path-item-actions {
    justify-content: flex-start;
  }
}

@media (max-width: 760px) {
  .card-head {
    flex-direction: column;
  }

  .summary-grid,
  .detail-grid {
    grid-template-columns: 1fr;
  }

  .graph-canvas {
    min-height: 540px;
  }
}
</style>
