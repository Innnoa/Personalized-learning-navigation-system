#pragma once

#include <drogon/HttpController.h>

namespace api
{
class FeedbackController : public drogon::HttpController<FeedbackController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FeedbackController::submit, "/api/feedback/submit",
                  drogon::Post);
    ADD_METHOD_TO(FeedbackController::rollback, "/api/feedback/rollback",
                  drogon::Post);
    ADD_METHOD_TO(FeedbackController::practiceQuestions, "/api/feedback/practice-questions",
                  drogon::Get);
    METHOD_LIST_END

    void submit(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void rollback(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void practiceQuestions(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
