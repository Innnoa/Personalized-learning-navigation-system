<template>
  <PageLayout
    eyebrow="练习检测 · 流程承接页"
    title="练习检测入口占位页"
    description="当前页用于承接学习反馈流转，先展示上下文与跳转入口，题目模块将在后续阶段接入。"
  >
    <section class="practice-check-layout">
      <article class="card surface-panel intro-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">当前状态</p>
            <h2>本页仅完成流程交接与占位展示</h2>
          </div>
          <p class="caption">
            当前已经接收到练习检测上下文；题目模块后续接入，现阶段先用于确认知识点、批次与来源页面是否正确。
          </p>
        </div>

        <p class="intro-note">
          题目模块后续接入后，此处将承接正式练习内容、答题记录与结果回传。
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

      <article class="card surface-panel action-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">页面动作</p>
            <h3>继续浏览现有模块</h3>
          </div>
        </div>

        <div class="action-row">
          <button type="button" class="ghost-button" @click="goHome">
            返回首页
          </button>
          <button type="button" class="ghost-button" @click="goLearnerProfile">
            前往学习者画像
          </button>
        </div>
      </article>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed } from "vue";
import { useRouter } from "vue-router";

import PageLayout from "../components/PageLayout.vue";
import { useNavigationStore } from "../stores/navigationStore";

const router = useRouter();
const navigationStore = useNavigationStore();

const practiceCheckContext = computed(
  () => navigationStore.practiceCheckContext || {},
);

const practiceScopeText = computed(() => {
  const scopeLabel = practiceCheckContext.value.scopeLabel || "";
  const scopeCode = practiceCheckContext.value.scopeCode || "";

  if (scopeLabel && scopeCode) {
    return `${scopeLabel}（${scopeCode}）`;
  }

  return scopeLabel || scopeCode || "未指定";
});

async function goHome() {
  await router.push({
    name: "home",
  });
}

async function goLearnerProfile() {
  await router.push({
    name: "learner-profile",
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
