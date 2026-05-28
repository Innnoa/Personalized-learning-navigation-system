#include "TeacherStudentController.h"

#include "services/TeacherStudentService.h"

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
    if (jsonPtr)
    {
        return *jsonPtr;
    }
    std::string body = std::string(req->getBody());
    Json::CharReaderBuilder builder;
    Json::Value parsed;
    std::string errors;
    std::istringstream stream(body);
    if (!Json::parseFromStream(builder, stream, &parsed, &errors))
    {
        throw std::invalid_argument("请求体 JSON 解析失败: " + errors);
    }
    return parsed;
}
}

void TeacherStudentController::listStudents(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");
    const auto major = req->getParameter("major");
    const auto gradeLabel = req->getParameter("grade_label");

    try
    {
        respondJson(
            services::TeacherStudentService::listStudents(
                username, courseCode, major, gradeLabel),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取学生列表失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::courseStats(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherStudentService::buildCourseStats(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取课程统计失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::knowledgeStats(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherStudentService::buildKnowledgeStats(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取知识点统计失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::studentProgress(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &learnerCode) const
{
    const auto username = req->getParameter("username");
    const auto courseCode = req->getParameter("courseCode");

    try
    {
        respondJson(
            services::TeacherStudentService::buildStudentProgress(
                username, courseCode, learnerCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取学生进度失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::addStudent(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherStudentService::addStudent(username, courseCode, body),
            callback,
            drogon::k201Created);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "新增学生失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherStudentController::availableStudents(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");
    const auto major = req->getParameter("major");
    const auto gradeLabel = req->getParameter("grade_label");

    try
    {
        respondJson(
            services::TeacherStudentService::listAvailableStudents(
                username, courseCode, major, gradeLabel),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取可选学生列表失败。",
                     drogon::k500InternalServerError, callback);
    }
}
