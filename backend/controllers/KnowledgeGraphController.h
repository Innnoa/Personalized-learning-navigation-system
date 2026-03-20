#pragma once

#include <drogon/HttpController.h>

namespace api
{
class KnowledgeGraphController
    : public drogon::HttpController<KnowledgeGraphController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(KnowledgeGraphController::get, "/api/knowledge/graph",
                  drogon::Get);
    METHOD_LIST_END

    void get(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
