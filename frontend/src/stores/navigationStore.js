import { defineStore } from "pinia";

export const useNavigationStore = defineStore("navigation", {
  state: () => ({
    selectedCourse: "数据结构",
    recommendedPath: [],
    graphFilter: "all",
  }),
});
