#pragma once

#include <json/json.h>

namespace services
{
class PathPlanningService
{
  public:
    static Json::Value buildPathPayload(const Json::Value &requestJson);
    static Json::Value buildDetailPathPayload(const Json::Value &requestJson);
    static Json::Value buildAdjustedDetailPathPayload(
        const Json::Value &requestJson);
    static Json::Value buildAdjustedPathPayload(const Json::Value &requestJson);
};
}
