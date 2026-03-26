import { createRouter, createWebHistory } from "vue-router";

import HomeView from "../views/HomeView.vue";
import DetailLearningView from "../views/DetailLearningView.vue";
import LearningGraphView from "../views/LearningGraphView.vue";
import LearnerProfileView from "../views/LearnerProfileView.vue";
import ResourceRecommendationView from "../views/ResourceRecommendationView.vue";

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
    {
      path: "/learning-graph",
      name: "learning-graph",
      component: LearningGraphView,
    },
    {
      path: "/detail-learning",
      name: "detail-learning",
      component: DetailLearningView,
    },
    {
      path: "/resource-recommendation/:code",
      name: "resource-recommendation",
      component: ResourceRecommendationView,
    },
  ],
});

export default router;
