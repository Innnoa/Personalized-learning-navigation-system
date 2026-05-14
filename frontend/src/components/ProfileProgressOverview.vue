<template>
  <section class="card" aria-label="学习进度概览">
    <div class="card-head">
      <div>
        <p class="label">总体掌握度</p>
        <strong class="mastery-value">{{ normalizedAverageMasteryPercent }}%</strong>
      </div>
      <p class="caption">已按当前画像汇总掌握状态分布</p>
    </div>

    <div v-if="normalizedSegments.length > 0" class="segment-list">
      <article
        v-for="segment in normalizedSegments"
        :key="segment.key"
        class="segment-item"
      >
        <div class="segment-head">
          <strong>{{ segment.label }}</strong>
          <span>{{ segment.count }} 个知识点 · {{ segment.displayPercent }}%</span>
        </div>
        <div class="bar-track" aria-hidden="true">
          <div class="bar-fill" :style="{ width: `${segment.displayPercent}%` }"></div>
        </div>
      </article>
    </div>

    <p v-else class="empty-tip">暂无可展示的进度分布。</p>
  </section>
</template>

<script setup>
import { computed } from "vue";

const props = defineProps({
  averageMasteryPercent: {
    type: Number,
    default: 0,
  },
  segments: {
    type: [Array, null],
    default: () => [],
    validator: (value) => value == null || (Array.isArray(value) && value.every((segment) => segment !== null
      && typeof segment === "object"
      && !Array.isArray(segment)
      && typeof segment.label === "string"
      && segment.label.trim() !== ""
      && typeof segment.count === "number"
      && Number.isFinite(segment.count)
      && typeof segment.percent === "number"
      && Number.isFinite(segment.percent))),
  },
});

const isRenderableObject = (value) => value !== null && typeof value === "object" && !Array.isArray(value);

const hasNonEmptyText = (value) => typeof value === "string" && value.trim() !== "";

const isFiniteNumber = (value) => typeof value === "number" && Number.isFinite(value);

const clampPercent = (value) => {
  if (!isFiniteNumber(value)) {
    return 0;
  }

  return Math.min(100, Math.max(0, value));
};

const normalizeText = (value) => {
  if (typeof value !== "string") {
    return "";
  }

  return value.trim();
};

const normalizeCount = (value) => {
  if (!isFiniteNumber(value) || value < 0) {
    return 0;
  }

  return value;
};

const buildSegmentKey = (segment, duplicates) => {
  const keyBase = [
    segment.key,
    segment.id,
    segment.knowledgePointCode,
    segment.code,
    segment.label,
  ]
    .map((part) => (part === null || part === undefined ? "" : String(part).trim()))
    .filter(Boolean)
    .join("-") || "segment";

  const duplicateCount = duplicates[keyBase] ?? 0;
  duplicates[keyBase] = duplicateCount + 1;

  return duplicateCount === 0 ? keyBase : `${keyBase}-${duplicateCount + 1}`;
};

const normalizedAverageMasteryPercent = computed(() => clampPercent(props.averageMasteryPercent));

const segmentItems = computed(() => (Array.isArray(props.segments)
  ? props.segments.filter((segment) => isRenderableObject(segment))
  : []));

const normalizedSegments = computed(() => {
  const duplicateKeys = {};

  return segmentItems.value
    .map((segment) => ({
      source: segment,
      label: normalizeText(segment.label),
    }))
    .filter(({ source, label }) => label !== ""
      && isFiniteNumber(source.count)
      && isFiniteNumber(source.percent))
    .map(({ source, label }) => ({
      key: buildSegmentKey(source, duplicateKeys),
      label,
      count: normalizeCount(source.count),
      displayPercent: clampPercent(source.percent),
    }));
});
</script>

<style scoped>
.card {
  background: var(--panel-surface);
  border: var(--panel-border);
  border-radius: 24px;
  padding: 22px;
  box-shadow: var(--panel-shadow);
  display: grid;
  gap: 16px;
}

.card-head {
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

.caption {
  margin: 0;
  max-width: 220px;
  text-align: right;
  color: #5f6d79;
  font-size: 0.92rem;
}

.mastery-value {
  display: block;
  font-size: 2rem;
  line-height: 1;
}

.segment-list {
  display: grid;
  gap: 14px;
}

.segment-item {
  display: grid;
  gap: 8px;
}

.segment-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: center;
}

.segment-head span {
  color: #5f6d79;
  font-size: 0.92rem;
}

.bar-track {
  height: 10px;
  background: #e7edf3;
  border-radius: 999px;
  overflow: hidden;
}

.bar-fill {
  height: 100%;
  border-radius: inherit;
  background: linear-gradient(90deg, #4a7bff 0%, #6f9dff 100%);
}

.empty-tip {
  margin: 0;
  color: #6d7781;
}

@media (max-width: 640px) {
  .card-head,
  .segment-head {
    flex-direction: column;
    align-items: start;
  }

  .caption {
    max-width: none;
    text-align: left;
  }
}
</style>
