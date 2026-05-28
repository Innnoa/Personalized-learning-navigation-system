import http from "./http";

// Read
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

// User management
export async function createAdminUser(body) {
  const { data } = await http.post("/admin/users", body);
  return data;
}

export async function updateAdminUser(username, body) {
  const { data } = await http.put(`/admin/users/${username}`, body);
  return data;
}

export async function toggleAdminUserStatus(username) {
  const { data } = await http.post(`/admin/users/${username}/toggle-status`);
  return data;
}

export async function setAdminUserRoles(username, body) {
  const { data } = await http.put(`/admin/users/${username}/roles`, body);
  return data;
}

// Course management
export async function updateAdminCourse(courseCode, body) {
  const { data } = await http.put(`/admin/courses/${courseCode}`, body);
  return data;
}

export async function deleteAdminCourse(courseCode) {
  const { data } = await http.delete(`/admin/courses/${courseCode}`);
  return data;
}

// Assignment management
export async function assignTeacherToCourse(body) {
  const { data } = await http.post("/admin/course-assignments", body);
  return data;
}

export async function unassignTeacherFromCourse(username, courseCode) {
  const { data } = await http.delete(`/admin/course-assignments/${username}/${courseCode}`);
  return data;
}

// Learner management
export async function fetchAdminLearners() {
  const { data } = await http.get("/admin/learners");
  return data;
}

export async function updateAdminLearner(learnerCode, body) {
  const { data } = await http.put(`/admin/learners/${learnerCode}`, body);
  return data;
}

export async function deleteAdminLearner(learnerCode) {
  const { data } = await http.delete(`/admin/learners/${learnerCode}`);
  return data;
}

export async function reassignLearnerCourse(learnerCode, body) {
  const { data } = await http.post(`/admin/learners/${learnerCode}/reassign-course`, body);
  return data;
}

export async function fetchAdminLogs(limit = 100) {
  const { data } = await http.get("/admin/logs", { params: { limit } });
  return data;
}
