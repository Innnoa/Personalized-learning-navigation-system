#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class TeacherDashboardService
{
  public:
    static Json::Value listCourses(const std::string &username);
    static Json::Value buildCourseOverview(const std::string &username,
                                           const std::string &courseCode);
};
}
