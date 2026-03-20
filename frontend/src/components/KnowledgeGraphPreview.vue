<template>
  <section class="card">
    <div class="card-head">
      <div>
        <p class="label">图谱预览</p>
        <h2>{{ title }}</h2>
      </div>
      <p class="caption">{{ caption }}</p>
    </div>

    <div v-if="isLoading" class="state state--loading">
      正在从 SQLite 加载数据结构课程知识图谱...
    </div>

    <div v-else-if="loadError" class="state state--error">
      {{ loadError }}
    </div>

    <template v-else>
      <dl class="meta-grid">
        <div>
          <dt>知识点</dt>
          <dd>{{ graphData.meta.knowledgePointCount }}</dd>
        </div>
        <div>
          <dt>依赖边</dt>
          <dd>{{ graphData.meta.dependencyCount }}</dd>
        </div>
        <div>
          <dt>章节数</dt>
          <dd>{{ graphData.meta.chapterCount }}</dd>
        </div>
        <div>
          <dt>核心节点</dt>
          <dd>{{ graphData.meta.coreKnowledgePointCount }}</dd>
        </div>
      </dl>

      <div class="scope-bar">
        <div class="breadcrumb-list">
          <button
            v-for="item in breadcrumbs"
            :key="item.scopeCode"
            type="button"
            class="breadcrumb-button"
            :class="{
              'breadcrumb-button--active': item.scopeCode === currentScopeCode,
            }"
            @click="navigateToScope(item.scopeCode)"
          >
            {{ item.label }}
          </button>
        </div>
        <p class="scope-note">{{ scopeNote }}</p>
      </div>

      <div class="graph-layout">
        <div ref="graphRef" class="graph-canvas"></div>

        <aside class="detail-panel">
          <template v-if="selectedNode">
            <div class="detail-head">
              <div>
                <p class="label label--detail">节点详情</p>
                <h3>{{ selectedNode.label }}</h3>
              </div>
              <span class="node-badge">{{ selectedNode.chapterName }}</span>
            </div>

            <dl class="detail-grid">
              <div>
                <dt>章节</dt>
                <dd>第{{ selectedNode.chapterNo }}章</dd>
              </div>
              <div>
                <dt>类型</dt>
                <dd>{{ resolveNodeTypeLabel(selectedNode.nodeType) }}</dd>
              </div>
              <div>
                <dt>难度</dt>
                <dd>{{ selectedNode.difficultyLevel }}/5</dd>
              </div>
              <div>
                <dt>预计学习</dt>
                <dd>{{ selectedNode.estimatedMinutes }} 分钟</dd>
              </div>
            </dl>

            <p class="detail-description">
              {{ selectedNode.description || "当前节点暂未补充说明。" }}
            </p>

            <section
              v-if="selectedNode.hasDetailGraph"
              class="relation-section relation-section--drilldown"
            >
              <div class="relation-head">
                <h4>细化图谱</h4>
                <span>可继续下钻</span>
              </div>
              <p class="detail-description detail-description--compact">
                当前节点已经整理出更细粒度图谱。可直接双击左侧节点进入，也可点击下方按钮跳转。
              </p>
              <button
                type="button"
                class="drilldown-button"
                @click="navigateToScope(selectedNode.detailScopeCode)"
              >
                进入该节点细化图谱
              </button>
            </section>

            <section class="relation-section">
              <div class="relation-head">
                <h4>直接前置依赖</h4>
                <span>{{ directPrerequisites.length }} 个</span>
              </div>
              <ul v-if="directPrerequisites.length > 0" class="relation-list">
                <li
                  v-for="item in directPrerequisites"
                  :key="`${selectedNode.code}-pre-${item.code}`"
                  class="relation-item"
                >
                  <strong>{{ item.label }}</strong>
                  <p>第{{ item.chapterNo }}章 · {{ resolveNodeTypeLabel(item.nodeType) }}</p>
                </li>
              </ul>
              <p v-else class="empty-tip">
                当前节点没有直接前置依赖，可作为一条子链路的起点。
              </p>
            </section>

            <section class="relation-section">
              <div class="relation-head">
                <h4>直接后继节点</h4>
                <span>{{ directDependents.length }} 个</span>
              </div>
              <ul v-if="directDependents.length > 0" class="relation-list">
                <li
                  v-for="item in directDependents"
                  :key="`${selectedNode.code}-next-${item.code}`"
                  class="relation-item"
                >
                  <strong>{{ item.label }}</strong>
                  <p>第{{ item.chapterNo }}章 · {{ resolveNodeTypeLabel(item.nodeType) }}</p>
                </li>
              </ul>
              <p v-else class="empty-tip">
                当前节点已经位于一条学习链路的末端或暂无后继节点。
              </p>
            </section>
          </template>

          <div v-else class="empty-tip empty-tip--panel">
            点击左侧知识点节点后，可在这里查看它的章节位置、难度、说明和直接依赖关系。
          </div>
        </aside>
      </div>

      <section v-if="isDetailScope" class="detail-path-panel">
        <div class="detail-path-head">
          <div>
            <p class="label">局部路径</p>
            <h3>{{ graphData.view.scopeName }}学习导航</h3>
          </div>
          <p class="caption">
            当前路径只在本细化图谱范围内规划，用来回答“这个主节点内部先学什么”。
          </p>
        </div>

        <form class="detail-path-form" @submit.prevent="submitDetailPlan">
          <label class="field">
            <span>范围内目标节点</span>
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
              <span>细化节点掌握度</span>
              <div class="detail-form-actions">
                <button
                  type="button"
                  class="ghost-button"
                  :disabled="!selectedNode"
                  @click="applySelectedNodeAsDetailTarget"
                >
                  使用当前选中节点
                </button>
                <button
                  type="button"
                  class="ghost-button"
                  @click="toggleDetailMasteryEditor"
                >
                  {{ showDetailMasteryEditor ? "收起" : "展开" }}
                </button>
              </div>
            </div>
            <p class="field-hint">
              默认把本范围内细化节点视为未掌握。你可以在这里模拟某些子点已掌握后的局部路径变化。
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

          <div class="detail-path-result-grid">
            <article class="detail-path-card">
              <h4>本轮推荐学习</h4>
              <ol class="detail-path-list">
                <li
                  v-for="item in detailScheduledItems"
                  :key="`detail-scheduled-${item.code}`"
                  class="detail-path-item"
                >
                  <div class="detail-path-item-head">
                    <strong>{{ item.name }}</strong>
                    <span>{{ item.estimatedMinutes }} 分钟</span>
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
              <h4>下一轮建议</h4>
              <ul class="detail-path-list detail-path-list--plain">
                <li
                  v-for="item in detailDeferredItems"
                  :key="`detail-deferred-${item.code}`"
                  class="detail-path-item"
                >
                  <div class="detail-path-item-head">
                    <strong>{{ item.name }}</strong>
                    <span>{{ item.estimatedMinutes }} 分钟</span>
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
            <div class="detail-path-head detail-path-head--nested">
              <div>
                <p class="label">学习反馈</p>
                <h4>根据本范围学习情况重新规划</h4>
              </div>
              <p class="caption">
                只对“本轮推荐学习”中的细化节点提交反馈，系统会按相同调整规则即时重算当前 scope 的局部路径。
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

                <div class="detail-feedback-fields">
                  <label class="field">
                    <span>完成情况</span>
                    <select
                      v-model="detailFeedbackDraftByCode[item.code].completionStatus"
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
                        v-model.number="detailFeedbackDraftByCode[item.code].selfRatedMastery"
                        type="range"
                        min="0"
                        max="100"
                        step="5"
                      />
                      <strong>{{ detailFeedbackDraftByCode[item.code].selfRatedMastery }}%</strong>
                    </div>
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

          <article
            v-if="detailAdjustmentSummary"
            class="detail-adjustment-panel"
          >
            <div class="detail-path-head detail-path-head--nested">
              <div>
                <p class="label">操作摘要</p>
                <h4>最近一次局部反馈结果</h4>
              </div>
              <p class="caption">用于快速查看本范围学习反馈对掌握度和后续安排的影响。</p>
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

          <article
            v-if="detailPathComparison"
            class="detail-comparison-panel"
          >
            <div class="detail-path-head detail-path-head--nested">
              <div>
                <p class="label">路径变化</p>
                <h4>局部路径前后对比</h4>
              </div>
              <p class="caption">
                用于观察本次局部反馈后，当前 scope 内的本轮学习安排如何变化。
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
                <h4>{{ detailComparisonBeforeTitle }}</h4>
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
                    <p class="detail-path-item-meta">
                      掌握度 {{ item.masteryPercent }}%
                    </p>
                  </li>
                </ol>
              </article>

              <article class="detail-path-card">
                <h4>{{ detailComparisonAfterTitle }}</h4>
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
                    <p class="detail-path-item-meta">
                      掌握度 {{ item.masteryPercent }}%
                    </p>
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
                  <span
                    class="change-badge"
                    :class="changeBadgeClass(item.changeType)"
                  >
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
              本次局部反馈没有改变当前 scope 的本轮安排顺序，可继续学习后再观察下一次变化。
            </p>
          </article>

          <article
            v-if="detailResourceRecommendations.length > 0"
            class="detail-resource-panel"
          >
            <div class="detail-path-head detail-path-head--nested">
              <div>
                <p class="label">资源建议</p>
                <h4>本范围配套资源</h4>
              </div>
              <p class="caption">
                优先展示与当前细化节点直接相关的资源；若已配置 focusNodeCode，会自动对齐到对应子节点。
              </p>
            </div>

            <div class="detail-resource-sections">
              <section
                v-for="section in detailResourceRecommendations"
                :key="`detail-resource-${section.code}`"
                class="detail-resource-section"
              >
                  <div class="relation-head">
                    <div>
                      <strong>{{ section.name }}</strong>
                      <p class="detail-path-item-meta">
                        {{ section.status === "scheduled" ? "本轮推荐学习" : "下一轮建议" }}
                        · {{ section.resourceCount }} 条资源
                      </p>
                      <p
                        v-if="section.primaryResourceTitle"
                        class="detail-resource-primary-summary"
                      >
                        {{ section.primaryResourcePriorityLabel || "优先资源" }}：{{ section.primaryResourceTitle }}
                      </p>
                      <ul
                        v-if="getDetailSectionResourceLayerSummary(section).length > 0"
                        class="detail-resource-layer-summary"
                      >
                        <li
                          v-for="item in getDetailSectionResourceLayerSummary(section)"
                          :key="`${section.code}-${item.layer}`"
                          class="detail-resource-layer-chip"
                          :class="detailResourceLayerClass(item.layer)"
                        >
                          <strong>{{ item.layer }}</strong>
                          <span>{{ item.count }}</span>
                        </li>
                      </ul>
                    </div>
                    <span class="resource-usage-note">{{ section.recommendedUsage }}</span>
                  </div>

                <ul class="detail-resource-list">
                  <li
                    v-for="resource in section.resources"
                    :key="`${section.code}-${resource.url}`"
                    class="detail-resource-item"
                  >
                    <div class="detail-path-item-head">
                      <a
                        :href="resource.url"
                        target="_blank"
                        rel="noreferrer"
                        class="resource-link"
                      >
                        {{ resource.title }}
                      </a>
                      <div class="detail-resource-badges">
                        <span
                          v-if="resource.isPrimaryRecommendation"
                          class="detail-resource-top-badge"
                        >
                          优先看
                        </span>
                        <span
                          v-if="resource.resourceLayer"
                          class="detail-resource-layer-badge"
                          :class="detailResourceLayerClass(resource.resourceLayer)"
                        >
                          {{ resource.resourceLayer }}
                        </span>
                        <span class="node-badge node-badge--resource">
                          {{ formatResourceType(resource.type) }}
                        </span>
                      </div>
                    </div>
                    <p class="detail-path-item-meta">
                      {{ resource.source }}
                    </p>
                    <p class="detail-path-item-reason">
                      {{ resource.description }}
                    </p>
                    <p
                      v-if="resource.selectionReason"
                      class="detail-path-item-reason detail-path-item-reason--strong"
                    >
                      选择原因：{{ resource.selectionReason }}
                    </p>
                    <p
                      v-if="resource.resourceLayerHint"
                      class="detail-path-item-reason"
                    >
                      分层说明：{{ resource.resourceLayerHint }}
                    </p>
                  </li>
                </ul>
              </section>
            </div>
          </article>
        </template>
      </section>
    </template>
  </section>
</template>

<script setup>
import { computed, nextTick, onBeforeUnmount, onMounted, ref } from "vue";

import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import { adjustDetailLearningPath, generateDetailLearningPath } from "../api/path";

const graphRef = ref(null);
const isLoading = ref(true);
const loadError = ref("");
const graphData = ref(null);
const selectedNodeCode = ref("");
const currentScopeCode = ref("root");
const detailMinuteOptions = [20, 30, 40, 60, 90, 120];
const resourceTypeLabelMap = {
  video: "视频教程",
  article: "文本教程",
  document: "参考资料",
};
const resourceLayerOrder = ["课程风格优先", "图文补充", "答辩复盘"];
const detailPlanning = ref(false);
const detailPlanError = ref("");
const detailPlanResult = ref(null);
const detailAvailableMinutes = ref(40);
const detailTargetCode = ref("");
const showDetailMasteryEditor = ref(false);
const detailMasteryPercentByCode = ref({});
const detailMasterySnapshotsByScope = ref({});
const detailTargetByScope = ref({});
const detailAvailableMinutesByScope = ref({});
const detailAdjustmentStatusLabelMap = {
  completed: "已完成",
  partial: "部分完成",
  blocked: "学习受阻",
};
const detailFeedbackDraftByCode = ref({});
const detailAdjusting = ref(false);
const detailAdjustmentError = ref("");
const detailAdjustmentSummary = ref(null);
const detailAdjustmentItems = ref([]);
const detailPathComparison = ref(null);
let cyInstance = null;

const title = computed(() => {
  if (graphData.value?.view?.scopeCode && graphData.value.view.scopeCode !== "root") {
    return `${graphData.value.view.scopeName}细化图谱`;
  }

  if (graphData.value?.course?.name) {
    return `${graphData.value.course.name}课程主线`;
  }

  return "数据结构课程主线";
});

const caption = computed(() => {
  if (isLoading.value) {
    return "真实课程数据加载中";
  }

  if (loadError.value) {
    return "接口加载失败";
  }

  if (graphData.value?.view?.sourceReference) {
    return graphData.value.view.sourceReference;
  }

  return `${graphData.value.meta.knowledgePointCount} 个知识点 / ${graphData.value.meta.dependencyCount} 条依赖边`;
});

const breadcrumbs = computed(() => graphData.value?.breadcrumbs || []);

const scopeNote = computed(() => {
  if (!graphData.value?.view) {
    return "";
  }

  if (graphData.value.view.scopeCode === "root") {
    return "双击带“可细化”标记的节点，可进入下一层知识图谱。";
  }

  return (
    graphData.value.view.description ||
    "当前正在查看细化图谱，可通过上方面包屑返回任意上层。"
  );
});

const isDetailScope = computed(() => currentScopeCode.value !== "root");

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

    if (left.displayOrder !== right.displayOrder) {
      return left.displayOrder - right.displayOrder;
    }

    return left.code.localeCompare(right.code);
  });

  return nodes;
});

const selectedNode = computed(() => {
  if (!graphData.value?.nodes?.length || !selectedNodeCode.value) {
    return null;
  }

  return (
    graphData.value.nodes.find((node) => node.code === selectedNodeCode.value) ||
    null
  );
});

const directPrerequisites = computed(() => {
  if (!selectedNode.value || !graphData.value?.edges?.length) {
    return [];
  }

  const prerequisiteCodes = graphData.value.edges
    .filter((edge) => edge.target === selectedNode.value.code)
    .map((edge) => edge.source);

  return graphData.value.nodes.filter((node) => prerequisiteCodes.includes(node.code));
});

const directDependents = computed(() => {
  if (!selectedNode.value || !graphData.value?.edges?.length) {
    return [];
  }

  const dependentCodes = graphData.value.edges
    .filter((edge) => edge.source === selectedNode.value.code)
    .map((edge) => edge.target);

  return graphData.value.nodes.filter((node) => dependentCodes.includes(node.code));
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
    return "当前时间预算足以覆盖本范围目标链路，可直接作为该主节点内部的执行计划。";
  }

  return "当前时间预算不足以完成本范围目标链路，系统已把部分细化节点顺延到下一轮。";
});

const detailComparisonBeforeTitle = "调整前本轮安排";
const detailComparisonAfterTitle = "调整后本轮安排";

const nodeTypeLabelMap = {
  foundation: "基础概念",
  structure: "核心结构",
  implementation: "实现方法",
  algorithm: "算法方法",
  application: "应用场景",
  abstraction: "抽象建模",
};

function formatResourceType(type) {
  return resourceTypeLabelMap[type] || "学习资源";
}

function detailResourceLayerClass(layer) {
  return {
    "detail-resource-layer--primary": layer === "课程风格优先",
    "detail-resource-layer--supplement": layer === "图文补充",
    "detail-resource-layer--review": layer === "答辩复盘",
  };
}

function getDetailSectionResourceLayerSummary(section) {
  return resourceLayerOrder
    .map((layer) => ({
      layer,
      count: Number(section?.resourceLayerSummary?.[layer] || 0),
    }))
    .filter((item) => item.count > 0);
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

function resolveDefaultDetailTargetCode(nodes = [], edges = []) {
  if (!Array.isArray(nodes) || nodes.length === 0) {
    return "";
  }

  const outdegreeByCode = Object.fromEntries(nodes.map((node) => [node.code, 0]));
  edges.forEach((edge) => {
    if (Object.hasOwn(outdegreeByCode, edge.source)) {
      outdegreeByCode[edge.source] += 1;
    }
  });

  const sortedNodes = [...nodes].sort((left, right) => {
    if (left.displayOrder !== right.displayOrder) {
      return left.displayOrder - right.displayOrder;
    }

    return left.code.localeCompare(right.code);
  });
  const terminalNodes = sortedNodes.filter((node) => outdegreeByCode[node.code] === 0);

  return terminalNodes.at(-1)?.code || sortedNodes.at(-1)?.code || "";
}

function createDefaultDetailMastery(nodes = []) {
  return Object.fromEntries(nodes.map((node) => [node.code, 0]));
}

function initializeDetailPlannerState(payload) {
  const scopeCode = payload?.view?.scopeCode || "root";
  if (scopeCode === "root") {
    detailPlanResult.value = null;
    detailPlanError.value = "";
    clearDetailOperationOutputs();
    detailTargetCode.value = "";
    detailMasteryPercentByCode.value = {};
    detailFeedbackDraftByCode.value = {};
    showDetailMasteryEditor.value = false;
    return;
  }

  const storedMasterySnapshot = detailMasterySnapshotsByScope.value[scopeCode];
  detailMasteryPercentByCode.value =
    storedMasterySnapshot || createDefaultDetailMastery(payload.nodes || []);

  const defaultTargetCode = resolveDefaultDetailTargetCode(
    payload.nodes || [],
    payload.edges || [],
  );
  detailTargetCode.value =
    detailTargetByScope.value[scopeCode] || defaultTargetCode;
  detailAvailableMinutes.value = detailAvailableMinutesByScope.value[scopeCode] || 40;
  detailPlanResult.value = null;
  detailPlanError.value = "";
  clearDetailOperationOutputs();
  detailFeedbackDraftByCode.value = {};
}

function buildDetailMasteryPayload() {
  return Object.fromEntries(
    Object.entries(detailMasteryPercentByCode.value).map(([code, percent]) => [
      code,
      Number(percent || 0) / 100,
    ]),
  );
}

function saveDetailPlannerDrafts() {
  if (!isDetailScope.value) {
    return;
  }

  detailMasterySnapshotsByScope.value = {
    ...detailMasterySnapshotsByScope.value,
    [currentScopeCode.value]: { ...detailMasteryPercentByCode.value },
  };
  detailTargetByScope.value = {
    ...detailTargetByScope.value,
    [currentScopeCode.value]: detailTargetCode.value,
  };
  detailAvailableMinutesByScope.value = {
    ...detailAvailableMinutesByScope.value,
    [currentScopeCode.value]: detailAvailableMinutes.value,
  };
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

  if (status === "not-in-path") {
    return "未进入路径";
  }

  return status;
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

function changeBadgeClass(changeType) {
  return {
    "change-badge--mastered": changeType === "mastered",
    "change-badge--advanced": changeType === "advanced",
    "change-badge--deferred": changeType === "deferred",
    "change-badge--reordered": changeType === "reordered",
  };
}

function syncDetailFeedbackDrafts() {
  detailFeedbackDraftByCode.value = Object.fromEntries(
    detailScheduledItems.value.map((item) => [
      item.code,
      {
        completionStatus: "completed",
        selfRatedMastery: Math.max(item.masteryPercent, 85),
      },
    ]),
  );
}

function clearDetailOperationOutputs() {
  detailAdjustmentError.value = "";
  detailAdjustmentSummary.value = null;
  detailAdjustmentItems.value = [];
  detailPathComparison.value = null;
}

function buildDetailFeedbackItems() {
  return detailScheduledItems.value.map((item) => ({
    code: item.code,
    completionStatus:
      detailFeedbackDraftByCode.value[item.code]?.completionStatus || "completed",
    selfRatedMastery:
      Number(detailFeedbackDraftByCode.value[item.code]?.selfRatedMastery || 0) / 100,
  }));
}

function applyDetailMasterySnapshot(snapshot) {
  detailMasteryPercentByCode.value = Object.fromEntries(
    detailTargetOptions.value.map((node) => [
      node.code,
      Math.round(Number(snapshot?.[node.code] ?? 0) * 100),
    ]),
  );
  saveDetailPlannerDrafts();
}

function buildElements(payload) {
  const nodes = payload.nodes.map((node) => ({
    data: {
      id: node.id,
      label: node.label,
      chapterLabel: `第${node.chapterNo}章`,
      nodeType: node.nodeType,
      difficultyLevel: node.difficultyLevel,
      hasDetailGraph: node.hasDetailGraph ? 1 : 0,
      detailScopeCode: node.detailScopeCode || "",
    },
  }));

  const edges = payload.edges.map((edge) => ({
    data: {
      id: edge.id,
      source: edge.source,
      target: edge.target,
      relationType: edge.relationType,
    },
  }));

  return [...nodes, ...edges];
}

function refreshGraphState() {
  if (!cyInstance) {
    return;
  }

  cyInstance.elements().removeClass(
    "is-selected is-context is-context-edge is-muted is-path-scheduled is-path-deferred is-path-mastered is-path-target",
  );

  if (selectedNodeCode.value) {
    const targetNode = cyInstance.getElementById(selectedNodeCode.value);
    if (targetNode && !targetNode.empty()) {
      const contextElements = targetNode
        .union(targetNode.connectedEdges())
        .union(targetNode.predecessors())
        .union(targetNode.successors());

      cyInstance.elements().addClass("is-muted");
      contextElements.removeClass("is-muted");
      contextElements.nodes().addClass("is-context");
      contextElements.edges().addClass("is-context-edge");
      targetNode.removeClass("is-muted");
      targetNode.removeClass("is-context");
      targetNode.addClass("is-selected");
    }
  }

  for (const item of detailPlanResult.value?.path || []) {
    const node = cyInstance.getElementById(item.code);
    if (!node || node.empty()) {
      continue;
    }

    if (item.status === "scheduled") {
      node.addClass("is-path-scheduled");
    } else if (item.status === "deferred") {
      node.addClass("is-path-deferred");
    } else if (item.status === "mastered") {
      node.addClass("is-path-mastered");
    }

    if (item.isTarget) {
      node.addClass("is-path-target");
    }
  }
}

async function renderGraph(payload) {
  if (!graphRef.value) {
    return;
  }

  if (cyInstance) {
    cyInstance.destroy();
  }

  const elements = buildElements(payload);
  const cytoscape = (await import("cytoscape")).default;

  cyInstance = cytoscape({
    container: graphRef.value,
    elements,
    layout: {
      name: "breadthfirst",
      directed: true,
      padding: 28,
      spacingFactor: 1.45,
      avoidOverlap: true,
      nodeDimensionsIncludeLabels: true,
    },
    style: [
      {
        selector: "node",
        style: {
          label: "data(label)",
          "background-color": "#0c6a71",
          color: "#ffffff",
          "font-size": "12px",
          "text-valign": "center",
          "text-halign": "center",
          width: 92,
          height: 92,
          "text-wrap": "wrap",
          "text-max-width": 78,
          "border-width": 3,
          "border-color": "#d9f1ef",
          "overlay-opacity": 0,
        },
      },
      {
        selector: "node[nodeType = 'foundation']",
        style: {
          "background-color": "#205072",
        },
      },
      {
        selector: "node[nodeType = 'structure']",
        style: {
          "background-color": "#0c6a71",
        },
      },
      {
        selector: "node[nodeType = 'implementation']",
        style: {
          "background-color": "#3b7d3a",
        },
      },
      {
        selector: "node[nodeType = 'algorithm']",
        style: {
          "background-color": "#d97706",
        },
      },
      {
        selector: "node[nodeType = 'application']",
        style: {
          "background-color": "#b45309",
        },
      },
      {
        selector: "node[nodeType = 'abstraction']",
        style: {
          "background-color": "#6b5ca5",
        },
      },
      {
        selector: "node[hasDetailGraph = 1]",
        style: {
          "border-style": "dashed",
          "border-width": 4,
          "border-color": "#f7b267",
        },
      },
      {
        selector: "edge",
        style: {
          width: 3,
          "line-color": "#ff9c48",
          "target-arrow-color": "#ff9c48",
          "target-arrow-shape": "triangle",
          "curve-style": "round-taxi",
          "taxi-direction": "downward",
          "taxi-turn": "38%",
          "taxi-turn-min-distance": 26,
          "edge-distances": "node-position",
          "overlay-opacity": 0,
          "arrow-scale": 1.05,
        },
      },
      {
        selector: "node.is-selected",
        style: {
          "border-width": 5,
          "border-color": "#ff9c48",
          "border-opacity": 1,
          opacity: 1,
          "text-opacity": 1,
          "z-index": 999,
        },
      },
      {
        selector: "node.is-context",
        style: {
          opacity: 0.96,
          "text-opacity": 1,
          "border-width": 4,
          "border-color": "#8fd6cc",
          "border-opacity": 0.95,
        },
      },
      {
        selector: "edge.is-context-edge",
        style: {
          width: 4,
          opacity: 0.92,
          "line-color": "#f08c2e",
          "target-arrow-color": "#f08c2e",
          "z-index": 998,
        },
      },
      {
        selector: "node.is-muted",
        style: {
          opacity: 0.22,
          "text-opacity": 0.35,
          "border-opacity": 0.18,
        },
      },
      {
        selector: "edge.is-muted",
        style: {
          opacity: 0.12,
        },
      },
      {
        selector: "node.is-path-scheduled",
        style: {
          opacity: 1,
          "text-opacity": 1,
          "border-width": 5,
          "border-color": "#f7b267",
          "border-opacity": 1,
        },
      },
      {
        selector: "node.is-path-deferred",
        style: {
          opacity: 0.9,
          "text-opacity": 1,
          "border-width": 5,
          "border-style": "dashed",
          "border-color": "#f59e0b",
          "border-opacity": 1,
        },
      },
      {
        selector: "node.is-path-mastered",
        style: {
          opacity: 0.95,
          "text-opacity": 1,
          "border-width": 5,
          "border-color": "#2f855a",
          "border-opacity": 1,
        },
      },
      {
        selector: "node.is-path-target",
        style: {
          "border-width": 6,
          "border-color": "#ef4444",
          "border-opacity": 1,
        },
      },
    ],
  });

  cyInstance.on("tap", "node", (event) => {
    const selectedCode = event.target.id();
    selectNode(selectedCode);
  });

  cyInstance.on("dbltap", "node", (event) => {
    const targetNode = event.target;
    const detailScopeCode = targetNode.data("detailScopeCode");
    if (!detailScopeCode) {
      return;
    }

    navigateToScope(detailScopeCode);
  });

  if (payload.nodes.length > 0) {
    selectNode(payload.nodes[0].code);
  }
}

function selectNode(code) {
  selectedNodeCode.value = code;
  refreshGraphState();
}

function resolveNodeTypeLabel(nodeType) {
  return nodeTypeLabelMap[nodeType] || nodeType;
}

async function loadGraph(scopeCode = "root") {
  isLoading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchKnowledgeGraph(
      scopeCode && scopeCode !== "root" ? { scopeCode } : {},
    );
    graphData.value = payload;
    currentScopeCode.value = payload?.view?.scopeCode || "root";
    selectedNodeCode.value = "";
    isLoading.value = false;
    await nextTick();
    await renderGraph(payload);
    initializeDetailPlannerState(payload);

    if ((payload?.view?.scopeCode || "root") !== "root") {
      await submitDetailPlan();
    }
  } catch (error) {
    isLoading.value = false;
    loadError.value =
      "未能加载真实知识图谱，请先执行数据库初始化脚本并确认后端服务已启动。";
    console.error(error);
  }
}

function navigateToScope(scopeCode) {
  loadGraph(scopeCode || "root");
}

function toggleDetailMasteryEditor() {
  showDetailMasteryEditor.value = !showDetailMasteryEditor.value;
}

function applySelectedNodeAsDetailTarget() {
  if (!selectedNode.value) {
    return;
  }

  detailTargetCode.value = selectedNode.value.code;
}

async function submitDetailPlan() {
  if (!isDetailScope.value) {
    return;
  }

  detailPlanning.value = true;
  detailPlanError.value = "";
  clearDetailOperationOutputs();
  saveDetailPlannerDrafts();

  try {
    const targetNodeCode =
      detailTargetCode.value ||
      resolveDefaultDetailTargetCode(graphData.value?.nodes || [], graphData.value?.edges || []);
    detailTargetCode.value = targetNodeCode;

    const payload = await generateDetailLearningPath({
      scopeCode: currentScopeCode.value,
      targetNodeCode,
      availableMinutes: detailAvailableMinutes.value,
      masteryByCode: buildDetailMasteryPayload(),
    });
    detailPlanResult.value = payload;
    syncDetailFeedbackDrafts();
    refreshGraphState();
  } catch (error) {
    detailPlanError.value =
      error?.response?.data?.detail || "本范围学习路径生成失败，请检查后端日志。";
    detailPlanResult.value = null;
    refreshGraphState();
    console.error(error);
  } finally {
    detailPlanning.value = false;
  }
}

async function submitDetailAdjustment() {
  if (!isDetailScope.value || detailScheduledItems.value.length === 0) {
    return;
  }

  detailAdjusting.value = true;
  detailAdjustmentError.value = "";
  const beforePlanSnapshot = clonePlanSnapshot(detailPlanResult.value);
  saveDetailPlannerDrafts();

  try {
    const payload = await adjustDetailLearningPath({
      scopeCode: currentScopeCode.value,
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
    refreshGraphState();
  } catch (error) {
    detailAdjustmentError.value =
      error?.response?.data?.detail || "本范围学习反馈调整失败，请检查后端日志。";
    console.error(error);
  } finally {
    detailAdjusting.value = false;
  }
}

onMounted(async () => {
  await loadGraph();
});

onBeforeUnmount(() => {
  if (cyInstance) {
    cyInstance.destroy();
    cyInstance = null;
  }
});
</script>

<style scoped>
.card {
  background: rgba(255, 255, 255, 0.8);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
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

h2 {
  margin: 0;
  font-size: 1.3rem;
}

.caption {
  margin: 0;
  color: #66727d;
  font-size: 0.9rem;
}

.meta-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
  margin-top: 18px;
}

.scope-bar {
  display: grid;
  gap: 12px;
  margin-top: 18px;
  padding: 16px 18px;
  border-radius: 18px;
  background: rgba(244, 248, 247, 0.86);
}

.breadcrumb-list {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.breadcrumb-button,
.drilldown-button {
  border: none;
  border-radius: 999px;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease;
}

.breadcrumb-button {
  padding: 8px 14px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-weight: 700;
}

.breadcrumb-button--active {
  background: #0c6a71;
  color: #ffffff;
}

.scope-note {
  margin: 0;
  color: #44515c;
  line-height: 1.6;
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

.graph-canvas {
  width: 100%;
  min-height: clamp(420px, 44vw, 620px);
  border-radius: 18px;
  background:
    linear-gradient(180deg, rgba(12, 106, 113, 0.08), rgba(12, 106, 113, 0.02)),
    rgba(244, 248, 247, 0.7);
}

.graph-layout {
  display: grid;
  grid-template-columns: minmax(0, 1.4fr) minmax(280px, 0.9fr);
  gap: 16px;
  margin-top: 18px;
}

.detail-path-panel {
  margin-top: 18px;
  border-radius: 22px;
  background: rgba(247, 250, 249, 0.86);
  padding: 20px;
}

.detail-path-head {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: baseline;
}

.detail-path-head--nested {
  margin-bottom: 16px;
}

.detail-path-form {
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
  line-height: 1.6;
}

.detail-form-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
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
  font-weight: 700;
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

.ghost-button:disabled,
.submit-button:disabled {
  opacity: 0.65;
  cursor: not-allowed;
}

.detail-mastery-grid {
  grid-column: 1 / -1;
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
}

.detail-mastery-item {
  display: grid;
  gap: 8px;
  padding: 14px;
  border-radius: 18px;
  background: rgba(255, 255, 255, 0.92);
}

.slider-row {
  display: grid;
  grid-template-columns: minmax(0, 1fr) auto;
  gap: 12px;
  align-items: center;
}

.detail-summary-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
  margin-top: 20px;
}

.detail-summary-grid div {
  padding: 14px;
  border-radius: 18px;
  background: rgba(255, 255, 255, 0.92);
}

.detail-result-message {
  margin: 18px 0 0;
  padding: 14px 16px;
  border-radius: 16px;
  font-weight: 600;
}

.detail-result-message--ok {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.detail-result-message--warn {
  background: rgba(186, 93, 23, 0.12);
  color: #8a4d10;
}

.detail-path-result-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px;
  margin-top: 18px;
}

.detail-path-card,
.detail-resource-panel,
.detail-feedback-panel,
.detail-adjustment-panel,
.detail-comparison-panel {
  border-radius: 20px;
  background: rgba(255, 255, 255, 0.92);
  padding: 18px;
}

.detail-resource-panel,
.detail-feedback-panel,
.detail-adjustment-panel,
.detail-comparison-panel {
  margin-top: 18px;
}

.detail-path-list {
  display: grid;
  gap: 12px;
  margin: 16px 0 0;
  padding-left: 20px;
}

.detail-path-list--plain {
  padding-left: 0;
  list-style: none;
}

.detail-path-item {
  border-radius: 16px;
  background: rgba(244, 248, 247, 0.86);
  padding: 14px;
}

.detail-path-item-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: start;
}

.detail-path-item-meta,
.detail-path-item-reason {
  margin: 8px 0 0;
  color: #44515c;
  line-height: 1.55;
}

.detail-feedback-form,
.detail-adjustment-list {
  display: grid;
  gap: 14px;
  margin-top: 18px;
}

.detail-adjustment-list {
  padding: 0;
  list-style: none;
}

.detail-feedback-item {
  border-radius: 20px;
  background: rgba(247, 250, 249, 0.86);
  padding: 16px;
}

.detail-feedback-fields {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 14px;
  margin-top: 14px;
}

.detail-summary-grid--compact {
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

.detail-path-item-reason--strong {
  color: #214861;
  font-weight: 600;
}

.detail-resource-sections {
  display: grid;
  gap: 14px;
}

.detail-resource-section {
  border-radius: 18px;
  background: rgba(244, 248, 247, 0.86);
  padding: 16px;
}

.detail-resource-primary-summary {
  margin: 8px 0 0;
  color: #214861;
  font-size: 0.92rem;
  font-weight: 600;
  line-height: 1.5;
}

.detail-resource-layer-summary {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 10px 0 0;
  padding: 0;
  list-style: none;
}

.detail-resource-layer-chip,
.detail-resource-layer-badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 4px 10px;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 700;
}

.detail-resource-list {
  display: grid;
  gap: 12px;
  margin: 14px 0 0;
  padding: 0;
  list-style: none;
}

.detail-resource-item {
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.92);
  padding: 14px;
}

.detail-resource-badges {
  display: flex;
  align-items: center;
  gap: 8px;
  flex-wrap: wrap;
  justify-content: end;
}

.detail-resource-layer--primary {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.detail-resource-layer--supplement {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.detail-resource-layer--review {
  background: rgba(55, 82, 129, 0.12);
  color: #27456f;
}

.resource-link {
  color: #0c5960;
  font-weight: 700;
  text-decoration: none;
}

.resource-link:hover {
  text-decoration: underline;
}

.resource-usage-note {
  max-width: 320px;
  color: #53616c;
  font-size: 0.9rem;
  line-height: 1.5;
}

.detail-resource-top-badge {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  background: rgba(225, 147, 35, 0.16);
  color: #8a4d10;
  font-size: 0.82rem;
  font-weight: 700;
}

.resource-phase,
.resource-focus-node,
.resource-focus-tag {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 700;
}

.resource-phase {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.resource-focus-node {
  background: rgba(55, 82, 129, 0.12);
  color: #27456f;
}

.resource-focus-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 10px 0 0;
  padding: 0;
  list-style: none;
}

.resource-focus-tag {
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
}

.detail-panel {
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.86);
  padding: 18px;
}

.detail-head,
.relation-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: start;
}

.label--detail {
  margin-bottom: 4px;
}

h3,
h4 {
  margin: 0;
}

.node-badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-size: 0.88rem;
  font-weight: 700;
}

.node-badge--resource {
  font-size: 0.82rem;
}

.detail-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 10px;
  margin-top: 16px;
}

.detail-grid div {
  padding: 12px 14px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.92);
}

.detail-description {
  margin: 16px 0 0;
  color: #44515c;
  line-height: 1.65;
}

.detail-description--compact {
  margin-top: 12px;
}

.relation-section {
  margin-top: 18px;
}

.relation-section--drilldown {
  padding: 14px 16px;
  border-radius: 18px;
  background: rgba(255, 255, 255, 0.92);
}

.relation-head span {
  color: #66727d;
  font-size: 0.88rem;
}

.relation-list {
  display: grid;
  gap: 10px;
  margin: 14px 0 0;
  padding: 0;
  list-style: none;
}

.relation-item {
  padding: 12px 14px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.92);
}

.relation-item p,
.empty-tip {
  margin: 6px 0 0;
  color: #44515c;
}

.empty-tip--panel {
  margin-top: 0;
  line-height: 1.6;
}

.drilldown-button {
  margin-top: 14px;
  padding: 10px 16px;
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  font-weight: 700;
}

.breadcrumb-button:hover,
.drilldown-button:hover {
  transform: translateY(-1px);
}

@media (max-width: 640px) {
  .card-head {
    flex-direction: column;
    align-items: start;
  }

  .meta-grid,
  .detail-summary-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}

@media (max-width: 980px) {
  .graph-layout,
  .detail-grid,
  .detail-path-form,
  .detail-mastery-grid,
  .detail-path-result-grid,
  .detail-summary-grid,
  .detail-feedback-fields {
    grid-template-columns: 1fr;
  }

  .detail-path-head,
  .field-head,
  .detail-path-item-head,
  .relation-head {
    flex-direction: column;
    align-items: start;
  }

  .detail-resource-badges {
    justify-content: start;
  }
}
</style>
