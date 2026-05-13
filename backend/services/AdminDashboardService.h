#pragma once

#include <json/json.h>

namespace services
{
class AdminDashboardService
{
  public:
    static Json::Value listUsers();
    static Json::Value listCourses();
    static Json::Value listCourseAssignments();
};
}
