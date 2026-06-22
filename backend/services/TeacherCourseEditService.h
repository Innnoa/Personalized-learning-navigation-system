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

    // Resource CRUD
    static Json::Value readResources(const std::string &username,
                                     const std::string &courseCode);
    static Json::Value listResources(const std::string &username,
                                     const std::string &courseCode,
                                     const Json::Value &query);
    static Json::Value createResource(const std::string &username,
                                      const std::string &courseCode,
                                      const Json::Value &body);
    static Json::Value updateResource(const std::string &username,
                                      const std::string &courseCode,
                                      int resourceId,
                                      const Json::Value &body);
    static Json::Value deleteResource(const std::string &username,
                                      const std::string &courseCode,
                                      int resourceId);
    static Json::Value writeResources(const std::string &username,
                                      const std::string &courseCode,
                                      const Json::Value &body);

    // Course creation
    static Json::Value createCourse(const std::string &username,
                                    const Json::Value &body);
};
}
