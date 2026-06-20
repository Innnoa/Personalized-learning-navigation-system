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
          <template v-if="roleScope === 'admin'">
            <RouterLink to="/admin" class="page-nav-link">后台概览</RouterLink>
            <RouterLink to="/admin/users" class="page-nav-link">用户管理</RouterLink>
            <RouterLink to="/admin/courses" class="page-nav-link">课程管理</RouterLink>
            <RouterLink to="/admin/assignments" class="page-nav-link">分配管理</RouterLink>
            <RouterLink to="/admin/learners" class="page-nav-link">学习者</RouterLink>
            <RouterLink to="/admin/logs" class="page-nav-link">日志</RouterLink>
            <button type="button" class="page-nav-link logout-btn" @click="handleLogout">退出</button>
          </template>
          <template v-else-if="roleScope === 'teacher'">
            <RouterLink to="/teacher" class="page-nav-link">
              我的课程
            </RouterLink>
            <RouterLink to="/teacher/courses" class="page-nav-link">
              课程列表
            </RouterLink>
            <button type="button" class="page-nav-link logout-btn" @click="handleLogout">
              退出
            </button>
          </template>
          <template v-else>
            <RouterLink to="/learning-graph" class="page-nav-link">
              学习图谱
            </RouterLink>
            <RouterLink :to="plannerNavTarget" class="page-nav-link">
              路径规划
            </RouterLink>
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
            <select
              v-if="authStore.linkedLearners.length > 1"
              :value="authStore.activeLearnerCode"
              class="course-switcher"
              @change="switchCourse($event.target.value)"
            >
              <option v-for="ll in authStore.linkedLearners" :key="ll.learnerCode" :value="ll.learnerCode">
                {{ ll.courseName || ll.courseCode }}
              </option>
            </select>
            <button type="button" class="page-nav-link logout-btn" @click="handleLogout">
              退出
            </button>
          </template>
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
import { useRoute, useRouter } from "vue-router";

import { logout } from "../api/auth";
import { useAuthStore } from "../stores/authStore";
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
      "当前课程固定为数据结构，页面按路径规划、学习图谱、推荐资源和学习者画像组织，便于分模块演示。",
  },
  roleScope: {
    type: String,
    default: "student",
  },
});

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();
const navigationStore = useNavigationStore();
const plannerNavTarget = computed(() => {
  const scopeCode = String(navigationStore.plannerContext?.scopeCode || "root");
  const targetCode = String(navigationStore.plannerContext?.targetCode || "");

  if (!scopeCode || scopeCode === "root") {
    if (!targetCode) {
      return "/path-planning";
    }

    return {
      name: "home",
      query: {
        target: targetCode,
      },
    };
  }

  if (!scopeCode) {
    return "/path-planning";
  }
  return {
    name: "home",
    query: targetCode
      ? {
          scope: scopeCode,
          target: targetCode,
        }
      : {
          scope: scopeCode,
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

const isResourceNavActive = computed(
  () => route.name === "resource-recommendation" || Boolean(route.params?.code),
);

async function handleLogout() {
  try {
    await logout();
  } catch {
    // proceed to clear local session even if server call fails
  }
  authStore.clearSession();
  router.push({ name: "login" });
}

function switchCourse(learnerCode) {
  authStore.switchLearner(learnerCode);
  window.location.reload();
}
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
  background: inherit;
  color: inherit;
}

.course-switcher {
  padding: 6px 12px;
  border-radius: 999px;
  border: 1px solid rgba(12, 106, 113, 0.24);
  background: rgba(255, 255, 255, 0.86);
  color: #0c6a71;
  font-weight: 700;
  font-size: 0.9rem;
  cursor: pointer;
}
</style>
