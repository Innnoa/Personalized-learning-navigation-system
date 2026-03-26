#pragma once

#include <json/json.h>

namespace services
{
class DemoResetService
{
  public:
    static Json::Value resetDemoState(const Json::Value &requestJson);
};
}
