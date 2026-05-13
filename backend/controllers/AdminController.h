#pragma once

#include <drogon/HttpController.h>

namespace api
{
class AdminController : public drogon::HttpController<AdminController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AdminController::listUsers, "/api/admin/users", drogon::Get);
    ADD_METHOD_TO(AdminController::listCourses, "/api/admin/courses", drogon::Get);
    ADD_METHOD_TO(AdminController::listCourseAssignments,
                  "/api/admin/course-assignments", drogon::Get);
    METHOD_LIST_END

    void listUsers(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void listCourses(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void listCourseAssignments(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
