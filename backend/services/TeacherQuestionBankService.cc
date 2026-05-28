#include "services/TeacherQuestionBankService.h"

#include <drogon/drogon.h>

#include <fstream>
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
}

namespace services
{
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
