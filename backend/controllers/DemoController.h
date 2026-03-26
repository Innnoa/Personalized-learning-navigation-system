#pragma once

#include <drogon/HttpController.h>

namespace api
{
class DemoController : public drogon::HttpController<DemoController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(DemoController::reset, "/api/demo/reset", drogon::Post);
    METHOD_LIST_END

    void reset(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
