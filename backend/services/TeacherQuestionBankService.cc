#include "services/TeacherQuestionBankService.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>

namespace
{
auto getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 数据库连接。");
    }
    return client;
}

void verifyTeacherCourseAssignment(const std::string &username,
                                   const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select 1 "
        "from teacher_course_assignments tca "
        "join users u on u.id = tca.user_id "
        "join courses c on c.id = tca.course_id "
        "where u.username = ? and c.code = ? limit 1",
        username,
        courseCode);

    if (result.empty())
    {
        throw std::invalid_argument("教师未分配到指定课程。");
    }
}

std::string getQuestionBankFilePath(const std::string &courseCode)
{
    return "../config/question_bank_" + courseCode + ".json";
}

Json::Value readQuestionFile(const std::string &courseCode)
{
    const auto path = getQuestionBankFilePath(courseCode);
    std::ifstream file(path);
    if (!file.is_open())
    {
        Json::Value empty;
        empty["banks"] = Json::arrayValue;
        return empty;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    std::istringstream stream(content);
    if (!Json::parseFromStream(builder, stream, &root, &errors))
    {
        Json::Value empty;
        empty["banks"] = Json::arrayValue;
        return empty;
    }

    return root;
}

void writeQuestionFile(const std::string &courseCode, const Json::Value &root)
{
    const auto path = getQuestionBankFilePath(courseCode);
    std::ofstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("无法写入题库文件: " + path);
    }
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    file << Json::writeString(builder, root);
    file.close();
}

int findCourseId(const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select id from courses where code = ? limit 1", courseCode);
    if (result.empty())
    {
        throw std::invalid_argument("未找到指定课程。");
    }
    return result.front()["id"].as<int>();
}

int findKnowledgePointIdByCode(int courseId, const std::string &code)
{
    const auto result = getClient()->execSqlSync(
        "select id from knowledge_points where code = ? and course_id = ? limit 1",
        code,
        std::to_string(courseId));
    if (result.empty())
    {
        throw std::invalid_argument("未找到指定知识点。");
    }
    return result.front()["id"].as<int>();
}

int mapLegacyDifficulty(const std::string &difficulty)
{
    if (difficulty == "易")
    {
        return 1;
    }
    if (difficulty == "easy")
    {
        return 1;
    }
    if (difficulty == "中")
    {
        return 3;
    }
    if (difficulty == "medium")
    {
        return 3;
    }
    if (difficulty == "难")
    {
        return 5;
    }
    if (difficulty == "hard")
    {
        return 5;
    }
    return 2;
}

std::string normalizeLegacyQuestionType(const std::string &type)
{
    if (type == "单选题")
    {
        return "single_choice";
    }
    if (type == "多选题")
    {
        return "multiple_choice";
    }
    if (type == "判断题")
    {
        return "true_false";
    }
    if (type == "填空题" || type == "简答题")
    {
        return "short_answer";
    }
    if (type == "计算题")
    {
        return "short_answer";
    }
    if (type == "single-choice" || type.empty())
    {
        return "single_choice";
    }
    if (type == "multiple-choice")
    {
        return "multiple_choice";
    }
    if (type == "true-false")
    {
        return "true_false";
    }
    if (type == "short-answer")
    {
        return "short_answer";
    }
    return type;
}

void ensureQuestionBankMigrationForCourse(int courseId, const std::string &courseCode)
{
    (void)courseId;
    (void)courseCode;
}

Json::Value parseJsonObjectField(const std::string &value, Json::ValueType fallbackType)
{
    Json::Value fallback(fallbackType);
    if (value.empty())
    {
        return fallback;
    }

    Json::CharReaderBuilder builder;
    Json::Value parsed;
    std::string errors;
    std::istringstream stream(value);
    if (Json::parseFromStream(builder, stream, &parsed, &errors))
    {
        return parsed;
    }

    return fallback;
}

std::string resolveDifficultyLabel(int difficultyLevel)
{
    if (difficultyLevel <= 1)
    {
        return "easy";
    }
    if (difficultyLevel >= 5)
    {
        return "hard";
    }
    return "medium";
}

double clampMasteryValue(double value)
{
    return std::clamp(value, 0.0, 1.0);
}

std::string normalizeQuestionType(const std::string &type)
{
    static const std::set<std::string> kSupportedTypes = {
        "single_choice",
        "multiple_choice",
        "true_false",
        "short_answer",
    };

    const auto normalized = normalizeLegacyQuestionType(type);
    if (!kSupportedTypes.count(normalized))
    {
        throw std::invalid_argument("不支持的题目类型。");
    }
    return normalized;
}

std::string getStoredQuestionType(const std::string &questionType)
{
    return questionType;
}

std::string resolveQuestionTypeForResponse(const std::string &storedQuestionType,
                                           const std::string &prompt,
                                           const Json::Value &answer)
{
    (void)prompt;
    (void)answer;
    return storedQuestionType;
}

void validateQuestionPayload(const std::string &questionType, const Json::Value &body)
{
    (void)questionType;
    (void)body;
}

Json::Value buildQuestionPayloadRow(const drogon::orm::Row &row)
{
    Json::Value question;
    const auto answer =
        parseJsonObjectField(row["answer_json"].as<std::string>(), Json::objectValue);
    const auto prompt = row["prompt"].as<std::string>();
    question["id"] = row["id"].as<int>();
    question["questionCode"] = row["question_code"].as<std::string>();
    question["questionType"] = resolveQuestionTypeForResponse(
        row["question_type"].as<std::string>(), prompt, answer);
    question["difficultyLevel"] = row["difficulty_level"].as<int>();
    question["importanceWeight"] = row["importance_weight"].as<double>();
    question["prompt"] = prompt;
    question["options"] = parseJsonObjectField(row["options_json"].as<std::string>(), Json::arrayValue);
    question["answer"] = answer;
    question["explanation"] = row["explanation"].as<std::string>();
    question["estimatedSeconds"] = row["estimated_seconds"].as<int>();
    question["minMastery"] = row["min_mastery"].as<double>();
    question["maxMastery"] = row["max_mastery"].as<double>();
    question["status"] = row["status"].as<std::string>();
    return question;
}
}

namespace services
{
Json::Value TeacherQuestionBankService::listQuestionBanks(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &query)
{
    (void)query;
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto result = getClient()->execSqlSync(
        "select qb.id, qb.bank_code, qb.bank_name, qb.description, qb.status, "
        "kp.code as knowledge_point_code, kp.name as knowledge_point_name, "
        "(select count(*) from questions q where q.bank_id = qb.id) as question_count "
        "from question_banks qb "
        "join knowledge_points kp on kp.id = qb.knowledge_point_id "
        "where qb.course_id = ? "
        "order by kp.display_order asc, qb.id asc",
        std::to_string(courseId));

    Json::Value payload;
    payload["courseCode"] = courseCode;
    Json::Value banks(Json::arrayValue);
    for (const auto &row : result)
    {
        Json::Value bank;
        bank["id"] = row["id"].as<int>();
        bank["knowledgePointCode"] = row["knowledge_point_code"].as<std::string>();
        bank["knowledgePointName"] = row["knowledge_point_name"].as<std::string>();
        bank["bankCode"] = row["bank_code"].as<std::string>();
        bank["bankName"] = row["bank_name"].as<std::string>();
        bank["description"] = row["description"].as<std::string>();
        bank["status"] = row["status"].as<std::string>();
        bank["questionCount"] = row["question_count"].as<int>();
        banks.append(bank);
    }
    payload["questionBanks"] = banks;
    return payload;
}

Json::Value TeacherQuestionBankService::createQuestionBank(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto knowledgePointCode = body.get("knowledgePointCode", "").asString();
    const auto bankCode = body.get("bankCode", "").asString();
    const auto bankName = body.get("bankName", "").asString();
    if (knowledgePointCode.empty() || bankCode.empty() || bankName.empty())
    {
        throw std::invalid_argument("知识点代码、题库编码和题库名称不能为空。");
    }

    const auto pointId = findKnowledgePointIdByCode(courseId, knowledgePointCode);

    getClient()->execSqlSync(
        "insert into question_banks "
        "(course_id, knowledge_point_id, bank_code, bank_name, description, status) "
        "values (?, ?, ?, ?, ?, ?)",
        std::to_string(courseId),
        std::to_string(pointId),
        bankCode,
        bankName,
        body.get("description", "").asString(),
        body.get("status", "active").asString());

    const auto result = getClient()->execSqlSync(
        "select qb.id, qb.bank_code, qb.bank_name, qb.description, qb.status, "
        "kp.code as knowledge_point_code, kp.name as knowledge_point_name "
        "from question_banks qb "
        "join knowledge_points kp on kp.id = qb.knowledge_point_id "
        "where qb.id = last_insert_rowid() limit 1");

    Json::Value payload;
    payload["created"] = true;
    payload["questionBank"]["id"] = result.front()["id"].as<int>();
    payload["questionBank"]["knowledgePointCode"] = result.front()["knowledge_point_code"].as<std::string>();
    payload["questionBank"]["knowledgePointName"] = result.front()["knowledge_point_name"].as<std::string>();
    payload["questionBank"]["bankCode"] = result.front()["bank_code"].as<std::string>();
    payload["questionBank"]["bankName"] = result.front()["bank_name"].as<std::string>();
    payload["questionBank"]["description"] = result.front()["description"].as<std::string>();
    payload["questionBank"]["status"] = result.front()["status"].as<std::string>();
    return payload;
}

Json::Value TeacherQuestionBankService::updateQuestionBank(
    const std::string &username,
    const std::string &courseCode,
    int bankId,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto existing = getClient()->execSqlSync(
        "select id from question_banks where id = ? and course_id = ? limit 1",
        std::to_string(bankId),
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定题库。");
    }

    if (body.isMember("knowledgePointCode"))
    {
        const auto pointId = findKnowledgePointIdByCode(
            courseId, body["knowledgePointCode"].asString());
        getClient()->execSqlSync(
            "update question_banks set knowledge_point_id = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(pointId),
            std::to_string(bankId));
    }
    if (body.isMember("bankCode"))
    {
        getClient()->execSqlSync(
            "update question_banks set bank_code = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["bankCode"].asString(),
            std::to_string(bankId));
    }
    if (body.isMember("bankName"))
    {
        getClient()->execSqlSync(
            "update question_banks set bank_name = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["bankName"].asString(),
            std::to_string(bankId));
    }
    if (body.isMember("description"))
    {
        getClient()->execSqlSync(
            "update question_banks set description = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["description"].asString(),
            std::to_string(bankId));
    }
    if (body.isMember("status"))
    {
        getClient()->execSqlSync(
            "update question_banks set status = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["status"].asString(),
            std::to_string(bankId));
    }

    const auto result = getClient()->execSqlSync(
        "select qb.id, qb.bank_code, qb.bank_name, qb.description, qb.status, "
        "kp.code as knowledge_point_code, kp.name as knowledge_point_name "
        "from question_banks qb "
        "join knowledge_points kp on kp.id = qb.knowledge_point_id "
        "where qb.id = ? limit 1",
        std::to_string(bankId));

    Json::Value payload;
    payload["updated"] = true;
    payload["questionBank"]["id"] = result.front()["id"].as<int>();
    payload["questionBank"]["knowledgePointCode"] = result.front()["knowledge_point_code"].as<std::string>();
    payload["questionBank"]["knowledgePointName"] = result.front()["knowledge_point_name"].as<std::string>();
    payload["questionBank"]["bankCode"] = result.front()["bank_code"].as<std::string>();
    payload["questionBank"]["bankName"] = result.front()["bank_name"].as<std::string>();
    payload["questionBank"]["description"] = result.front()["description"].as<std::string>();
    payload["questionBank"]["status"] = result.front()["status"].as<std::string>();
    return payload;
}

Json::Value TeacherQuestionBankService::deleteQuestionBank(
    const std::string &username,
    const std::string &courseCode,
    int bankId)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto existing = getClient()->execSqlSync(
        "select id from question_banks where id = ? and course_id = ? limit 1",
        std::to_string(bankId),
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定题库。");
    }

    getClient()->execSqlSync(
        "delete from question_banks where id = ? and course_id = ?",
        std::to_string(bankId),
        std::to_string(courseId));

    Json::Value payload;
    payload["deleted"] = true;
    return payload;
}

Json::Value TeacherQuestionBankService::listQuestions(
    const std::string &username,
    const std::string &courseCode,
    int bankId,
    const Json::Value &query)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto bankResult = getClient()->execSqlSync(
        "select id, bank_code, bank_name from question_banks "
        "where id = ? and course_id = ? limit 1",
        std::to_string(bankId),
        std::to_string(courseId));
    if (bankResult.empty())
    {
        throw std::invalid_argument("未找到指定题库。");
    }

    const int page = std::max(1, query.get("page", 1).asInt());
    const int pageSize = std::clamp(query.get("pageSize", 50).asInt(), 1, 200);
    const int offset = (page - 1) * pageSize;
    const auto keyword = query.get("keyword", "").asString();
    const auto questionType = query.get("questionType", "").asString();

    const std::string keywordLike = "%" + keyword + "%";
    const auto totalResult = getClient()->execSqlSync(
        "select count(*) as count "
        "from questions "
        "where bank_id = ? "
        "and (? = '' or question_type = ?) "
        "and (? = '' or question_code like ? or prompt like ?)",
        std::to_string(bankId),
        questionType,
        questionType,
        keyword,
        keywordLike,
        keywordLike);

    const auto result = getClient()->execSqlSync(
        "select id, question_code, question_type, difficulty_level, importance_weight, prompt, options_json, "
        "answer_json, explanation, estimated_seconds, min_mastery, max_mastery, display_order, status "
        "from questions "
        "where bank_id = ? "
        "and (? = '' or question_type = ?) "
        "and (? = '' or question_code like ? or prompt like ?) "
        "order by display_order asc, id asc limit ? offset ?",
        std::to_string(bankId),
        questionType,
        questionType,
        keyword,
        keywordLike,
        keywordLike,
        std::to_string(pageSize),
        std::to_string(offset));

    Json::Value payload;
    payload["questionBank"]["id"] = bankId;
    payload["questionBank"]["bankCode"] = bankResult.front()["bank_code"].as<std::string>();
    payload["questionBank"]["bankName"] = bankResult.front()["bank_name"].as<std::string>();
    payload["page"] = page;
    payload["pageSize"] = pageSize;
    payload["totalCount"] = totalResult.front()["count"].as<int>();
    Json::Value questions(Json::arrayValue);
    for (const auto &row : result)
    {
        questions.append(buildQuestionPayloadRow(row));
    }
    payload["questions"] = questions;
    return payload;
}

Json::Value TeacherQuestionBankService::createQuestion(
    const std::string &username,
    const std::string &courseCode,
    int bankId,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto bankResult = getClient()->execSqlSync(
        "select knowledge_point_id from question_banks where id = ? and course_id = ? limit 1",
        std::to_string(bankId),
        std::to_string(courseId));
    if (bankResult.empty())
    {
        throw std::invalid_argument("未找到指定题库。");
    }
    const auto knowledgePointId = bankResult.front()["knowledge_point_id"].as<int>();
    const auto questionType =
        normalizeQuestionType(body.get("questionType", "single_choice").asString());
    const auto storedQuestionType = getStoredQuestionType(questionType);
    validateQuestionPayload(questionType, body);

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    const auto nextOrderResult = getClient()->execSqlSync(
        "select coalesce(max(display_order), -1) + 1 as next_order from questions where bank_id = ?",
        std::to_string(bankId));

    getClient()->execSqlSync(
        "insert into questions "
        "(bank_id, course_id, knowledge_point_id, question_code, question_type, "
        "difficulty_level, importance_weight, prompt, options_json, answer_json, explanation, "
        "estimated_seconds, min_mastery, max_mastery, display_order, status) "
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
        std::to_string(bankId),
        std::to_string(courseId),
        std::to_string(knowledgePointId),
        body.get("questionCode", "").asString(),
        storedQuestionType,
        std::to_string(body.get("difficultyLevel", 1).asInt()),
        std::to_string(std::max(0.0, body.get("importanceWeight", 1.0).asDouble())),
        body.get("prompt", "").asString(),
        Json::writeString(builder, body["options"]),
        Json::writeString(builder, body["answer"]),
        body.get("explanation", "").asString(),
        std::to_string(std::max(1, body.get("estimatedSeconds", 60).asInt())),
        std::to_string(clampMasteryValue(body.get("minMastery", 0.0).asDouble())),
        std::to_string(clampMasteryValue(body.get("maxMastery", 1.0).asDouble())),
        std::to_string(nextOrderResult.front()["next_order"].as<int>()),
        body.get("status", "active").asString());

    const auto created = getClient()->execSqlSync(
        "select id, question_code, question_type, difficulty_level, importance_weight, prompt, options_json, "
        "answer_json, explanation, estimated_seconds, min_mastery, max_mastery, status "
        "from questions where id = last_insert_rowid() limit 1");

    Json::Value payload;
    payload["created"] = true;
    payload["question"] = buildQuestionPayloadRow(created.front());
    return payload;
}

Json::Value TeacherQuestionBankService::updateQuestion(
    const std::string &username,
    const std::string &courseCode,
    int bankId,
    int questionId,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto existing = getClient()->execSqlSync(
        "select q.id from questions q "
        "join question_banks qb on qb.id = q.bank_id "
        "where q.id = ? and q.bank_id = ? and qb.course_id = ? limit 1",
        std::to_string(questionId),
        std::to_string(bankId),
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定题目。");
    }

    const auto currentQuestion = getClient()->execSqlSync(
        "select question_type, prompt, answer_json "
        "from questions where id = ? limit 1",
        std::to_string(questionId));

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    Json::Value validationBody(Json::objectValue);
    validationBody["prompt"] = body.isMember("prompt")
                                   ? body["prompt"]
                                   : Json::Value(currentQuestion.front()["prompt"].as<std::string>());
    validationBody["answer"] = body.isMember("answer")
                                   ? body["answer"]
                                   : parseJsonObjectField(
                                         currentQuestion.front()["answer_json"].as<std::string>(),
                                         Json::objectValue);
    const auto questionType = normalizeQuestionType(
        body.isMember("questionType")
            ? body["questionType"].asString()
            : currentQuestion.front()["question_type"].as<std::string>());
    const auto storedQuestionType = getStoredQuestionType(questionType);
    validateQuestionPayload(questionType, validationBody);

    if (body.isMember("questionCode"))
    {
        getClient()->execSqlSync(
            "update questions set question_code = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["questionCode"].asString(),
            std::to_string(questionId));
    }
    if (body.isMember("questionType"))
    {
        getClient()->execSqlSync(
            "update questions set question_type = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            storedQuestionType,
            std::to_string(questionId));
    }
    if (body.isMember("difficultyLevel"))
    {
        getClient()->execSqlSync(
            "update questions set difficulty_level = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(body["difficultyLevel"].asInt()),
            std::to_string(questionId));
    }
    if (body.isMember("importanceWeight"))
    {
        getClient()->execSqlSync(
            "update questions set importance_weight = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(std::max(0.0, body["importanceWeight"].asDouble())),
            std::to_string(questionId));
    }
    if (body.isMember("prompt"))
    {
        getClient()->execSqlSync(
            "update questions set prompt = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["prompt"].asString(),
            std::to_string(questionId));
    }
    if (body.isMember("options"))
    {
        getClient()->execSqlSync(
            "update questions set options_json = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            Json::writeString(builder, body["options"]),
            std::to_string(questionId));
    }
    if (body.isMember("answer"))
    {
        getClient()->execSqlSync(
            "update questions set answer_json = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            Json::writeString(builder, body["answer"]),
            std::to_string(questionId));
    }
    if (body.isMember("explanation"))
    {
        getClient()->execSqlSync(
            "update questions set explanation = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["explanation"].asString(),
            std::to_string(questionId));
    }
    if (body.isMember("estimatedSeconds"))
    {
        getClient()->execSqlSync(
            "update questions set estimated_seconds = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(std::max(1, body["estimatedSeconds"].asInt())),
            std::to_string(questionId));
    }
    if (body.isMember("minMastery"))
    {
        getClient()->execSqlSync(
            "update questions set min_mastery = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(clampMasteryValue(body["minMastery"].asDouble())),
            std::to_string(questionId));
    }
    if (body.isMember("maxMastery"))
    {
        getClient()->execSqlSync(
            "update questions set max_mastery = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(clampMasteryValue(body["maxMastery"].asDouble())),
            std::to_string(questionId));
    }
    if (body.isMember("status"))
    {
        getClient()->execSqlSync(
            "update questions set status = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["status"].asString(),
            std::to_string(questionId));
    }

    const auto updated = getClient()->execSqlSync(
        "select id, question_code, question_type, difficulty_level, importance_weight, prompt, options_json, "
        "answer_json, explanation, estimated_seconds, min_mastery, max_mastery, status "
        "from questions where id = ? limit 1",
        std::to_string(questionId));

    Json::Value payload;
    payload["updated"] = true;
    payload["question"] = buildQuestionPayloadRow(updated.front());
    return payload;
}

Json::Value TeacherQuestionBankService::deleteQuestion(
    const std::string &username,
    const std::string &courseCode,
    int bankId,
    int questionId)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);
    ensureQuestionBankMigrationForCourse(courseId, courseCode);

    const auto existing = getClient()->execSqlSync(
        "select q.id from questions q "
        "join question_banks qb on qb.id = q.bank_id "
        "where q.id = ? and q.bank_id = ? and qb.course_id = ? limit 1",
        std::to_string(questionId),
        std::to_string(bankId),
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定题目。");
    }

    getClient()->execSqlSync(
        "delete from questions where id = ? and bank_id = ?",
        std::to_string(questionId),
        std::to_string(bankId));

    Json::Value payload;
    payload["deleted"] = true;
    return payload;
}

Json::Value TeacherQuestionBankService::listPracticeQuestions(
    const std::string &targetCode,
    int previousMasteryPercent)
{
    const auto courseResult = getClient()->execSqlSync(
        "select id from courses where code = 'data-structures' limit 1");
    if (courseResult.empty())
    {
        Json::Value payload;
        payload["questions"] = Json::arrayValue;
        return payload;
    }

    const auto courseId = courseResult.front()["id"].as<int>();
    ensureQuestionBankMigrationForCourse(courseId, "data-structures");

    const auto pointResult = getClient()->execSqlSync(
        "select id from knowledge_points where code = ? and course_id = ? limit 1",
        targetCode,
        std::to_string(courseId));
    if (pointResult.empty())
    {
        Json::Value payload;
        payload["questions"] = Json::arrayValue;
        return payload;
    }

    const auto pointId = pointResult.front()["id"].as<int>();
    const double currentMastery =
        clampMasteryValue(static_cast<double>(previousMasteryPercent) / 100.0);
    constexpr int kPracticeBudgetSeconds = 300;
    const auto result = getClient()->execSqlSync(
        "select q.id, q.question_code, q.question_type, q.difficulty_level, q.importance_weight, "
        "q.prompt, q.options_json, q.answer_json, q.explanation, q.estimated_seconds, "
        "q.min_mastery, q.max_mastery "
        "from questions q "
        "join question_banks qb on qb.id = q.bank_id "
        "where qb.knowledge_point_id = ? and qb.status = 'active' and q.status = 'active' "
        "order by q.display_order asc, q.id asc",
        std::to_string(pointId));

    struct PracticeQuestionCandidate
    {
        Json::Value payload;
        int score = 0;
        int estimatedSeconds = 60;
    };

    std::vector<PracticeQuestionCandidate> candidates;
    candidates.reserve(result.size());

    for (const auto &row : result)
    {
        const double minMastery = row["min_mastery"].as<double>();
        const double maxMastery = row["max_mastery"].as<double>();
        if (currentMastery < minMastery || currentMastery > maxMastery)
        {
            continue;
        }

        const int difficultyLevel = row["difficulty_level"].as<int>();
        const int estimatedSeconds = row["estimated_seconds"].as<int>();
        const double importanceWeight = row["importance_weight"].as<double>();

        int preferredDifficulty = 2;
        if (currentMastery >= 0.75)
        {
            preferredDifficulty = 4;
        }
        else if (currentMastery >= 0.45)
        {
            preferredDifficulty = 3;
        }

        const int difficultyScore =
            std::max(0, 120 - std::abs(difficultyLevel - preferredDifficulty) * 35);
        const int timeScore =
            estimatedSeconds <= kPracticeBudgetSeconds
                ? std::max(0, 100 - estimatedSeconds / 6)
                : -240;
        const int importanceScore =
            static_cast<int>(std::round(importanceWeight * 100.0));

        Json::Value question;
        const auto answer =
            parseJsonObjectField(row["answer_json"].as<std::string>(), Json::objectValue);
        const auto prompt = row["prompt"].as<std::string>();
        question["id"] = row["question_code"].as<std::string>();
        question["type"] = resolveQuestionTypeForResponse(
            row["question_type"].as<std::string>(), prompt, answer);
        question["difficulty"] = resolveDifficultyLabel(difficultyLevel);
        question["difficultyLevel"] = difficultyLevel;
        question["importanceWeight"] = importanceWeight;
        question["prompt"] = prompt;
        question["options"] = parseJsonObjectField(row["options_json"].as<std::string>(), Json::arrayValue);
        if (answer.isMember("correctIndexes") && answer["correctIndexes"].isArray() &&
                 !answer["correctIndexes"].empty())
        {
            question["correctIndex"] = answer["correctIndexes"][0].asInt();
        }
        else
        {
            question["correctIndex"] = 0;
        }
        question["explanation"] = row["explanation"].as<std::string>();
        question["estimatedSeconds"] = estimatedSeconds;
        question["minMastery"] = minMastery;
        question["maxMastery"] = maxMastery;

        candidates.push_back(PracticeQuestionCandidate{
            question,
            importanceScore + difficultyScore + timeScore,
            estimatedSeconds,
        });
    }

    std::stable_sort(
        candidates.begin(),
        candidates.end(),
        [](const PracticeQuestionCandidate &left, const PracticeQuestionCandidate &right) {
            if (left.score != right.score)
            {
                return left.score > right.score;
            }
            if (left.estimatedSeconds != right.estimatedSeconds)
            {
                return left.estimatedSeconds < right.estimatedSeconds;
            }
            return left.payload["id"].asString() < right.payload["id"].asString();
        });

    Json::Value payload;
    payload["targetCode"] = targetCode;
    Json::Value questions(Json::arrayValue);
    int usedBudgetSeconds = 0;
    for (const auto &candidate : candidates)
    {
        if (!questions.empty() &&
            usedBudgetSeconds + candidate.estimatedSeconds > kPracticeBudgetSeconds)
        {
            continue;
        }

        usedBudgetSeconds += candidate.estimatedSeconds;
        questions.append(candidate.payload);
        if (questions.size() >= 5)
        {
            break;
        }
    }
    payload["questions"] = questions;
    return payload;
}

Json::Value TeacherQuestionBankService::readQuestions(
    const std::string &username,
    const std::string &courseCode)
{
    verifyTeacherCourseAssignment(username, courseCode);
    return readQuestionFile(courseCode);
}

Json::Value TeacherQuestionBankService::writeQuestions(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    writeQuestionFile(courseCode, body);

    Json::Value payload;
    payload["saved"] = true;
    int count = 0;
    if (body.isMember("banks") && body["banks"].isArray())
    {
        for (const auto &bank : body["banks"])
        {
            if (bank.isMember("questions") && bank["questions"].isArray())
            {
                count += static_cast<int>(bank["questions"].size());
            }
        }
    }
    payload["count"] = count;
    return payload;
}
}
