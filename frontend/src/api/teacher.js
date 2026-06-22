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

export async function createCourseResource(courseCode, body, params = {}) {
  const { data } = await http.post(
    `/teacher/courses/${courseCode}/resources`,
    body,
    { params },
  );
  return data;
}

export async function updateCourseResource(courseCode, resourceId, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/resources/${resourceId}`,
    body,
    { params },
  );
  return data;
}

export async function deleteCourseResource(courseCode, resourceId, params = {}) {
  const { data } = await http.delete(
    `/teacher/courses/${courseCode}/resources/${resourceId}`,
    { params },
  );
  return data;
}

export async function fetchQuestionBanks(courseCode, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/question-banks`,
    { params },
  );
  return data;
}

export async function createQuestionBank(courseCode, body, params = {}) {
  const { data } = await http.post(
    `/teacher/courses/${courseCode}/question-banks`,
    body,
    { params },
  );
  return data;
}

export async function updateQuestionBank(courseCode, bankId, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/question-banks/${bankId}`,
    body,
    { params },
  );
  return data;
}

export async function deleteQuestionBank(courseCode, bankId, params = {}) {
  const { data } = await http.delete(
    `/teacher/courses/${courseCode}/question-banks/${bankId}`,
    { params },
  );
  return data;
}

export async function fetchBankQuestions(courseCode, bankId, params = {}) {
  const { data } = await http.get(
    `/teacher/courses/${courseCode}/question-banks/${bankId}/questions`,
    { params },
  );
  return data;
}

export async function createBankQuestion(courseCode, bankId, body, params = {}) {
  const { data } = await http.post(
    `/teacher/courses/${courseCode}/question-banks/${bankId}/questions`,
    body,
    { params },
  );
  return data;
}

export async function updateBankQuestion(courseCode, bankId, questionId, body, params = {}) {
  const { data } = await http.put(
    `/teacher/courses/${courseCode}/question-banks/${bankId}/questions/${questionId}`,
    body,
    { params },
  );
  return data;
}

export async function deleteBankQuestion(courseCode, bankId, questionId, params = {}) {
  const { data } = await http.delete(
    `/teacher/courses/${courseCode}/question-banks/${bankId}/questions/${questionId}`,
    { params },
  );
  return data;
}
