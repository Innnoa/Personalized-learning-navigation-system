#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class TeacherQuestionBankService
{
  public:
    static Json::Value readQuestions(const std::string &username,
                                     const std::string &courseCode);
    static Json::Value writeQuestions(const std::string &username,
                                      const std::string &courseCode,
                                      const Json::Value &body);
};
}
