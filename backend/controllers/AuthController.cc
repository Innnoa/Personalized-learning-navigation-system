#include "AuthController.h"

#include "services/AdminManageService.h"
#include "services/AuthService.h"

#include <stdexcept>

using namespace api;

namespace
{
const char kSessionUsernameKey[] = "username";

Json::Value buildUnauthenticatedPayload()
{
    Json::Value payload;
    payload["authenticated"] = false;
    return payload;
}

void respondJson(const Json::Value &payload,
                 std::function<void(const drogon::HttpResponsePtr &)> &callback,
                 drogon::HttpStatusCode statusCode = drogon::k200OK)
{
    auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
    response->setStatusCode(statusCode);
    callback(response);
}
}

void AuthController::login(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto requestJson = req->getJsonObject();
    if (!requestJson)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "请求体必须是 JSON。";
        respondJson(payload, callback, drogon::k400BadRequest);
        return;
    }

    try
    {
        const auto username = (*requestJson)["username"].asString();
        const auto password = (*requestJson)["password"].asString();
        auto payload = services::AuthService::login(username, password);
        req->session()->insert(kSessionUsernameKey, username);
        services::AdminManageService::logOperation(username, "登录", username, "");
        respondJson(payload, callback);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "登录失败。";
        payload["detail"] = error.what();
        respondJson(payload, callback, drogon::k400BadRequest);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "登录请求处理失败。";
        payload["detail"] = error.what();
        respondJson(payload, callback, drogon::k500InternalServerError);
    }
}

void AuthController::logout(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    req->session()->clear();
    respondJson(buildUnauthenticatedPayload(), callback);
}

void AuthController::session(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto username = req->session()->getOptional<std::string>(kSessionUsernameKey);
    if (!username.has_value() || username->empty())
    {
        respondJson(buildUnauthenticatedPayload(), callback);
        return;
    }

    try
    {
        respondJson(services::AuthService::buildSessionPayload(*username), callback);
    }
    catch (const std::exception &)
    {
        req->session()->clear();
        respondJson(buildUnauthenticatedPayload(), callback);
    }
}

void AuthController::registerStudent(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto requestJson = req->getJsonObject();
    if (!requestJson)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "请求体必须是 JSON。";
        respondJson(payload, callback, drogon::k400BadRequest);
        return;
    }

    try
    {
        const auto role = (*requestJson).get("role", "student").asString();
        const auto username = (*requestJson)["username"].asString();
        const auto password = (*requestJson)["password"].asString();
        const auto displayName = (*requestJson)["displayName"].asString();

        Json::Value payload;
        if (role == "teacher")
        {
            const auto employeeCode = (*requestJson).get("employeeCode", "").asString();
            payload = services::AuthService::registerTeacher(
                username, password, displayName, employeeCode);
        }
        else
        {
            const auto major = (*requestJson)["major"].asString();
            const auto gradeLabel = (*requestJson)["gradeLabel"].asString();
            payload = services::AuthService::registerStudent(
                username, password, displayName, major, gradeLabel);
        }

        req->session()->insert(kSessionUsernameKey, username);
        services::AdminManageService::logOperation(username, "注册", username, role);
        respondJson(payload, callback, drogon::k201Created);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "注册失败。";
        payload["detail"] = error.what();
        respondJson(payload, callback, drogon::k400BadRequest);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "注册请求处理失败。";
        payload["detail"] = error.what();
        respondJson(payload, callback, drogon::k500InternalServerError);
    }
}
