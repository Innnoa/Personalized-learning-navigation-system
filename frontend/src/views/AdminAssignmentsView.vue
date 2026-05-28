<template>
  <PageLayout eyebrow="管理后台" title="教师分配" description="指派教师到课程或撤销分配。" role-scope="admin">
    <div class="admin-actions">
      <button type="button" class="btn btn--primary" @click="showAssign = !showAssign">{{ showAssign ? "收起" : "新增分配" }}</button>
      <span v-if="msg" class="edit-message" :class="msgClass">{{ msg }}</span>
    </div>

    <div v-if="showAssign" class="create-panel">
      <h4>指派教师到课程</h4>
      <div class="create-grid">
        <input v-model="assignForm.username" placeholder="教师用户名" class="edit-input" />
        <input v-model="assignForm.courseCode" placeholder="课程代码" class="edit-input" />
      </div>
      <div class="create-actions">
        <button class="btn btn--primary" @click="submitAssign" :disabled="saving">指派</button>
        <button class="btn" @click="showAssign = false">取消</button>
      </div>
    </div>

    <p v-if="loadError" class="state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">加载中...</p>

    <table v-else class="admin-table">
      <thead>
        <tr><th>教师</th><th>课程代码</th><th>课程名称</th><th>操作</th></tr>
      </thead>
      <tbody>
        <tr v-for="a in assignments" :key="a.teacherUsername + a.courseCode">
          <td>{{ a.teacherUsername }}</td>
          <td>{{ a.courseCode }}</td>
          <td>{{ a.courseName }}</td>
          <td>
            <button class="btn btn--small" style="color:#9b3333" @click="confirmRemove(a)">撤销</button>
          </td>
        </tr>
      </tbody>
    </table>
  </PageLayout>
</template>

<script setup>
import { onMounted, reactive, ref } from "vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchAdminAssignments, assignTeacherToCourse, unassignTeacherFromCourse } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const assignments = ref([]);
const showAssign = ref(false);
const saving = ref(false);
const msg = ref("");
const msgClass = ref("");
const assignForm = reactive({ username: "", courseCode: "" });

async function load() {
  loading.value = true;
  try { assignments.value = (await fetchAdminAssignments())?.assignments || []; }
  catch { loadError.value = "加载失败"; }
  finally { loading.value = false; }
}

async function submitAssign() {
  if (!assignForm.username || !assignForm.courseCode) return;
  saving.value = true;
  try {
    await assignTeacherToCourse({ username: assignForm.username, courseCode: assignForm.courseCode });
    msg.value = "已指派"; msgClass.value = "edit-message--ok";
    showAssign.value = false;
    assignForm.username = ""; assignForm.courseCode = "";
    await load();
  } catch (e) { msg.value = "失败: " + (e?.response?.data?.detail || e.message); msgClass.value = "edit-message--error"; }
  finally { saving.value = false; }
}

async function confirmRemove(a) {
  if (!confirm(`撤销 ${a.teacherUsername} 的 ${a.courseCode} 授课权限？`)) return;
  try {
    await unassignTeacherFromCourse(a.teacherUsername, a.courseCode);
    msg.value = "已撤销"; msgClass.value = "edit-message--ok";
    await load();
  } catch (e) { msg.value = "撤销失败"; msgClass.value = "edit-message--error"; }
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
.create-panel { padding: 14px; border: 1px solid #d8e0e6; border-radius: 12px; background: #f9fdfd; }
.create-panel h4 { margin: 0 0 10px; color: #15364a; }
.create-grid { display: grid; gap: 8px; grid-template-columns: 1fr 1fr; }
.edit-input { padding: 6px 8px; border: 1px solid #a0b8c4; border-radius: 4px; font-size: 0.85rem; width: 100%; box-sizing: border-box; }
.create-actions { display: flex; gap: 8px; margin-top: 8px; }
.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }
.state-message, .state-message--error { color: #51606d; }
.state-message--error { color: #9b3333; }
</style>
