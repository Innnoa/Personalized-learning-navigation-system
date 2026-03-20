#include "HealthController.h"

#include "services/SystemStatusService.h"

using namespace api;

void HealthController::get(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) const
{
    (void)req;
    auto response = drogon::HttpResponse::newHttpJsonResponse(
        services::SystemStatusService::buildHealthPayload());
    callback(response);
}
