<template>
  <section class="questions-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载题库数据...</p>

    <template v-else>
      <div class="questions-actions">
        <button type="button" class="btn btn--primary" @click="startNewQuestion">
          + 新增题目
        </button>
        <label class="filter-label">
          知识点
          <select v-model="filterCode" class="filter-select">
            <option value="">全部</option>
            <option v-for="bank in banks" :key="bank.knowledgePointCode" :value="bank.knowledgePointCode">
              {{ bank.knowledgePointCode }}
            </option>
          </select>
        </label>
        <button type="button" class="btn" @click="saveAll" :disabled="saving">
          {{ saving ? "保存中..." : "保存全部" }}
        </button>
        <span v-if="saveMessage" class="edit-message" :class="saveMessageClass">{{ saveMessage }}</span>
      </div>

      <!-- New/Edit Form -->
      <div v-if="editingQuestion !== null" class="edit-panel">
        <h4>{{ editingQuestion.id === "__new__" ? "新增题目" : "编辑题目" }}</h4>
        <div class="edit-grid">
          <div class="edit-field">
            <label>知识点代码</label>
            <input v-model="editForm.knowledgePointCode" class="edit-input" placeholder="如: ds-intro" />
          </div>
          <div class="edit-field">
            <label>题目ID</label>
            <input v-model="editForm.id" class="edit-input" placeholder="如: ds-intro-1" :disabled="editingQuestion.id !== '__new__'" />
          </div>
          <div class="edit-field">
            <label>难度</label>
            <select v-model="editForm.difficulty" class="edit-select">
              <option value="easy">easy</option>
              <option value="medium">medium</option>
              <option value="hard">hard</option>
            </select>
          </div>
          <div class="edit-field edit-field--full">
            <label>题面</label>
            <textarea v-model="editForm.prompt" class="edit-textarea" rows="3" placeholder="题面内容"></textarea>
          </div>
          <div v-for="(opt, oi) in editForm.options" :key="oi" class="edit-field">
            <label>选项 {{ ['A','B','C','D'][oi] }}
              <span v-if="editForm.correctIndex === oi" class="correct-badge">✓ 正确答案</span>
            </label>
            <input v-model="editForm.options[oi]" class="edit-input" :placeholder="'选项 ' + ['A','B','C','D'][oi]" />
          </div>
          <div class="edit-field">
            <label>正确答案</label>
            <select v-model.number="editForm.correctIndex" class="edit-select">
              <option :value="0">A</option>
              <option :value="1">B</option>
              <option :value="2">C</option>
              <option :value="3">D</option>
            </select>
          </div>
          <div class="edit-field edit-field--full">
            <label>解析</label>
            <textarea v-model="editForm.explanation" class="edit-textarea" rows="2" placeholder="答案解析"></textarea>
          </div>
        </div>
        <div class="edit-actions">
          <button type="button" class="btn btn--primary" @click="saveQuestion">{{ editingQuestion.id === "__new__" ? "添加" : "保存" }}</button>
          <button type="button" class="btn" @click="editingQuestion = null">取消</button>
        </div>
      </div>

      <div v-if="filteredQuestions.length === 0" class="state-message">暂无匹配的题目。</div>

      <div v-else class="question-grid">
        <article
          v-for="(q, idx) in filteredQuestions"
          :key="q.id"
          class="question-card"
          data-testid="question-card"
        >
          <div class="question-header">
            <strong>{{ q.id }}</strong>
            <div class="question-badges">
              <span class="badge badge--kp">{{ q.knowledgePointCode }}</span>
              <span class="badge" :class="'badge--' + q.difficulty">{{ q.difficulty }}</span>
            </div>
          </div>
          <p class="question-prompt">{{ q.prompt }}</p>
          <ul class="question-options">
            <li v-for="(opt, oi) in q.options" :key="oi" :class="{ 'option--correct': oi === q.correctIndex }">
              {{ ['A','B','C','D'][oi] }}. {{ opt }}
            </li>
          </ul>
          <p v-if="q.explanation" class="question-explanation">解析: {{ q.explanation }}</p>
          <div class="question-footer">
            <button type="button" class="btn btn--small" @click="startEdit(q.knowledgePointCode, idx)">编辑</button>
            <button type="button" class="btn btn--small btn--danger" @click="deleteQuestion(q.knowledgePointCode, idx)">删除</button>
          </div>
        </article>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, reactive, ref } from "vue";
import { useRoute } from "vue-router";

import { fetchCourseQuestions, saveCourseQuestions } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const banks = ref([]);
const filterCode = ref("");
const editingQuestion = ref(null);
const saving = ref(false);
const saveMessage = ref("");
const saveMessageClass = ref("");

const editForm = reactive({
  knowledgePointCode: "",
  id: "",
  difficulty: "easy",
  prompt: "",
  options: ["", "", "", ""],
  correctIndex: 0,
  explanation: "",
});

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));

const flatQuestions = computed(() => {
  const result = [];
  for (const bank of banks.value) {
    if (bank.knowledgePointCode && bank.questions) {
      for (const q of bank.questions) {
        result.push({ ...q, knowledgePointCode: bank.knowledgePointCode });
      }
    }
  }
  return result;
});

const filteredQuestions = computed(() => {
  if (!filterCode.value) return flatQuestions.value;
  return flatQuestions.value.filter((q) => q.knowledgePointCode === filterCode.value);
});

async function loadQuestions() {
  loading.value = true;
  loadError.value = "";
  try {
    const payload = await fetchCourseQuestions(courseCode.value, {
      username: teacherUsername.value,
    });
    banks.value = payload?.banks || [];
  } catch (error) {
    loadError.value = "未能读取题库数据，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

function startNewQuestion() {
  editingQuestion.value = { id: "__new__" };
  editForm.knowledgePointCode = "";
  editForm.id = "";
  editForm.difficulty = "easy";
  editForm.prompt = "";
  editForm.options = ["", "", "", ""];
  editForm.correctIndex = 0;
  editForm.explanation = "";
}

function startEdit(kpCode, idx) {
  const bank = banks.value.find((b) => b.knowledgePointCode === kpCode);
  if (!bank) return;
  const q = bank.questions[idx];
  editingQuestion.value = q;
  editForm.knowledgePointCode = kpCode;
  editForm.id = q.id || "";
  editForm.difficulty = q.difficulty || "easy";
  editForm.prompt = q.prompt || "";
  editForm.options = [...(q.options || ["", "", "", ""])];
  editForm.correctIndex = q.correctIndex ?? 0;
  editForm.explanation = q.explanation || "";
}

function saveQuestion() {
  if (!editForm.knowledgePointCode || !editForm.prompt) {
    saveMessage.value = "知识点代码和题面不能为空";
    saveMessageClass.value = "edit-message--error";
    return;
  }
  const qData = {
    id: editForm.id || editForm.knowledgePointCode + "-" + Date.now(),
    type: "single-choice",
    difficulty: editForm.difficulty,
    prompt: editForm.prompt,
    options: [...editForm.options],
    correctIndex: editForm.correctIndex,
    explanation: editForm.explanation,
  };

  const bank = banks.value.find((b) => b.knowledgePointCode === editForm.knowledgePointCode);
  if (editingQuestion.value?.id === "__new__") {
    if (bank) {
      bank.questions.push(qData);
    } else {
      banks.value.push({
        knowledgePointCode: editForm.knowledgePointCode,
        questions: [qData],
      });
    }
  } else {
    // Update existing
    if (bank) {
      const idx = bank.questions.findIndex((q) => q.id === editingQuestion.value?.id);
      if (idx >= 0) {
        bank.questions[idx] = qData;
      }
    }
  }
  editingQuestion.value = null;
  saveMessage.value = "修改已暂存（局部），点击保存全部持久化。";
  saveMessageClass.value = "edit-message--ok";
}

function deleteQuestion(kpCode, idx) {
  if (!confirm("确定要删除此题目吗？")) return;
  const bank = banks.value.find((b) => b.knowledgePointCode === kpCode);
  if (bank) {
    bank.questions.splice(idx, 1);
    saveMessage.value = "题目已移除（局部），点击保存全部持久化。";
    saveMessageClass.value = "edit-message--ok";
  }
}

async function saveAll() {
  saving.value = true;
  saveMessage.value = "";
  try {
    await saveCourseQuestions(
      courseCode.value,
      { banks: banks.value },
      { username: teacherUsername.value },
    );
    saveMessage.value = "题库已保存";
    saveMessageClass.value = "edit-message--ok";
  } catch (error) {
    saveMessage.value = "保存失败: " + (error?.response?.data?.detail || error.message);
    saveMessageClass.value = "edit-message--error";
  } finally {
    saving.value = false;
  }
}

onMounted(loadQuestions);
</script>

<style scoped>
.questions-layout { display: grid; gap: 18px; }
.questions-actions { display: flex; align-items: center; gap: 12px; flex-wrap: wrap; }

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
.edit-grid { display: grid; gap: 10px; grid-template-columns: 1fr 1fr; }
.edit-field { display: grid; gap: 4px; }
.edit-field--full { grid-column: 1 / -1; }
.edit-field label { font-size: 0.85rem; color: #51606d; }
.edit-input, .edit-select, .edit-textarea {
  padding: 6px 8px;
  border: 1px solid #a0b8c4;
  border-radius: 4px;
  font-size: 0.85rem;
  width: 100%;
  box-sizing: border-box;
}
.edit-actions { margin-top: 10px; display: flex; gap: 8px; }
.correct-badge { color: #1a7a3a; font-weight: 700; margin-left: 8px; }

.question-grid { display: grid; gap: 16px; }
.question-card {
  padding: 16px;
  border: 1px solid #d8e0e6;
  border-radius: 12px;
  background: #ffffff;
}
.question-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px; }
.question-badges { display: flex; gap: 6px; }

.badge {
  padding: 2px 8px;
  border-radius: 6px;
  font-size: 0.8rem;
  font-weight: 600;
  background: #eef7f8;
  color: #0c6a71;
}
.badge--kp { background: #e0f2e9; color: #1a7a3a; }
.badge--easy { background: #e0f2e9; color: #1a7a3a; }
.badge--medium { background: #fef3d0; color: #8a6d1b; }
.badge--hard { background: #fce4e4; color: #9b3333; }

.question-prompt { margin: 0 0 10px; color: #15364a; line-height: 1.5; }
.question-options { list-style: none; padding: 0; margin: 0 0 8px; display: grid; gap: 4px; }
.question-options li { padding: 4px 8px; border-radius: 4px; font-size: 0.9rem; color: #51606d; }
.option--correct { background: #e0f2e9; color: #1a7a3a; font-weight: 600; }
.question-explanation { margin: 0 0 10px; font-size: 0.85rem; color: #51606d; }
.question-footer { display: flex; gap: 8px; }

.state-message { color: #51606d; margin: 0; }
.state-message--error { color: #9b3333; }
</style>
