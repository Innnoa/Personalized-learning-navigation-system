#pragma once

#include <string>
#include <vector>

namespace algorithm::graph
{
struct KnowledgePoint
{
    int dbId = 0;
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

struct KnowledgeDependency
{
    int dbId = 0;
    std::string sourceCode;
    std::string targetCode;
    std::string relationType;
    std::string description;
};

struct KnowledgeGraph
{
    int courseId = 0;
    std::string courseCode;
    std::string courseName;
    std::string courseDescription;
    std::string targetAudience;
    std::vector<KnowledgePoint> points;
    std::vector<KnowledgeDependency> dependencies;
};
}
