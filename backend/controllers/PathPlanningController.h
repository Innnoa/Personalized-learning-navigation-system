#pragma once

#include <drogon/HttpController.h>

namespace api
{
class PathPlanningController
    : public drogon::HttpController<PathPlanningController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PathPlanningController::generate, "/api/path/generate",
                  drogon::Post);
    ADD_METHOD_TO(PathPlanningController::adjust, "/api/path/adjust",
                  drogon::Post);
    METHOD_LIST_END

    void generate(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    void adjust(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
};
}
