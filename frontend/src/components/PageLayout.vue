<template>
  <div class="page-shell">
    <header class="hero">
      <div>
        <p class="eyebrow">{{ eyebrow }}</p>
        <h1>{{ title }}</h1>
        <p class="hero-copy">
          {{ description }}
        </p>
        <nav class="page-nav">
          <RouterLink to="/" class="page-nav-link">
            首页
          </RouterLink>
          <RouterLink to="/learning-graph" class="page-nav-link">
            学习图谱
          </RouterLink>
          <RouterLink
            v-if="detailLearningNavTarget"
            :to="detailLearningNavTarget"
            class="page-nav-link"
            :class="{
              'page-nav-link--active': isDetailLearningNavActive,
            }"
          >
            细化学习
          </RouterLink>
          <span v-else class="page-nav-link page-nav-link--disabled">
            细化学习
          </span>
          <RouterLink
            v-if="resourceNavTarget"
            :to="resourceNavTarget"
            class="page-nav-link"
            :class="{
              'page-nav-link--active': isResourceNavActive,
            }"
          >
            推荐资源
          </RouterLink>
          <span v-else class="page-nav-link page-nav-link--disabled">
            推荐资源
          </span>
          <RouterLink to="/learner-profile" class="page-nav-link">
            学习者画像
          </RouterLink>
        </nav>
      </div>
      <slot name="hero-side" />
    </header>

    <main class="content">
      <slot />
    </main>
  </div>
</template>

<script setup>
import { computed } from "vue";
import { useRoute } from "vue-router";

import { useNavigationStore } from "../stores/navigationStore";

defineProps({
  eyebrow: {
    type: String,
    default: "本科毕设 · 最小可运行骨架",
  },
  title: {
    type: String,
    default: "基于知识点依赖图的个性化学习导航系统",
  },
  description: {
    type: String,
    default:
      "当前课程固定为“数据结构”，页面按路径规划、学习图谱、推荐资源和学习者画像组织，便于分模块演示。",
  },
});

const route = useRoute();
const navigationStore = useNavigationStore();

const detailLearningNavTarget = computed(() => {
  const currentScopeCode = String(route.query?.scope || "");
  if (
    route.name === "detail-learning" &&
    currentScopeCode &&
    navigationStore.detailLearningSectionByScopeCode(currentScopeCode)
  ) {
    return {
      name: "detail-learning",
      query: {
        scope: currentScopeCode,
      },
    };
  }

  const fallbackScopeCode =
    navigationStore.detailLearningSummary?.selectedScopeCode ||
    navigationStore.detailLearningSections[0]?.scopeCode ||
    "";
  if (!fallbackScopeCode) {
    return null;
  }

  return {
    name: "detail-learning",
    query: {
      scope: fallbackScopeCode,
    },
  };
});

const resourceNavCode = computed(() => {
  const currentCode = String(route.params?.code || "");
  if (currentCode) {
    return currentCode;
  }

  return navigationStore.resourceRecommendationSections[0]?.code || "";
});

const resourceNavLevel = computed(() => {
  const currentLevel = String(route.query?.level || "");
  if (currentLevel === "detail" || currentLevel === "main") {
    return currentLevel;
  }

  return navigationStore.resourceRecommendationSummary?.contextMode || "main";
});

const resourceNavTarget = computed(() => {
  if (!resourceNavCode.value) {
    return null;
  }

  return {
    name: "resource-recommendation",
    params: {
      code: resourceNavCode.value,
    },
    query: {
      level: resourceNavLevel.value,
    },
  };
});

const isDetailLearningNavActive = computed(() => route.name === "detail-learning");

const isResourceNavActive = computed(
  () => route.name === "resource-recommendation" || Boolean(route.params?.code),
);
</script>

<style scoped>
.page-shell {
  width: 100%;
  max-width: none;
  padding: 36px clamp(20px, 3vw, 44px) 56px;
}

.hero {
  display: grid;
  grid-template-columns: minmax(0, 1.7fr) minmax(320px, 1fr);
  gap: clamp(20px, 2vw, 30px);
  align-items: start;
  margin-bottom: 28px;
}

.eyebrow {
  margin: 0 0 10px;
  color: #0c6a71;
  font-size: 0.92rem;
  font-weight: 700;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

h1 {
  margin: 0 0 16px;
  font-size: clamp(2rem, 4vw, 3.4rem);
  line-height: 1.08;
}

.hero-copy {
  max-width: 72ch;
  margin: 0;
  font-size: 1.02rem;
  color: #36424d;
}

.page-nav {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  margin-top: 22px;
}

.page-nav-link {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  min-width: 116px;
  padding: 10px 16px;
  border-radius: 999px;
  border: 1px solid rgba(12, 106, 113, 0.16);
  background: rgba(255, 255, 255, 0.86);
  color: #0c5960;
  font-weight: 700;
  text-decoration: none;
  transition: transform 0.15s ease, background 0.15s ease, color 0.15s ease;
}

.page-nav-link:hover {
  transform: translateY(-1px);
  background: rgba(12, 106, 113, 0.08);
}

.page-nav-link.router-link-exact-active {
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  border-color: transparent;
  box-shadow: var(--panel-shadow-strong);
}

.page-nav-link--active {
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  border-color: transparent;
  box-shadow: var(--panel-shadow-strong);
}

.page-nav-link--disabled {
  background: rgba(241, 245, 247, 0.92);
  color: #93a0aa;
  border-color: rgba(147, 160, 170, 0.26);
  cursor: not-allowed;
}

.page-nav-link--disabled:hover {
  transform: none;
  background: rgba(241, 245, 247, 0.92);
}

.content {
  display: grid;
  gap: 18px;
}

@media (max-width: 860px) {
  .hero {
    grid-template-columns: 1fr;
  }
}
</style>
