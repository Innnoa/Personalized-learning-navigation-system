<template>
  <section class="resources-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载学习资源...</p>

    <template v-else>
      <div class="resources-actions">
        <button type="button" class="btn btn--primary" @click="startNewResource">
          + 新增资源
        </button>
        <label class="filter-label">
          知识点
          <select v-model="filterCode" class="filter-select">
            <option value="">全部</option>
            <option v-for="kpr in resources" :key="kpr.knowledgePointCode" :value="kpr.knowledgePointCode">
              {{ kpr.knowledgePointCode }}
            </option>
          </select>
        </label>
        <button type="button" class="btn" @click="saveAll" :disabled="saving">
          {{ saving ? "保存中..." : "保存全部" }}
        </button>
        <span v-if="saveMessage" class="edit-message" :class="saveMessageClass">{{ saveMessage }}</span>
      </div>

      <div v-if="editingNew" class="edit-panel">
        <h4>新增学习资源</h4>
        <div class="edit-grid">
          <input v-model="editForm.knowledgePointCode" placeholder="知识点代码" class="edit-input" />
          <input v-model="editForm.title" placeholder="资源标题" class="edit-input" />
          <input v-model="editForm.type" placeholder="类型 (video/article)" class="edit-input" />
          <input v-model="editForm.source" placeholder="来源" class="edit-input" />
          <input v-model="editForm.url" placeholder="URL" class="edit-input" />
          <input v-model="editForm.recommendedPhase" placeholder="推荐阶段 (预习/巩固/延伸学习)" class="edit-input" />
          <textarea v-model="editForm.description" placeholder="描述" class="edit-textarea" rows="2"></textarea>
          <textarea v-model="editForm.recommendedUsage" placeholder="推荐用法" class="edit-textarea" rows="2"></textarea>
        </div>
        <div class="edit-actions">
          <button type="button" class="btn btn--primary" @click="addResource">添加</button>
          <button type="button" class="btn" @click="editingNew = false">取消</button>
        </div>
      </div>

      <div v-if="filteredResources.length === 0" class="state-message">暂无匹配的学习资源。</div>

      <div v-else class="resource-grid">
        <article
          v-for="(res, idx) in filteredResources"
          :key="res.knowledgePointCode + '-' + idx"
          class="resource-card"
          data-testid="resource-card"
        >
          <div class="resource-header">
            <strong>{{ res.title }}</strong>
            <span class="resource-badge">{{ res.type }}</span>
          </div>
          <div class="resource-meta">
            <span class="resource-kp">知识点: {{ res.knowledgePointCode }}</span>
            <span>{{ res.source }}</span>
            <span>阶段: {{ res.recommendedPhase }}</span>
          </div>
          <p class="resource-url">
            <a :href="res.url" target="_blank">{{ res.url }}</a>
          </p>
          <p class="resource-desc" v-if="res.description">{{ res.description }}</p>
          <div class="resource-footer">
            <button type="button" class="btn btn--small btn--danger" @click="removeResource(res.knowledgePointCode, idx)">删除</button>
          </div>
        </article>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, reactive, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchCourseResources, saveCourseResources } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const resources = ref([]);
const filterCode = ref("");
const editingNew = ref(false);
const saving = ref(false);
const saveMessage = ref("");
const saveMessageClass = ref("");

const editForm = reactive({
  knowledgePointCode: "",
  title: "",
  type: "video",
  source: "",
  url: "",
  description: "",
  recommendedUsage: "",
  recommendedPhase: "预习",
});

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));

// Flatten resources for display
const flatResources = computed(() => {
  const result = [];
  for (const kpr of resources.value) {
    if (kpr.knowledgePointCode && kpr.resources) {
      for (const r of kpr.resources) {
        result.push({ ...r, knowledgePointCode: kpr.knowledgePointCode });
      }
    }
  }
  return result;
});

const filteredResources = computed(() => {
  if (!filterCode.value) return flatResources.value;
  return flatResources.value.filter((r) => r.knowledgePointCode === filterCode.value);
});

async function loadResources() {
  loading.value = true;
  loadError.value = "";
  try {
    const payload = await fetchCourseResources(courseCode.value, {
      username: teacherUsername.value,
    });
    resources.value = payload?.knowledgePointResources || [];
  } catch (error) {
    loadError.value = "未能读取学习资源，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

function startNewResource() {
  editingNew.value = true;
  editForm.knowledgePointCode = "";
  editForm.title = "";
  editForm.type = "video";
  editForm.source = "";
  editForm.url = "";
  editForm.description = "";
  editForm.recommendedUsage = "";
  editForm.recommendedPhase = "预习";
}

function addResource() {
  if (!editForm.knowledgePointCode || !editForm.title) {
    saveMessage.value = "知识点代码和标题不能为空";
    saveMessageClass.value = "edit-message--error";
    return;
  }
  const newRes = {
    title: editForm.title,
    type: editForm.type,
    source: editForm.source,
    url: editForm.url,
    description: editForm.description,
    recommendedUsage: editForm.recommendedUsage,
    recommendedPhase: editForm.recommendedPhase,
    focusTags: [],
  };
  const existing = resources.value.find(
    (r) => r.knowledgePointCode === editForm.knowledgePointCode,
  );
  if (existing) {
    existing.resources.push(newRes);
  } else {
    resources.value.push({
      knowledgePointCode: editForm.knowledgePointCode,
      resources: [newRes],
    });
  }
  editingNew.value = false;
  saveMessage.value = "资源已添加（局部），点击保存全部持久化。";
  saveMessageClass.value = "edit-message--ok";
}

function removeResource(kpCode, idx) {
  if (!confirm("确定要删除此资源吗？")) return;
  const kpr = resources.value.find((r) => r.knowledgePointCode === kpCode);
  if (kpr) {
    kpr.resources.splice(idx, 1);
    saveMessage.value = "资源已移除（局部），点击保存全部持久化。";
    saveMessageClass.value = "edit-message--ok";
  }
}

async function saveAll() {
  saving.value = true;
  saveMessage.value = "";
  try {
    await saveCourseResources(
      courseCode.value,
      { knowledgePointResources: resources.value },
      { username: teacherUsername.value },
    );
    saveMessage.value = "学习资源已保存";
    saveMessageClass.value = "edit-message--ok";
  } catch (error) {
    saveMessage.value = "保存失败: " + (error?.response?.data?.detail || error.message);
    saveMessageClass.value = "edit-message--error";
  } finally {
    saving.value = false;
  }
}

onMounted(loadResources);
</script>

<style scoped>
.resources-layout { display: grid; gap: 18px; }
.resources-actions { display: flex; align-items: center; gap: 12px; flex-wrap: wrap; }

.btn {
  padding: 8px 16px;
  border: 1px solid #d8e0e6;
  border-radius: 8px;
  background: #ffffff;
  color: #15364a;
  font-weight: 600;
  cursor: pointer;
  transition: background 0.15s;
}
.btn:hover { background: #eef7f8; }
.btn--primary { background: #0c6a71; color: #fff; border-color: #0c6a71; }
.btn--primary:hover { background: #0b5a60; }
.btn--danger { color: #9b3333; border-color: #e0cccc; }
.btn--danger:hover { background: #fce4e4; }
.btn--small { padding: 4px 10px; font-size: 0.85rem; }

.filter-label { display: flex; align-items: center; gap: 8px; color: #51606d; font-size: 0.9rem; }
.filter-select { padding: 6px 10px; border: 1px solid #d8e0e6; border-radius: 8px; background: #fff; }
.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }

.edit-panel {
  padding: 16px;
  border: 1px solid #d8e0e6;
  border-radius: 12px;
  background: #f9fdfd;
}
.edit-panel h4 { margin: 0 0 12px; color: #15364a; }
.edit-grid { display: grid; gap: 8px; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); }
.edit-input, .edit-textarea {
  padding: 6px 8px;
  border: 1px solid #a0b8c4;
  border-radius: 4px;
  font-size: 0.85rem;
  width: 100%;
  box-sizing: border-box;
}
.edit-actions { margin-top: 10px; display: flex; gap: 8px; }

.resource-grid { display: grid; gap: 16px; }
.resource-card {
  padding: 16px;
  border: 1px solid #d8e0e6;
  border-radius: 12px;
  background: #ffffff;
}
.resource-header { display: flex; justify-content: space-between; align-items: flex-start; }
.resource-badge {
  padding: 2px 8px;
  border-radius: 6px;
  background: #eef7f8;
  color: #0c6a71;
  font-size: 0.8rem;
  font-weight: 600;
}
.resource-meta { display: flex; gap: 12px; flex-wrap: wrap; margin-top: 6px; color: #51606d; font-size: 0.85rem; }
.resource-kp { font-family: monospace; color: #0c6a71; }
.resource-url { margin: 8px 0 0; font-size: 0.85rem; word-break: break-all; }
.resource-url a { color: #0c6a71; }
.resource-desc { margin: 6px 0 0; color: #51606d; font-size: 0.85rem; }
.resource-footer { margin-top: 10px; }

.state-message { color: #51606d; margin: 0; }
.state-message--error { color: #9b3333; }
</style>
