#pragma once

#include <json/json.h>

namespace services
{
class SystemStatusService
{
  public:
    static Json::Value buildHealthPayload();
};
}
