import { createRouter, createWebHistory } from "vue-router";

import { useAuthStore } from "../stores/authStore";
import HomeView from "../views/HomeView.vue";
import DetailLearningView from "../views/DetailLearningView.vue";
import LearningGraphView from "../views/LearningGraphView.vue";
import LearnerProfileView from "../views/LearnerProfileView.vue";
import PracticeCheckView from "../views/PracticeCheckView.vue";
import LoginView from "../views/LoginView.vue";
import RegisterView from "../views/RegisterView.vue";
import ResourceRecommendationView from "../views/ResourceRecommendationView.vue";
import AdminAssignmentsView from "../views/AdminAssignmentsView.vue";
import AdminCoursesView from "../views/AdminCoursesView.vue";
import AdminDashboardView from "../views/AdminDashboardView.vue";
import AdminLearnersView from "../views/AdminLearnersView.vue";
import AdminLogsView from "../views/AdminLogsView.vue";
import AdminUsersView from "../views/AdminUsersView.vue";
import TeacherCourseOverviewView from "../views/TeacherCourseOverviewView.vue";
import TeacherCoursePage from "../views/TeacherCoursePage.vue";
import TeacherCoursesView from "../views/TeacherCoursesView.vue";
import TeacherCourseStatsView from "../views/TeacherCourseStatsView.vue";
import TeacherDashboardView from "../views/TeacherDashboardView.vue";
import TeacherKnowledgeStatsView from "../views/TeacherKnowledgeStatsView.vue";
import TeacherStudentsView from "../views/TeacherStudentsView.vue";
import TeacherStudentProgressView from "../views/TeacherStudentProgressView.vue";
import TeacherResourcesView from "../views/TeacherResourcesView.vue";
import TeacherQuestionsView from "../views/TeacherQuestionsView.vue";

function resolveRoleFallback(activeRole) {
  if (activeRole === "teacher") {
    return { name: "teacher-dashboard" };
  }

  if (activeRole === "admin") {
    return { name: "admin-dashboard" };
  }

  return { name: "learning-graph" };
}

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      name: "home",
      component: HomeView,
      meta: { roleScope: "student" },
    },
    {
      path: "/learner-profile",
      name: "learner-profile",
      component: LearnerProfileView,
      meta: { roleScope: "student" },
    },
    {
      path: "/learning-graph",
      name: "learning-graph",
      component: LearningGraphView,
      meta: { roleScope: "student" },
    },
    {
      path: "/detail-learning",
      name: "detail-learning",
      component: DetailLearningView,
      meta: { roleScope: "student" },
    },
    {
      path: "/resource-recommendation/:code",
      name: "resource-recommendation",
      component: ResourceRecommendationView,
      meta: { roleScope: "student" },
    },
    {
      path: "/login",
      name: "login",
      component: LoginView,
    },
    {
      path: "/register",
      name: "register",
      component: RegisterView,
    },
    {
      path: "/teacher",
      name: "teacher-dashboard",
      component: TeacherDashboardView,
      meta: { roleScope: "teacher" },
    },
    {
      path: "/teacher/courses",
      name: "teacher-courses",
      component: TeacherCoursesView,
      meta: { roleScope: "teacher" },
    },
    {
      path: "/teacher/courses/:courseCode",
      component: TeacherCoursePage,
      props: true,
      meta: { roleScope: "teacher" },
      children: [
        {
          path: "",
          redirect: { name: "teacher-course-overview" },
        },
        {
          path: "overview",
          name: "teacher-course-overview",
          component: TeacherCourseOverviewView,
          meta: { roleScope: "teacher" },
        },
        {
          path: "stats",
          name: "teacher-course-stats",
          component: TeacherCourseStatsView,
          meta: { roleScope: "teacher" },
        },
        {
          path: "students",
          name: "teacher-course-students",
          component: TeacherStudentsView,
          meta: { roleScope: "teacher" },
        },
        {
          path: "points",
          name: "teacher-course-points",
          component: TeacherKnowledgeStatsView,
          meta: { roleScope: "teacher" },
        },
        {
          path: "resources",
          name: "teacher-course-resources",
          component: TeacherResourcesView,
          meta: { roleScope: "teacher" },
        },
        {
          path: "questions",
          name: "teacher-course-questions",
          component: TeacherQuestionsView,
          meta: { roleScope: "teacher" },
        },
      ],
    },
    {
      path: "/teacher/courses/:courseCode/students/:learnerCode",
      name: "teacher-student-progress",
      component: TeacherStudentProgressView,
      props: true,
      meta: { roleScope: "teacher" },
    },
    {
      path: "/admin",
      name: "admin-dashboard",
      component: AdminDashboardView,
      meta: { roleScope: "admin" },
    },
    {
      path: "/admin/users",
      name: "admin-users",
      component: AdminUsersView,
      meta: { roleScope: "admin" },
    },
    {
      path: "/admin/courses",
      name: "admin-courses",
      component: AdminCoursesView,
      meta: { roleScope: "admin" },
    },
    {
      path: "/admin/assignments",
      name: "admin-assignments",
      component: AdminAssignmentsView,
      meta: { roleScope: "admin" },
    },
    {
      path: "/admin/learners",
      name: "admin-learners",
      component: AdminLearnersView,
      meta: { roleScope: "admin" },
    },
    {
      path: "/admin/logs",
      name: "admin-logs",
      component: AdminLogsView,
      meta: { roleScope: "admin" },
    },
    {
      path: "/practice-check",
      name: "practice-check",
      component: PracticeCheckView,
    },
  ],
});

router.beforeEach((to) => {
  const authStore = useAuthStore();
  const requiredRole = to.meta.roleScope;

  if (!requiredRole) {
    return true;
  }

  if (!authStore.isAuthenticated) {
    return {
      name: "login",
      query: { redirect: to.fullPath },
    };
  }

  if (authStore.activeRole !== requiredRole) {
    return resolveRoleFallback(authStore.activeRole);
  }

  return true;
});

export default router;
