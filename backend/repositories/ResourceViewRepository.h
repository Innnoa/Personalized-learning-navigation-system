#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace repositories
{
struct ResourceViewRecordWrite
{
    int learnerId = 0;
    int knowledgePointId = 0;
    std::string resourceTitle;
    std::string resourceUrl;
    std::string resourceType;
    std::string resourceSource;
    std::string resourceLayer;
    std::string recommendedPhase;
    std::string sourceContext;
    std::string scopeCode;
    std::string linkedReasonSummary;
    std::string interactionType;
};

struct ResourceViewRecord
{
    int knowledgePointId = 0;
    std::string code;
    std::string name;
    int chapterNo = 0;
    std::string chapterName;
    std::string resourceTitle;
    std::string resourceUrl;
    std::string resourceType;
    std::string resourceSource;
    std::string resourceLayer;
    std::string recommendedPhase;
    std::string sourceContext;
    std::string scopeCode;
    std::string linkedReasonSummary;
    std::string interactionType;
    std::string recordedAt;
};

struct ResourceLatestInteractionRecord
{
    std::string resourceUrl;
    std::string interactionType;
};

class ResourceViewRepository
{
  public:
    static void insertResourceViewRecord(const ResourceViewRecordWrite &record);
    static int countResourceViewRecordsByLearnerId(int learnerId);
    static std::vector<ResourceViewRecord> listRecentResourceViewRecordsByLearnerId(
        int learnerId, std::size_t limit = 6);
    static std::vector<ResourceLatestInteractionRecord>
    listLatestResourceInteractionsByLearnerAndKnowledgePointId(
        int learnerId, int knowledgePointId);
};
}
