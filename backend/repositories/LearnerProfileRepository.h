#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace repositories
{
struct LearnerRecord
{
    int id = 0;
    std::string code;
    std::string name;
    std::string major;
    std::string gradeLabel;
    int targetCourseId = 0;
    std::string targetCourseCode;
    std::string targetCourseName;
    std::string note;
};

struct LearnerMasteryRecord
{
    int knowledgePointId = 0;
    std::string code;
    std::string name;
    int chapterNo = 0;
    std::string chapterName;
    double masteryScore = 0.0;
};

struct LearnerFeedbackRecord
{
    int knowledgePointId = 0;
    std::string code;
    std::string name;
    int chapterNo = 0;
    std::string chapterName;
    std::string completionStatus;
    double selfRatedMastery = 0.0;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    std::string ruleApplied;
    std::string recordedAt;
};

class LearnerProfileRepository
{
  public:
    static std::optional<LearnerRecord> findLearnerByCode(
        const std::string &learnerCode);
    static std::vector<LearnerMasteryRecord> listMasteryByLearnerAndCourseId(
        int learnerId, int courseId);
    static int countFeedbackRecordsByLearnerId(int learnerId);
    static std::vector<LearnerFeedbackRecord> listRecentFeedbackRecordsByLearnerId(
        int learnerId, std::size_t limit = 6);
};
}
