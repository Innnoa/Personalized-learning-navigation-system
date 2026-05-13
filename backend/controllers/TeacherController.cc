#include "TeacherController.h"

#include "services/TeacherDashboardService.h"

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

void TeacherController::listCourses(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(services::TeacherDashboardService::listCourses(username), callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "教师课程列表请求不合法。", drogon::k400BadRequest,
                     callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "教师课程列表读取失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherController::courseOverview(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherDashboardService::buildCourseOverview(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "教师课程概览请求不合法。", drogon::k400BadRequest,
                     callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "教师课程概览读取失败。",
                     drogon::k500InternalServerError, callback);
    }
}
