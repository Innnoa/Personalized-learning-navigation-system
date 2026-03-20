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
      "当前课程已固定为“数据结构”，本页用于承接前端首页、后端健康检查、知识图谱预览与后续路径规划结果展示。",
  },
});
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

.page-nav-link.router-link-active {
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  border-color: transparent;
  box-shadow: 0 12px 24px rgba(12, 106, 113, 0.18);
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
