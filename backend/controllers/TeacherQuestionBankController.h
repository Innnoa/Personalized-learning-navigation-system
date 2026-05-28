#pragma once

#include <drogon/HttpController.h>

namespace api
{
class TeacherQuestionBankController : public drogon::HttpController<TeacherQuestionBankController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeacherQuestionBankController::readQuestions,
                  "/api/teacher/courses/{1}/questions", drogon::Get);
    ADD_METHOD_TO(TeacherQuestionBankController::writeQuestions,
                  "/api/teacher/courses/{1}/questions", drogon::Put);
    METHOD_LIST_END

    void readQuestions(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       const std::string &courseCode) const;
    void writeQuestions(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                        const std::string &courseCode) const;
};
}
