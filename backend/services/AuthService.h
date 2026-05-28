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
    static Json::Value registerStudent(const std::string &username,
                                       const std::string &password,
                                       const std::string &displayName,
                                       const std::string &major,
                                       const std::string &gradeLabel);
    static Json::Value registerTeacher(const std::string &username,
                                       const std::string &password,
                                       const std::string &displayName,
                                       const std::string &employeeCode);
    static Json::Value buildSessionPayload(const std::string &username);
  private:
    static std::string hashPassword(const std::string &password);
};
}
