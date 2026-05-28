#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class TeacherStudentService
{
  public:
    static Json::Value listStudents(const std::string &username,
                                    const std::string &courseCode,
                                    const std::string &major,
                                    const std::string &gradeLabel);
    static Json::Value buildCourseStats(const std::string &username,
                                        const std::string &courseCode);
    static Json::Value buildKnowledgeStats(const std::string &username,
                                           const std::string &courseCode);
    static Json::Value buildStudentProgress(const std::string &username,
                                            const std::string &courseCode,
                                            const std::string &learnerCode);
    static Json::Value addStudent(const std::string &username,
                                  const std::string &courseCode,
                                  const Json::Value &body);
    static Json::Value listAvailableStudents(const std::string &username,
                                             const std::string &courseCode,
                                             const std::string &major,
                                             const std::string &gradeLabel);
};
}
