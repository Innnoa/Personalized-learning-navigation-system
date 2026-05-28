#include "AdminController.h"

#include "services/AdminDashboardService.h"
#include "services/AdminManageService.h"

using namespace api;

namespace
{
void respondJson(const Json::Value &payload,
                 std::function<void(const drogon::HttpResponsePtr &)> &callback,
                 drogon::HttpStatusCode statusCode = drogon::k200OK)
{
    auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
    response->setStatusCode(statusCode);
    callback(response);
}

void respondError(const std::exception &error,
                  const std::string &message,
                  drogon::HttpStatusCode statusCode,
                  std::function<void(const drogon::HttpResponsePtr &)> &callback)
{
    Json::Value payload;
    payload["status"] = "error";
    payload["message"] = message;
    payload["detail"] = error.what();
    respondJson(payload, callback, statusCode);
}

Json::Value parseBody(const drogon::HttpRequestPtr &req)
{
    const auto &jsonPtr = req->getJsonObject();
    if (jsonPtr) return *jsonPtr;
    std::string body = std::string(req->getBody());
    if (body.empty()) return Json::Value(Json::objectValue);
    Json::CharReaderBuilder builder;
    Json::Value parsed;
    std::string errors;
    std::istringstream stream(body);
    if (!Json::parseFromStream(builder, stream, &parsed, &errors))
        throw std::invalid_argument("请求体 JSON 解析失败: " + errors);
    return parsed;
}
}

// ---------- Read ----------

void AdminController::listUsers(const drogon::HttpRequestPtr &,
                                std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try { respondJson(services::AdminDashboardService::listUsers(), callback); }
    catch (const std::exception &e) { respondError(e, "用户列表读取失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::listCourses(const drogon::HttpRequestPtr &,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try { respondJson(services::AdminDashboardService::listCourses(), callback); }
    catch (const std::exception &e) { respondError(e, "课程列表读取失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::listCourseAssignments(const drogon::HttpRequestPtr &,
                                            std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try { respondJson(services::AdminDashboardService::listCourseAssignments(), callback); }
    catch (const std::exception &e) { respondError(e, "分配列表读取失败。", drogon::k500InternalServerError, callback); }
}

// ---------- Users ----------

void AdminController::createUser(const drogon::HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try
    {
        auto body = parseBody(req);
        auto payload = services::AdminManageService::createUser(body);
        services::AdminManageService::logOperation("admin", "创建用户", body["username"].asString(), "");
        respondJson(payload, callback, drogon::k201Created);
    }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "创建用户失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::updateUser(const drogon::HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                 const std::string &username) const
{
    try { respondJson(services::AdminManageService::updateUser(username, parseBody(req)), callback); }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "更新用户失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::toggleUserStatus(const drogon::HttpRequestPtr &,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                       const std::string &username) const
{
    try {
        auto payload = services::AdminManageService::toggleUserStatus(username);
        services::AdminManageService::logOperation("admin", "切换用户状态", username, payload["status"].asString());
        respondJson(payload, callback);
    }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "切换状态失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::setUserRoles(const drogon::HttpRequestPtr &req,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                   const std::string &username) const
{
    try {
        auto payload = services::AdminManageService::setUserRoles(username, parseBody(req));
        services::AdminManageService::logOperation("admin", "设置用户角色", username, "");
        respondJson(payload, callback);
    }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "设置角色失败。", drogon::k500InternalServerError, callback); }
}

// ---------- Courses ----------

void AdminController::updateCourse(const drogon::HttpRequestPtr &req,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                   const std::string &courseCode) const
{
    try {
        auto payload = services::AdminManageService::updateCourse(courseCode, parseBody(req));
        services::AdminManageService::logOperation("admin", "更新课程", courseCode, "");
        respondJson(payload, callback);
    }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "更新课程失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::deleteCourse(const drogon::HttpRequestPtr &,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                   const std::string &courseCode) const
{
    try {
        auto payload = services::AdminManageService::deleteCourse(courseCode);
        services::AdminManageService::logOperation("admin", "删除课程", courseCode, "");
        respondJson(payload, callback);
    }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "删除课程失败。", drogon::k500InternalServerError, callback); }
}

// ---------- Assignments ----------

void AdminController::assignTeacher(const drogon::HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try {
        auto body = parseBody(req);
        auto payload = services::AdminManageService::assignTeacher(body);
        services::AdminManageService::logOperation("admin", "指派教师",
            body["username"].asString(), body["courseCode"].asString());
        respondJson(payload, callback, drogon::k201Created);
    }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "分配教师失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::unassignTeacher(const drogon::HttpRequestPtr &,
                                      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                      const std::string &username,
                                      const std::string &courseCode) const
{
    try { respondJson(services::AdminManageService::unassignTeacher(username, courseCode), callback); }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "撤销分配失败。", drogon::k500InternalServerError, callback); }
}

// ---------- Learners ----------

void AdminController::listLearners(const drogon::HttpRequestPtr &,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try { respondJson(services::AdminManageService::listAllLearners(), callback); }
    catch (const std::exception &e) { respondError(e, "学习者列表失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::updateLearner(const drogon::HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                    const std::string &learnerCode) const
{
    try { respondJson(services::AdminManageService::updateLearner(learnerCode, parseBody(req)), callback); }
    catch (const std::exception &e) { respondError(e, "更新失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::deleteLearner(const drogon::HttpRequestPtr &,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                    const std::string &learnerCode) const
{
    try { respondJson(services::AdminManageService::deleteLearner(learnerCode), callback); }
    catch (const std::exception &e) { respondError(e, "删除失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::reassignLearnerCourse(const drogon::HttpRequestPtr &req,
                                             std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                             const std::string &learnerCode) const
{
    try { respondJson(services::AdminManageService::reassignLearnerCourse(learnerCode, parseBody(req)), callback); }
    catch (const std::invalid_argument &e) { respondError(e, "请求不合法。", drogon::k400BadRequest, callback); }
    catch (const std::exception &e) { respondError(e, "转课失败。", drogon::k500InternalServerError, callback); }
}

void AdminController::listLogs(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    int limit = 100;
    try { limit = std::stoi(req->getParameter("limit")); } catch (...) {}
    try { respondJson(services::AdminManageService::listLogs(limit), callback); }
    catch (const std::exception &e) { respondError(e, "读取日志失败。", drogon::k500InternalServerError, callback); }
}
