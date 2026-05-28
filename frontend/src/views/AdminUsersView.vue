<template>
  <PageLayout eyebrow="管理后台" title="用户管理" description="创建、禁用用户，设置角色。" role-scope="admin">
    <div class="admin-actions">
      <button type="button" class="btn btn--primary" @click="showCreate = !showCreate">{{ showCreate ? "收起" : "新建用户" }}</button>
      <span v-if="msg" class="edit-message" :class="msgClass">{{ msg }}</span>
    </div>

    <div v-if="showCreate" class="create-panel">
      <h4>新建用户</h4>
      <div class="create-grid">
        <input v-model="form.username" placeholder="用户名" class="edit-input" />
        <input v-model="form.displayName" placeholder="显示名" class="edit-input" />
        <input v-model="form.password" placeholder="密码" type="text" class="edit-input" />
        <div class="edit-field">
          <label class="check-label"><input type="checkbox" v-model="form.roles" value="student" /> student</label>
          <label class="check-label"><input type="checkbox" v-model="form.roles" value="teacher" /> teacher</label>
          <label class="check-label"><input type="checkbox" v-model="form.roles" value="admin" /> admin</label>
        </div>
      </div>
      <div class="create-actions">
        <button type="button" class="btn btn--primary" @click="submitCreate" :disabled="saving">创建</button>
        <button type="button" class="btn" @click="showCreate = false">取消</button>
      </div>
    </div>

    <p v-if="loadError" class="state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">加载中...</p>

    <table v-else class="admin-table">
      <thead>
        <tr><th>用户名</th><th>显示名</th><th>状态</th><th>角色</th><th>操作</th></tr>
      </thead>
      <tbody>
        <tr v-for="u in users" :key="u.username">
          <td>{{ u.username }}</td>
          <td>{{ u.displayName }}</td>
          <td><span :class="'status-' + u.status">{{ u.status }}</span></td>
          <td>{{ (u.roles || []).join("/") }}</td>
          <td class="cell-actions">
            <button class="btn btn--small" @click="toggleStatus(u)">{{ u.status === "active" ? "禁用" : "启用" }}</button>
            <button class="btn btn--small" @click="startEditRoles(u)">角色</button>
          </td>
        </tr>
      </tbody>
    </table>

    <div v-if="editingUser" class="create-panel" style="margin-top:12px">
      <h4>设置角色: {{ editingUser.username }}</h4>
      <div>
        <label class="check-label"><input type="checkbox" :checked="editingRoles.includes('student')" @change="toggleEditRole('student')" /> student</label>
        <label class="check-label"><input type="checkbox" :checked="editingRoles.includes('teacher')" @change="toggleEditRole('teacher')" /> teacher</label>
        <label class="check-label"><input type="checkbox" :checked="editingRoles.includes('admin')" @change="toggleEditRole('admin')" /> admin</label>
      </div>
      <div class="create-actions" style="margin-top:8px">
        <button class="btn btn--primary" @click="saveRoles" :disabled="saving">保存</button>
        <button class="btn" @click="editingUser = null">取消</button>
      </div>
    </div>
  </PageLayout>
</template>

<script setup>
import { onMounted, reactive, ref } from "vue";
import PageLayout from "../components/PageLayout.vue";
import { fetchAdminUsers, createAdminUser, toggleAdminUserStatus, setAdminUserRoles } from "../api/admin";

const loading = ref(true);
const loadError = ref("");
const users = ref([]);
const showCreate = ref(false);
const editingUser = ref(null);
const editingRoles = ref([]);
const saving = ref(false);
const msg = ref("");
const msgClass = ref("");

const form = reactive({ username: "", displayName: "", password: "", roles: [] });

async function load() {
  loading.value = true;
  loadError.value = "";
  try {
    users.value = (await fetchAdminUsers())?.users || [];
  } catch { loadError.value = "加载失败"; }
  finally { loading.value = false; }
}

async function submitCreate() {
  if (!form.username || !form.password) return;
  saving.value = true;
  try {
    await createAdminUser({ username: form.username, displayName: form.displayName, password: form.password, roles: [...form.roles] });
    msg.value = "已创建";
    msgClass.value = "edit-message--ok";
    showCreate.value = false;
    form.username = ""; form.displayName = ""; form.password = ""; form.roles = [];
    await load();
  } catch (e) { msg.value = "失败: " + (e?.response?.data?.detail || e.message); msgClass.value = "edit-message--error"; }
  finally { saving.value = false; }
}

async function toggleStatus(u) {
  try {
    await toggleAdminUserStatus(u.username);
    msg.value = u.username + " 已" + (u.status === "active" ? "禁用" : "启用");
    msgClass.value = "edit-message--ok";
    await load();
  } catch (e) { msg.value = "失败"; msgClass.value = "edit-message--error"; }
}

function startEditRoles(u) {
  editingUser.value = u;
  editingRoles.value = [...(u.roles || [])];
  msg.value = "";
}

function toggleEditRole(role) {
  const i = editingRoles.value.indexOf(role);
  if (i >= 0) editingRoles.value.splice(i, 1);
  else editingRoles.value.push(role);
}

async function saveRoles() {
  saving.value = true;
  try {
    await setAdminUserRoles(editingUser.value.username, { roles: editingRoles.value });
    msg.value = "角色已更新";
    msgClass.value = "edit-message--ok";
    editingUser.value = null;
    await load();
  } catch (e) { msg.value = "失败"; msgClass.value = "edit-message--error"; }
  finally { saving.value = false; }
}

onMounted(load);
</script>

<style scoped>
.admin-view { display: grid; gap: 16px; }
.admin-actions { display: flex; align-items: center; gap: 12px; flex-wrap: wrap; }
.btn { padding: 8px 16px; border: 1px solid #d8e0e6; border-radius: 8px; background: #fff; color: #15364a; font-weight: 600; cursor: pointer; }
.btn:hover { background: #eef7f8; }
.btn--primary { background: #0c6a71; color: #fff; border-color: #0c6a71; }
.btn--small { padding: 4px 10px; font-size: 0.85rem; }
.admin-table { width: 100%; border-collapse: collapse; font-size: 0.9rem; }
.admin-table th, .admin-table td { padding: 10px 12px; border-bottom: 1px solid #d8e0e6; text-align: left; }
.admin-table th { color: #51606d; }
.cell-actions { display: flex; gap: 6px; }
.status-active { color: #1a7a3a; font-weight: 600; }
.status-disabled { color: #9b3333; }
.create-panel { padding: 14px; border: 1px solid #d8e0e6; border-radius: 12px; background: #f9fdfd; }
.create-panel h4 { margin: 0 0 10px; color: #15364a; }
.create-grid { display: grid; gap: 8px; grid-template-columns: 1fr 1fr; }
.edit-input { padding: 6px 8px; border: 1px solid #a0b8c4; border-radius: 4px; font-size: 0.85rem; width: 100%; box-sizing: border-box; }
.edit-field { display: flex; gap: 12px; align-items: center; }
.check-label { display: inline-flex; align-items: center; gap: 4px; font-size: 0.85rem; cursor: pointer; }
.create-actions { display: flex; gap: 8px; }
.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }
.state-message, .state-message--error { color: #51606d; }
.state-message--error { color: #9b3333; }
</style>
