#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class AdminManageService
{
  public:
    // User management
    static Json::Value createUser(const Json::Value &body);
    static Json::Value updateUser(const std::string &username, const Json::Value &body);
    static Json::Value toggleUserStatus(const std::string &username);
    static Json::Value setUserRoles(const std::string &username, const Json::Value &body);

    // Course management
    static Json::Value updateCourse(const std::string &courseCode, const Json::Value &body);
    static Json::Value deleteCourse(const std::string &courseCode);

    // Assignment management
    static Json::Value assignTeacher(const Json::Value &body);
    static Json::Value unassignTeacher(const std::string &username,
                                       const std::string &courseCode);

    // Learner management
    static Json::Value listAllLearners();
    static Json::Value updateLearner(const std::string &learnerCode,
                                     const Json::Value &body);
    static Json::Value deleteLearner(const std::string &learnerCode);
    static Json::Value reassignLearnerCourse(const std::string &learnerCode,
                                             const Json::Value &body);

    // Logging
    static void logOperation(const std::string &username,
                             const std::string &action,
                             const std::string &target,
                             const std::string &detail);
    static Json::Value listLogs(int limit);
};
}
