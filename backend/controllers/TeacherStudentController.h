#pragma once

#include <drogon/HttpController.h>

namespace api
{
class TeacherStudentController : public drogon::HttpController<TeacherStudentController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeacherStudentController::listStudents,
                  "/api/teacher/courses/{1}/students", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::courseStats,
                  "/api/teacher/courses/{1}/stats", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::knowledgeStats,
                  "/api/teacher/courses/{1}/knowledge-stats", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::studentProgress,
                  "/api/teacher/students/{1}/progress", drogon::Get);
    ADD_METHOD_TO(TeacherStudentController::addStudent,
                  "/api/teacher/courses/{1}/students", drogon::Post);
    ADD_METHOD_TO(TeacherStudentController::availableStudents,
                  "/api/teacher/courses/{1}/available-students", drogon::Get);
    METHOD_LIST_END

    void listStudents(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      const std::string &courseCode) const;
    void courseStats(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     const std::string &courseCode) const;
    void knowledgeStats(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                        const std::string &courseCode) const;
    void studentProgress(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         const std::string &learnerCode) const;
    void addStudent(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                    const std::string &courseCode) const;
    void availableStudents(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                           const std::string &courseCode) const;
};
}
