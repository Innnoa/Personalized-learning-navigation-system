#include "ResourceViewController.h"

#include "services/ResourceViewService.h"

using namespace api;

void ResourceViewController::record(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
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
        return;
    }

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::ResourceViewService::recordResourceView(*requestJson));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "资源行为记录请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "资源行为记录保存失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}
