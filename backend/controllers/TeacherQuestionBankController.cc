#include "TeacherQuestionBankController.h"

#include "services/TeacherQuestionBankService.h"

using namespace api;

namespace
{
void respondJson(const Json::Value &payload,
                 std::function<void(const drogon::HttpResponsePtr &)> &callback,
                 drogon::HttpStatusCode statusCode = drogon::k200OK)
{
    auto response = drogon::HttpResponse::newHttpJsonResponse(payload);
    response->setStatusCode(statusCode);
    callback(response);
}

void respondError(const std::exception &error,
                  const std::string &message,
                  drogon::HttpStatusCode statusCode,
                  std::function<void(const drogon::HttpResponsePtr &)> &callback)
{
    Json::Value payload;
    payload["status"] = "error";
    payload["message"] = message;
    payload["detail"] = error.what();
    respondJson(payload, callback, statusCode);
}

Json::Value parseBody(const drogon::HttpRequestPtr &req)
{
    const auto &jsonPtr = req->getJsonObject();
    if (jsonPtr)
    {
        return *jsonPtr;
    }

    std::string body = std::string(req->getBody());
    if (body.empty())
    {
        return Json::Value(Json::objectValue);
    }

    Json::CharReaderBuilder builder;
    Json::Value parsed;
    std::string errors;
    std::istringstream stream(body);
    if (!Json::parseFromStream(builder, stream, &parsed, &errors))
    {
        throw std::invalid_argument("请求体 JSON 解析失败: " + errors);
    }

    return parsed;
}
}

void TeacherQuestionBankController::listQuestionBanks(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherQuestionBankService::listQuestionBanks(
                username, courseCode, Json::Value(Json::objectValue)),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取题库列表失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::createQuestionBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherQuestionBankService::createQuestionBank(
                username, courseCode, body),
            callback,
            drogon::k201Created);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "创建题库失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::updateQuestionBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int bankId) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherQuestionBankService::updateQuestionBank(
                username, courseCode, bankId, body),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "更新题库失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::deleteQuestionBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int bankId) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherQuestionBankService::deleteQuestionBank(
                username, courseCode, bankId),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "删除题库失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::listQuestionsByBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int bankId) const
{
    const auto username = req->getParameter("username");

    try
    {
        Json::Value query(Json::objectValue);
        query["page"] = req->getOptionalParameter<int>("page").value_or(1);
        query["pageSize"] = req->getOptionalParameter<int>("pageSize").value_or(50);
        query["keyword"] = req->getParameter("keyword");
        query["questionType"] = req->getParameter("questionType");
        respondJson(
            services::TeacherQuestionBankService::listQuestions(
                username, courseCode, bankId, query),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取题目失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::createQuestionByBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int bankId) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherQuestionBankService::createQuestion(
                username, courseCode, bankId, body),
            callback,
            drogon::k201Created);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "创建题目失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::updateQuestionByBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int bankId,
    int questionId) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherQuestionBankService::updateQuestion(
                username, courseCode, bankId, questionId, body),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "更新题目失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::deleteQuestionByBank(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode,
    int bankId,
    int questionId) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherQuestionBankService::deleteQuestion(
                username, courseCode, bankId, questionId),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "删除题目失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::readQuestions(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        respondJson(
            services::TeacherQuestionBankService::readQuestions(username, courseCode),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "读取题库失败。",
                     drogon::k500InternalServerError, callback);
    }
}

void TeacherQuestionBankController::writeQuestions(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    const std::string &courseCode) const
{
    const auto username = req->getParameter("username");

    try
    {
        const auto body = parseBody(req);
        respondJson(
            services::TeacherQuestionBankService::writeQuestions(
                username, courseCode, body),
            callback);
    }
    catch (const std::invalid_argument &error)
    {
        respondError(error, "请求不合法。", drogon::k400BadRequest, callback);
    }
    catch (const std::exception &error)
    {
        respondError(error, "保存题库失败。",
                     drogon::k500InternalServerError, callback);
    }
}
