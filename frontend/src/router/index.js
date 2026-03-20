import { createRouter, createWebHistory } from "vue-router";

import HomeView from "../views/HomeView.vue";
import LearnerProfileView from "../views/LearnerProfileView.vue";

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      name: "home",
      component: HomeView,
    },
    {
      path: "/learner-profile",
      name: "learner-profile",
      component: LearnerProfileView,
    },
  ],
});

export default router;
