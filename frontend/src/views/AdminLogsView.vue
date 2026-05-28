<template>
  <PageLayout eyebrow="管理后台" title="操作日志" description="查看管理员关键操作记录。" role-scope="admin">
    <h4>操作日志（最近 {{ limit }} 条）</h4>
    <p v-if="loadError" class="state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">加载中...</p>
    <table v-else class="admin-table">
      <thead>
        <tr><th>时间</th><th>用户</th><th>操作</th><th>目标</th><th>详情</th></tr>
      </thead>
      <tbody>
        <tr v-for="(l, i) in logs" :key="i">
          <td>{{ l.timestamp }}</td>
          <td>{{ l.username }}</td>
          <td>{{ l.action }}</td>
          <td>{{ l.target }}</td>
          <td>{{ l.detail }}</td>
        </tr>
      </tbody>
    </table>
    <p v-if="!loading && logs.length === 0" class="state-message">暂无操作记录。</p>
  </PageLayout>
</template>

<script setup>
import { onMounted, ref } from "vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchAdminLogs } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const logs = ref([]);
const limit = 100;

onMounted(async () => {
  try { logs.value = (await fetchAdminLogs(limit))?.logs || []; }
  catch { loadError.value = "加载失败"; }
  finally { loading.value = false; }
});
</script>

<style scoped>
.admin-view { display: grid; gap: 16px; }
.admin-view h4 { margin: 0; color: #15364a; }
.admin-table { width: 100%; border-collapse: collapse; font-size: 0.85rem; }
.admin-table th, .admin-table td { padding: 8px 10px; border-bottom: 1px solid #d8e0e6; text-align: left; }
.admin-table th { color: #51606d; }
.state-message, .state-message--error { color: #51606d; }
.state-message--error { color: #9b3333; }
</style>
