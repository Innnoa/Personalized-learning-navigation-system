#include "services/AuthService.h"

#include "repositories/UserAccountRepository.h"

#include <drogon/drogon.h>

#include <openssl/sha.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
bool isUserActive(const std::string &status)
{
    return status == "active";
}

Json::Value buildAuthenticatedPayload(
    const repositories::UserAccountRecord &user,
    const std::vector<repositories::UserRoleRecord> &roles)
{
    if (roles.empty())
    {
        throw std::invalid_argument("当前账号未配置角色。");
    }

    Json::Value payload;
    payload["authenticated"] = true;
    payload["user"]["id"] = user.id;
    payload["user"]["username"] = user.username;
    payload["user"]["displayName"] = user.displayName;
    payload["user"]["status"] = user.status;

    Json::Value roleItems(Json::arrayValue);
    for (const auto &role : roles)
    {
        roleItems.append(role.roleCode);
    }

    payload["roles"] = roleItems;
    payload["activeRole"] = roles.front().roleCode;

    if (payload["activeRole"].asString() == "student")
    {
        const auto allLinks =
            repositories::UserAccountRepository::findAllLearnerLinksForUser(user.id);

        Json::Value linkedLearnersArr(Json::arrayValue);
        for (const auto &link : allLinks)
        {
            Json::Value item;
            item["learnerId"] = link.learnerId;
            item["learnerCode"] = link.learnerCode;
            item["learnerName"] = link.learnerName;
            item["courseCode"] = link.courseCode;
            item["courseName"] = link.courseName;
            linkedLearnersArr.append(item);
        }
        payload["linkedLearners"] = linkedLearnersArr;

        if (!allLinks.empty())
        {
            payload["linkedLearner"]["learnerId"] = allLinks.front().learnerId;
            payload["linkedLearner"]["learnerCode"] = allLinks.front().learnerCode;
            payload["linkedLearner"]["learnerName"] = allLinks.front().learnerName;
            payload["linkedLearner"]["courseCode"] = allLinks.front().courseCode;
            payload["linkedLearner"]["courseName"] = allLinks.front().courseName;
        }
        else
        {
            payload["linkedLearner"] = Json::nullValue;
        }
    }
    else
    {
        payload["linkedLearner"] = Json::nullValue;
    }

    return payload;
}

}

namespace services
{
Json::Value AuthService::login(const std::string &username,
                               const std::string &password)
{
    const auto user =
        repositories::UserAccountRepository::findUserByUsername(username);
    if (!user.has_value() || !isUserActive(user->status))
    {
        throw std::invalid_argument("用户名或密码错误。");
    }

    if (user->passwordHash != hashPassword(password))
    {
        throw std::invalid_argument("用户名或密码错误。");
    }

    const auto roles = repositories::UserAccountRepository::listRolesForUser(user->id);
    return buildAuthenticatedPayload(*user, roles);
}

Json::Value AuthService::buildSessionPayload(const std::string &username)
{
    const auto user =
        repositories::UserAccountRepository::findUserByUsername(username);
    if (!user.has_value() || !isUserActive(user->status))
    {
        throw std::invalid_argument("当前会话无效。");
    }

    const auto roles = repositories::UserAccountRepository::listRolesForUser(user->id);
    return buildAuthenticatedPayload(*user, roles);
}

std::string AuthService::hashPassword(const std::string &password)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(password.c_str()),
           password.size(), hash);
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    return oss.str();
}

Json::Value AuthService::registerStudent(const std::string &username,
                                         const std::string &password,
                                         const std::string &displayName,
                                         const std::string &major,
                                         const std::string &gradeLabel)
{
    if (username.empty() || password.empty())
        throw std::invalid_argument("用户名和密码不能为空。");
    if (password.size() < 4)
        throw std::invalid_argument("密码至少需要 4 个字符。");

    auto existing = repositories::UserAccountRepository::findUserByUsername(username);
    if (existing.has_value())
        throw std::invalid_argument("用户名已存在。");

    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
        throw std::runtime_error("数据库连接失败。");

    const auto hashed = hashPassword(password);

    client->execSqlSync(
        "insert into users (username, password_hash, display_name, status) "
        "values (?, ?, ?, 'active')", username, hashed, displayName);

    const auto userResult = client->execSqlSync(
        "select id from users where username = ? limit 1", username);
    const int userId = userResult.front()["id"].as<int>();

    client->execSqlSync(
        "insert or ignore into user_roles (user_id, role_code) values (?, 'student')",
        std::to_string(userId));

    // Do NOT auto-create learner record — teacher/admin assigns course later

    return buildSessionPayload(username);
}

Json::Value AuthService::registerTeacher(const std::string &username,
                                         const std::string &password,
                                         const std::string &displayName,
                                         const std::string &employeeCode)
{
    if (username.empty() || password.empty())
        throw std::invalid_argument("用户名和密码不能为空。");
    if (password.size() < 4)
        throw std::invalid_argument("密码至少需要 4 个字符。");

    auto existing = repositories::UserAccountRepository::findUserByUsername(username);
    if (existing.has_value())
        throw std::invalid_argument("用户名已存在。");

    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
        throw std::runtime_error("数据库连接失败。");

    const auto hashed = hashPassword(password);

    client->execSqlSync(
        "insert into users (username, password_hash, display_name, status) "
        "values (?, ?, ?, 'active')", username, hashed, displayName);

    const auto userResult = client->execSqlSync(
        "select id from users where username = ? limit 1", username);
    const int userId = userResult.front()["id"].as<int>();

    client->execSqlSync(
        "insert or ignore into user_roles (user_id, role_code) values (?, 'teacher')",
        std::to_string(userId));

    return buildSessionPayload(username);
}
}
