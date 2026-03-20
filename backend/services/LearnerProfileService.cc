#include "services/LearnerProfileService.h"

#include "repositories/LearnerProfileRepository.h"

#include <drogon/drogon.h>

#include <cmath>
#include <stdexcept>

namespace
{
std::string readDefaultLearnerCode()
{
    const auto &config = drogon::app().getCustomConfig();
    if (config.isMember("default_learner_code") &&
        config["default_learner_code"].isString())
    {
        const auto learnerCode = config["default_learner_code"].asString();
        if (!learnerCode.empty())
        {
            return learnerCode;
        }
    }

    return "demo-learner";
}
}

namespace services
{
Json::Value LearnerProfileService::buildProfilePayload(
    const std::string &learnerCode)
{
    const auto effectiveLearnerCode =
        learnerCode.empty() ? readDefaultLearnerCode() : learnerCode;
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            effectiveLearnerCode);

    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者画像。");
    }

    const auto masteryRecords =
        repositories::LearnerProfileRepository::listMasteryByLearnerAndCourseId(
            learner->id, learner->targetCourseId);
    const int feedbackRecordCount =
        repositories::LearnerProfileRepository::countFeedbackRecordsByLearnerId(
            learner->id);
    const auto recentFeedbackRecords =
        repositories::LearnerProfileRepository::listRecentFeedbackRecordsByLearnerId(
            learner->id);

    Json::Value masteryByCode;
    Json::Value masteryItems(Json::arrayValue);
    Json::Value recentFeedbackItems(Json::arrayValue);
    double masterySum = 0.0;
    int masteredCount = 0;
    int inProgressCount = 0;
    int notStartedCount = 0;

    for (const auto &item : masteryRecords)
    {
        masteryByCode[item.code] = item.masteryScore;

        const int masteryPercent =
            static_cast<int>(std::round(item.masteryScore * 100.0));
        masterySum += item.masteryScore;

        if (item.masteryScore >= 0.85)
        {
            ++masteredCount;
        }
        else if (item.masteryScore > 0.0)
        {
            ++inProgressCount;
        }
        else
        {
            ++notStartedCount;
        }

        Json::Value masteryItem;
        masteryItem["knowledgePointId"] = item.knowledgePointId;
        masteryItem["code"] = item.code;
        masteryItem["name"] = item.name;
        masteryItem["chapterNo"] = item.chapterNo;
        masteryItem["chapterName"] = item.chapterName;
        masteryItem["masteryScore"] = item.masteryScore;
        masteryItem["masteryPercent"] = masteryPercent;
        masteryItems.append(masteryItem);
    }

    for (const auto &record : recentFeedbackRecords)
    {
        Json::Value feedbackItem;
        feedbackItem["knowledgePointId"] = record.knowledgePointId;
        feedbackItem["code"] = record.code;
        feedbackItem["name"] = record.name;
        feedbackItem["chapterNo"] = record.chapterNo;
        feedbackItem["chapterName"] = record.chapterName;
        feedbackItem["completionStatus"] = record.completionStatus;
        feedbackItem["selfRatedMastery"] = record.selfRatedMastery;
        feedbackItem["selfRatedMasteryPercent"] =
            static_cast<int>(std::round(record.selfRatedMastery * 100.0));
        feedbackItem["previousMastery"] = record.previousMastery;
        feedbackItem["previousMasteryPercent"] =
            static_cast<int>(std::round(record.previousMastery * 100.0));
        feedbackItem["updatedMastery"] = record.updatedMastery;
        feedbackItem["updatedMasteryPercent"] =
            static_cast<int>(std::round(record.updatedMastery * 100.0));
        feedbackItem["ruleApplied"] = record.ruleApplied;
        feedbackItem["recordedAt"] = record.recordedAt;
        recentFeedbackItems.append(feedbackItem);
    }

    Json::Value payload;
    payload["status"] = "ok";
    payload["message"] = "学习者画像已从 SQLite 读取。";
    payload["learner"]["id"] = learner->id;
    payload["learner"]["code"] = learner->code;
    payload["learner"]["name"] = learner->name;
    payload["learner"]["major"] = learner->major;
    payload["learner"]["gradeLabel"] = learner->gradeLabel;
    payload["learner"]["note"] = learner->note;
    payload["course"]["id"] = learner->targetCourseId;
    payload["course"]["code"] = learner->targetCourseCode;
    payload["course"]["name"] = learner->targetCourseName;
    payload["summary"]["knowledgePointCount"] =
        static_cast<int>(masteryRecords.size());
    payload["summary"]["masteredCount"] = masteredCount;
    payload["summary"]["inProgressCount"] = inProgressCount;
    payload["summary"]["notStartedCount"] = notStartedCount;
    payload["summary"]["feedbackRecordCount"] = feedbackRecordCount;
    payload["summary"]["averageMasteryPercent"] =
        masteryRecords.empty()
            ? 0
            : static_cast<int>(std::round(
                  masterySum * 100.0 / static_cast<double>(masteryRecords.size())));
    payload["masteryByCode"] = masteryByCode;
    payload["masteryItems"] = masteryItems;
    payload["recentFeedbackItems"] = recentFeedbackItems;

    return payload;
}
}
