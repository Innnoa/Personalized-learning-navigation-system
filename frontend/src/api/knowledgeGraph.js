import http from "./http";

export async function fetchKnowledgeGraph(params = {}) {
  const { data } = await http.get("/knowledge/graph", { params });
  return data;
}
