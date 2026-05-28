<template>
  <PageLayout
    :eyebrow="`教师工作台 · 课程详情`"
    :title="courseCode"
    description="课程内学生进度、统计与知识点分布概览。"
    role-scope="teacher"
  >
    <nav class="sub-nav">
      <RouterLink
        :to="{ name: 'teacher-course-overview', params: { courseCode } }"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('teacher-course-overview') }"
      >
        课程信息
      </RouterLink>
      <RouterLink
        :to="{ name: 'teacher-course-stats', params: { courseCode } }"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('teacher-course-stats') }"
      >
        学情总览
      </RouterLink>
      <RouterLink
        :to="{ name: 'teacher-course-students', params: { courseCode } }"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('teacher-course-students') }"
      >
        学生列表
      </RouterLink>
      <RouterLink
        :to="{ name: 'teacher-course-points', params: { courseCode } }"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('teacher-course-points') }"
      >
        知识点分布
      </RouterLink>
      <RouterLink
        :to="{ name: 'teacher-course-resources', params: { courseCode } }"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('teacher-course-resources') }"
      >
        学习资源
      </RouterLink>
      <RouterLink
        :to="{ name: 'teacher-course-questions', params: { courseCode } }"
        class="tab-link"
        :class="{ 'tab-link--active': isTabActive('teacher-course-questions') }"
      >
        题库管理
      </RouterLink>
    </nav>
    <section class="course-page-content">
      <RouterView />
    </section>
  </PageLayout>
</template>

<script setup>
import { useRoute } from "vue-router";

import PageLayout from "../components/PageLayout.vue";

defineProps({
  courseCode: { type: String, required: true },
});

const route = useRoute();

function isTabActive(name) {
  return route.name === name;
}
</script>

<style scoped>
.sub-nav {
  display: flex;
  gap: 0;
  border-bottom: 2px solid #d8e0e6;
  margin-bottom: 20px;
}

.tab-link {
  padding: 10px 20px;
  color: #51606d;
  text-decoration: none;
  font-weight: 600;
  border-bottom: 3px solid transparent;
  margin-bottom: -2px;
  transition: color 0.2s, border-color 0.2s;
}

.tab-link:hover {
  color: #0c6a71;
}

.tab-link--active {
  color: #0c6a71;
  border-bottom-color: #0c6a71;
}

.course-page-content {
  min-height: 200px;
}
</style>
