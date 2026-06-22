#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class TeacherQuestionBankService
{
  public:
    static Json::Value listQuestionBanks(const std::string &username,
                                         const std::string &courseCode,
                                         const Json::Value &query);
    static Json::Value createQuestionBank(const std::string &username,
                                          const std::string &courseCode,
                                          const Json::Value &body);
    static Json::Value updateQuestionBank(const std::string &username,
                                          const std::string &courseCode,
                                          int bankId,
                                          const Json::Value &body);
    static Json::Value deleteQuestionBank(const std::string &username,
                                          const std::string &courseCode,
                                          int bankId);
    static Json::Value listQuestions(const std::string &username,
                                     const std::string &courseCode,
                                     int bankId,
                                     const Json::Value &query);
    static Json::Value createQuestion(const std::string &username,
                                      const std::string &courseCode,
                                      int bankId,
                                      const Json::Value &body);
    static Json::Value updateQuestion(const std::string &username,
                                      const std::string &courseCode,
                                      int bankId,
                                      int questionId,
                                      const Json::Value &body);
    static Json::Value deleteQuestion(const std::string &username,
                                      const std::string &courseCode,
                                      int bankId,
                                      int questionId);
    static Json::Value listPracticeQuestions(const std::string &targetCode,
                                             int previousMasteryPercent);
    static Json::Value readQuestions(const std::string &username,
                                     const std::string &courseCode);
    static Json::Value writeQuestions(const std::string &username,
                                      const std::string &courseCode,
                                      const Json::Value &body);
};
}
