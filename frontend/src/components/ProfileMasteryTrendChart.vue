<template>
  <section class="card" aria-label="掌握度趋势图">
    <div class="card-head">
    <div>
        <p class="label">掌握度趋势</p>
        <h3>最近反馈变化</h3>
      </div>
      <p v-if="normalizedTrendSummary" class="summary" :class="`summary--${normalizedTrendSummary.direction}`">
        {{ normalizedTrendSummary.message }}
      </p>
    </div>

    <div v-if="plottedPoints.length > 0" class="chart-shell">
      <svg
        class="chart"
        viewBox="0 0 320 120"
        role="img"
        aria-label="掌握度趋势折线图"
      >
        <polyline
          class="chart-line"
          fill="none"
          :points="polylinePoints"
        />
        <circle
          v-for="point in plottedPoints"
          :key="point.key"
          class="chart-point"
          :cx="point.x"
          :cy="point.y"
          r="4"
        />
      </svg>

        <div class="point-list">
          <article v-for="point in plottedPoints" :key="point.key" class="point-item">
          <strong>{{ point.label }}</strong>
          <span>{{ point.displayValue }}%</span>
          </article>
        </div>
      </div>

    <p v-else class="empty-tip">暂无掌握度趋势数据。</p>
  </section>
</template>

<script setup>
import { computed } from "vue";

const props = defineProps({
  points: {
    type: [Array, null],
    default: () => [],
    validator: (value) => value == null || (Array.isArray(value) && value.every((point) => point !== null
      && typeof point === "object"
      && !Array.isArray(point)
      && typeof point.label === "string"
      && point.label.trim() !== ""
      && Number.isFinite(Number(point.value)))),
  },
  trendSummary: {
    type: Object,
    default: null,
  },
});

const isRenderableObject = (value) => value !== null && typeof value === "object" && !Array.isArray(value);

const clampPercent = (value) => {
  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return 0;
  }

  return Math.min(100, Math.max(0, numericValue));
};

const normalizeText = (value) => {
  if (typeof value !== "string") {
    return "";
  }

  return value.trim();
};

const normalizeDisplayValue = (value) => {
  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return null;
  }

  return clampPercent(numericValue);
};

const buildPointKey = (point, duplicates) => {
  const keyBase = [
    point?.key,
    point?.id,
    point?.knowledgePointCode,
    point?.code,
    point?.label,
  ]
    .map((part) => (part === null || part === undefined ? "" : String(part).trim()))
    .filter(Boolean)
    .join("-") || "point";

  const duplicateCount = duplicates[keyBase] ?? 0;
  duplicates[keyBase] = duplicateCount + 1;

  return duplicateCount === 0 ? keyBase : `${keyBase}-${duplicateCount + 1}`;
};

const pointItems = computed(() => (Array.isArray(props.points)
  ? props.points.filter((point) => isRenderableObject(point))
  : []));

const normalizeTrendDirection = (value) => {
  if (value === "up" || value === "down" || value === "flat") {
    return value;
  }

  return "flat";
};

const normalizedTrendSummary = computed(() => {
  if (!props.trendSummary || typeof props.trendSummary !== "object") {
    return null;
  }

  const message = normalizeText(props.trendSummary.message);

  if (message === "") {
    return null;
  }

  return {
    direction: normalizeTrendDirection(props.trendSummary.direction),
    message,
  };
});

const plottedPoints = computed(() => {
  const items = pointItems.value
    .map((point) => ({
      source: point,
      label: normalizeText(point.label),
      displayValue: normalizeDisplayValue(point.value),
    }))
    .filter((point) => point.label !== "" && point.displayValue !== null);

  if (items.length === 0) {
    return [];
  }

  const xStep = items.length === 1 ? 0 : 280 / (items.length - 1);
  const duplicateKeys = {};

  return items.map(({ source, label, displayValue }, index) => ({
    key: buildPointKey(source, duplicateKeys),
    label,
    displayValue,
    x: 20 + index * xStep,
    y: 100 - displayValue * 0.8,
  }));
});

const polylinePoints = computed(() => plottedPoints.value.map((point) => `${point.x},${point.y}`).join(" "));
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

.summary {
  margin: 0;
  max-width: 240px;
  font-size: 0.92rem;
  text-align: right;
}

.summary--up {
  color: #176b39;
}

.summary--down {
  color: #b24a39;
}

.summary--flat {
  color: #5f6d79;
}

.chart-shell {
  display: grid;
  gap: 14px;
}

.chart {
  width: 100%;
  height: auto;
  padding: 12px;
  border-radius: 18px;
  background: linear-gradient(180deg, #f8fbff 0%, #eef4ff 100%);
}

.chart-line {
  stroke: #4a7bff;
  stroke-width: 3;
  stroke-linecap: round;
  stroke-linejoin: round;
}

.chart-point {
  fill: #ffffff;
  stroke: #4a7bff;
  stroke-width: 3;
}

.point-list {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(88px, 1fr));
  gap: 10px;
}

.point-item {
  display: grid;
  gap: 4px;
  padding: 10px 12px;
  border-radius: 16px;
  background: #f7f9fc;
}

.point-item span {
  color: #4a5b68;
}

.empty-tip {
  margin: 0;
  color: #6d7781;
}

@media (max-width: 640px) {
  .card-head {
    flex-direction: column;
  }

  .summary {
    max-width: none;
    text-align: left;
  }
}
</style>
