#include "services/AuthService.h"

#include "repositories/UserAccountRepository.h"

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
        const auto learnerLink =
            repositories::UserAccountRepository::findLearnerLinkForUser(user.id);
        if (learnerLink.has_value())
        {
            payload["linkedLearner"]["learnerId"] = learnerLink->learnerId;
            payload["linkedLearner"]["learnerCode"] = learnerLink->learnerCode;
            payload["linkedLearner"]["learnerName"] = learnerLink->learnerName;
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

    if (user->passwordHash != password)
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
}
