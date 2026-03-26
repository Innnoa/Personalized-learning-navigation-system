#pragma once

#include <optional>
#include <string>
#include <vector>

namespace repositories
{
struct CourseRecord
{
    int id = 0;
    std::string code;
    std::string name;
    std::string description;
    std::string targetAudience;
};

struct KnowledgePointRecord
{
    int id = 0;
    std::string code;
    std::string name;
    int chapterNo = 0;
    std::string chapterName;
    std::string nodeType;
    int difficultyLevel = 1;
    int estimatedMinutes = 20;
    int displayOrder = 0;
    bool isCore = true;
    std::string description;
};

struct KnowledgeDependencyRecord
{
    int id = 0;
    int prerequisitePointId = 0;
    int targetPointId = 0;
    std::string relationType;
    std::string description;
};

class KnowledgeGraphRepository
{
  public:
    static std::optional<CourseRecord> findCourseByName(
        const std::string &courseName);
    static std::optional<KnowledgePointRecord> findKnowledgePointByCourseIdAndCode(
        int courseId, const std::string &knowledgePointCode);
    static std::vector<KnowledgePointRecord> listKnowledgePointsByCourseId(
        int courseId);
    static std::vector<KnowledgeDependencyRecord> listDependenciesByCourseId(
        int courseId);
};
}
