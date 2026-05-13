import http from "./http";

export async function fetchAdminUsers() {
  const { data } = await http.get("/admin/users");
  return data;
}

export async function fetchAdminCourses() {
  const { data } = await http.get("/admin/courses");
  return data;
}

export async function fetchAdminAssignments() {
  const { data } = await http.get("/admin/course-assignments");
  return data;
}
