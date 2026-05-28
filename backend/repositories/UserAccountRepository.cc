#include "repositories/UserAccountRepository.h"

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
std::optional<UserAccountRecord> UserAccountRepository::findUserByUsername(
    const std::string &username)
{
    const auto result = getClient()->execSqlSync(
        "select id, username, password_hash, display_name, status "
        "from users where username = ? limit 1",
        username);

    if (result.empty())
    {
        return std::nullopt;
    }

    const auto &row = result.front();
    return UserAccountRecord{row["id"].as<int>(),
                             row["username"].as<std::string>(),
                             row["password_hash"].as<std::string>(),
                             row["display_name"].as<std::string>(),
                             row["status"].as<std::string>()};
}

std::vector<UserRoleRecord> UserAccountRepository::listRolesForUser(int userId)
{
    const auto result = getClient()->execSqlSync(
        "select role_code from user_roles where user_id = ? order by id asc",
        userId);

    std::vector<UserRoleRecord> roles;
    roles.reserve(result.size());

    for (const auto &row : result)
    {
        roles.push_back(UserRoleRecord{row["role_code"].as<std::string>()});
    }

    return roles;
}

std::optional<UserLearnerLinkRecord>
UserAccountRepository::findLearnerLinkForUser(int userId)
{
    const auto result = getClient()->execSqlSync(
        "select l.id as learner_id, l.code as learner_code, l.name as learner_name "
        "from user_learner_links ull "
        "join learners l on l.id = ull.learner_id "
        "where ull.user_id = ? limit 1",
        userId);

    if (result.empty())
    {
        return std::nullopt;
    }

    const auto &row = result.front();
    return UserLearnerLinkRecord{row["learner_id"].as<int>(),
                                 row["learner_code"].as<std::string>(),
                                 row["learner_name"].as<std::string>()};
}

std::vector<UserLearnerLinkRecord>
UserAccountRepository::findAllLearnerLinksForUser(int userId)
{
    const auto result = getClient()->execSqlSync(
        "select l.id as learner_id, l.code as learner_code, l.name as learner_name, "
        "c.code as course_code, c.name as course_name "
        "from user_learner_links ull "
        "join learners l on l.id = ull.learner_id "
        "join courses c on c.id = l.target_course_id "
        "where ull.user_id = ?", userId);

    std::vector<UserLearnerLinkRecord> links;
    for (const auto &row : result)
    {
        UserLearnerLinkRecord link;
        link.learnerId = row["learner_id"].as<int>();
        link.learnerCode = row["learner_code"].as<std::string>();
        link.learnerName = row["learner_name"].as<std::string>();
        link.courseCode = row["course_code"].as<std::string>();
        link.courseName = row["course_name"].as<std::string>();
        links.push_back(link);
    }
    return links;
}
}
