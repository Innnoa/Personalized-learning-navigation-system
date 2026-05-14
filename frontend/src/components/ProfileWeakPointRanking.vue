<template>
  <section class="card" aria-label="薄弱知识点排行">
    <div class="card-head">
      <div>
        <p class="label">薄弱点排行</p>
        <h3>优先补强建议</h3>
      </div>
      <p class="caption">按传入顺序展示，请在上层准备补强优先级</p>
    </div>

    <ul v-if="normalizedItems.length > 0" class="weak-list">
      <li v-for="item in normalizedItems" :key="item.itemKey" class="weak-item">
        <div>
          <strong>{{ item.name }}</strong>
          <p>{{ item.chapterText }}</p>
        </div>
        <div class="weak-side">
          <span>{{ item.masteryPercent }}%</span>
          <button
            type="button"
            class="action-button"
            :disabled="!item.code"
            @click="handleOpenResource(item.code)"
          >
            查看推荐资源
          </button>
        </div>
      </li>
    </ul>

    <p v-else class="empty-tip">当前没有需要重点补强的知识点。</p>
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
      && typeof item.name === "string"
      && item.name.trim() !== ""
      && typeof item.masteryPercent === "number"
      && Number.isFinite(item.masteryPercent)
      && (item.code === null || item.code === undefined || typeof item.code === "string"))),
  },
});

const isRenderableObject = (value) => value !== null && typeof value === "object" && !Array.isArray(value);

const isFiniteNumber = (value) => typeof value === "number" && Number.isFinite(value);

const emit = defineEmits({
  "open-resource": (payload) => payload && typeof payload === "object"
    && !Array.isArray(payload)
    && typeof payload.code === "string"
    && payload.code.trim() !== "",
});

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

const buildWeakPointKey = (item, duplicates) => {
  const keyBase = [
    item.code,
    item.id,
    item.key,
    item.name,
  ]
    .map((part) => (part === null || part === undefined ? "" : String(part).trim()))
    .filter(Boolean)
    .join("-") || "weak-point";

  const duplicateCount = duplicates[keyBase] ?? 0;
  duplicates[keyBase] = duplicateCount + 1;

  return duplicateCount === 0 ? keyBase : `${keyBase}-${duplicateCount + 1}`;
};

const weakPointItems = computed(() => (Array.isArray(props.items)
  ? props.items.filter((item) => isRenderableObject(item))
  : []));

const handleOpenResource = (code) => {
  if (!code) {
    return;
  }

  emit("open-resource", { code });
};

const normalizedItems = computed(() => {
  const duplicateKeys = {};

  return weakPointItems.value
    .map((item) => {
      const rawCode = normalizeText(item.code);
      const code = rawCode || null;
      const name = normalizeText(item.name);
      const chapterNo = item.chapterNo;
      const chapterName = normalizeText(item.chapterName);
      const hasChapterNo = chapterNo !== null && chapterNo !== undefined && `${chapterNo}`.trim() !== "";
      const hasChapterName = chapterName !== "";

      let chapterText = "章节信息待补充";

      if (hasChapterNo && hasChapterName) {
        chapterText = `第${chapterNo}章 · ${chapterName}`;
      } else if (hasChapterNo) {
        chapterText = `第${chapterNo}章`;
      } else if (hasChapterName) {
        chapterText = chapterName;
      }

      return {
        source: item,
        code,
        name,
        masteryPercent: clampPercent(item.masteryPercent),
        chapterText,
      };
    })
    .filter((item) => item.name !== "" && isFiniteNumber(item.source.masteryPercent))
    .map((item) => ({
      itemKey: buildWeakPointKey(item.source, duplicateKeys),
      code: item.code,
      name: item.name,
      masteryPercent: item.masteryPercent,
      chapterText: item.chapterText,
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

.weak-list {
  list-style: none;
  margin: 0;
  padding: 0;
  display: grid;
  gap: 12px;
}

.weak-item {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  align-items: center;
  padding: 14px 16px;
  border-radius: 18px;
  background: #f7f9fc;
}

.weak-item p {
  margin: 6px 0 0;
  color: #5f6d79;
}

.weak-side {
  display: grid;
  justify-items: end;
  gap: 8px;
}

.weak-side span {
  font-weight: 700;
}

.action-button {
  border: none;
  border-radius: 999px;
  padding: 8px 14px;
  background: #1f5eff;
  color: #ffffff;
  font-weight: 600;
  cursor: pointer;
}

.action-button:hover {
  background: #194ed4;
}

.action-button:disabled {
  background: #b8c3d1;
  cursor: not-allowed;
}

.empty-tip {
  margin: 0;
  color: #6d7781;
}

@media (max-width: 640px) {
  .card-head,
  .weak-item {
    flex-direction: column;
    align-items: start;
  }

  .weak-side {
    width: 100%;
    justify-items: start;
  }
}
</style>
