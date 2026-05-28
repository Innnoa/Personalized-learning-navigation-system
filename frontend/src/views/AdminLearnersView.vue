<template>
  <PageLayout eyebrow="管理后台" title="学习者管理" description="查看、编辑、删除全局学习者，变更所属课程。" role-scope="admin">
    <div class="admin-actions">
      <span v-if="msg" class="edit-message" :class="msgClass">{{ msg }}</span>
    </div>
    <p v-if="loadError" class="state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">加载中...</p>
    <table v-else class="admin-table">
      <thead>
        <tr><th>姓名</th><th>代码</th><th>专业</th><th>年级</th><th>课程</th><th>操作</th></tr>
      </thead>
      <tbody>
        <tr v-for="l in learners" :key="l.learnerCode">
          <template v-if="editingCode === l.learnerCode">
            <td><input v-model="editForm.name" class="edit-input" /></td>
            <td>{{ l.learnerCode }}</td>
            <td><input v-model="editForm.major" class="edit-input" /></td>
            <td><input v-model="editForm.gradeLabel" class="edit-input" /></td>
            <td>{{ l.courses ? l.courses.map(c => c.courseName || c.courseCode).join(', ') : '' }}</td>
            <td class="cell-actions">
              <button class="btn btn--small btn--primary" @click="saveEdit(l.learnerCode)">保存</button>
              <button class="btn btn--small" @click="editingCode = null">取消</button>
            </td>
          </template>
          <template v-else>
            <td>{{ l.learnerName }}</td>
            <td>{{ l.learnerCode }}</td>
            <td>{{ l.major }}</td>
            <td>{{ l.gradeLabel }}</td>
            <td>{{ l.courses ? l.courses.map(c => c.courseName || c.courseCode).join(', ') : '未分配' }}</td>
            <td class="cell-actions">
              <button class="btn btn--small" @click="startEdit(l)">编辑</button>
              <button class="btn btn--small" style="color:#9b3333" @click="confirmDelete(l)">删除</button>
            </td>
          </template>
        </tr>
      </tbody>
    </table>
  </PageLayout>
</template>

<script setup>
import { onMounted, reactive, ref } from "vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchAdminLearners, updateAdminLearner, deleteAdminLearner, reassignLearnerCourse } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const learners = ref([]);
const editingCode = ref(null);
const msg = ref("");
const msgClass = ref("");
const editForm = reactive({ name: "", major: "", gradeLabel: "" });

async function load() {
  loading.value = true; loadError.value = "";
  try { learners.value = (await fetchAdminLearners())?.learners || []; }
  catch { loadError.value = "加载失败"; }
  finally { loading.value = false; }
}

function startEdit(l) {
  editingCode.value = l.learnerCode;
  editForm.name = l.learnerName;
  editForm.major = l.major;
  editForm.gradeLabel = l.gradeLabel;
}

async function saveEdit(code) {
  try {
    await updateAdminLearner(code, { name: editForm.name, major: editForm.major, gradeLabel: editForm.gradeLabel });
    msg.value = "已更新"; msgClass.value = "edit-message--ok";
    editingCode.value = null;
    await load();
  } catch (e) { msg.value = "失败"; msgClass.value = "edit-message--error"; }
}

async function confirmDelete(l) {
  if (!confirm(`确定删除学习者 "${l.learnerName}"？`)) return;
  try { await deleteAdminLearner(l.learnerCode); msg.value = "已删除"; await load(); }
  catch (e) { msg.value = "删除失败"; msgClass.value = "edit-message--error"; }
}

onMounted(load);
</script>

<style scoped>
.admin-view { display: grid; gap: 16px; }
.admin-actions { display: flex; gap: 12px; align-items: center; }
.admin-table { width: 100%; border-collapse: collapse; font-size: 0.9rem; }
.admin-table th, .admin-table td { padding: 10px 12px; border-bottom: 1px solid #d8e0e6; text-align: left; }
.admin-table th { color: #51606d; }
.btn { padding: 8px 16px; border: 1px solid #d8e0e6; border-radius: 8px; background: #fff; color: #15364a; font-weight: 600; cursor: pointer; }
.btn:hover { background: #eef7f8; }
.btn--primary { background: #0c6a71; color: #fff; border-color: #0c6a71; }
.btn--small { padding: 4px 10px; font-size: 0.85rem; }
.cell-actions { display: flex; gap: 6px; }
.edit-input { padding: 4px 6px; border: 1px solid #a0b8c4; border-radius: 4px; font-size: 0.85rem; width: 100%; box-sizing: border-box; }
.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }
.state-message, .state-message--error { color: #51606d; }
.state-message--error { color: #9b3333; }
</style>
