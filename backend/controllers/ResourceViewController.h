#pragma once

#include <drogon/HttpController.h>

namespace api
{
class ResourceViewController : public drogon::HttpController<ResourceViewController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ResourceViewController::record, "/api/resource/view",
                  drogon::Post);
    METHOD_LIST_END

    void record(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
