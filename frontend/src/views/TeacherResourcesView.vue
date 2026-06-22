<template>
  <section class="resources-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载学习资源...</p>

    <template v-else>
      <div class="toolbar">
        <button type="button" class="btn btn--primary" @click="openCreateDialog">
          + 新增资源
        </button>
        <input v-model.trim="keyword" class="filter-input" placeholder="搜索标题 / 来源 / 描述" />
        <select v-model="filterCode" class="filter-select">
          <option value="">全部知识点</option>
          <option v-for="point in knowledgePointOptions" :key="point" :value="point">
            {{ point }}
          </option>
        </select>
        <select v-model="filterType" class="filter-select">
          <option value="">全部类型</option>
          <option value="video">video</option>
          <option value="article">article</option>
          <option value="slides">slides</option>
          <option value="book">book</option>
          <option value="website">website</option>
          <option value="exercise">exercise</option>
          <option value="other">other</option>
        </select>
        <select v-model="filterPhase" class="filter-select">
          <option value="">全部阶段</option>
          <option value="preview">preview</option>
          <option value="learn">learn</option>
          <option value="practice">practice</option>
          <option value="review">review</option>
          <option value="extend">extend</option>
        </select>
      </div>

      <div v-if="saveMessage" class="edit-message" :class="saveMessageClass">{{ saveMessage }}</div>

      <div class="table-shell">
        <table class="resource-table">
          <thead>
            <tr>
              <th>标题</th>
              <th>知识点</th>
              <th>类型</th>
              <th>阶段</th>
              <th>权重 / 时长</th>
              <th>掌握度区间</th>
              <th>来源</th>
              <th>状态</th>
              <th>操作</th>
            </tr>
          </thead>
          <tbody>
            <tr v-if="filteredResources.length === 0">
              <td colspan="9" class="empty-cell">暂无匹配的学习资源。</td>
            </tr>
            <tr v-for="resource in filteredResources" :key="resource.id">
              <td>
                <strong>{{ resource.title }}</strong>
                <p v-if="resource.description" class="meta-line">{{ resource.description }}</p>
              </td>
              <td>{{ resource.knowledgePointCode || "课程公共资源" }}</td>
              <td>{{ resource.resourceType }}</td>
              <td>{{ resource.recommendedPhase }}</td>
              <td>{{ formatWeight(resource.importanceWeight) }} / {{ resource.estimatedMinutes }} 分钟</td>
              <td>{{ formatMasteryRange(resource.minMastery, resource.maxMastery) }}</td>
              <td>
                <span>{{ resource.source || "-" }}</span>
                <a v-if="resource.url" :href="resource.url" target="_blank" class="resource-link">
                  打开链接
                </a>
              </td>
              <td>{{ resource.status }}</td>
              <td class="actions-cell">
                <button type="button" class="btn btn--small" @click="openEditDialog(resource)">编辑</button>
                <button type="button" class="btn btn--small btn--danger" @click="removeResource(resource.id)">删除</button>
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <div v-if="dialogOpen" class="dialog-backdrop" @click.self="closeDialog">
        <div class="dialog-panel">
          <div class="dialog-head">
            <h3>{{ editingResourceId === null ? "新增学习资源" : "编辑学习资源" }}</h3>
            <button type="button" class="btn btn--small" @click="closeDialog">关闭</button>
          </div>
          <div class="form-grid">
            <label class="field">
              <span>知识点代码</span>
              <input v-model="editForm.knowledgePointCode" class="edit-input" placeholder="留空表示课程公共资源" />
            </label>
            <label class="field">
              <span>资源标题</span>
              <input v-model="editForm.title" class="edit-input" />
            </label>
            <label class="field">
              <span>资源类型</span>
              <select v-model="editForm.resourceType" class="edit-input">
                <option value="video">video</option>
                <option value="article">article</option>
                <option value="slides">slides</option>
                <option value="book">book</option>
                <option value="website">website</option>
                <option value="exercise">exercise</option>
                <option value="other">other</option>
              </select>
            </label>
            <label class="field">
              <span>推荐阶段</span>
              <select v-model="editForm.recommendedPhase" class="edit-input">
                <option value="preview">preview</option>
                <option value="learn">learn</option>
                <option value="practice">practice</option>
                <option value="review">review</option>
                <option value="extend">extend</option>
              </select>
            </label>
            <label class="field">
              <span>来源</span>
              <input v-model="editForm.source" class="edit-input" />
            </label>
            <label class="field">
              <span>链接</span>
              <input v-model="editForm.url" class="edit-input" />
            </label>
            <label class="field">
              <span>推荐权重</span>
              <input v-model.number="editForm.importanceWeight" class="edit-input" type="number" min="0" step="0.1" />
            </label>
            <label class="field">
              <span>预计时长（分钟）</span>
              <input v-model.number="editForm.estimatedMinutes" class="edit-input" type="number" min="1" />
            </label>
            <label class="field">
              <span>最低掌握度</span>
              <input v-model.number="editForm.minMastery" class="edit-input" type="number" min="0" max="1" step="0.05" />
            </label>
            <label class="field">
              <span>最高掌握度</span>
              <input v-model.number="editForm.maxMastery" class="edit-input" type="number" min="0" max="1" step="0.05" />
            </label>
            <label class="field field--full">
              <span>推荐用法</span>
              <textarea v-model="editForm.recommendedUsage" class="edit-input" rows="2" />
            </label>
            <label class="field field--full">
              <span>描述</span>
              <textarea v-model="editForm.description" class="edit-input" rows="3" />
            </label>
          </div>
          <div class="dialog-actions">
            <button type="button" class="btn btn--primary" :disabled="submitting" @click="submitResource">
              {{ submitting ? "提交中..." : editingResourceId === null ? "创建" : "保存" }}
            </button>
            <button type="button" class="btn" @click="closeDialog">取消</button>
          </div>
        </div>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, reactive, ref } from "vue";
import { useRoute } from "vue-router";

import {
  createCourseResource,
  deleteCourseResource,
  fetchCourseResources,
  updateCourseResource,
} from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const submitting = ref(false);
const resources = ref([]);
const keyword = ref("");
const filterCode = ref("");
const filterType = ref("");
const filterPhase = ref("");
const dialogOpen = ref(false);
const editingResourceId = ref(null);
const saveMessage = ref("");
const saveMessageClass = ref("");

const editForm = reactive({
  knowledgePointCode: "",
  title: "",
  resourceType: "article",
  source: "",
  url: "",
  importanceWeight: 1,
  estimatedMinutes: 20,
  minMastery: 0,
  maxMastery: 1,
  description: "",
  recommendedUsage: "",
  recommendedPhase: "learn",
});

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));

const knowledgePointOptions = computed(() =>
  [...new Set(resources.value.map((item) => item.knowledgePointCode).filter(Boolean))],
);

const filteredResources = computed(() =>
  resources.value.filter((item) => {
    if (filterCode.value && item.knowledgePointCode !== filterCode.value) return false;
    if (filterType.value && item.resourceType !== filterType.value) return false;
    if (filterPhase.value && item.recommendedPhase !== filterPhase.value) return false;
    if (!keyword.value) return true;
    const haystack = [item.title, item.source, item.description].join(" ").toLowerCase();
    return haystack.includes(keyword.value.toLowerCase());
  }),
);

function resetForm() {
  editForm.knowledgePointCode = "";
  editForm.title = "";
  editForm.resourceType = "article";
  editForm.source = "";
  editForm.url = "";
  editForm.importanceWeight = 1;
  editForm.estimatedMinutes = 20;
  editForm.minMastery = 0;
  editForm.maxMastery = 1;
  editForm.description = "";
  editForm.recommendedUsage = "";
  editForm.recommendedPhase = "learn";
}

function openCreateDialog() {
  editingResourceId.value = null;
  resetForm();
  dialogOpen.value = true;
}

function openEditDialog(resource) {
  editingResourceId.value = resource.id;
  editForm.knowledgePointCode = resource.knowledgePointCode || "";
  editForm.title = resource.title || "";
  editForm.resourceType = resource.resourceType || "article";
  editForm.source = resource.source || "";
  editForm.url = resource.url || "";
  editForm.importanceWeight = Number(resource.importanceWeight ?? 1);
  editForm.estimatedMinutes = Number(resource.estimatedMinutes ?? 20);
  editForm.minMastery = Number(resource.minMastery ?? 0);
  editForm.maxMastery = Number(resource.maxMastery ?? 1);
  editForm.description = resource.description || "";
  editForm.recommendedUsage = resource.recommendedUsage || "";
  editForm.recommendedPhase = resource.recommendedPhase || "learn";
  dialogOpen.value = true;
}

function closeDialog() {
  dialogOpen.value = false;
  editingResourceId.value = null;
}

function buildPayload() {
  return {
    knowledgePointCode: editForm.knowledgePointCode,
    title: editForm.title,
    resourceType: editForm.resourceType,
    source: editForm.source,
    url: editForm.url,
    importanceWeight: Number(editForm.importanceWeight ?? 1),
    estimatedMinutes: Number(editForm.estimatedMinutes ?? 20),
    minMastery: Number(editForm.minMastery ?? 0),
    maxMastery: Number(editForm.maxMastery ?? 1),
    description: editForm.description,
    recommendedUsage: editForm.recommendedUsage,
    recommendedPhase: editForm.recommendedPhase,
    focusTags: [],
  };
}

function formatWeight(value) {
  return Number(value ?? 1).toFixed(1);
}

function formatMasteryRange(minValue, maxValue) {
  return `${Math.round(Number(minValue ?? 0) * 100)}% - ${Math.round(Number(maxValue ?? 1) * 100)}%`;
}

async function loadResources() {
  loading.value = true;
  loadError.value = "";
  try {
    const payload = await fetchCourseResources(courseCode.value, {
      username: teacherUsername.value,
    });
    resources.value = payload?.resources || [];
    filterCode.value = typeof route.query.focusPoint === "string" ? route.query.focusPoint : "";
  } catch (error) {
    loadError.value = "未能读取学习资源，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

async function submitResource() {
  if (!editForm.title.trim()) {
    saveMessage.value = "资源标题不能为空";
    saveMessageClass.value = "edit-message--error";
    return;
  }

  submitting.value = true;
  try {
    if (editingResourceId.value === null) {
      await createCourseResource(courseCode.value, buildPayload(), {
        username: teacherUsername.value,
      });
      saveMessage.value = "学习资源已创建";
    } else {
      await updateCourseResource(courseCode.value, editingResourceId.value, buildPayload(), {
        username: teacherUsername.value,
      });
      saveMessage.value = "学习资源已更新";
    }
    saveMessageClass.value = "edit-message--ok";
    closeDialog();
    await loadResources();
  } catch (error) {
    saveMessage.value = "保存失败: " + (error?.response?.data?.detail || error.message);
    saveMessageClass.value = "edit-message--error";
  } finally {
    submitting.value = false;
  }
}

async function removeResource(resourceId) {
  if (!confirm("确定要删除此资源吗？")) return;
  try {
    await deleteCourseResource(courseCode.value, resourceId, {
      username: teacherUsername.value,
    });
    saveMessage.value = "学习资源已删除";
    saveMessageClass.value = "edit-message--ok";
    await loadResources();
  } catch (error) {
    saveMessage.value = "删除失败: " + (error?.response?.data?.detail || error.message);
    saveMessageClass.value = "edit-message--error";
  }
}

onMounted(loadResources);
</script>

<style scoped>
.resources-layout { display: grid; gap: 18px; }
.toolbar { display: flex; gap: 12px; flex-wrap: wrap; align-items: center; }
.filter-input, .filter-select, .edit-input {
  padding: 8px 10px;
  border: 1px solid #c7d5df;
  border-radius: 8px;
  background: #fff;
}
.table-shell {
  overflow-x: auto;
  border: 1px solid #d8e0e6;
  border-radius: 14px;
  background: #fff;
}
.resource-table {
  width: 100%;
  border-collapse: collapse;
}
.resource-table th,
.resource-table td {
  padding: 14px 12px;
  border-bottom: 1px solid #eef2f5;
  text-align: left;
  vertical-align: top;
}
.resource-table th {
  background: #f7fbfd;
  color: #4f6270;
  font-size: 0.9rem;
}
.meta-line {
  margin: 6px 0 0;
  color: #607380;
  font-size: 0.85rem;
}
.resource-link {
  display: inline-block;
  margin-left: 8px;
  color: #0c6a71;
}
.empty-cell { text-align: center; color: #607380; }
.actions-cell { white-space: nowrap; }
.dialog-backdrop {
  position: fixed;
  inset: 0;
  background: rgba(14, 28, 36, 0.42);
  display: grid;
  place-items: center;
  padding: 24px;
}
.dialog-panel {
  width: min(860px, 100%);
  max-height: 90vh;
  overflow: auto;
  background: #fff;
  border-radius: 18px;
  padding: 20px;
  box-shadow: 0 20px 60px rgba(17, 35, 48, 0.18);
}
.dialog-head, .dialog-actions {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}
.form-grid {
  display: grid;
  gap: 12px;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  margin: 18px 0;
}
.field { display: grid; gap: 6px; }
.field--full { grid-column: 1 / -1; }
.btn {
  padding: 8px 16px;
  border: 1px solid #d8e0e6;
  border-radius: 8px;
  background: #ffffff;
  color: #15364a;
  font-weight: 600;
  cursor: pointer;
}
.btn--primary { background: #0c6a71; color: #fff; border-color: #0c6a71; }
.btn--danger { color: #9b3333; border-color: #e0cccc; }
.btn--small { padding: 4px 10px; font-size: 0.85rem; }
.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }
</style>
