import http from "./http";

export async function generateLearningPath(payload) {
  const { data } = await http.post("/path/generate", payload);
  return data;
}

export async function adjustLearningPath(payload) {
  const { data } = await http.post("/path/adjust", payload);
  return data;
}
