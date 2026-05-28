#pragma once

#include <drogon/HttpController.h>

namespace api
{
class TeacherCourseEditController : public drogon::HttpController<TeacherCourseEditController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TeacherCourseEditController::createKnowledgePoint,
                  "/api/teacher/courses/{1}/knowledge-points", drogon::Post);
    ADD_METHOD_TO(TeacherCourseEditController::updateKnowledgePoint,
                  "/api/teacher/courses/{1}/knowledge-points/{2}", drogon::Put);
    ADD_METHOD_TO(TeacherCourseEditController::deleteKnowledgePoint,
                  "/api/teacher/courses/{1}/knowledge-points/{2}", drogon::Delete);
    ADD_METHOD_TO(TeacherCourseEditController::readDependencies,
                  "/api/teacher/courses/{1}/knowledge-dependencies", drogon::Get);
    ADD_METHOD_TO(TeacherCourseEditController::updateDependencies,
                  "/api/teacher/courses/{1}/knowledge-dependencies", drogon::Put);
    ADD_METHOD_TO(TeacherCourseEditController::readResources,
                  "/api/teacher/courses/{1}/resources", drogon::Get);
    ADD_METHOD_TO(TeacherCourseEditController::writeResources,
                  "/api/teacher/courses/{1}/resources", drogon::Put);
    METHOD_LIST_END

    void createKnowledgePoint(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &courseCode) const;
    void updateKnowledgePoint(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &courseCode,
                              const std::string &pointCode) const;
    void deleteKnowledgePoint(const drogon::HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                              const std::string &courseCode,
                              const std::string &pointCode) const;
    void readDependencies(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                          const std::string &courseCode) const;
    void updateDependencies(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            const std::string &courseCode) const;
    void readResources(const drogon::HttpRequestPtr &req,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                       const std::string &courseCode) const;
    void writeResources(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                        const std::string &courseCode) const;
};
}
