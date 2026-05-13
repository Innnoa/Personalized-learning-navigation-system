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
