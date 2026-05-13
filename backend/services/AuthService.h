#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class AuthService
{
  public:
    static Json::Value login(const std::string &username,
                             const std::string &password);
    static Json::Value buildSessionPayload(const std::string &username);
};
}
