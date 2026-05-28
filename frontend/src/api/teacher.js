import http from "./http";

export async function fetchTeacherCourses(params = {}) {
  const { data } = await http.get("/teacher/courses", { params });
  return data;
}

export async function fetchTeacherCourseOverview(params = {}) {
  const { courseCode, ...query } = params;
  const { data } = await http.get(`/teacher/courses/${courseCode}/overview`, {
    params: query,
  });
  return data;
}

export async function fetchTeacherStudents(courseCode, params = {}) {
  const { data } = await http.get(`/teacher/courses/${courseCode}/students`, {
    params,
  });
  return data;
}

export async function fetchTeacherCourseStats(courseCode, params = {}) {
  const { data } = await http.get(`/teacher/courses/${courseCode}/stats`, {
    params,
  });
  return data;
}

export async function fetchTeacherKnowledgeStats(courseCode, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/knowledge-stats`,
    { params },
  );
  return data;
}

export async function fetchTeacherStudentProgress(learnerCode, params = {}) {
  const { data } = await http.get(
    `/teacher/students/${learnerCode}/progress`,
    { params },
  );
  return data;
}

// Course edit APIs (B phase)

export async function createKnowledgePoint(courseCode, body, params = {}) {
  const { data } = await http.post(
    `/teacher/courses/${courseCode}/knowledge-points`,
    body,
    { params },
  );
  return data;
}

export async function updateKnowledgePoint(courseCode, pointCode, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/knowledge-points/${pointCode}`,
    body,
    { params },
  );
  return data;
}

export async function deleteKnowledgePoint(courseCode, pointCode, params = {}) {
  const { data } = await http.delete(
    `/teacher/courses/${courseCode}/knowledge-points/${pointCode}`,
    { params },
  );
  return data;
}

export async function updateDependencies(courseCode, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/knowledge-dependencies`,
    body,
    { params },
  );
  return data;
}

export async function fetchDependencies(courseCode, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/knowledge-dependencies`,
    { params },
  );
  return data;
}

export async function createTeacherCourse(body, params = {}) {
  const { data } = await http.post("/teacher/courses", body, { params });
  return data;
}

export async function addStudentToCourse(courseCode, body, params = {}) {
  const { data } = await http.post(
    `/teacher/courses/${courseCode}/students`,
    body,
    { params },
  );
  return data;
}

export async function fetchAvailableStudents(courseCode, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/available-students`,
    { params },
  );
  return data;
}

export async function fetchCourseResources(courseCode, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/resources`,
    { params },
  );
  return data;
}

export async function saveCourseResources(courseCode, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/resources`,
    body,
    { params },
  );
  return data;
}

export async function fetchCourseQuestions(courseCode, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/questions`,
    { params },
  );
  return data;
}

export async function saveCourseQuestions(courseCode, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/questions`,
    body,
    { params },
  );
  return data;
}
