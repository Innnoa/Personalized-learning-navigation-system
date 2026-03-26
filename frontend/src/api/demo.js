import http from "./http";

export async function resetDemoState(payload = {}) {
  const { data } = await http.post("/demo/reset", payload);
  return data;
}
