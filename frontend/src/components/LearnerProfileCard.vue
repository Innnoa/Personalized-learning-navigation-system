<template>
  <article class="card">
    <div class="card-head">
      <div>
        <p class="label">学习者画像</p>
        <h2>默认演示学习者</h2>
      </div>
      <p class="caption">当前数据库中的掌握度将作为路径规划默认输入</p>
    </div>

    <div v-if="loading" class="state state--loading">
      正在读取学习者画像...
    </div>

    <div v-else-if="error" class="state state--error">
      {{ error }}
    </div>

    <template v-else-if="profile">
      <div class="identity-card">
        <strong>{{ profile.learner.name }}</strong>
        <p>{{ profile.learner.major }} · {{ profile.learner.gradeLabel }}</p>
        <p>目标课程：{{ profile.course.name }}</p>
      </div>

      <div class="profile-narrative">
        <section class="section narrative-section">
          <div class="section-head">
            <div>
              <p class="section-label">进度总览</p>
              <h3>当前掌握状态</h3>
            </div>
            <span>先看整体进度，再决定后续补强重点</span>
          </div>

          <dl class="summary-grid">
            <div>
              <dt>平均掌握度</dt>
              <dd>{{ profile.summary.averageMasteryPercent }}%</dd>
            </div>
            <div>
              <dt>已掌握</dt>
              <dd>{{ profile.summary.masteredCount }} 个</dd>
            </div>
            <div>
              <dt>进行中</dt>
              <dd>{{ profile.summary.inProgressCount }} 个</dd>
            </div>
            <div>
              <dt>未开始</dt>
              <dd>{{ profile.summary.notStartedCount }} 个</dd>
            </div>
            <div>
              <dt>反馈记录</dt>
              <dd>{{ profile.summary.feedbackRecordCount }} 条</dd>
            </div>
            <div>
              <dt>资源行为</dt>
              <dd>{{ profile.summary.resourceViewRecordCount || 0 }} 条</dd>
            </div>
            <div>
              <dt>知识点总数</dt>
              <dd>{{ profile.summary.knowledgePointCount }} 个</dd>
            </div>
          </dl>

          <ProfileProgressOverview
            :average-mastery-percent="profile.summary.averageMasteryPercent"
            :segments="progressSegments"
          />
        </section>

        <section class="section narrative-section">
          <div class="section-head">
            <div>
              <p class="section-label">聚焦观察</p>
              <h3>掌握趋势与学习活动</h3>
            </div>
            <span>结合趋势与活动构成判断近期学习状态</span>
          </div>

          <div class="profile-spotlight-grid">
            <ProfileMasteryTrendChart
              :points="feedbackTrendPoints"
              :trend-summary="trendSummary"
            />
            <ProfileActivityComposition :items="feedbackCompositionItems" />
          </div>
        </section>

        <section class="section narrative-section">
          <div class="section-head">
            <div>
              <p class="section-label">薄弱点排行</p>
              <h3>优先补强知识点</h3>
            </div>
            <span>低掌握度节点优先进入资源预览</span>
          </div>

          <div
            class="weak-ranking-shell"
            :class="{ 'weak-ranking-shell--busy': Boolean(resourcePreviewCode) }"
          >
            <ProfileWeakPointRanking
              :items="weakRankingItems"
              @open-resource="handleOpenWeakPointResource"
            />
          </div>

          <p v-if="resourcePreviewCode" class="state state--loading state--inline">
            正在准备该知识点的资源预览，请稍候...
          </p>

          <p v-if="resourcePreviewError" class="state state--error state--inline">
            {{ resourcePreviewError }}
          </p>
        </section>

        <section class="section narrative-section narrative-section--supporting">
          <div class="section-head">
            <div>
              <p class="section-label">支撑证据</p>
              <h3>近期学习轨迹</h3>
            </div>
            <span>保留最近反馈与资源行为，作为画像解读的上下文</span>
          </div>

          <section class="supporting-section">
            <div class="section-head">
              <h4>最近学习记录</h4>
              <span>展示最近 {{ recentFeedbackItems.length }} 条</span>
            </div>

            <ul v-if="recentFeedbackItems.length > 0" class="record-list">
          <li
            v-for="item in recentFeedbackItems"
            :key="`${item.code}-${item.recordedAt}`"
            class="record-item"
          >
            <div class="record-main">
              <div>
                <strong>{{ item.name }}</strong>
                <p>第{{ item.chapterNo }}章 · {{ item.chapterName }}</p>
              </div>
              <span class="status-badge">
                {{ feedbackStatusLabelMap[item.completionStatus] || item.completionStatus }}
              </span>
            </div>
            <p class="record-mastery">
              掌握度 {{ item.previousMasteryPercent }}% ->
              {{ item.updatedMasteryPercent }}%
            </p>
            <p class="record-meta">
              自评 {{ item.selfRatedMasteryPercent }}% · {{ item.recordedAt }}
            </p>
          </li>
            </ul>

            <p v-else class="empty-tip">
              当前还没有学习反馈记录，提交一次反馈后会在这里展示最近学习过程。
            </p>
          </section>

          <section class="supporting-section">
            <div class="section-head">
              <h4>最近资源行为</h4>
              <span>展示最近 {{ recentResourceViewItems.length }} 条</span>
            </div>

            <ul v-if="recentResourceViewItems.length > 0" class="record-list">
          <li
            v-for="item in recentResourceViewItems"
            :key="`${item.resourceUrl}-${item.recordedAt}`"
            class="record-item"
          >
            <div class="record-main">
              <div>
                <strong>{{ item.resourceTitle }}</strong>
                <p>对应知识点：{{ item.name }} · 第{{ item.chapterNo }}章</p>
              </div>
              <div class="record-badge-group">
                <span
                  class="status-badge"
                  :class="resourceInteractionBadgeClass(item.interactionType)"
                >
                  {{
                    item.interactionTypeLabel ||
                    resourceInteractionLabelMap[item.interactionType] ||
                    "已打开"
                  }}
                </span>
                <span class="status-badge status-badge--resource">
                  {{ resourceTypeLabelMap[item.resourceType] || "学习资源" }}
                </span>
              </div>
            </div>
            <p class="record-mastery">
              {{ item.resourceLayer || "资源建议" }} · {{ item.sourceContextLabel }}
            </p>
            <p v-if="item.linkedReasonSummary" class="record-meta">
              {{ item.linkedReasonSummary }}
            </p>
            <p class="record-meta">
              {{ item.resourceSource }} · {{ item.recordedAt }}
            </p>
          </li>
            </ul>

            <p v-else class="empty-tip">
              当前还没有资源行为记录，点击资源标题或使用“记为已看 / 记为学完 / 稍后再看”后会在这里展示最近轨迹。
            </p>
          </section>
        </section>
      </div>
    </template>
  </article>
</template>

<script setup>
import { computed } from "vue";

import {
  buildFeedbackCompositionItems,
  buildFeedbackTrendPoints,
  buildProgressSegments,
  buildTrendSummary,
  buildWeakPointRankingItems,
} from "./profileCharts";
import ProfileActivityComposition from "./ProfileActivityComposition.vue";
import ProfileMasteryTrendChart from "./ProfileMasteryTrendChart.vue";
import ProfileProgressOverview from "./ProfileProgressOverview.vue";
import ProfileWeakPointRanking from "./ProfileWeakPointRanking.vue";

const props = defineProps({
  profile: {
    type: Object,
    default: null,
  },
  loading: {
    type: Boolean,
    default: false,
  },
  error: {
    type: String,
    default: "",
  },
  resourcePreviewCode: {
    type: String,
    default: "",
  },
  resourcePreviewError: {
    type: String,
    default: "",
  },
});
const emit = defineEmits(["open-resource"]);

const feedbackStatusLabelMap = {
  completed: "已完成",
  partial: "部分完成",
  blocked: "学习受阻",
};
const resourceTypeLabelMap = {
  video: "视频教程",
  article: "文本教程",
  document: "参考资料",
};
const resourceInteractionLabelMap = {
  opened: "已打开",
  viewed: "记为已看",
  completed: "已学完",
  save_for_later: "稍后再看",
};

const recentFeedbackItems = computed(() => props.profile?.recentFeedbackItems || []);
const recentResourceViewItems = computed(
  () => props.profile?.recentResourceViewItems || [],
);
const progressSegments = computed(() =>
  buildProgressSegments(props.profile?.summary),
);
const feedbackTrendPoints = computed(() =>
  buildFeedbackTrendPoints(props.profile?.analytics),
);
const feedbackCompositionItems = computed(() =>
  buildFeedbackCompositionItems(props.profile?.analytics),
);
const trendSummary = computed(() =>
  buildTrendSummary(props.profile?.analytics),
);
const weakRankingItems = computed(() =>
  buildWeakPointRankingItems(props.profile?.masteryItems),
);

function handleOpenWeakPointResource(payload) {
  if (props.resourcePreviewCode) {
    return;
  }

  const item = weakRankingItems.value.find((candidate) => candidate.code === payload?.code);

  if (!item) {
    return;
  }

  emit("open-resource", item);
}

function resourceInteractionBadgeClass(interactionType) {
  return {
    "status-badge--opened": interactionType === "opened",
    "status-badge--viewed": interactionType === "viewed",
    "status-badge--completed": interactionType === "completed",
    "status-badge--save-later": interactionType === "save_for_later",
  };
}
</script>

<style scoped>
.card {
  background: var(--panel-surface);
  border: var(--panel-border);
  border-radius: 24px;
  padding: 22px;
  box-shadow: var(--panel-shadow);
  contain: paint;
  display: grid;
  gap: 18px;
}

.card-head,
.section-head {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  align-items: start;
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

.profile-narrative {
  display: grid;
  gap: 22px;
}

.identity-card {
  padding: 16px 18px;
  border-radius: 20px;
  background: rgba(244, 248, 247, 0.86);
}

.identity-card p {
  margin: 6px 0 0;
  color: #44515c;
}

.summary-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
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

.section {
  display: grid;
  gap: 16px;
}

.narrative-section {
  padding-top: 4px;
}

.narrative-section--supporting {
  padding-top: 8px;
}

.section-label {
  margin: 0 0 6px;
  font-size: 0.84rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

.section-head span {
  color: #66727d;
  font-size: 0.88rem;
}

.profile-spotlight-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px;
}

.supporting-section {
  display: grid;
  gap: 14px;
}

.weak-ranking-shell--busy {
  pointer-events: none;
  opacity: 0.76;
}

h4 {
  margin: 0;
  font-size: 1.02rem;
}

.empty-tip {
  margin: 6px 0 0;
  color: #44515c;
}

.record-list {
  display: grid;
  gap: 12px;
  padding: 0;
  list-style: none;
}

.record-item {
  padding: 14px;
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.86);
  contain: layout paint;
}

@supports (content-visibility: auto) {
  .section,
  .record-item {
    content-visibility: auto;
  }

  .section {
    contain-intrinsic-size: 280px;
  }

  .record-item {
    contain-intrinsic-size: 120px;
  }
}

.record-main {
  display: flex;
  justify-content: space-between;
  gap: 14px;
  align-items: start;
}

.record-badge-group {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  justify-content: end;
}

.record-main p,
.record-mastery,
.record-meta {
  margin: 6px 0 0;
  color: #44515c;
}

.record-mastery {
  font-weight: 700;
  color: #0c5960;
}

.record-meta {
  font-size: 0.88rem;
  color: #66727d;
}

.status-badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-size: 0.84rem;
  font-weight: 700;
}

.status-badge--resource {
  background: rgba(55, 82, 129, 0.12);
  color: #27456f;
}

.status-badge--opened {
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
}

.status-badge--viewed {
  background: rgba(12, 106, 113, 0.12);
  color: #0c5960;
}

.status-badge--completed {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.status-badge--save-later {
  background: rgba(161, 118, 23, 0.12);
  color: #8a4d10;
}

.state {
  margin-top: 18px;
  border-radius: 18px;
  padding: 18px;
  font-weight: 600;
}

.state--inline {
  padding: 14px 16px;
}

.state--loading {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.state--error {
  background: rgba(176, 47, 47, 0.1);
  color: #8d2323;
}

@media (max-width: 980px) {
  .summary-grid {
    grid-template-columns: 1fr;
  }

  .profile-spotlight-grid {
    grid-template-columns: 1fr;
  }

  .card-head,
  .section-head,
  .record-main,
  .record-badge-group {
    flex-direction: column;
    align-items: start;
  }
}
</style>
