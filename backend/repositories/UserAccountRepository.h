#pragma once

#include <optional>
#include <string>
#include <vector>

namespace repositories
{
struct UserAccountRecord
{
    int id = 0;
    std::string username;
    std::string passwordHash;
    std::string displayName;
    std::string status;
};

struct UserRoleRecord
{
    std::string roleCode;
};

struct UserLearnerLinkRecord
{
    int learnerId = 0;
    std::string learnerCode;
    std::string learnerName;
    std::string courseCode;
    std::string courseName;
};

class UserAccountRepository
{
  public:
    static std::optional<UserAccountRecord> findUserByUsername(
        const std::string &username);
    static std::vector<UserRoleRecord> listRolesForUser(int userId);
    static std::optional<UserLearnerLinkRecord> findLearnerLinkForUser(int userId);
    static std::vector<UserLearnerLinkRecord> findAllLearnerLinksForUser(int userId);
};
}
