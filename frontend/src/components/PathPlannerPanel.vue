<template>
  <section class="planner-fragment">
    <article class="card planner-core-card">
      <div class="card-head">
        <div>
          <p class="label">路径规划</p>
          <h2>最小版个性化学习导航</h2>
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
              <h3>资源、反馈与路径变化</h3>
            </div>
            <p class="caption">
              首屏聚焦“图谱 + 路径”，下方用于查看配套资源、提交学习反馈，并观察路径如何变化。
            </p>
          </div>
          <ul class="planner-extension-chips">
            <li class="planner-extension-chip">资源建议</li>
            <li class="planner-extension-chip">学习反馈</li>
            <li class="planner-extension-chip">操作摘要</li>
            <li class="planner-extension-chip">路径变化</li>
          </ul>
        </article>

        <div class="planner-extension-columns">
          <div class="planner-extension-main">
            <article class="card planner-section-card resource-recommendation-card">
              <div class="section-headline">
                <div>
                  <p class="label">资源建议</p>
                  <h3>当前学习导航配套资源</h3>
                </div>
                <p class="caption">系统会根据本轮与下一轮节点，给出可直接使用的视频和文本教程。</p>
              </div>

              <div
                v-if="resourceRecommendations.length === 0"
                class="empty-tip"
              >
                当前路径中的节点还没有配置资源建议，可先查看推荐理由与掌握度变化。
              </div>

              <div v-else>
                <div class="resource-toolbar">
                  <div class="resource-toolbar-main">
                    <div class="resource-filter-tabs">
                      <button
                        v-for="option in resourceFilterOptions"
                        :key="`resource-filter-${option.value}`"
                        type="button"
                        class="resource-filter-tab"
                        :class="{
                          'resource-filter-tab--active': resourceFilter === option.value,
                        }"
                        @click="resourceFilter = option.value"
                      >
                        {{ option.label }}
                        <span>{{ getResourceFilterCount(option.value) }}</span>
                      </button>
                    </div>
                    <ul class="resource-layer-overview">
                      <li
                        v-for="item in resourceLayerOverview"
                        :key="`resource-layer-overview-${item.layer}`"
                        class="resource-layer-chip"
                        :class="resourceLayerClass(item.layer)"
                      >
                        <strong>{{ item.layer }}</strong>
                        <span>{{ item.count }} 条</span>
                      </li>
                    </ul>
                  </div>
                  <p class="resource-toolbar-tip">
                    当前显示 {{ visibleResourceRecommendations.length }} 组资源，默认优先展开本轮推荐学习。
                  </p>
                </div>

                <div
                  v-if="visibleResourceRecommendations.length === 0"
                  class="empty-tip resource-empty-tip"
                >
                  当前筛选条件下暂无资源建议，可切换到其他分组查看。
                </div>

                <div v-else class="resource-sections">
                  <section
                    v-for="section in visibleResourceRecommendations"
                    :key="`resource-${section.code}`"
                    class="resource-section"
                  >
                    <div class="resource-section-head">
                      <div>
                        <div class="resource-section-title-row">
                          <strong>{{ section.name }}</strong>
                          <span
                            class="resource-section-status"
                            :class="{
                              'resource-section-status--scheduled':
                                section.status === 'scheduled',
                              'resource-section-status--deferred':
                                section.status === 'deferred',
                            }"
                          >
                            {{ formatResourceSectionStatus(section.status) }}
                          </span>
                        </div>
                        <p class="path-item-meta">
                          {{ section.resourceCount }} 条资源
                        </p>
                        <p
                          v-if="section.primaryResourceTitle"
                          class="resource-primary-summary"
                        >
                          {{ section.primaryResourcePriorityLabel || "优先资源" }}：{{ section.primaryResourceTitle }}
                        </p>
                        <ul
                          v-if="getSectionResourceLayerSummary(section).length > 0"
                          class="resource-layer-summary"
                        >
                          <li
                            v-for="item in getSectionResourceLayerSummary(section)"
                            :key="`${section.code}-${item.layer}`"
                            class="resource-layer-chip resource-layer-chip--compact"
                            :class="resourceLayerClass(item.layer)"
                          >
                            <strong>{{ item.layer }}</strong>
                            <span>{{ item.count }}</span>
                          </li>
                        </ul>
                      </div>
                      <div class="resource-section-actions">
                        <span class="resource-section-usage">
                          {{ section.recommendedUsage }}
                        </span>
                        <button
                          type="button"
                          class="resource-toggle"
                          @click="toggleResourceSection(section.code)"
                        >
                          {{ isResourceSectionExpanded(section) ? "收起资源" : "展开资源" }}
                        </button>
                      </div>
                    </div>

                    <ul
                      v-if="isResourceSectionExpanded(section)"
                      class="resource-list"
                    >
                      <li
                        v-for="resource in section.resources"
                        :key="`${section.code}-${resource.url}`"
                        class="resource-item"
                      >
                        <div class="resource-item-head">
                          <a
                            :href="resource.url"
                            target="_blank"
                            rel="noreferrer"
                            class="resource-link"
                          >
                            {{ resource.title }}
                          </a>
                          <div class="resource-item-badges">
                            <span
                              v-if="resource.isPrimaryRecommendation"
                              class="resource-top-badge"
                            >
                              优先看
                            </span>
                            <span
                              v-if="resource.resourceLayer"
                              class="resource-layer-badge"
                              :class="resourceLayerClass(resource.resourceLayer)"
                            >
                              {{ resource.resourceLayer }}
                            </span>
                            <span class="resource-type">
                              {{ formatResourceType(resource.type) }}
                            </span>
                          </div>
                        </div>
                        <div class="resource-tag-row">
                          <span
                            v-if="resource.recommendedPhase"
                            class="resource-phase"
                          >
                            {{ resource.recommendedPhase }}
                          </span>
                          <span
                            v-if="resource.focusNodeLabel"
                            class="resource-focus-node"
                          >
                            对应细化节点：{{ resource.focusNodeLabel }}
                          </span>
                        </div>
                        <ul
                          v-if="getResourceFocusTags(resource).length > 0"
                          class="resource-focus-tags"
                        >
                          <li
                            v-for="tag in getResourceFocusTags(resource)"
                            :key="`${section.code}-${resource.url}-${tag}`"
                            class="resource-focus-tag"
                          >
                            {{ tag }}
                          </li>
                        </ul>
                        <p class="resource-meta">{{ resource.source }}</p>
                        <p class="resource-description">{{ resource.description }}</p>
                        <p
                          v-if="getResourceSelectionReason(resource)"
                          class="resource-selection-reason"
                        >
                          选择原因：{{ getResourceSelectionReason(resource) }}
                        </p>
                        <p
                          v-if="resource.resourceLayerHint"
                          class="resource-layer-hint"
                        >
                          分层说明：{{ resource.resourceLayerHint }}
                        </p>
                        <p class="resource-usage">
                          使用建议：{{ resource.recommendedUsage }}
                        </p>
                      </li>
                    </ul>

                    <p
                      v-else
                      class="resource-collapsed-tip"
                    >
                      已收起该节点的资源建议，点击“展开资源”后可查看视频和文本教程。
                    </p>
                  </section>
                </div>
              </div>
            </article>

            <section class="card planner-section-card feedback-panel">
              <div class="section-headline">
                <div>
                  <p class="label">学习反馈</p>
                  <h3>根据本轮学习情况调整推荐路径</h3>
                </div>
                <div class="feedback-head-side">
                  <p class="caption">只对“本轮推荐学习”中的节点提交反馈，系统会据此更新掌握度并重新规划。</p>
                  <button
                    type="button"
                    class="ghost-button"
                    :disabled="rollingBack || adjusting || !canRollbackFeedback"
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

              <form
                v-else
                class="feedback-form"
                @submit.prevent="submitAdjustment"
              >
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

                  <div class="feedback-fields">
                    <label class="field">
                      <span>完成情况</span>
                      <select
                        v-model="feedbackDraftByCode[item.code].completionStatus"
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
                          v-model.number="feedbackDraftByCode[item.code].selfRatedMastery"
                          type="range"
                          min="0"
                          max="100"
                          step="5"
                        />
                        <strong>{{ feedbackDraftByCode[item.code].selfRatedMastery }}%</strong>
                      </div>
                    </label>
                  </div>
                </article>

                <button class="submit-button" :disabled="adjusting">
                  {{ adjusting ? "正在调整路径..." : "提交反馈并调整路径" }}
                </button>
              </form>

              <div v-if="adjustError" class="state state--error state--result">
                {{ adjustError }}
              </div>
              <div v-if="rollbackError" class="state state--error state--result">
                {{ rollbackError }}
              </div>
            </section>
          </div>

          <aside class="planner-extension-side">
            <article class="card planner-section-card planner-overview-card">
              <div class="section-headline">
                <div>
                  <p class="label">辅助摘要</p>
                  <h3>当前导航速览</h3>
                </div>
                <p class="caption">右侧固定查看本轮状态，不用反复在长内容里上下滚动。</p>
              </div>

              <dl class="planner-overview-grid">
                <div>
                  <dt>资源分组</dt>
                  <dd>{{ resourceRecommendations.length }} 组</dd>
                </div>
                <div>
                  <dt>本轮节点</dt>
                  <dd>{{ scheduledItems.length }} 个</dd>
                </div>
                <div>
                  <dt>反馈记录</dt>
                  <dd>{{ feedbackRecordCount }} 条</dd>
                </div>
                <div>
                  <dt>路径变化</dt>
                  <dd>{{ pathComparison?.changeItems.length || 0 }} 项</dd>
                </div>
              </dl>

              <p class="overview-note">
                {{ extensionOverviewMessage }}
              </p>

              <ul
                v-if="pathComparison?.changeItems.length > 0"
                class="planner-overview-change-list"
              >
                <li
                  v-for="item in pathComparison.changeItems.slice(0, 3)"
                  :key="`overview-${item.code}`"
                  class="planner-overview-change-item"
                >
                  <strong>{{ item.name }}</strong>
                  <span
                    class="change-badge"
                    :class="changeBadgeClass(item.changeType)"
                  >
                    {{ item.changeLabel }}
                  </span>
                </li>
              </ul>
            </article>

            <article
              v-if="adjustmentSummary || rollbackSummary"
              class="card planner-section-card planner-section-card--summary"
            >
              <div class="section-headline">
                <div>
                  <p class="label">操作摘要</p>
                  <h3>最近一次反馈或回退</h3>
                </div>
                <p class="caption">用于快速查看最近一次操作对掌握度的影响。</p>
              </div>

              <template v-if="adjustmentSummary">
                <dl class="summary-grid feedback-summary">
                  <div>
                    <dt>反馈节点</dt>
                    <dd>{{ adjustmentSummary.feedbackItemCount }} 个</dd>
                  </div>
                  <div>
                    <dt>已完成</dt>
                    <dd>{{ adjustmentSummary.completedCount }} 个</dd>
                  </div>
                  <div>
                    <dt>部分完成</dt>
                    <dd>{{ adjustmentSummary.partialCount }} 个</dd>
                  </div>
                  <div>
                    <dt>学习受阻</dt>
                    <dd>{{ adjustmentSummary.blockedCount }} 个</dd>
                  </div>
                </dl>

                <article class="result-card result-card--wide">
                  <h3>最近一次调整说明</h3>
                  <ul class="path-list plain-list">
                    <li
                      v-for="item in adjustmentItems"
                      :key="`${item.code}-${item.completionStatus}`"
                      class="path-item"
                    >
                      <div class="path-item-head">
                        <strong>{{ resolveKnowledgePointLabel(item.code) }}</strong>
                        <span>
                          {{ Math.round(item.previousMastery * 100) }}% ->
                          {{ Math.round(item.updatedMastery * 100) }}%
                        </span>
                      </div>
                      <p class="path-item-meta">
                        反馈结果：{{ adjustmentStatusLabelMap[item.completionStatus] }}
                      </p>
                      <p class="path-item-reason">
                        {{ item.adjustmentReasons.join(" ") }}
                      </p>
                    </li>
                  </ul>
                </article>
              </template>

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
            <p class="caption">用于观察最近一次反馈或回退后，本轮学习安排如何变化。</p>
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

import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import {
  rollbackLatestLearningFeedback,
  submitLearningFeedback,
} from "../api/feedback";
import { generateLearningPath } from "../api/path";

const props = defineProps({
  learnerCode: {
    type: String,
    default: "demo-learner",
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

const emit = defineEmits(["feedback-saved"]);

const minuteOptions = [60, 90, 120, 150, 180, 240];
const adjustmentStatusLabelMap = {
  completed: "已完成",
  partial: "部分完成",
  blocked: "学习受阻",
};
const resourceTypeLabelMap = {
  video: "视频教程",
  article: "文本教程",
  document: "参考资料",
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
const planResult = ref(null);
const feedbackDraftByCode = ref({});
const adjusting = ref(false);
const adjustError = ref("");
const adjustmentSummary = ref(null);
const adjustmentItems = ref([]);
const rollingBack = ref(false);
const rollbackError = ref("");
const rollbackSummary = ref(null);
const rollbackItems = ref([]);
const pathComparison = ref(null);
const pathComparisonMode = ref("adjust");
const optionsReady = ref(false);
const initialPlanInitialized = ref(false);
const expandedExplanationByCode = ref({});
const resourceFilter = ref("all");
const expandedResourceSectionByCode = ref({});

const resourceFilterOptions = [
  { value: "all", label: "全部资源" },
  { value: "scheduled", label: "只看本轮" },
  { value: "deferred", label: "只看下一轮" },
];
const resourceLayerOrder = ["课程风格优先", "图文补充", "答辩复盘"];

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

const resourceFilterCountMap = computed(() => ({
  all: resourceRecommendations.value.length,
  scheduled: resourceRecommendations.value.filter(
    (section) => section.status === "scheduled",
  ).length,
  deferred: resourceRecommendations.value.filter(
    (section) => section.status === "deferred",
  ).length,
}));

const visibleResourceRecommendations = computed(() => {
  if (resourceFilter.value === "all") {
    return resourceRecommendations.value;
  }

  return resourceRecommendations.value.filter(
    (section) => section.status === resourceFilter.value,
  );
});

const resourceLayerOverview = computed(() =>
  resourceLayerOrder.map((layer) => ({
    layer,
    count: visibleResourceRecommendations.value.reduce((total, section) => {
      const summary = section.resourceLayerSummary || {};
      return total + Number(summary[layer] || 0);
    }, 0),
  })),
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
    : "调整前当前轮次没有待学习节点。",
);

const comparisonAfterEmptyTip = computed(() =>
  pathComparisonMode.value === "rollback"
    ? "回退后当前轮次暂时没有新的待学习节点。"
    : "调整后当前轮次暂时没有新的待学习节点。",
);

const comparisonChangeTitle = computed(() =>
  pathComparisonMode.value === "rollback" ? "回退后路径变化" : "关键路径变化",
);

const comparisonNoChangeTip = computed(() =>
  pathComparisonMode.value === "rollback"
    ? "本次回退主要恢复了掌握度，路径状态没有出现明显变化。"
    : "本次反馈主要更新了掌握度，路径状态没有出现明显变化。",
);

const extensionOverviewMessage = computed(() => {
  if (rollbackSummary.value) {
    return `最近一次操作为反馈回退，已恢复 ${rollbackSummary.value.feedbackItemCount} 个节点的掌握度，可继续重新判断后再提交。`;
  }

  if (adjustmentSummary.value) {
    return `最近一次操作为学习反馈提交，本轮共处理 ${adjustmentSummary.value.feedbackItemCount} 个节点，路径已按新掌握度重算。`;
  }

  if (pathComparison.value?.changeItems.length) {
    return "下方已生成路径变化明细，可以先看右侧速览，再进入整宽对比区查看前后安排差异。";
  }

  if (resourceRecommendations.value.length > 0) {
    return "建议先在左侧查看资源建议，再对本轮节点提交反馈，系统会据此动态调整后续学习导航。";
  }

  return "当前扩展区主要用于查看资源、提交反馈和理解路径变化，适合作为答辩演示的辅助说明区。";
});

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

function getResourceFocusTags(resource) {
  return resource?.focusTags || [];
}

function getResourceSelectionReason(resource) {
  return resource?.selectionReason || "";
}

function resourceLayerClass(layer) {
  return {
    "resource-layer--primary": layer === "课程风格优先",
    "resource-layer--supplement": layer === "图文补充",
    "resource-layer--review": layer === "答辩复盘",
  };
}

function getSectionResourceLayerSummary(section) {
  return resourceLayerOrder
    .map((layer) => ({
      layer,
      count: Number(section?.resourceLayerSummary?.[layer] || 0),
    }))
    .filter((item) => item.count > 0);
}

function getResourceFilterCount(filter) {
  return resourceFilterCountMap.value[filter] || 0;
}

function formatResourceSectionStatus(status) {
  if (status === "scheduled") {
    return "本轮推荐学习";
  }

  if (status === "deferred") {
    return "下一轮建议";
  }

  return "资源建议";
}

function formatResourceType(type) {
  return resourceTypeLabelMap[type] || "学习资源";
}

function toggleResourceSection(code) {
  expandedResourceSectionByCode.value = {
    ...expandedResourceSectionByCode.value,
    [code]: !expandedResourceSectionByCode.value[code],
  };
}

function isResourceSectionExpanded(section) {
  const expanded = expandedResourceSectionByCode.value[section.code];
  if (typeof expanded === "boolean") {
    return expanded;
  }

  return section.status === "scheduled";
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

function syncFeedbackDrafts() {
  feedbackDraftByCode.value = Object.fromEntries(
    scheduledItems.value.map((item) => [
      item.code,
      {
        completionStatus: "completed",
        selfRatedMastery: Math.max(item.masteryPercent, 85),
      },
    ]),
  );
}

function clearOperationOutputs() {
  adjustError.value = "";
  rollbackError.value = "";
  adjustmentSummary.value = null;
  adjustmentItems.value = [];
  rollbackSummary.value = null;
  rollbackItems.value = [];
  pathComparison.value = null;
}

async function loadKnowledgePoints() {
  optionsLoading.value = true;
  optionsError.value = "";

  try {
    const payload = await fetchKnowledgeGraph();
    knowledgePoints.value = payload.nodes;
    selectedTargetCode.value =
      payload.nodes[payload.nodes.length - 1]?.code || "";
  } catch (error) {
    optionsError.value =
      "未能读取知识点选项，请先初始化数据库并启动后端服务。";
    console.error(error);
  } finally {
    optionsLoading.value = false;
  }
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

  initialPlanInitialized.value = true;
  await submitPlan();
}

async function submitPlan() {
  planning.value = true;
  planError.value = "";
  clearOperationOutputs();
  expandedExplanationByCode.value = {};

  try {
    planResult.value = await generateLearningPath({
      targetCodes: [selectedTargetCode.value],
      availableMinutes: availableMinutes.value,
      masteryByCode: buildMasteryPayload(),
    });
    syncFeedbackDrafts();
  } catch (error) {
    planError.value =
      error?.response?.data?.detail || "学习路径生成失败，请检查后端日志。";
    console.error(error);
  } finally {
    planning.value = false;
  }
}

function buildFeedbackItems() {
  return scheduledItems.value.map((item) => ({
    code: item.code,
    completionStatus:
      feedbackDraftByCode.value[item.code]?.completionStatus || "completed",
    selfRatedMastery:
      Number(feedbackDraftByCode.value[item.code]?.selfRatedMastery || 0) / 100,
  }));
}

async function submitAdjustment() {
  adjusting.value = true;
  clearOperationOutputs();
  const beforePlanSnapshot = clonePlanSnapshot(planResult.value);

  try {
    const payload = await submitLearningFeedback({
      learnerCode: props.learnerCode,
      masteryByCode: buildMasteryPayload(),
      feedbackItems: buildFeedbackItems(),
    });

    const updatedMasteryByCode = payload.masteryByCode || {};
    applyMasterySnapshot(updatedMasteryByCode);
    adjustmentSummary.value = payload.feedbackSummary || null;
    adjustmentItems.value = payload.adjustments || [];
    expandedExplanationByCode.value = {};
    emit("feedback-saved", payload);
    const nextPlanResult = await generateLearningPath({
      targetCodes: [selectedTargetCode.value],
      availableMinutes: availableMinutes.value,
      masteryByCode: updatedMasteryByCode,
    });
    planResult.value = nextPlanResult;
    pathComparisonMode.value = "adjust";
    pathComparison.value = buildPathComparison(beforePlanSnapshot, nextPlanResult);
    syncFeedbackDrafts();
  } catch (error) {
    adjustError.value =
      error?.response?.data?.detail || "学习反馈保存失败，请检查后端日志。";
    console.error(error);
  } finally {
    adjusting.value = false;
  }
}

async function rollbackLatestAdjustment() {
  if (!canRollbackFeedback.value || !planResult.value) {
    return;
  }

  rollingBack.value = true;
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
      targetCodes: [selectedTargetCode.value],
      availableMinutes: availableMinutes.value,
      masteryByCode: updatedMasteryByCode,
    });
    planResult.value = nextPlanResult;
    pathComparisonMode.value = "rollback";
    pathComparison.value = buildPathComparison(beforePlanSnapshot, nextPlanResult);
    syncFeedbackDrafts();
  } catch (error) {
    rollbackError.value =
      error?.response?.data?.detail || "学习反馈回退失败，请检查后端日志。";
    console.error(error);
  } finally {
    rollingBack.value = false;
  }
}

watch(
  resourceRecommendations,
  (sections) => {
    expandedResourceSectionByCode.value = Object.fromEntries(
      sections.map((section) => [section.code, section.status === "scheduled"]),
    );
  },
  { immediate: true },
);

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

onMounted(async () => {
  await loadKnowledgePoints();
  optionsReady.value = true;
  if (!optionsError.value) {
    await initializePlannerFromProfile();
  }
});
</script>

<style scoped>
.planner-fragment {
  display: contents;
}

.card {
  background: rgba(255, 255, 255, 0.82);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
}

.planner-core-card {
  grid-column: 2;
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
  align-self: start;
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

.planner-overview-card {
  position: sticky;
  top: 24px;
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

.planner-overview-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
}

.planner-overview-grid div {
  padding: 14px;
  border-radius: 18px;
  background: rgba(244, 248, 247, 0.86);
}

.overview-note {
  margin: 0;
  padding: 14px 16px;
  border-radius: 18px;
  background: rgba(12, 106, 113, 0.08);
  color: #29434a;
  line-height: 1.65;
}

.planner-overview-change-list {
  display: grid;
  gap: 10px;
  margin: 0;
  padding: 0;
  list-style: none;
}

.planner-overview-change-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 12px;
  padding: 12px 14px;
  border-radius: 16px;
  background: rgba(247, 250, 249, 0.86);
}

.planner-overview-change-item strong {
  color: #24323b;
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

.resource-recommendation-card {
  margin-top: 0;
}

.resource-toolbar {
  display: flex;
  justify-content: space-between;
  gap: 14px;
  align-items: start;
  margin-top: 18px;
}

.resource-toolbar-main {
  display: grid;
  gap: 12px;
}

.resource-filter-tabs {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.resource-layer-overview,
.resource-layer-summary {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 0;
  padding: 0;
  list-style: none;
}

.resource-filter-tab,
.resource-toggle {
  border: none;
  border-radius: 999px;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease;
}

.resource-filter-tab {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 8px 14px;
  background: rgba(12, 106, 113, 0.08);
  color: #0c5960;
  font-weight: 700;
}

.resource-filter-tab span {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  min-width: 24px;
  padding: 2px 8px;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.84);
  color: #214861;
  font-size: 0.8rem;
}

.resource-filter-tab--active {
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
}

.resource-filter-tab--active span {
  background: rgba(255, 255, 255, 0.18);
  color: #ffffff;
}

.resource-filter-tab:hover,
.resource-toggle:hover {
  transform: translateY(-1px);
}

.resource-toolbar-tip,
.resource-collapsed-tip {
  margin: 0;
  color: #53616c;
  line-height: 1.6;
}

.resource-empty-tip {
  margin-top: 18px;
}

.resource-layer-chip,
.resource-layer-badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 5px 11px;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 700;
}

.resource-layer-chip strong,
.resource-layer-badge {
  font-weight: 700;
}

.resource-layer-chip span {
  opacity: 0.9;
}

.resource-layer-chip--compact {
  padding: 4px 10px;
  font-size: 0.78rem;
}

.resource-layer--primary {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.resource-layer--supplement {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.resource-layer--review {
  background: rgba(55, 82, 129, 0.12);
  color: #27456f;
}

.resource-sections {
  display: grid;
  gap: 16px;
  margin-top: 18px;
}

.resource-section {
  border-radius: 20px;
  background: rgba(247, 250, 249, 0.86);
  padding: 16px;
}

.resource-section-head {
  display: flex;
  justify-content: space-between;
  gap: 14px;
  align-items: start;
}

.resource-section-title-row {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 10px;
}

.resource-section-status {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 700;
}

.resource-section-status--scheduled {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.resource-section-status--deferred {
  background: rgba(186, 93, 23, 0.12);
  color: #8a4d10;
}

.resource-section-actions {
  display: grid;
  gap: 10px;
  justify-items: end;
}

.resource-primary-summary {
  margin: 8px 0 0;
  color: #214861;
  font-size: 0.92rem;
  font-weight: 600;
  line-height: 1.5;
}

.resource-layer-summary {
  margin-top: 10px;
}

.resource-section-usage {
  max-width: 320px;
  color: #53616c;
  font-size: 0.9rem;
  line-height: 1.5;
  text-align: right;
}

.resource-toggle {
  padding: 7px 12px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-weight: 700;
}

.resource-list {
  display: grid;
  gap: 12px;
  margin: 14px 0 0;
  padding: 0;
  list-style: none;
}

.resource-item {
  border-radius: 16px;
  border: 1px solid rgba(22, 32, 42, 0.08);
  background: rgba(255, 255, 255, 0.94);
  padding: 14px;
}

.resource-item-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: start;
}

.resource-item-badges {
  display: flex;
  align-items: center;
  gap: 8px;
  flex-wrap: wrap;
  justify-content: end;
}

.resource-link {
  color: #0c5960;
  font-weight: 700;
  text-decoration: none;
}

.resource-link:hover {
  text-decoration: underline;
}

.resource-type {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-size: 0.82rem;
  font-weight: 700;
  white-space: nowrap;
}

.resource-meta,
.resource-description,
.resource-selection-reason,
.resource-usage {
  margin: 8px 0 0;
  color: #44515c;
  line-height: 1.5;
}

.resource-selection-reason {
  color: #214861;
  font-weight: 600;
}

.resource-layer-hint {
  margin: 8px 0 0;
  color: #53616c;
  line-height: 1.5;
}

.resource-tag-row {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 10px;
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

.resource-top-badge {
  display: inline-flex;
  align-items: center;
  padding: 4px 10px;
  border-radius: 999px;
  background: rgba(225, 147, 35, 0.16);
  color: #8a4d10;
  font-size: 0.82rem;
  font-weight: 700;
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
  .planner-overview-grid,
  .planner-extension-stack {
    grid-template-columns: 1fr;
  }

  .planner-overview-card {
    position: static;
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
  .resource-toolbar,
  .path-item-head,
  .planner-overview-change-item,
  .resource-section-head,
  .resource-item-head,
  .feedback-head,
  .feedback-item-head {
    flex-direction: column;
    align-items: start;
  }

  .feedback-head-side {
    justify-items: start;
  }

  .resource-section-actions {
    justify-items: start;
  }

  .resource-section-usage {
    max-width: none;
    text-align: left;
  }

  .resource-item-badges {
    justify-content: start;
  }
}
</style>
