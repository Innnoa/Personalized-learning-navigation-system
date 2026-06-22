#include <drogon/HttpClient.h>
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/TeacherQuestionBankService.h"

#include <stdexcept>
#include <string>

namespace
{
bool approxEqual(double left, double right, double epsilon = 1e-9)
{
    return std::abs(left - right) <= epsilon;
}

constexpr uint16_t kTeacherQuestionBankTestPort = 18991;

drogon::orm::DbClientPtr getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 测试数据库连接。");
    }
    return client;
}

std::pair<drogon::ReqResult, drogon::HttpResponsePtr> sendRequest(
    const drogon::HttpClientPtr &client,
    const drogon::HttpRequestPtr &request)
{
    return client->sendRequest(request, 5.0);
}

void cleanupQuestionBankCode(const std::string &bankCode)
{
    getClient()->execSqlSync(
        "delete from question_banks where bank_code = ?",
        bankCode);
}
}

DROGON_TEST(TeacherQuestionBankServiceCreatesBankAndMultiTypeQuestions)
{
    cleanupQuestionBankCode("ds-intro-crud-bank");

    Json::Value bankBody;
    bankBody["knowledgePointCode"] = "ds-intro";
    bankBody["bankCode"] = "ds-intro-crud-bank";
    bankBody["bankName"] = "绪论测试题库";
    bankBody["description"] = "用于题库 CRUD 测试。";

    const auto createdBank =
        services::TeacherQuestionBankService::createQuestionBank(
            "teacher_demo", "data-structures", bankBody);
    REQUIRE(createdBank["created"].asBool() == true);
    const auto bankId = createdBank["questionBank"]["id"].asInt();

    Json::Value questionBody;
    questionBody["questionCode"] = "intro-multi-1";
    questionBody["questionType"] = "multiple_choice";
    questionBody["difficultyLevel"] = 3;
    questionBody["prompt"] = "以下哪些属于数据结构研究内容？";
    questionBody["options"] = Json::arrayValue;
    questionBody["options"].append("逻辑结构");
    questionBody["options"].append("存储结构");
    questionBody["options"].append("天气变化");
    questionBody["options"].append("运算");
    Json::Value answer;
    Json::Value indexes(Json::arrayValue);
    indexes.append(0);
    indexes.append(1);
    indexes.append(3);
    answer["correctIndexes"] = indexes;
    questionBody["answer"] = answer;
    questionBody["explanation"] = "数据结构研究逻辑结构、存储结构和运算。";
    questionBody["estimatedSeconds"] = 90;
    questionBody["importanceWeight"] = 1.6;
    questionBody["minMastery"] = 0.10;
    questionBody["maxMastery"] = 0.55;

    const auto createdQuestion =
        services::TeacherQuestionBankService::createQuestion(
            "teacher_demo", "data-structures", bankId, questionBody);
    REQUIRE(createdQuestion["created"].asBool() == true);
    CHECK(approxEqual(createdQuestion["question"]["importanceWeight"].asDouble(), 1.6));
    CHECK(approxEqual(createdQuestion["question"]["minMastery"].asDouble(), 0.10));
    CHECK(approxEqual(createdQuestion["question"]["maxMastery"].asDouble(), 0.55));

    const auto questions =
        services::TeacherQuestionBankService::listQuestions(
            "teacher_demo", "data-structures", bankId, Json::Value(Json::objectValue));
    REQUIRE(questions["questions"].isArray());
    REQUIRE(questions["questions"].size() >= 1U);
    CHECK(approxEqual(questions["questions"][0]["importanceWeight"].asDouble(), 1.6));
    CHECK(questions["questions"][0]["estimatedSeconds"].asInt() == 90);

    const auto questionId = questions["questions"][0]["id"].asInt();

    Json::Value updateBankBody;
    updateBankBody["bankName"] = "绪论测试题库（已更新）";
    const auto updatedBank =
        services::TeacherQuestionBankService::updateQuestionBank(
            "teacher_demo", "data-structures", bankId, updateBankBody);
    CHECK(updatedBank["updated"].asBool() == true);

    Json::Value updateQuestionBody;
    updateQuestionBody["prompt"] = "以下哪些属于数据结构的核心研究内容？";
    updateQuestionBody["importanceWeight"] = 2.1;
    updateQuestionBody["estimatedSeconds"] = 45;
    updateQuestionBody["minMastery"] = 0.35;
    updateQuestionBody["maxMastery"] = 0.90;
    const auto updatedQuestion =
        services::TeacherQuestionBankService::updateQuestion(
            "teacher_demo", "data-structures", bankId, questionId, updateQuestionBody);
    CHECK(updatedQuestion["updated"].asBool() == true);
    CHECK(approxEqual(updatedQuestion["question"]["importanceWeight"].asDouble(), 2.1));
    CHECK(updatedQuestion["question"]["estimatedSeconds"].asInt() == 45);
    CHECK(approxEqual(updatedQuestion["question"]["minMastery"].asDouble(), 0.35));
    CHECK(approxEqual(updatedQuestion["question"]["maxMastery"].asDouble(), 0.90));

    const auto deletedQuestion =
        services::TeacherQuestionBankService::deleteQuestion(
            "teacher_demo", "data-structures", bankId, questionId);
    CHECK(deletedQuestion["deleted"].asBool() == true);

    const auto deletedBank =
        services::TeacherQuestionBankService::deleteQuestionBank(
            "teacher_demo", "data-structures", bankId);
    CHECK(deletedBank["deleted"].asBool() == true);

    cleanupQuestionBankCode("ds-intro-crud-bank");
}

DROGON_TEST(TeacherQuestionBankHttpReadsQuestionBanks)
{
    auto client = drogon::HttpClient::newHttpClient("127.0.0.1", kTeacherQuestionBankTestPort);

    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(drogon::Get);
    request->setPath("/api/teacher/courses/data-structures/question-banks?username=teacher_demo");

    const auto [result, response] = sendRequest(client, request);

    REQUIRE(result == drogon::ReqResult::Ok);
    REQUIRE(response != nullptr);
    CHECK(response->getStatusCode() == drogon::k200OK);
}

DROGON_TEST(TeacherQuestionBankServiceListsPracticeQuestionsForStudentFlow)
{
    cleanupQuestionBankCode("queue-practice-flow-bank");

    Json::Value bankBody;
    bankBody["knowledgePointCode"] = "queue";
    bankBody["bankCode"] = "queue-practice-flow-bank";
    bankBody["bankName"] = "队列练习题库";

    const auto createdBank =
        services::TeacherQuestionBankService::createQuestionBank(
            "teacher_demo", "data-structures", bankBody);
    REQUIRE(createdBank["created"].asBool() == true);
    const auto bankId = createdBank["questionBank"]["id"].asInt();

    Json::Value questionBody;
    questionBody["questionCode"] = "queue-practice-1";
    questionBody["questionType"] = "single_choice";
    questionBody["difficultyLevel"] = 2;
    questionBody["importanceWeight"] = 1.4;
    questionBody["prompt"] = "队列最典型的访问顺序是什么？";
    questionBody["options"] = Json::arrayValue;
    questionBody["options"].append("后进先出");
    questionBody["options"].append("先进先出");
    questionBody["options"].append("随机访问");
    questionBody["options"].append("双端访问");
    Json::Value answer;
    Json::Value indexes(Json::arrayValue);
    indexes.append(1);
    answer["correctIndexes"] = indexes;
    questionBody["answer"] = answer;
    questionBody["explanation"] = "队列遵循 FIFO。";
    questionBody["estimatedSeconds"] = 50;
    questionBody["minMastery"] = 0.0;
    questionBody["maxMastery"] = 0.6;

    const auto createdQuestion =
        services::TeacherQuestionBankService::createQuestion(
            "teacher_demo", "data-structures", bankId, questionBody);
    REQUIRE(createdQuestion["created"].asBool() == true);

    const auto payload =
        services::TeacherQuestionBankService::listPracticeQuestions("queue", 20);

    REQUIRE(payload["questions"].isArray());
    CHECK(payload["questions"].size() >= 1U);
    if (!payload["questions"].empty())
    {
        CHECK(payload["questions"][0].isMember("prompt"));
        CHECK(payload["questions"][0].isMember("correctIndex"));
    }

    cleanupQuestionBankCode("queue-practice-flow-bank");
}

DROGON_TEST(TeacherQuestionBankServicePrioritizesPracticeQuestionsByWeightMasteryDifficultyAndTime)
{
    cleanupQuestionBankCode("queue-practice-priority-bank");

    Json::Value bankBody;
    bankBody["knowledgePointCode"] = "queue";
    bankBody["bankCode"] = "queue-practice-priority-bank";
    bankBody["bankName"] = "队列练习排序测试题库";

    const auto createdBank =
        services::TeacherQuestionBankService::createQuestionBank(
            "teacher_demo", "data-structures", bankBody);
    REQUIRE(createdBank["created"].asBool() == true);
    const auto bankId = createdBank["questionBank"]["id"].asInt();

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    const auto createQuestion = [&](const std::string &code,
                                    const std::string &prompt,
                                    int difficultyLevel,
                                    int estimatedSeconds,
                                    double importanceWeight,
                                    double minMastery,
                                    double maxMastery) {
        Json::Value questionBody;
        questionBody["questionCode"] = code;
        questionBody["questionType"] = "single_choice";
        questionBody["difficultyLevel"] = difficultyLevel;
        questionBody["prompt"] = prompt;
        questionBody["options"] = Json::arrayValue;
        questionBody["options"].append("A");
        questionBody["options"].append("B");
        questionBody["options"].append("C");
        questionBody["options"].append("D");
        Json::Value answer;
        Json::Value indexes(Json::arrayValue);
        indexes.append(0);
        answer["correctIndexes"] = indexes;
        questionBody["answer"] = answer;
        questionBody["estimatedSeconds"] = estimatedSeconds;
        questionBody["importanceWeight"] = importanceWeight;
        questionBody["minMastery"] = minMastery;
        questionBody["maxMastery"] = maxMastery;

        const auto created =
            services::TeacherQuestionBankService::createQuestion(
                "teacher_demo", "data-structures", bankId, questionBody);
        REQUIRE(created["created"].asBool() == true);
    };

    createQuestion("queue-fit-1",
                   "高权重且适合低掌握度的队列题 1",
                   2,
                   35,
                   2.8,
                   0.0,
                   0.4);
    createQuestion("queue-fit-2",
                   "高权重且适合低掌握度的队列题 2",
                   1,
                   30,
                   2.2,
                   0.0,
                   0.4);
    createQuestion("queue-too-hard",
                   "超出当前掌握度区间的高阶队列题",
                   5,
                   40,
                   5.0,
                   0.75,
                   1.0);
    createQuestion("queue-too-long",
                   "时间成本过高的队列题",
                   2,
                   420,
                   4.0,
                   0.0,
                   0.5);

    const auto payload =
        services::TeacherQuestionBankService::listPracticeQuestions("queue", 20);

    REQUIRE(payload["questions"].isArray());
    REQUIRE(payload["questions"].size() >= 2U);
    CHECK(payload["questions"][0]["id"].asString() == "queue-fit-1");
    CHECK(payload["questions"][1]["id"].asString() == "queue-fit-2");

    for (const auto &question : payload["questions"])
    {
        CHECK(question["id"].asString() != "queue-too-hard");
    }

    cleanupQuestionBankCode("queue-practice-priority-bank");
}
