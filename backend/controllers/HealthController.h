#pragma once

#include <drogon/HttpController.h>

namespace api
{
class HealthController : public drogon::HttpController<HealthController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthController::get, "/api/health", drogon::Get);
    METHOD_LIST_END

    void get(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
