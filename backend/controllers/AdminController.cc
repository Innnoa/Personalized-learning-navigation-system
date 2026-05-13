#include "AdminController.h"

#include "services/AdminDashboardService.h"

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
}

void AdminController::listUsers(
    const drogon::HttpRequestPtr &,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try
    {
        respondJson(services::AdminDashboardService::listUsers(), callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "管理员用户列表读取失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void AdminController::listCourses(
    const drogon::HttpRequestPtr &,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try
    {
        respondJson(services::AdminDashboardService::listCourses(), callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "管理员课程列表读取失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void AdminController::listCourseAssignments(
    const drogon::HttpRequestPtr &,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try
    {
        respondJson(services::AdminDashboardService::listCourseAssignments(),
                    callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "管理员授课分配列表读取失败。",
                     drogon::k500InternalServerError, callback);
    }
}
