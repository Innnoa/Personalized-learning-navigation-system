#pragma once

#include <drogon/HttpController.h>

namespace api
{
class TeacherController : public drogon::HttpController<TeacherController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeacherController::listCourses, "/api/teacher/courses", drogon::Get);
    ADD_METHOD_TO(TeacherController::courseOverview,
                  "/api/teacher/courses/{1}/overview", drogon::Get);
    METHOD_LIST_END

    void listCourses(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void courseOverview(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                        const std::string &courseCode) const;
};
}
