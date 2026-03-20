#pragma once

#include <drogon/HttpController.h>

namespace api
{
class LearnerProfileController
    : public drogon::HttpController<LearnerProfileController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(LearnerProfileController::get, "/api/learner/profile",
                  drogon::Get);
    METHOD_LIST_END

    void get(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
