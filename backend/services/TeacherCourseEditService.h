#pragma once

#include <json/json.h>

#include <string>

namespace services
{
class TeacherCourseEditService
{
  public:
    // Knowledge point CRUD
    static Json::Value createKnowledgePoint(const std::string &username,
                                            const std::string &courseCode,
                                            const Json::Value &body);
    static Json::Value updateKnowledgePoint(const std::string &username,
                                            const std::string &courseCode,
                                            const std::string &pointCode,
                                            const Json::Value &body);
    static Json::Value deleteKnowledgePoint(const std::string &username,
                                            const std::string &courseCode,
                                            const std::string &pointCode);

    // Dependencies
    static Json::Value readDependencies(const std::string &username,
                                        const std::string &courseCode);
    static Json::Value updateDependencies(const std::string &username,
                                          const std::string &courseCode,
                                          const Json::Value &body);

    // Resource file management
    static Json::Value readResources(const std::string &username,
                                     const std::string &courseCode);
    static Json::Value writeResources(const std::string &username,
                                      const std::string &courseCode,
                                      const Json::Value &body);

    // Course creation
    static Json::Value createCourse(const std::string &username,
                                    const Json::Value &body);
};
}
