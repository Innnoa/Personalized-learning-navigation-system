#include "TeacherCourseEditController.h"

#include "services/TeacherCourseEditService.h"

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

std::string readBodyString(const drogon::HttpRequestPtr &req)
{
    const auto &bodyPtr = req->getJsonObject();
    if (bodyPtr)
    {
        return bodyPtr->toStyledString();
    }
    return std::string(req->getBody());
}

Json::Value parseBody(const drogon::HttpRequestPtr &req)
{
    const auto &jsonPtr = req->getJsonObject();
    if (jsonPtr)
    {
        return *jsonPtr;
    }

    std::string body = std::string(req->getBody());
    if (body.empty())
    {
        return Json::Value(Json::objectValue);
    }

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

void TeacherCourseEditController::createKnowledgePoint(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherCourseEditService::createKnowledgePoint(
                username, courseCode, body),
            callback,
            drogon::k201Created);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "创建知识点失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::updateKnowledgePoint(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    const std::string &pointCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherCourseEditService::updateKnowledgePoint(
                username, courseCode, pointCode, body),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "更新知识点失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::deleteKnowledgePoint(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    const std::string &pointCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherCourseEditService::deleteKnowledgePoint(
                username, courseCode, pointCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "删除知识点失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::readDependencies(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherCourseEditService::readDependencies(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取依赖关系失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::updateDependencies(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherCourseEditService::updateDependencies(
                username, courseCode, body),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "更新依赖关系失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::readResources(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherCourseEditService::readResources(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取学习资源失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::createResource(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherCourseEditService::createResource(
                username, courseCode, body),
            callback,
            drogon::k201Created);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "创建学习资源失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::updateResource(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int resourceId) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherCourseEditService::updateResource(
                username, courseCode, resourceId, body),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "更新学习资源失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherCourseEditController::deleteResource(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int resourceId) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherCourseEditService::deleteResource(
                username, courseCode, resourceId),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "删除学习资源失败。",
                     drogon::k500InternalServerError, callback);
    }
}
