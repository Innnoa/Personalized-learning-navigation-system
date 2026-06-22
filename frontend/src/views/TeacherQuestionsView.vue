<template>
  <section class="questions-layout">
    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loadingBanks" class="state-message">正在加载题库数据...</p>

    <template v-else>
      <div class="banks-panel">
        <div class="panel-head">
          <h3>题库列表</h3>
          <button type="button" class="btn btn--primary" @click="openBankDialog(null)">+ 新增题库</button>
        </div>
        <div v-if="banks.length === 0" class="empty-note">当前课程暂无题库。</div>
        <div v-else class="bank-list">
          <button
            v-for="bank in banks"
            :key="bank.id"
            type="button"
            class="bank-item"
            :class="{ 'bank-item--active': selectedBankId === bank.id }"
            @click="selectBank(bank)"
          >
            <strong>{{ bank.bankName }}</strong>
            <span>{{ bank.knowledgePointCode }}</span>
            <small>{{ bank.questionCount }} 题</small>
          </button>
        </div>
      </div>

      <div class="questions-panel">
        <div class="panel-head">
          <div>
            <h3>{{ selectedBank?.bankName || "请先选择题库" }}</h3>
            <p class="panel-subtitle">
              {{ selectedBank ? `${selectedBank.knowledgePointCode} · ${selectedBank.questionCount} 题` : "在左侧选择题库后查看与管理题目。" }}
            </p>
          </div>
          <div class="panel-actions">
            <button v-if="selectedBank" type="button" class="btn" @click="openBankDialog(selectedBank)">编辑题库</button>
            <button v-if="selectedBank" type="button" class="btn btn--danger" @click="removeBank(selectedBank.id)">删除题库</button>
            <button v-if="selectedBank" type="button" class="btn btn--primary" @click="openQuestionDialog(null)">+ 新增题目</button>
          </div>
        </div>

        <div v-if="selectedBank" class="toolbar">
          <select v-model="questionTypeFilter" class="filter-select">
            <option value="">全部题型</option>
            <option value="single_choice">单选题</option>
            <option value="multiple_choice">多选题</option>
            <option value="true_false">判断题</option>
            <option value="short_answer">简答题</option>
          </select>
          <input v-model.trim="keyword" class="filter-input" placeholder="搜索题干 / 题号" />
          <button type="button" class="btn" @click="reloadCurrentBank">搜索</button>
        </div>

        <p v-if="loadingQuestions" class="state-message">正在加载题目...</p>
        <div v-else class="table-shell">
          <table class="question-table">
            <thead>
              <tr>
                <th>题目编码</th>
                <th>题型</th>
                <th>难度</th>
                <th>权重 / 时长</th>
                <th>掌握度区间</th>
                <th>题干摘要</th>
                <th>状态</th>
                <th>操作</th>
              </tr>
            </thead>
            <tbody>
              <tr v-if="visibleQuestions.length === 0">
                <td colspan="8" class="empty-cell">暂无匹配的题目。</td>
              </tr>
              <tr v-for="question in visibleQuestions" :key="question.id">
                <td>{{ question.questionCode }}</td>
                <td>{{ questionTypeLabel(question.questionType) }}</td>
                <td>{{ question.difficultyLevel }}</td>
                <td>{{ formatWeight(question.importanceWeight) }} / {{ question.estimatedSeconds }} 秒</td>
                <td>{{ formatMasteryRange(question.minMastery, question.maxMastery) }}</td>
                <td>{{ question.prompt }}</td>
                <td>{{ question.status }}</td>
                <td class="actions-cell">
                  <button type="button" class="btn btn--small" @click="openQuestionDialog(question)">编辑</button>
                  <button type="button" class="btn btn--small btn--danger" @click="removeQuestion(question.id)">删除</button>
                </td>
              </tr>
            </tbody>
          </table>
        </div>
        <div v-if="selectedBank && totalCount > pageSize" class="pagination-bar">
          <button type="button" class="btn btn--small" :disabled="currentPage <= 1" @click="changePage(currentPage - 1)">上一页</button>
          <span>第 {{ currentPage }} / {{ totalPages }} 页 · 共 {{ totalCount }} 题</span>
          <button type="button" class="btn btn--small" :disabled="currentPage >= totalPages" @click="changePage(currentPage + 1)">下一页</button>
        </div>
      </div>

      <div v-if="bankDialogOpen" class="dialog-backdrop" @click.self="closeBankDialog">
        <div class="dialog-panel dialog-panel--small">
          <div class="dialog-head">
            <h3>{{ editingBank ? "编辑题库" : "新增题库" }}</h3>
            <button type="button" class="btn btn--small" @click="closeBankDialog">关闭</button>
          </div>
          <div class="form-grid">
            <label class="field">
              <span>知识点代码</span>
              <input v-model="bankForm.knowledgePointCode" class="edit-input" />
            </label>
            <label class="field">
              <span>题库编码</span>
              <input v-model="bankForm.bankCode" class="edit-input" />
            </label>
            <label class="field field--full">
              <span>题库名称</span>
              <input v-model="bankForm.bankName" class="edit-input" />
            </label>
            <label class="field field--full">
              <span>题库说明</span>
              <textarea v-model="bankForm.description" class="edit-input" rows="3" />
            </label>
          </div>
          <div class="dialog-actions">
            <button type="button" class="btn btn--primary" :disabled="submitting" @click="submitBank">
              {{ submitting ? "提交中..." : editingBank ? "保存" : "创建" }}
            </button>
            <button type="button" class="btn" @click="closeBankDialog">取消</button>
          </div>
        </div>
      </div>

      <div v-if="questionDialogOpen" class="dialog-backdrop" @click.self="closeQuestionDialog">
        <div class="dialog-panel">
          <div class="dialog-head">
            <h3>{{ editingQuestion ? "编辑题目" : "新增题目" }}</h3>
            <button type="button" class="btn btn--small" @click="closeQuestionDialog">关闭</button>
          </div>
          <div class="form-grid">
            <label class="field">
              <span>题目编码</span>
              <input v-model="questionForm.questionCode" class="edit-input" />
            </label>
            <label class="field">
              <span>题型</span>
              <select v-model="questionForm.questionType" class="edit-input">
                <option value="single_choice">单选题</option>
                <option value="multiple_choice">多选题</option>
                <option value="true_false">判断题</option>
                <option value="short_answer">简答题</option>
              </select>
            </label>
            <label class="field">
              <span>难度</span>
              <input v-model.number="questionForm.difficultyLevel" class="edit-input" type="number" min="1" max="5" />
            </label>
            <label class="field">
              <span>建议作答时长（秒）</span>
              <input v-model.number="questionForm.estimatedSeconds" class="edit-input" type="number" min="10" />
            </label>
            <label class="field">
              <span>推荐权重</span>
              <input v-model.number="questionForm.importanceWeight" class="edit-input" type="number" min="0" step="0.1" />
            </label>
            <label class="field">
              <span>最低掌握度</span>
              <input v-model.number="questionForm.minMastery" class="edit-input" type="number" min="0" max="1" step="0.05" />
            </label>
            <label class="field">
              <span>最高掌握度</span>
              <input v-model.number="questionForm.maxMastery" class="edit-input" type="number" min="0" max="1" step="0.05" />
            </label>
            <label class="field field--full">
              <span>题干</span>
              <textarea v-model="questionForm.prompt" class="edit-input" rows="3" />
            </label>

            <template v-if="questionForm.questionType === 'single_choice' || questionForm.questionType === 'multiple_choice'">
              <label v-for="(option, index) in questionForm.options" :key="index" class="field">
                <span>选项 {{ index + 1 }}</span>
                <input v-model="questionForm.options[index]" class="edit-input" />
              </label>
              <label class="field field--full">
                <span>正确答案索引（多选用英文逗号分隔）</span>
                <input v-model="questionForm.answerIndexesText" class="edit-input" placeholder="如 1 或 1,3" />
              </label>
            </template>

            <template v-else-if="questionForm.questionType === 'true_false'">
              <label class="field field--full">
                <span>正确答案</span>
                <select v-model="questionForm.booleanAnswer" class="edit-input">
                  <option value="true">正确</option>
                  <option value="false">错误</option>
                </select>
              </label>
            </template>

            <template v-else>
              <label class="field field--full">
                <span>参考答案</span>
                <textarea v-model="questionForm.referenceAnswer" class="edit-input" rows="3" />
              </label>
              <label class="field field--full">
                <span>关键词（英文逗号分隔）</span>
                <input v-model="questionForm.keywordText" class="edit-input" />
              </label>
            </template>

            <label class="field field--full">
              <span>解析</span>
              <textarea v-model="questionForm.explanation" class="edit-input" rows="3" />
            </label>
          </div>
          <p v-if="questionFormError" class="state-message state-message--error">{{ questionFormError }}</p>
          <div class="dialog-actions">
            <button type="button" class="btn btn--primary" :disabled="submitting" @click="submitQuestion">
              {{ submitting ? "提交中..." : editingQuestion ? "保存" : "创建" }}
            </button>
            <button type="button" class="btn" @click="closeQuestionDialog">取消</button>
          </div>
        </div>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, onMounted, reactive, ref, watch } from "vue";
import { useRoute } from "vue-router";

import {
  createBankQuestion,
  createQuestionBank,
  deleteBankQuestion,
  deleteQuestionBank,
  fetchBankQuestions,
  fetchQuestionBanks,
  updateBankQuestion,
  updateQuestionBank,
} from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loadingBanks = ref(true);
const loadingQuestions = ref(false);
const loadError = ref("");
const submitting = ref(false);
const banks = ref([]);
const questions = ref([]);
const selectedBankId = ref(null);
const keyword = ref("");
const questionTypeFilter = ref("");
const currentPage = ref(1);
const pageSize = ref(50);
const totalCount = ref(0);
const bankDialogOpen = ref(false);
const questionDialogOpen = ref(false);
const editingBank = ref(null);
const editingQuestion = ref(null);
const questionFormError = ref("");

const bankForm = reactive({
  knowledgePointCode: "",
  bankCode: "",
  bankName: "",
  description: "",
});

const questionForm = reactive({
  questionCode: "",
  questionType: "single_choice",
  difficultyLevel: 1,
  importanceWeight: 1,
  minMastery: 0,
  maxMastery: 1,
  prompt: "",
  options: ["", "", "", ""],
  answerIndexesText: "1",
  booleanAnswer: "true",
  referenceAnswer: "",
  keywordText: "",
  explanation: "",
  estimatedSeconds: 60,
});

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const selectedBank = computed(() => banks.value.find((item) => item.id === selectedBankId.value) || null);

const visibleQuestions = computed(() => questions.value);
const totalPages = computed(() => Math.max(1, Math.ceil(totalCount.value / pageSize.value)));

function questionTypeLabel(value) {
  return {
    single_choice: "单选题",
    multiple_choice: "多选题",
    true_false: "判断题",
    short_answer: "简答题",
  }[value] || value;
}

function resetBankForm() {
  bankForm.knowledgePointCode = "";
  bankForm.bankCode = "";
  bankForm.bankName = "";
  bankForm.description = "";
}

function resetQuestionForm() {
  questionFormError.value = "";
  questionForm.questionCode = "";
  questionForm.questionType = "single_choice";
  questionForm.difficultyLevel = 1;
  questionForm.importanceWeight = 1;
  questionForm.minMastery = 0;
  questionForm.maxMastery = 1;
  questionForm.prompt = "";
  questionForm.options = ["", "", "", ""];
  questionForm.answerIndexesText = "1";
  questionForm.booleanAnswer = "true";
  questionForm.referenceAnswer = "";
  questionForm.keywordText = "";
  questionForm.explanation = "";
  questionForm.estimatedSeconds = 60;
}

async function loadBanks() {
  loadingBanks.value = true;
  loadError.value = "";
  try {
    const payload = await fetchQuestionBanks(courseCode.value, {
      username: teacherUsername.value,
    });
    banks.value = payload?.questionBanks || [];
    const focusedPointCode =
      typeof route.query.focusPoint === "string" ? route.query.focusPoint : "";
    const focusedBank = focusedPointCode
      ? banks.value.find((item) => item.knowledgePointCode === focusedPointCode)
      : null;
    loadingBanks.value = false;
    if (focusedBank) {
      void selectBank(focusedBank);
    } else if (!selectedBankId.value && banks.value.length > 0) {
      void selectBank(banks.value[0]);
    }
  } catch (error) {
    loadError.value = "未能读取题库数据，请稍后重试。";
    console.error(error);
  } finally {
    if (banks.value.length === 0) {
      loadingBanks.value = false;
    }
  }
}

async function selectBank(bank) {
  selectedBankId.value = bank.id;
  loadingQuestions.value = true;
  try {
    const payload = await fetchBankQuestions(courseCode.value, bank.id, {
      username: teacherUsername.value,
      page: currentPage.value,
      pageSize: pageSize.value,
      keyword: keyword.value,
      questionType: questionTypeFilter.value,
    });
    questions.value = payload?.questions || [];
    totalCount.value = Number(payload?.totalCount || 0);
  } catch (error) {
    loadError.value = "未能读取题目数据，请稍后重试。";
    console.error(error);
  } finally {
    loadingQuestions.value = false;
  }
}

async function reloadCurrentBank() {
  currentPage.value = 1;
  if (selectedBank.value) {
    await selectBank(selectedBank.value);
  }
}

async function changePage(page) {
  currentPage.value = page;
  if (selectedBank.value) {
    await selectBank(selectedBank.value);
  }
}

function openBankDialog(bank) {
  editingBank.value = bank;
  if (bank) {
    bankForm.knowledgePointCode = bank.knowledgePointCode || "";
    bankForm.bankCode = bank.bankCode || "";
    bankForm.bankName = bank.bankName || "";
    bankForm.description = bank.description || "";
  } else {
    resetBankForm();
  }
  bankDialogOpen.value = true;
}

function closeBankDialog() {
  bankDialogOpen.value = false;
  editingBank.value = null;
}

async function submitBank() {
  submitting.value = true;
  try {
    const payload = {
      knowledgePointCode: bankForm.knowledgePointCode,
      bankCode: bankForm.bankCode,
      bankName: bankForm.bankName,
      description: bankForm.description,
    };
    if (editingBank.value) {
      await updateQuestionBank(courseCode.value, editingBank.value.id, payload, {
        username: teacherUsername.value,
      });
    } else {
      await createQuestionBank(courseCode.value, payload, {
        username: teacherUsername.value,
      });
    }
    closeBankDialog();
    await loadBanks();
  } catch (error) {
    loadError.value = error?.response?.data?.detail || error.message;
  } finally {
    submitting.value = false;
  }
}

async function removeBank(bankId) {
  if (!confirm("确定要删除此题库吗？")) return;
  try {
    await deleteQuestionBank(courseCode.value, bankId, {
      username: teacherUsername.value,
    });
    if (selectedBankId.value === bankId) {
      selectedBankId.value = null;
      questions.value = [];
    }
    await loadBanks();
  } catch (error) {
    loadError.value = error?.response?.data?.detail || error.message;
  }
}

function openQuestionDialog(question) {
  questionFormError.value = "";
  editingQuestion.value = question;
  if (question) {
    questionForm.questionCode = question.questionCode || "";
    questionForm.questionType = question.questionType || "single_choice";
    questionForm.difficultyLevel = question.difficultyLevel || 1;
    questionForm.importanceWeight = Number(question.importanceWeight ?? 1);
    questionForm.minMastery = Number(question.minMastery ?? 0);
    questionForm.maxMastery = Number(question.maxMastery ?? 1);
    questionForm.prompt = question.prompt || "";
    questionForm.options = Array.isArray(question.options) && question.options.length > 0
      ? [...question.options]
      : ["", "", "", ""];
    questionForm.answerIndexesText = Array.isArray(question.answer?.correctIndexes)
      ? question.answer.correctIndexes.map((index) => index + 1).join(",")
      : "1";
    questionForm.booleanAnswer = question.answer?.correctBoolean ? "true" : "false";
    questionForm.referenceAnswer = question.answer?.referenceAnswer || "";
    questionForm.keywordText = Array.isArray(question.answer?.keywords)
      ? question.answer.keywords.join(",")
      : "";
    questionForm.explanation = question.explanation || "";
    questionForm.estimatedSeconds = question.estimatedSeconds || 60;
  } else {
    resetQuestionForm();
  }
  questionDialogOpen.value = true;
}

function closeQuestionDialog() {
  questionDialogOpen.value = false;
  editingQuestion.value = null;
  questionFormError.value = "";
}

function buildQuestionAnswer() {
  if (questionForm.questionType === "true_false") {
    return { correctBoolean: questionForm.booleanAnswer === "true" };
  }
  if (questionForm.questionType === "short_answer") {
    return {
      referenceAnswer: questionForm.referenceAnswer,
      keywords: questionForm.keywordText
        .split(",")
        .map((item) => item.trim())
        .filter(Boolean),
    };
  }
  return {
    correctIndexes: questionForm.answerIndexesText
      .split(",")
      .map((item) => Number(item.trim()) - 1)
      .filter((item) => Number.isInteger(item) && item >= 0),
  };
}

async function submitQuestion() {
  if (!selectedBank.value) return;

  submitting.value = true;
  try {
    const payload = {
      questionCode: questionForm.questionCode,
      questionType: questionForm.questionType,
      difficultyLevel: questionForm.difficultyLevel,
      importanceWeight: questionForm.importanceWeight,
      minMastery: questionForm.minMastery,
      maxMastery: questionForm.maxMastery,
      prompt: questionForm.prompt,
      options:
        questionForm.questionType === "short_answer"
          ? []
          : questionForm.questionType === "true_false"
            ? ["正确", "错误"]
            : questionForm.options.filter((item) => item !== ""),
      answer: buildQuestionAnswer(),
      explanation: questionForm.explanation,
      estimatedSeconds: questionForm.estimatedSeconds,
    };

    if (editingQuestion.value) {
      await updateBankQuestion(
        courseCode.value,
        selectedBank.value.id,
        editingQuestion.value.id,
        payload,
        { username: teacherUsername.value },
      );
    } else {
      await createBankQuestion(courseCode.value, selectedBank.value.id, payload, {
        username: teacherUsername.value,
      });
    }

    closeQuestionDialog();
    await selectBank(selectedBank.value);
    await loadBanks();
  } catch (error) {
    loadError.value = error?.response?.data?.detail || error.message;
  } finally {
    submitting.value = false;
  }
}

function formatWeight(value) {
  return Number(value ?? 1).toFixed(1);
}

function formatMasteryRange(minValue, maxValue) {
  return `${Math.round(Number(minValue ?? 0) * 100)}% - ${Math.round(Number(maxValue ?? 1) * 100)}%`;
}

async function removeQuestion(questionId) {
  if (!selectedBank.value || !confirm("确定要删除此题目吗？")) return;
  try {
    await deleteBankQuestion(courseCode.value, selectedBank.value.id, questionId, {
      username: teacherUsername.value,
    });
    await selectBank(selectedBank.value);
    await loadBanks();
  } catch (error) {
    loadError.value = error?.response?.data?.detail || error.message;
  }
}

onMounted(loadBanks);
</script>

<style scoped>
.questions-layout {
  display: grid;
  gap: 18px;
  grid-template-columns: 280px minmax(0, 1fr);
}
.banks-panel,
.questions-panel {
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #fff;
  padding: 18px;
}
.panel-head,
.panel-actions,
.dialog-head,
.dialog-actions,
.toolbar {
  display: flex;
  align-items: center;
  gap: 12px;
}
.panel-head {
  justify-content: space-between;
}
.panel-actions {
  flex-wrap: wrap;
}
.panel-subtitle {
  margin: 6px 0 0;
  color: #607380;
  font-size: 0.9rem;
}
.bank-list {
  display: grid;
  gap: 10px;
  margin-top: 14px;
}
.bank-item {
  display: grid;
  gap: 4px;
  padding: 12px;
  border: 1px solid #d8e0e6;
  border-radius: 12px;
  background: #fff;
  text-align: left;
  cursor: pointer;
}
.bank-item--active {
  border-color: #0c6a71;
  background: #eef7f8;
}
.toolbar {
  margin: 16px 0;
  flex-wrap: wrap;
}
.filter-input,
.filter-select,
.edit-input {
  padding: 8px 10px;
  border: 1px solid #c7d5df;
  border-radius: 8px;
  background: #fff;
}
.table-shell {
  overflow-x: auto;
  border: 1px solid #e6edf2;
  border-radius: 12px;
}
.question-table {
  width: 100%;
  border-collapse: collapse;
}
.question-table th,
.question-table td {
  padding: 14px 12px;
  border-bottom: 1px solid #eef2f5;
  text-align: left;
  vertical-align: top;
}
.question-table th {
  background: #f7fbfd;
  color: #4f6270;
}
.actions-cell {
  white-space: nowrap;
}
.dialog-backdrop {
  position: fixed;
  inset: 0;
  background: rgba(14, 28, 36, 0.42);
  display: grid;
  place-items: center;
  padding: 24px;
}
.dialog-panel {
  width: min(900px, 100%);
  max-height: 90vh;
  overflow: auto;
  background: #fff;
  border-radius: 18px;
  padding: 20px;
  box-shadow: 0 20px 60px rgba(17, 35, 48, 0.18);
}
.dialog-panel--small {
  width: min(620px, 100%);
}
.form-grid {
  display: grid;
  gap: 12px;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  margin: 18px 0;
}
.field {
  display: grid;
  gap: 6px;
}
.field--full {
  grid-column: 1 / -1;
}
.btn {
  padding: 8px 16px;
  border: 1px solid #d8e0e6;
  border-radius: 8px;
  background: #ffffff;
  color: #15364a;
  font-weight: 600;
  cursor: pointer;
}
.btn--primary {
  background: #0c6a71;
  color: #fff;
  border-color: #0c6a71;
}
.btn--danger {
  color: #9b3333;
  border-color: #e0cccc;
}
.btn--small {
  padding: 4px 10px;
  font-size: 0.85rem;
}
.empty-note,
.empty-cell {
  color: #607380;
}
</style>
