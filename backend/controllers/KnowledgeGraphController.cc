#include "KnowledgeGraphController.h"

#include "services/KnowledgeGraphService.h"

using namespace api;

void KnowledgeGraphController::get(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    try
    {
        const auto scopeCode = req->getParameter("scopeCode");
        const auto courseCode = req->getParameter("courseCode");
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::KnowledgeGraphService::buildGraphPayload(scopeCode, courseCode));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "知识图谱请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "知识图谱加载失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}
