#pragma once

#include <json/json.h>

namespace services
{
class ResourceViewService
{
  public:
    static Json::Value recordResourceView(const Json::Value &requestJson);
};
}
