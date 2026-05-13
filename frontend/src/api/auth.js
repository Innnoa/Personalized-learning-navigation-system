import http from "./http";

export async function login(payload = {}) {
  const { data } = await http.post("/auth/login", payload);
  return data;
}

export async function logout() {
  const { data } = await http.post("/auth/logout");
  return data;
}

export async function fetchSession() {
  const { data } = await http.get("/auth/session");
  return data;
}
