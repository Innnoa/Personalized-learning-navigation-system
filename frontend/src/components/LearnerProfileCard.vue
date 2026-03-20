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
            <div>
              <strong>{{ item.name }}</strong>
              <p>第{{ item.chapterNo }}章 · {{ item.chapterName }}</p>
            </div>
            <span>{{ item.masteryPercent }}%</span>
          </li>
        </ul>

        <p v-if="weakItems.length === 0" class="empty-tip">
          当前画像中的知识点已全部达到较稳定掌握度。
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
});

const feedbackStatusLabelMap = {
  completed: "已完成",
  partial: "部分完成",
  blocked: "学习受阻",
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
</script>

<style scoped>
.card {
  background: rgba(255, 255, 255, 0.82);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
}

.card-head,
.section-head,
.weak-item {
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
}

.weak-item p,
.empty-tip {
  margin: 6px 0 0;
  color: #44515c;
}

.weak-item span {
  font-weight: 700;
  color: #0c5960;
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
}

.record-main {
  display: flex;
  justify-content: space-between;
  gap: 14px;
  align-items: start;
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

@media (max-width: 980px) {
  .summary-grid {
    grid-template-columns: 1fr;
  }

  .card-head,
  .section-head,
  .weak-item,
  .record-main {
    flex-direction: column;
    align-items: start;
  }
}
</style>
