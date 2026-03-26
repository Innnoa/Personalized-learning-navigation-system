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

      <section class="section">
        <div class="section-head">
          <h3>当前待补强知识点</h3>
          <span>低掌握度优先展示</span>
        </div>

        <ul class="weak-list">
          <li v-for="item in weakItems" :key="item.code" class="weak-item">
            <div class="weak-item-main">
              <div>
                <strong>{{ item.name }}</strong>
                <p>第{{ item.chapterNo }}章 · {{ item.chapterName }}</p>
              </div>
              <div class="weak-item-side">
                <span>{{ item.masteryPercent }}%</span>
                <button
                  type="button"
                  class="weak-action-button"
                  :disabled="Boolean(resourcePreviewCode)"
                  @click="emit('open-resource', item)"
                >
                  {{
                    resourcePreviewCode === item.code
                      ? "准备资源中..."
                      : "查看推荐资源"
                  }}
                </button>
              </div>
            </div>
          </li>
        </ul>

        <p v-if="weakItems.length === 0" class="empty-tip">
          当前画像中的知识点已全部达到较稳定掌握度。
        </p>
        <p v-if="resourcePreviewError" class="state state--error state--inline">
          {{ resourcePreviewError }}
        </p>
      </section>

      <section class="section">
        <div class="section-head">
          <h3>最近学习记录</h3>
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

      <section class="section">
        <div class="section-head">
          <h3>最近资源行为</h3>
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
    </template>
  </article>
</template>

<script setup>
import { computed } from "vue";

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

const weakItems = computed(() => {
  if (!props.profile?.masteryItems) {
    return [];
  }

  return [...props.profile.masteryItems]
    .filter((item) => item.masteryPercent < 85)
    .sort((left, right) => left.masteryPercent - right.masteryPercent)
    .slice(0, 5);
});

const recentFeedbackItems = computed(() => props.profile?.recentFeedbackItems || []);
const recentResourceViewItems = computed(
  () => props.profile?.recentResourceViewItems || [],
);

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

.identity-card {
  margin-top: 18px;
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
  margin-top: 18px;
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
  margin-top: 20px;
}

.section-head span {
  color: #66727d;
  font-size: 0.88rem;
}

.weak-list {
  display: grid;
  gap: 12px;
  margin: 16px 0 0;
  padding: 0;
  list-style: none;
}

.weak-item {
  padding: 14px;
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.86);
  contain: layout paint;
}

.weak-item-main {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  align-items: start;
}

.weak-item p,
.empty-tip {
  margin: 6px 0 0;
  color: #44515c;
}

.weak-item-side {
  display: grid;
  gap: 10px;
  justify-items: end;
}

.weak-item span {
  font-weight: 700;
  color: #0c5960;
}

.weak-action-button {
  border: 1px solid rgba(12, 106, 113, 0.16);
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.08);
  color: #0c5960;
  padding: 8px 14px;
  font-weight: 700;
  cursor: pointer;
}

.weak-action-button:disabled {
  cursor: wait;
  opacity: 0.72;
}

.record-list {
  display: grid;
  gap: 12px;
  margin: 16px 0 0;
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
  .weak-item,
  .record-item {
    content-visibility: auto;
  }

  .section {
    contain-intrinsic-size: 280px;
  }

  .weak-item,
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

  .card-head,
  .section-head,
  .weak-item-main,
  .record-main,
  .record-badge-group {
    flex-direction: column;
    align-items: start;
  }

  .weak-item-side {
    justify-items: start;
  }
}
</style>
