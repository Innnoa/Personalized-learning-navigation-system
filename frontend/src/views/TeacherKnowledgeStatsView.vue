<template>
  <section class="points-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载知识点数据...</p>

    <template v-else>
      <div class="points-actions">
        <button type="button" class="btn btn--primary" @click="startCreate">
          + 新建知识点
        </button>
        <span v-if="editMessage" class="edit-message" :class="editMessageClass">{{ editMessage }}</span>
      </div>

      <div v-if="points.length === 0 && !editingCode" class="state-message">
        暂无知识点数据。
        <p class="state-hint">点击"+ 新建知识点"开始添加课程知识点。</p>
      </div>

      <div v-else class="points-table-wrapper">
        <table class="points-table">
          <thead>
            <tr>
              <th>章节</th>
              <th>知识点</th>
              <th>难度</th>
              <th>平均掌握度</th>
              <th>学生数</th>
              <th>操作</th>
            </tr>
          </thead>
          <tbody>
            <tr
              v-for="point in points"
              :key="point.code"
              data-testid="knowledge-row"
              :class="{ 'row--editing': editingCode === point.code }"
            >
              <template v-if="editingCode === point.code">
                <td>
                  <input v-model.number="editForm.chapterNo" type="number" min="1" class="edit-input" size="4" />
                  <input v-model="editForm.chapterName" type="text" class="edit-input" placeholder="章节名" />
                </td>
                <td>
                  <input v-model="editForm.name" type="text" class="edit-input" placeholder="名称" style="width:140px" />
                  <div class="edit-code">{{ point.code }}</div>
                </td>
                <td>
                  <select v-model.number="editForm.difficultyLevel" class="edit-select">
                    <option v-for="d in 5" :key="d" :value="d">★{{ d }}</option>
                  </select>
                </td>
                <td class="cell-mastery">
                  <div class="mini-bar">
                    <div class="mini-bar__track"><div class="mini-bar__fill" :style="{ width: masteryPercent(point.avgMastery) }"></div></div>
                    <span class="mini-bar__value">{{ masteryPercent(point.avgMastery) }}</span>
                  </div>
                </td>
                <td class="cell-count">{{ point.studentCount }}</td>
                <td>
                  <button type="button" class="btn btn--small btn--primary" @click="saveEdit(point.code)">保存</button>
                  <button type="button" class="btn btn--small" @click="cancelEdit">取消</button>
                </td>
              </template>
              <template v-else>
                <td class="cell-chapter">{{ point.chapterNo }}. {{ point.chapterName }}</td>
                <td class="cell-name">{{ point.name }}</td>
                <td class="cell-difficulty">{{ difficultyStars(point.difficultyLevel) }}</td>
                <td class="cell-mastery">
                  <div class="mini-bar">
                    <div class="mini-bar__track"><div class="mini-bar__fill" :style="{ width: masteryPercent(point.avgMastery) }"></div></div>
                    <span class="mini-bar__value">{{ masteryPercent(point.avgMastery) }}</span>
                  </div>
                </td>
                <td class="cell-count">{{ point.studentCount }}</td>
                <td>
                  <button type="button" class="btn btn--small" @click="startEdit(point)">编辑</button>
                  <button type="button" class="btn btn--small btn--danger" @click="confirmDelete(point)">删除</button>
                </td>
              </template>
            </tr>
            <tr v-if="editingCode === '__new__'">
              <td>
                <input v-model="editForm.chapterNo" type="number" min="1" class="edit-input" size="4" />
                <input v-model="editForm.chapterName" type="text" class="edit-input" placeholder="章节名" />
              </td>
              <td>
                <input v-model="editForm.code" type="text" class="edit-input" placeholder="code" style="width:110px" />
                <input v-model="editForm.name" type="text" class="edit-input" placeholder="名称" style="width:140px" />
              </td>
              <td>
                <select v-model.number="editForm.difficultyLevel" class="edit-select">
                  <option v-for="d in 5" :key="d" :value="d">★{{ d }}</option>
                </select>
              </td>
              <td>-</td>
              <td>-</td>
              <td>
                <button type="button" class="btn btn--small btn--primary" @click="saveNew">创建</button>
                <button type="button" class="btn btn--small" @click="cancelEdit">取消</button>
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <!-- Dependencies editor -->
      <div class="deps-section">
        <button type="button" class="btn" @click="toggleDepsEditor">
          {{ showDepsEditor ? "收起依赖编辑" : "编辑依赖关系" }}
        </button>
        <span v-if="depMessage" class="edit-message" :class="depMessageClass">{{ depMessage }}</span>

        <div v-if="showDepsEditor" class="deps-panel">
          <p class="deps-hint">为每个知识点勾选它的前置知识点。保存时将全量替换该课程的所有依赖关系。</p>
          <table class="deps-table">
            <thead>
              <tr>
                <th>知识点</th>
                <th>前置知识点（勾选即添加依赖）</th>
              </tr>
            </thead>
            <tbody>
              <tr v-for="kp in points" :key="'dep-' + kp.code">
                <td class="deps-target">{{ kp.code }}<br/><small>{{ kp.name }}</small></td>
                <td class="deps-prereqs">
                  <div class="deps-prereq-list">
                    <label
                      v-for="pre in points.filter(p => p.code !== kp.code)"
                      :key="'pre-' + kp.code + '-' + pre.code"
                      class="dep-checkbox"
                    >
                      <input
                        type="checkbox"
                        :checked="isDependency(kp.code, pre.code)"
                        @change="toggleDependency(kp.code, pre.code)"
                      />
                      {{ pre.code }}
                    </label>
                  </div>
                </td>
              </tr>
            </tbody>
          </table>
          <button type="button" class="btn btn--primary" @click="saveDependencies">
            保存依赖关系
          </button>
        </div>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, reactive, ref } from "vue";
import { useRoute } from "vue-router";

import {
  fetchTeacherKnowledgeStats,
  createKnowledgePoint,
  updateKnowledgePoint,
  deleteKnowledgePoint,
  fetchDependencies,
  updateDependencies,
} from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const knowledgeData = ref(null);
const editingCode = ref(null);
const editMessage = ref("");
const editMessageClass = ref("");

// Dependency editor state
const showDepsEditor = ref(false);
const depSelections = ref([]); // [{knowledgePointCode, prerequisiteCode}]
const depMessage = ref("");
const depMessageClass = ref("");

const editForm = reactive({
  code: "",
  name: "",
  chapterNo: 1,
  chapterName: "",
  difficultyLevel: 1,
  nodeType: "concept",
  estimatedMinutes: 20,
  displayOrder: 0,
});

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const points = computed(() => knowledgeData.value?.knowledgePoints || []);

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

function difficultyStars(level) {
  const stars = ["", "★", "★★", "★★★", "★★★★", "★★★★★"];
  return stars[level] || "";
}

async function loadPoints() {
  loading.value = true;
  loadError.value = "";
  try {
    knowledgeData.value = await fetchTeacherKnowledgeStats(courseCode.value, {
      username: teacherUsername.value,
    });
  } catch (error) {
    loadError.value = "未能读取知识点统计数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

function startEdit(point) {
  editingCode.value = point.code;
  editForm.code = point.code;
  editForm.name = point.name;
  editForm.chapterNo = point.chapterNo;
  editForm.chapterName = point.chapterName;
  editForm.difficultyLevel = point.difficultyLevel;
  editForm.nodeType = "concept";
  editForm.estimatedMinutes = 20;
  editForm.displayOrder = 0;
  editMessage.value = "";
}

function startCreate() {
  editingCode.value = "__new__";
  editForm.code = "";
  editForm.name = "";
  editForm.chapterNo = 1;
  editForm.chapterName = "";
  editForm.difficultyLevel = 1;
  editMessage.value = "";
}

function cancelEdit() {
  editingCode.value = null;
  editMessage.value = "";
}

async function saveEdit(code) {
  editMessage.value = "";
  editMessageClass.value = "";
  try {
    await updateKnowledgePoint(courseCode.value, code, {
      name: editForm.name,
      chapterNo: editForm.chapterNo,
      chapterName: editForm.chapterName,
      difficultyLevel: editForm.difficultyLevel,
      nodeType: editForm.nodeType,
      estimatedMinutes: editForm.estimatedMinutes,
      displayOrder: editForm.displayOrder,
    }, { username: teacherUsername.value });
    editingCode.value = null;
    editMessage.value = "知识点已更新";
    editMessageClass.value = "edit-message--ok";
    await loadPoints();
  } catch (error) {
    editMessage.value = "更新失败: " + (error?.response?.data?.detail || error.message);
    editMessageClass.value = "edit-message--error";
  }
}

async function saveNew() {
  if (!editForm.code || !editForm.name) {
    editMessage.value = "code 和 name 不能为空";
    editMessageClass.value = "edit-message--error";
    return;
  }
  try {
    await createKnowledgePoint(courseCode.value, {
      code: editForm.code,
      name: editForm.name,
      chapterNo: editForm.chapterNo,
      chapterName: editForm.chapterName,
      difficultyLevel: editForm.difficultyLevel,
      nodeType: editForm.nodeType,
      estimatedMinutes: editForm.estimatedMinutes,
    }, { username: teacherUsername.value });
    editingCode.value = null;
    editMessage.value = "知识点已创建";
    editMessageClass.value = "edit-message--ok";
    await loadPoints();
  } catch (error) {
    editMessage.value = "创建失败: " + (error?.response?.data?.detail || error.message);
    editMessageClass.value = "edit-message--error";
  }
}

async function confirmDelete(point) {
  if (!confirm(`确定要删除知识点 "${point.name}" 吗？此操作不可撤销。`)) return;
  try {
    await deleteKnowledgePoint(courseCode.value, point.code, {
      username: teacherUsername.value,
    });
    editMessage.value = "知识点已删除";
    editMessageClass.value = "edit-message--ok";
    await loadPoints();
  } catch (error) {
    editMessage.value = "删除失败: " + (error?.response?.data?.detail || error.message);
    editMessageClass.value = "edit-message--error";
  }
}

async function toggleDepsEditor() {
  showDepsEditor.value = !showDepsEditor.value;
  if (showDepsEditor.value) {
    depMessage.value = "正在加载现有依赖...";
    depMessageClass.value = "";
    try {
      const payload = await fetchDependencies(courseCode.value, {
        username: teacherUsername.value,
      });
      depSelections.value = payload?.dependencies || [];
      depMessage.value = "";
    } catch {
      depMessage.value = "加载依赖失败";
      depMessageClass.value = "edit-message--error";
    }
  }
}
function isDependency(targetCode, preCode) {
  return depSelections.value.some(
    (d) => d.knowledgePointCode === targetCode && d.prerequisiteCode === preCode,
  );
}

function toggleDependency(targetCode, preCode) {
  const idx = depSelections.value.findIndex(
    (d) => d.knowledgePointCode === targetCode && d.prerequisiteCode === preCode,
  );
  if (idx >= 0) {
    depSelections.value.splice(idx, 1);
  } else {
    depSelections.value.push({ knowledgePointCode: targetCode, prerequisiteCode: preCode });
  }
}

async function saveDependencies() {
  depMessage.value = "";
  depMessageClass.value = "";
  try {
    await updateDependencies(courseCode.value, {
      dependencies: depSelections.value,
    }, { username: teacherUsername.value });
    depMessage.value = "依赖关系已保存";
    depMessageClass.value = "edit-message--ok";
  } catch (error) {
    depMessage.value = "保存失败: " + (error?.response?.data?.detail || error.message);
    depMessageClass.value = "edit-message--error";
  }
}

onMounted(loadPoints);
</script>

<style scoped>
.points-layout { display: grid; gap: 18px; }
.points-actions { display: flex; align-items: center; gap: 12px; flex-wrap: wrap; }

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

.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }

.edit-input, .edit-select {
  padding: 4px 6px;
  border: 1px solid #a0b8c4;
  border-radius: 4px;
  font-size: 0.85rem;
  margin: 1px 2px;
}
.edit-code { font-size: 0.75rem; color: #8896a0; font-family: monospace; }

.row--editing { background: #f9fdfd; }

.points-table-wrapper { overflow-x: auto; }
.points-table { width: 100%; border-collapse: collapse; font-size: 0.9rem; }
.points-table th, .points-table td { padding: 8px 12px; text-align: left; border-bottom: 1px solid #d8e0e6; }
.points-table th { color: #51606d; font-weight: 600; white-space: nowrap; }
.points-table td { color: #15364a; }

.cell-chapter { color: #51606d; font-size: 0.85rem; }
.cell-name { font-weight: 600; }
.cell-difficulty { color: #b8960c; white-space: nowrap; }
.cell-count { text-align: center; }

.mini-bar { display: flex; align-items: center; gap: 8px; }
.mini-bar__track { width: 70px; height: 8px; background: #eef7f8; border-radius: 4px; overflow: hidden; }
.mini-bar__fill { height: 100%; background: #0c6a71; border-radius: 4px; }
.mini-bar__value { color: #0c6a71; font-weight: 700; font-size: 0.85rem; }

.state-message { color: #51606d; margin: 0; }
.state-message--error {
  color: #9b3333;
}

.state-hint {
  margin: 6px 0 0;
  font-size: 0.85rem;
  color: #8896a0;
}

@media (max-width: 720px) {
  .mini-bar__track { width: 50px; }
}

.deps-section { margin-top: 18px; }
.deps-panel { margin-top: 12px; padding: 16px; border: 1px solid #d8e0e6; border-radius: 12px; background: #f9fdfd; }
.deps-hint { margin: 0 0 12px; color: #51606d; font-size: 0.85rem; }
.deps-table { width: 100%; border-collapse: collapse; font-size: 0.85rem; }
.deps-table th, .deps-table td { padding: 8px 10px; border-bottom: 1px solid #d8e0e6; text-align: left; }
.deps-target { font-family: monospace; white-space: nowrap; }
.deps-prereqs { vertical-align: top; }
.deps-prereq-list { display: flex; flex-wrap: wrap; gap: 8px; }
.dep-checkbox { display: inline-flex; align-items: center; gap: 4px; color: #51606d; cursor: pointer; }
.dep-checkbox input { margin: 0; }
</style>
