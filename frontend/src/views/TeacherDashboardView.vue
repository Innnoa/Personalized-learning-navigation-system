<template>
  <PageLayout
    eyebrow="教师工作台"
    title="课程维护概览"
    description="查看已分配课程，创建新课程，并进入各课程的维护入口。"
    role-scope="teacher"
  >
    <section class="dashboard-stack">
      <div class="dashboard-actions">
        <button type="button" class="btn btn--primary" @click="showCreateForm = !showCreateForm">
          {{ showCreateForm ? "收起" : "创建课程" }}
        </button>
        <span v-if="createMessage" class="edit-message" :class="createMessageClass">{{ createMessage }}</span>
      </div>

      <!-- Create course form -->
      <div v-if="showCreateForm" class="create-panel">
        <h4>新建课程</h4>
        <div class="create-grid">
          <label class="create-field">
            课程代码
            <input v-model="createForm.code" class="edit-input" placeholder="如: algorithms" />
          </label>
          <label class="create-field">
            课程名称
            <input v-model="createForm.name" class="edit-input" placeholder="如: 算法设计与分析" />
          </label>
          <label class="create-field">
            适用对象
            <input v-model="createForm.targetAudience" class="edit-input" placeholder="如: 大二 计算机科学" />
          </label>
          <label class="create-field create-field--full">
            描述
            <textarea v-model="createForm.description" class="edit-textarea" rows="2" placeholder="课程简介"></textarea>
          </label>
        </div>
        <div class="create-actions">
          <button type="button" class="btn btn--primary" @click="submitCreateCourse" :disabled="creating">
            {{ creating ? "创建中..." : "创建" }}
          </button>
          <button type="button" class="btn" @click="showCreateForm = false">取消</button>
        </div>
      </div>

      <div class="summary-grid">
        <article class="summary-card">
          <p class="summary-label">已分配课程</p>
          <strong class="summary-value">{{ courses.length }}</strong>
        </article>
        <article class="summary-card">
          <p class="summary-label">维护入口</p>
          <strong class="summary-value">course overview</strong>
        </article>
      </div>

      <p v-if="loadError" class="state-message state-message--error">{{ loadError }}</p>
      <p v-else-if="loading" class="state-message">正在加载教师课程...</p>

      <div v-else class="course-grid">
        <article
          v-for="course in courses"
          :key="course.courseCode"
          class="course-card"
          data-testid="course-summary-card"
        >
          <div class="course-card__header">
            <h2>{{ course.courseName }}</h2>
            <span class="course-code">{{ course.courseCode }}</span>
          </div>
          <p class="course-audience">{{ course.targetAudience || "未填写适用对象" }}</p>
          <RouterLink
            class="course-link"
            :to="{ name: 'teacher-course-overview', params: { courseCode: course.courseCode } }"
          >
            查看课程详情
          </RouterLink>
        </article>
      </div>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, onMounted, reactive, ref } from "vue";

import PageLayout from "../components/PageLayout.vue";
import { createTeacherCourse, fetchTeacherCourses } from "../api/teacher";
import { useAuthStore } from "../stores/authStore";

const authStore = useAuthStore();
const loading = ref(true);
const loadError = ref("");
const teacherCourses = ref([]);
const teacherUsername = computed(() => String(authStore.currentUser?.username || ""));
const courses = computed(() => teacherCourses.value);

const showCreateForm = ref(false);
const creating = ref(false);
const createMessage = ref("");
const createMessageClass = ref("");

const createForm = reactive({
  code: "",
  name: "",
  description: "",
  targetAudience: "",
});

async function loadCourses() {
  loading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchTeacherCourses({
      username: teacherUsername.value,
    });
    teacherCourses.value = Array.isArray(payload?.courses) ? payload.courses : [];
  } catch (error) {
    loadError.value = "未能读取教师课程概览，请稍后重试。";
    console.error(error);
  } finally {
    loading.value = false;
  }
}

async function submitCreateCourse() {
  if (!createForm.code || !createForm.name) {
    createMessage.value = "课程代码和名称不能为空";
    createMessageClass.value = "edit-message--error";
    return;
  }
  creating.value = true;
  createMessage.value = "";
  try {
    await createTeacherCourse({
      code: createForm.code,
      name: createForm.name,
      description: createForm.description,
      targetAudience: createForm.targetAudience,
    }, { username: teacherUsername.value });
    createMessage.value = "课程 " + createForm.code + " 创建成功";
    createMessageClass.value = "edit-message--ok";
    showCreateForm.value = false;
    createForm.code = "";
    createForm.name = "";
    createForm.description = "";
    createForm.targetAudience = "";
    await loadCourses();
  } catch (error) {
    createMessage.value = "创建失败: " + (error?.response?.data?.detail || error.message);
    createMessageClass.value = "edit-message--error";
  } finally {
    creating.value = false;
  }
}

onMounted(loadCourses);
</script>

<style scoped>
.dashboard-stack {
  display: grid;
  gap: 18px;
}

.dashboard-actions {
  display: flex;
  align-items: center;
  gap: 12px;
  flex-wrap: wrap;
}

.summary-grid,
.course-grid {
  display: grid;
  gap: 16px;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
}

.summary-card,
.course-card {
  padding: 18px;
  border: 1px solid #d8e0e6;
  border-radius: 16px;
  background: #ffffff;
}

.summary-label,
.course-audience,
.state-message {
  margin: 0;
  color: #51606d;
}

.summary-value {
  display: block;
  margin-top: 10px;
  font-size: 1.5rem;
  color: #15364a;
}

.course-card__header {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: baseline;
}

.course-card__header h2 {
  margin: 0;
  font-size: 1.1rem;
}

.course-code {
  color: #0c6a71;
  font-weight: 700;
}

.course-link {
  display: inline-flex;
  margin-top: 14px;
  color: #0c6a71;
  font-weight: 700;
  text-decoration: none;
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

.create-panel {
  padding: 16px;
  border: 1px solid #d8e0e6;
  border-radius: 12px;
  background: #f9fdfd;
}
.create-panel h4 { margin: 0 0 12px; color: #15364a; }
.create-grid { display: grid; gap: 10px; grid-template-columns: 1fr 1fr; }
.create-field { display: grid; gap: 4px; font-size: 0.85rem; color: #51606d; }
.create-field--full { grid-column: 1 / -1; }
.edit-input, .edit-textarea {
  padding: 6px 8px;
  border: 1px solid #a0b8c4;
  border-radius: 4px;
  font-size: 0.85rem;
  width: 100%;
  box-sizing: border-box;
}
.create-actions { margin-top: 10px; display: flex; gap: 8px; }
</style>
