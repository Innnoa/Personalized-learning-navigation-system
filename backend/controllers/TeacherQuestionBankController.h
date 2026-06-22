#pragma once

#include <drogon/HttpController.h>

namespace api
{
class TeacherQuestionBankController : public drogon::HttpController<TeacherQuestionBankController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeacherQuestionBankController::listQuestionBanks,
                  "/api/teacher/courses/{1}/question-banks", drogon::Get);
    ADD_METHOD_TO(TeacherQuestionBankController::createQuestionBank,
                  "/api/teacher/courses/{1}/question-banks", drogon::Post);
    ADD_METHOD_TO(TeacherQuestionBankController::updateQuestionBank,
                  "/api/teacher/courses/{1}/question-banks/{2}", drogon::Put);
    ADD_METHOD_TO(TeacherQuestionBankController::deleteQuestionBank,
                  "/api/teacher/courses/{1}/question-banks/{2}", drogon::Delete);
    ADD_METHOD_TO(TeacherQuestionBankController::listQuestionsByBank,
                  "/api/teacher/courses/{1}/question-banks/{2}/questions", drogon::Get);
    ADD_METHOD_TO(TeacherQuestionBankController::createQuestionByBank,
                  "/api/teacher/courses/{1}/question-banks/{2}/questions", drogon::Post);
    ADD_METHOD_TO(TeacherQuestionBankController::updateQuestionByBank,
                  "/api/teacher/courses/{1}/question-banks/{2}/questions/{3}", drogon::Put);
    ADD_METHOD_TO(TeacherQuestionBankController::deleteQuestionByBank,
                  "/api/teacher/courses/{1}/question-banks/{2}/questions/{3}", drogon::Delete);
    ADD_METHOD_TO(TeacherQuestionBankController::readQuestions,
                  "/api/teacher/courses/{1}/questions", drogon::Get);
    ADD_METHOD_TO(TeacherQuestionBankController::writeQuestions,
                  "/api/teacher/courses/{1}/questions", drogon::Put);
    METHOD_LIST_END

    void listQuestionBanks(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                           const std::string &courseCode) const;
    void createQuestionBank(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            const std::string &courseCode) const;
    void updateQuestionBank(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            const std::string &courseCode,
                            int bankId) const;
    void deleteQuestionBank(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            const std::string &courseCode,
                            int bankId) const;
    void listQuestionsByBank(const drogon::HttpRequestPtr &req,
                             std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                             const std::string &courseCode,
                             int bankId) const;
    void createQuestionByBank(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &courseCode,
                              int bankId) const;
    void updateQuestionByBank(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &courseCode,
                              int bankId,
                              int questionId) const;
    void deleteQuestionByBank(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &courseCode,
                              int bankId,
                              int questionId) const;

    void readQuestions(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       const std::string &courseCode) const;
    void writeQuestions(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                        const std::string &courseCode) const;
};
}
