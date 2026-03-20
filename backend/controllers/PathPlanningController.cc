#include "PathPlanningController.h"

#include "services/PathPlanningService.h"

using namespace api;

namespace
{
const Json::Value *readRequestJsonOrRespond(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &callback)
{
    const auto requestJson = req->getJsonObject();
    if (!requestJson)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "请求体必须是 JSON。";

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
        return nullptr;
    }

    return requestJson.get();
}

void respondPathError(const std::exception &error, const std::string &message,
                      const drogon::HttpStatusCode statusCode,
                      std::function<void(const drogon::HttpResponsePtr &)> &callback)
{
    Json::Value payload;
    payload["status"] = "error";
    payload["message"] = message;
    payload["detail"] = error.what();

    auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
    response->setStatusCode(statusCode);
    callback(response);
}
}

void PathPlanningController::generate(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    auto requestJson = readRequestJsonOrRespond(req, callback);
    if (!requestJson)
    {
        return;
    }

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::PathPlanningService::buildPathPayload(*requestJson));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        respondPathError(error, "学习路径请求参数不合法。", drogon::k400BadRequest,
                         callback);
    }
    catch (const std::exception &error)
    {
        respondPathError(error, "学习路径生成失败。",
                         drogon::k500InternalServerError, callback);
    }
}

void PathPlanningController::generateDetail(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    auto requestJson = readRequestJsonOrRespond(req, callback);
    if (!requestJson)
    {
        return;
    }

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::PathPlanningService::buildDetailPathPayload(*requestJson));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        respondPathError(error, "细化学习路径请求参数不合法。",
                         drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondPathError(error, "细化学习路径生成失败。",
                         drogon::k500InternalServerError, callback);
    }
}

void PathPlanningController::adjustDetail(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    auto requestJson = readRequestJsonOrRespond(req, callback);
    if (!requestJson)
    {
        return;
    }

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::PathPlanningService::buildAdjustedDetailPathPayload(
                *requestJson));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        respondPathError(error, "细化路径调整请求参数不合法。",
                         drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondPathError(error, "细化路径调整失败。",
                         drogon::k500InternalServerError, callback);
    }
}

void PathPlanningController::adjust(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    auto requestJson = readRequestJsonOrRespond(req, callback);
    if (!requestJson)
    {
        return;
    }

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::PathPlanningService::buildAdjustedPathPayload(
                *requestJson));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        respondPathError(error, "路径调整请求参数不合法。", drogon::k400BadRequest,
                         callback);
    }
    catch (const std::exception &error)
    {
        respondPathError(error, "路径调整失败。",
                         drogon::k500InternalServerError, callback);
    }
}
