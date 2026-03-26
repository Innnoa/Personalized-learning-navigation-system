#include "DemoController.h"

#include "services/DemoResetService.h"

using namespace api;

void DemoController::reset(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto requestJson = req->getJsonObject();
    Json::Value emptyRequest(Json::objectValue);

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::DemoResetService::resetDemoState(
                requestJson ? *requestJson : emptyRequest));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "演示重置请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "演示初始状态恢复失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}
