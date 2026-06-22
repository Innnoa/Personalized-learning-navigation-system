import http from "./http";

export async function submitLearningFeedback(payload) {
  const { data } = await http.post("/feedback/submit", payload);
  return data;
}

export async function rollbackLatestLearningFeedback(payload = {}) {
  const { data } = await http.post("/feedback/rollback", payload);
  return data;
}

export async function fetchPracticeQuestions(params = {}) {
  const { data } = await http.get("/feedback/practice-questions", { params });
  return data;
}
