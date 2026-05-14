<template>
  <section class="card" aria-label="学习活动构成">
    <div class="card-head">
      <div>
        <p class="label">学习活动构成</p>
        <h3>反馈状态分布</h3>
      </div>
      <p class="caption">按最近反馈记录占比展示</p>
    </div>

    <div v-if="normalizedItems.length > 0" class="composition-list">
      <article v-for="item in normalizedItems" :key="item.key" class="composition-item">
        <div class="composition-head">
          <strong>{{ item.label }}</strong>
          <span>{{ item.count }} 条 · {{ item.displayPercent }}%</span>
        </div>
        <div class="bar-track" aria-hidden="true">
          <div class="bar-fill" :style="{ width: `${item.displayPercent}%` }"></div>
        </div>
      </article>
    </div>

    <p v-else class="empty-tip">暂无学习活动数据。</p>
  </section>
</template>

<script setup>
import { computed } from "vue";

const props = defineProps({
  items: {
    type: [Array, null],
    default: () => [],
    validator: (value) => value == null || (Array.isArray(value) && value.every((item) => item !== null
      && typeof item === "object"
      && !Array.isArray(item)
      && typeof item.label === "string"
      && item.label.trim() !== ""
      && typeof item.count === "number"
      && Number.isFinite(item.count)
      && typeof item.percent === "number"
      && Number.isFinite(item.percent))),
  },
});

const isRenderableObject = (value) => value !== null && typeof value === "object" && !Array.isArray(value);

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

const buildItemKey = (item, duplicates) => {
  const keyBase = [
    item.status,
    item.id,
    item.key,
    item.label,
  ]
    .map((part) => (part === null || part === undefined ? "" : String(part).trim()))
    .filter(Boolean)
    .join("-") || "item";

  const duplicateCount = duplicates[keyBase] ?? 0;
  duplicates[keyBase] = duplicateCount + 1;

  return duplicateCount === 0 ? keyBase : `${keyBase}-${duplicateCount + 1}`;
};

const compositionItems = computed(() => (Array.isArray(props.items)
  ? props.items.filter((item) => isRenderableObject(item))
  : []));

const normalizedItems = computed(() => {
  const duplicateKeys = {};

  return compositionItems.value
    .map((item) => ({
      source: item,
      label: normalizeText(item.label),
    }))
    .filter(({ source, label }) => label !== ""
      && isFiniteNumber(source.count)
      && isFiniteNumber(source.percent))
    .map(({ source, label }) => ({
      key: buildItemKey(source, duplicateKeys),
      status: normalizeText(source.status),
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

h3 {
  margin: 0;
  font-size: 1.1rem;
}

.caption {
  margin: 0;
  color: #5f6d79;
  font-size: 0.92rem;
}

.composition-list {
  display: grid;
  gap: 14px;
}

.composition-item {
  display: grid;
  gap: 8px;
}

.composition-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: center;
}

.composition-head span {
  color: #5f6d79;
  font-size: 0.92rem;
}

.bar-track {
  height: 10px;
  background: #edf1f6;
  border-radius: 999px;
  overflow: hidden;
}

.bar-fill {
  height: 100%;
  border-radius: inherit;
  background: linear-gradient(90deg, #52b788 0%, #7cdcb5 100%);
}

.empty-tip {
  margin: 0;
  color: #6d7781;
}

@media (max-width: 640px) {
  .card-head,
  .composition-head {
    flex-direction: column;
    align-items: start;
  }
}
</style>
