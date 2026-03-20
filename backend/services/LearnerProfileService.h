#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class LearnerProfileService
{
  public:
    static Json::Value buildProfilePayload(const std::string &learnerCode = "");
};
}
