import http from "./http";

export async function fetchLearnerProfile(params = {}) {
  const { data } = await http.get("/learner/profile", { params });
  return data;
}
