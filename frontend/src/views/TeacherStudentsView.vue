<template>
  <section class="students-layout">
    <div class="filter-bar">
      <label class="filter-label">
        专业
        <select v-model="filterMajor" class="filter-select" @change="loadStudents">
          <option value="">全部</option>
          <option v-for="m in availableMajors" :key="m" :value="m">{{ m }}</option>
        </select>
      </label>
      <label class="filter-label">
        年级
        <select v-model="filterGrade" class="filter-select" @change="loadStudents">
          <option value="">全部</option>
          <option v-for="g in availableGrades" :key="g" :value="g">{{ g }}</option>
        </select>
      </label>
    </div>

    <div class="students-actions">
      <button type="button" class="btn btn--primary" @click="toggleAddPanel">
        {{ showAddForm ? "收起" : "增添学生" }}
      </button>
      <span v-if="addMessage" class="edit-message" :class="addMessageClass">{{ addMessage }}</span>
    </div>

    <!-- Add student: pick from available list -->
    <div v-if="showAddForm" class="add-panel">
      <h4>选择学生加入课程</h4>
      <div class="add-filter-bar">
        <label class="filter-label">
          专业
          <select v-model="availMajorFilter" class="filter-select" @change="loadAvailableStudents">
            <option value="">全部</option>
            <option v-for="m in availMajors" :key="m" :value="m">{{ m }}</option>
          </select>
        </label>
        <label class="filter-label">
          年级
          <select v-model="availGradeFilter" class="filter-select" @change="loadAvailableStudents">
            <option value="">全部</option>
            <option v-for="g in availGrades" :key="g" :value="g">{{ g }}</option>
          </select>
        </label>
      </div>

      <p v-if="availLoading" class="state-message">正在加载可选学生...</p>
      <p v-else-if="availableStudents.length === 0" class="state-message">暂无可选学生。</p>
      <div v-else class="avail-grid">
        <label
          v-for="s in availableStudents"
          :key="s.learnerCode"
          class="avail-card"
          :class="{
            'avail-card--selected': selectedCodes.has(s.learnerCode),
            'avail-card--in-course': isInCurrentCourse(s),
          }"
        >
          <input
            type="checkbox"
            :checked="selectedCodes.has(s.learnerCode)"
            :disabled="isInCurrentCourse(s)"
            @change="toggleSelect(s.learnerCode)"
          />
          <div>
            <strong>{{ s.learnerName }}</strong>
            <span v-if="isInCurrentCourse(s)" class="avail-in-course-badge">已在课程中</span>
            <span class="avail-meta">{{ s.learnerCode }} · {{ s.major }} · {{ s.gradeLabel }}</span>
            <span class="avail-course">当前课程: {{ s.currentCourseName || s.currentCourseCode }}</span>
          </div>
        </label>
      </div>
      <div v-if="selectedCodes.size > 0" class="add-actions">
        <button type="button" class="btn btn--primary" @click="submitAddSelected" :disabled="adding">
          {{ adding ? "添加中..." : `添加选中 (${selectedCodes.size})` }}
        </button>
        <button type="button" class="btn" @click="showAddForm = false">取消</button>
      </div>
    </div>

    <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
    <p v-else-if="loading" class="state-message">正在加载学生列表...</p>
    <p v-else-if="students.length === 0" class="state-message">暂无学生数据。</p>

    <div v-else class="student-grid">
      <article
        v-for="student in students"
        :key="student.learnerCode"
        class="student-card"
        data-testid="student-card"
      >
        <div class="student-header">
          <h3>{{ student.learnerName }}</h3>
          <span class="student-meta">{{ student.major }} · {{ student.gradeLabel }}</span>
        </div>
        <div class="mastery-bar">
          <div class="mastery-bar__label">综合掌握度</div>
          <div class="mastery-bar__track">
            <div
              class="mastery-bar__fill"
              :style="{ width: masteryPercent(student.overallMastery) }"
            ></div>
          </div>
          <span class="mastery-bar__value">{{ masteryPercent(student.overallMastery) }}</span>
        </div>
        <p class="student-activity" v-if="student.lastActivityAt">
          最近活动：{{ student.lastActivityAt }}
        </p>
        <RouterLink
          class="student-link"
          :to="{
            name: 'teacher-student-progress',
            params: { courseCode, learnerCode: student.learnerCode },
          }"
        >
          查看详情
        </RouterLink>
      </article>
    </div>
  </section>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";

import { addStudentToCourse, fetchAvailableStudents, fetchTeacherStudents } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const route = useRoute();
const authStore = useAuthStore();

const loading = ref(true);
const loadError = ref("");
const allStudents = ref([]);
const filterMajor = ref("");
const filterGrade = ref("");

// Add student: pick from available list
const showAddForm = ref(false);
const adding = ref(false);
const addMessage = ref("");
const addMessageClass = ref("");
const availableStudents = ref([]);
const selectedCodes = ref(new Set());
const availLoading = ref(false);
const availMajorFilter = ref("");
const availGradeFilter = ref("");

function isInCurrentCourse(student) {
  return student.currentCourseCode === courseCode.value;
}

const courseCode = computed(() => String(route.params.courseCode || ""));
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const students = computed(() => allStudents.value);

const availableMajors = computed(() => {
  const set = new Set(allStudents.value.map((s) => s.major).filter(Boolean));
  return [...set].sort();
});

const availableGrades = computed(() => {
  const set = new Set(allStudents.value.map((s) => s.gradeLabel).filter(Boolean));
  return [...set].sort();
});

function masteryPercent(score) {
  return Math.round((score || 0) * 100) + "%";
}

async function loadStudents() {
  loading.value = true;
  loadError.value = "";
  try {
    const payload = await fetchTeacherStudents(courseCode.value, {
      username: teacherUsername.value,
      major: filterMajor.value || undefined,
      grade_label: filterGrade.value || undefined,
    });
    allStudents.value = Array.isArray(payload?.students) ? payload.students : [];
  } catch (error) {
    loadError.value = "未能读取学生列表，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

const availMajors = computed(() => {
  const set = new Set(availableStudents.value.map((s) => s.major).filter(Boolean));
  return [...set].sort();
});

const availGrades = computed(() => {
  const set = new Set(availableStudents.value.map((s) => s.gradeLabel).filter(Boolean));
  return [...set].sort();
});

async function toggleAddPanel() {
  showAddForm.value = !showAddForm.value;
  if (showAddForm.value) {
    await loadAvailableStudents();
  }
}

async function loadAvailableStudents() {
  availLoading.value = true;
  try {
    const payload = await fetchAvailableStudents(courseCode.value, {
      username: teacherUsername.value,
      major: availMajorFilter.value || undefined,
      grade_label: availGradeFilter.value || undefined,
    });
    availableStudents.value = payload?.students || [];
  } catch {
    addMessage.value = "加载可选学生失败";
    addMessageClass.value = "edit-message--error";
  } finally {
    availLoading.value = false;
  }
}

function toggleSelect(code) {
  const next = new Set(selectedCodes.value);
  if (next.has(code)) next.delete(code);
  else next.add(code);
  selectedCodes.value = next;
}

async function submitAddSelected() {
  adding.value = true;
  addMessage.value = "";
  let success = 0;
  let fail = 0;
  for (const code of selectedCodes.value) {
    const s = availableStudents.value.find((x) => x.learnerCode === code);
    if (!s) continue;
    try {
      await addStudentToCourse(courseCode.value, {
        learnerCode: code,
        learnerName: s.learnerName,
        major: s.major,
        gradeLabel: s.gradeLabel,
      }, { username: teacherUsername.value });
      success++;
    } catch {
      fail++;
    }
  }
  addMessage.value = `已添加 ${success} 名` + (fail ? `，${fail} 名失败` : "");
  addMessageClass.value = fail ? "edit-message--error" : "edit-message--ok";
  selectedCodes.value = new Set();
  showAddForm.value = false;
  adding.value = false;
  await loadStudents();
}

onMounted(loadStudents);
</script>

<style scoped>
.students-layout {
  display: grid;
  gap: 18px;
}

.filter-bar {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
}

.filter-label {
  display: flex;
  align-items: center;
  gap: 8px;
  color: #51606d;
  font-size: 0.9rem;
}

.filter-select {
  padding: 6px 10px;
  border: 1px solid #d8e0e6;
  border-radius: 8px;
  background: #ffffff;
  color: #15364a;
}

.student-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
}

.student-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.student-header h3 {
  margin: 0;
  font-size: 1.1rem;
  color: #15364a;
}

.student-meta {
  display: block;
  margin-top: 4px;
  color: #51606d;
  font-size: 0.9rem;
}

.mastery-bar {
  display: flex;
  align-items: center;
  gap: 10px;
  margin-top: 14px;
}

.mastery-bar__label {
  flex-shrink: 0;
  color: #51606d;
  font-size: 0.85rem;
}

.mastery-bar__track {
  flex: 1;
  height: 10px;
  background: #eef7f8;
  border-radius: 5px;
  overflow: hidden;
}

.mastery-bar__fill {
  height: 100%;
  background: #0c6a71;
  border-radius: 5px;
  transition: width 0.3s;
}

.mastery-bar__value {
  flex-shrink: 0;
  color: #0c6a71;
  font-weight: 700;
  font-size: 0.9rem;
}

.student-activity {
  margin: 10px 0 0;
  color: #8896a0;
  font-size: 0.8rem;
}

.student-link {
  display: inline-flex;
  margin-top: 12px;
  color: #0c6a71;
  font-weight: 700;
  text-decoration: none;
}

.state-message {
  color: #51606d;
  margin: 0;
}

.state-message--error {
  color: #9b3333;
}

.students-actions {
  display: flex;
  align-items: center;
  gap: 12px;
  flex-wrap: wrap;
}

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

.edit-message { font-size: 0.9rem; }
.edit-message--ok { color: #1a7a3a; }
.edit-message--error { color: #9b3333; }

.add-panel {
  padding: 16px;
  border: 1px solid #d8e0e6;
  border-radius: 12px;
  background: #f9fdfd;
}
.add-panel h4 { margin: 0 0 12px; color: #15364a; }
.add-grid { display: grid; gap: 10px; grid-template-columns: 1fr 1fr; }
.add-field { display: grid; gap: 4px; font-size: 0.85rem; color: #51606d; }
.edit-input {
  padding: 6px 8px;
  border: 1px solid #a0b8c4;
  border-radius: 4px;
  font-size: 0.85rem;
  width: 100%;
  box-sizing: border-box;
}
.add-actions { margin-top: 10px; display: flex; gap: 8px; }

.add-filter-bar {
  display: flex;
  gap: 12px;
  margin-bottom: 14px;
  flex-wrap: wrap;
}

.avail-grid {
  display: grid;
  gap: 8px;
  max-height: 320px;
  overflow-y: auto;
}

.avail-card {
  display: flex;
  align-items: flex-start;
  gap: 10px;
  padding: 10px;
  border: 1px solid #d8e0e6;
  border-radius: 8px;
  cursor: pointer;
  transition: background 0.15s;
}
.avail-card:hover { background: #f0f7f8; }
.avail-card--selected { background: #e0f2e9; border-color: #8fc9a5; }

.avail-meta {
  display: block;
  font-size: 0.8rem;
  color: #51606d;
  margin-top: 2px;
}

.avail-course {
  display: block;
  font-size: 0.75rem;
  color: #8896a0;
  margin-top: 2px;
}

.avail-card--in-course {
  opacity: 0.55;
  cursor: not-allowed;
  background: #f5f5f5;
}

.avail-card--in-course input {
  pointer-events: none;
}

.avail-in-course-badge {
  display: inline-block;
  margin-left: 8px;
  padding: 1px 8px;
  border-radius: 6px;
  background: #e0f2e9;
  color: #1a7a3a;
  font-size: 0.75rem;
  font-weight: 700;
}
</style>
