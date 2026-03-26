import http from "./http";

export async function recordResourceView(payload) {
  const { data } = await http.post("/resource/view", payload);
  return data;
}
