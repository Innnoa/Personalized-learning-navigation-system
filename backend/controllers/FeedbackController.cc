#include "FeedbackController.h"

#include "services/FeedbackService.h"
#include "services/TeacherQuestionBankService.h"

using namespace api;

void FeedbackController::submit(
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
            services::FeedbackService::submitFeedback(*requestJson));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "学习反馈请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "学习反馈保存失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}

void FeedbackController::rollback(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto requestJson = req->getJsonObject();
    Json::Value emptyRequest(Json::objectValue);

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::FeedbackService::rollbackLatestFeedback(
                requestJson ? *requestJson : emptyRequest));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "学习反馈回退请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "学习反馈回退失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}

void FeedbackController::practiceQuestions(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    const auto targetCode = req->getParameter("targetCode");
    const auto previousMasteryPercent = req->getOptionalParameter<int>("previousMasteryPercent").value_or(0);

    try
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(
            services::TeacherQuestionBankService::listPracticeQuestions(
                targetCode, previousMasteryPercent));
        callback(response);
    }
    catch (const std::invalid_argument &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "练习题请求不合法。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k400BadRequest);
        callback(response);
    }
    catch (const std::exception &error)
    {
        Json::Value payload;
        payload["status"] = "error";
        payload["message"] = "练习题读取失败。";
        payload["detail"] = error.what();

        auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
        response->setStatusCode(drogon::k500InternalServerError);
        callback(response);
    }
}
