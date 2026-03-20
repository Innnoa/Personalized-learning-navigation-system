<template>
  <section class="card">
    <div class="card-head">
      <div>
        <p class="label">后端连通性</p>
        <h2>健康检查</h2>
      </div>
      <span class="badge" :class="badgeClass">{{ badgeText }}</span>
    </div>

    <p class="message">{{ message }}</p>

    <dl v-if="health" class="meta-grid">
      <div>
        <dt>服务名</dt>
        <dd>{{ health.service }}</dd>
      </div>
      <div>
        <dt>课程</dt>
        <dd>{{ health.course }}</dd>
      </div>
      <div>
        <dt>状态</dt>
        <dd>{{ health.status }}</dd>
      </div>
      <div>
        <dt>时间</dt>
        <dd>{{ health.timestamp }}</dd>
      </div>
    </dl>
  </section>
</template>

<script setup>
import { computed } from "vue";

const props = defineProps({
  health: {
    type: Object,
    default: null,
  },
  error: {
    type: String,
    default: "",
  },
});

const badgeText = computed(() => (props.health ? "已连接" : "待启动"));
const badgeClass = computed(() => (props.health ? "badge--ok" : "badge--warn"));
const message = computed(() => {
  if (props.health) {
    return props.health.message;
  }

  return props.error || "后端尚未启动，当前页面展示的是前端骨架与演示图谱。";
});
</script>

<style scoped>
.card {
  background: rgba(255, 255, 255, 0.8);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
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

h2 {
  margin: 0;
  font-size: 1.3rem;
}

.message {
  margin: 16px 0 0;
  color: #32404a;
}

.meta-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 14px;
  margin: 18px 0 0;
}

dt {
  font-size: 0.82rem;
  color: #6d7781;
}

dd {
  margin: 4px 0 0;
  font-weight: 600;
}

.badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  font-size: 0.88rem;
  font-weight: 700;
}

.badge--ok {
  background: #dff4e6;
  color: #176b39;
}

.badge--warn {
  background: #fff0d8;
  color: #8a5700;
}

@media (max-width: 640px) {
  .meta-grid {
    grid-template-columns: 1fr;
  }
}
</style>
