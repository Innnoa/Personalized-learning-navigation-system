<template>
  <PageLayout eyebrow="管理后台" title="课程管理" description="编辑课程名称和适用对象，删除课程。" role-scope="admin">
    <div class="admin-actions">
      <span v-if="msg" class="edit-message" :class="msgClass">{{ msg }}</span>
    </div>
    <p v-if="loadError" class="state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">加载中...</p>
    <table v-else class="admin-table">
      <thead>
        <tr><th>课程代码</th><th>名称</th><th>适用对象</th><th>知识点数</th><th>操作</th></tr>
      </thead>
      <tbody>
        <tr v-for="c in courses" :key="c.courseCode">
          <template v-if="editingCode === c.courseCode">
            <td>{{ c.courseCode }}</td>
            <td><input v-model="editForm.name" class="edit-input" /></td>
            <td><input v-model="editForm.targetAudience" class="edit-input" /></td>
            <td>{{ c.knowledgePointCount }}</td>
            <td class="cell-actions">
              <button class="btn btn--small btn--primary" @click="saveEdit(c.courseCode)">保存</button>
              <button class="btn btn--small" @click="editingCode = null">取消</button>
            </td>
          </template>
          <template v-else>
            <td>{{ c.courseCode }}</td>
            <td>{{ c.courseName }}</td>
            <td>{{ c.targetAudience }}</td>
            <td>{{ c.knowledgePointCount }}</td>
            <td class="cell-actions">
              <button class="btn btn--small" @click="startEdit(c)">编辑</button>
              <button class="btn btn--small" style="color:#9b3333" @click="confirmDelete(c)">删除</button>
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
import { fetchAdminCourses, updateAdminCourse, deleteAdminCourse } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const courses = ref([]);
const editingCode = ref(null);
const msg = ref("");
const msgClass = ref("");

const editForm = reactive({ name: "", targetAudience: "" });

async function load() {
  loading.value = true;
  try { courses.value = (await fetchAdminCourses())?.courses || []; }
  catch { loadError.value = "加载失败"; }
  finally { loading.value = false; }
}

function startEdit(c) {
  editingCode.value = c.courseCode;
  editForm.name = c.courseName;
  editForm.targetAudience = c.targetAudience || "";
}

async function saveEdit(code) {
  try {
    await updateAdminCourse(code, { name: editForm.name, targetAudience: editForm.targetAudience });
    msg.value = "已更新"; msgClass.value = "edit-message--ok";
    editingCode.value = null;
    await load();
  } catch (e) { msg.value = "失败"; msgClass.value = "edit-message--error"; }
}

async function confirmDelete(c) {
  if (!confirm(`确定删除课程 "${c.courseName}"？`)) return;
  try {
    await deleteAdminCourse(c.courseCode);
    msg.value = "已删除"; msgClass.value = "edit-message--ok";
    await load();
  } catch (e) { msg.value = "删除失败: " + (e?.response?.data?.detail || e.message); msgClass.value = "edit-message--error"; }
}

onMounted(load);
</script>

<style scoped>
.admin-view { display: grid; gap: 16px; }
.admin-actions { display: flex; gap: 12px; flex-wrap: wrap; align-items: center; }
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
