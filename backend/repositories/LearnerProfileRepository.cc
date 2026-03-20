#include "repositories/LearnerProfileRepository.h"

#include <drogon/drogon.h>

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
}

namespace repositories
{
std::optional<LearnerRecord> LearnerProfileRepository::findLearnerByCode(
    const std::string &learnerCode)
{
    const auto result = getClient()->execSqlSync(
        "select l.id, l.code, l.name, l.major, l.grade_label, "
        "l.target_course_id, c.code as target_course_code, "
        "c.name as target_course_name, l.note "
        "from learners l "
        "join courses c on c.id = l.target_course_id "
        "where l.code = ? limit 1",
        learnerCode);

    if (result.empty())
    {
        return std::nullopt;
    }

    const auto &row = result.front();
    return LearnerRecord{
        row["id"].as<int>(),
        row["code"].as<std::string>(),
        row["name"].as<std::string>(),
        row["major"].as<std::string>(),
        row["grade_label"].as<std::string>(),
        row["target_course_id"].as<int>(),
        row["target_course_code"].as<std::string>(),
        row["target_course_name"].as<std::string>(),
        row["note"].as<std::string>()};
}

std::vector<LearnerMasteryRecord>
LearnerProfileRepository::listMasteryByLearnerAndCourseId(int learnerId,
                                                          int courseId)
{
    const auto result = getClient()->execSqlSync(
        "select kp.id as knowledge_point_id, kp.code, kp.name, kp.chapter_no, "
        "kp.chapter_name, coalesce(lm.mastery_score, 0.0) as mastery_score "
        "from knowledge_points kp "
        "left join learner_mastery lm "
        "on lm.knowledge_point_id = kp.id and lm.learner_id = ? "
        "where kp.course_id = ? "
        "order by kp.display_order asc, kp.id asc",
        learnerId, courseId);

    std::vector<LearnerMasteryRecord> masteryRecords;
    masteryRecords.reserve(result.size());

    for (const auto &row : result)
    {
        masteryRecords.push_back(LearnerMasteryRecord{
            row["knowledge_point_id"].as<int>(),
            row["code"].as<std::string>(),
            row["name"].as<std::string>(),
            row["chapter_no"].as<int>(),
            row["chapter_name"].as<std::string>(),
            row["mastery_score"].as<double>()});
    }

    return masteryRecords;
}

int LearnerProfileRepository::countFeedbackRecordsByLearnerId(int learnerId)
{
    const auto result = getClient()->execSqlSync(
        "select count(1) as total from learning_feedback_records "
        "where learner_id = ?",
        learnerId);

    if (result.empty())
    {
        return 0;
    }

    return result.front()["total"].as<int>();
}

std::vector<LearnerFeedbackRecord>
LearnerProfileRepository::listRecentFeedbackRecordsByLearnerId(
    int learnerId, std::size_t limit)
{
    const auto result = getClient()->execSqlSync(
        "select r.knowledge_point_id, kp.code, kp.name, kp.chapter_no, "
        "kp.chapter_name, r.completion_status, r.self_rated_mastery, "
        "r.previous_mastery, r.updated_mastery, r.rule_applied, r.recorded_at "
        "from learning_feedback_records r "
        "join knowledge_points kp on kp.id = r.knowledge_point_id "
        "where r.learner_id = ? "
        "order by r.recorded_at desc, r.id desc "
        "limit ?",
        learnerId,
        static_cast<long long>(limit));

    std::vector<LearnerFeedbackRecord> feedbackRecords;
    feedbackRecords.reserve(result.size());

    for (const auto &row : result)
    {
        feedbackRecords.push_back(LearnerFeedbackRecord{
            row["knowledge_point_id"].as<int>(),
            row["code"].as<std::string>(),
            row["name"].as<std::string>(),
            row["chapter_no"].as<int>(),
            row["chapter_name"].as<std::string>(),
            row["completion_status"].as<std::string>(),
            row["self_rated_mastery"].as<double>(),
            row["previous_mastery"].as<double>(),
            row["updated_mastery"].as<double>(),
            row["rule_applied"].as<std::string>(),
            row["recorded_at"].as<std::string>()});
    }

    return feedbackRecords;
}
}
