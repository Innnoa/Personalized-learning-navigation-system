#include "LearnerProfileController.h"

#include "services/LearnerProfileService.h"

using namespace api;

void LearnerProfileController::get(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try
    {
        const auto learnerCode = req->getParameter("learnerCode");
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::LearnerProfileService::buildProfilePayload(learnerCode));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "学习者画像请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "学习者画像加载失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}
