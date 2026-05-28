#pragma once

#include <drogon/HttpController.h>

namespace api
{
class AdminController : public drogon::HttpController<AdminController>
{
  public:
    METHOD_LIST_BEGIN
    // Read
    ADD_METHOD_TO(AdminController::listUsers, "/api/admin/users", drogon::Get);
    ADD_METHOD_TO(AdminController::listCourses, "/api/admin/courses", drogon::Get);
    ADD_METHOD_TO(AdminController::listCourseAssignments,
                  "/api/admin/course-assignments", drogon::Get);

    // User management
    ADD_METHOD_TO(AdminController::createUser, "/api/admin/users", drogon::Post);
    ADD_METHOD_TO(AdminController::updateUser, "/api/admin/users/{1}", drogon::Put);
    ADD_METHOD_TO(AdminController::toggleUserStatus,
                  "/api/admin/users/{1}/toggle-status", drogon::Post);
    ADD_METHOD_TO(AdminController::setUserRoles,
                  "/api/admin/users/{1}/roles", drogon::Put);

    // Course management
    ADD_METHOD_TO(AdminController::updateCourse, "/api/admin/courses/{1}", drogon::Put);
    ADD_METHOD_TO(AdminController::deleteCourse, "/api/admin/courses/{1}", drogon::Delete);

    // Assignment management
    ADD_METHOD_TO(AdminController::assignTeacher, "/api/admin/course-assignments",
                  drogon::Post);
    ADD_METHOD_TO(AdminController::unassignTeacher,
                  "/api/admin/course-assignments/{1}/{2}", drogon::Delete);

    // Learner management
    ADD_METHOD_TO(AdminController::listLearners, "/api/admin/learners", drogon::Get);
    ADD_METHOD_TO(AdminController::updateLearner,
                  "/api/admin/learners/{1}", drogon::Put);
    ADD_METHOD_TO(AdminController::deleteLearner,
                  "/api/admin/learners/{1}", drogon::Delete);
    ADD_METHOD_TO(AdminController::reassignLearnerCourse,
                  "/api/admin/learners/{1}/reassign-course", drogon::Post);

    // Logs
    ADD_METHOD_TO(AdminController::listLogs, "/api/admin/logs", drogon::Get);
    METHOD_LIST_END

    void listUsers(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void listCourses(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void listCourseAssignments(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;

    void createUser(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void updateUser(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                    const std::string &username) const;
    void toggleUserStatus(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                          const std::string &username) const;
    void setUserRoles(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      const std::string &username) const;

    void updateCourse(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      const std::string &courseCode) const;
    void deleteCourse(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                      const std::string &courseCode) const;

    void assignTeacher(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void unassignTeacher(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         const std::string &username,
                         const std::string &courseCode) const;

    void listLearners(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void updateLearner(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       const std::string &learnerCode) const;
    void deleteLearner(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       const std::string &learnerCode) const;
    void reassignLearnerCourse(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                               const std::string &learnerCode) const;

    void listLogs(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
