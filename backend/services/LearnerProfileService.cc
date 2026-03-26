#include "services/LearnerProfileService.h"

#include "repositories/LearnerProfileRepository.h"
#include "repositories/ResourceViewRepository.h"
#include "services/DetailLearningProgressService.h"

#include <drogon/drogon.h>

#include <cmath>
#include <stdexcept>
#include <unordered_map>

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

std::string resolveSourceContextLabel(const std::string &sourceContext,
                                      const std::string &scopeCode)
{
    if (sourceContext == "main_path_resource_panel")
    {
        return "首页资源建议区";
    }

    if (sourceContext == "resource_recommendation_page")
    {
        return "单节点资源页";
    }

    if (sourceContext == "detail_scope_resource_panel")
    {
        if (scopeCode.empty() || scopeCode == "root")
        {
            return "细化图谱资源区";
        }

        return "细化图谱资源区（" + scopeCode + "）";
    }

    return "资源建议区";
}

std::string resolveInteractionTypeLabel(const std::string &interactionType)
{
    if (interactionType == "viewed")
    {
        return "记为已看";
    }

    if (interactionType == "completed")
    {
        return "已学完";
    }

    if (interactionType == "save_for_later")
    {
        return "稍后再看";
    }

    return "已打开";
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
    const int resourceViewRecordCount =
        repositories::ResourceViewRepository::countResourceViewRecordsByLearnerId(
            learner->id);
    const auto recentResourceViewRecords =
        repositories::ResourceViewRepository::listRecentResourceViewRecordsByLearnerId(
            learner->id);

    Json::Value masteryByCode(Json::objectValue);
    Json::Value graphMasteryByCode(Json::objectValue);
    Json::Value detailMasteryByCode(Json::objectValue);
    Json::Value masteryItems(Json::arrayValue);
    Json::Value recentFeedbackItems(Json::arrayValue);
    Json::Value recentResourceViewItems(Json::arrayValue);
    double masterySum = 0.0;
    int masteredCount = 0;
    int inProgressCount = 0;
    int notStartedCount = 0;
    std::unordered_map<std::string, double> mainMasteryLookup;
    mainMasteryLookup.reserve(masteryRecords.size());

    for (const auto &item : masteryRecords)
    {
        masteryByCode[item.code] = item.masteryScore;
        mainMasteryLookup[item.code] = item.masteryScore;

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

    const auto detailProgressSnapshot =
        services::DetailLearningProgressService::buildProgressSnapshot(
            learner->id, learner->targetCourseId, mainMasteryLookup);
    for (const auto &[code, masteryScore] : detailProgressSnapshot.graphMasteryByCode)
    {
        graphMasteryByCode[code] = masteryScore;
    }
    for (const auto &[code, masteryScore] :
         detailProgressSnapshot.persistedDetailMasteryByCode)
    {
        detailMasteryByCode[code] = masteryScore;
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

    for (const auto &record : recentResourceViewRecords)
    {
        Json::Value resourceItem;
        resourceItem["knowledgePointId"] = record.knowledgePointId;
        resourceItem["code"] = record.code;
        resourceItem["name"] = record.name;
        resourceItem["chapterNo"] = record.chapterNo;
        resourceItem["chapterName"] = record.chapterName;
        resourceItem["resourceTitle"] = record.resourceTitle;
        resourceItem["resourceUrl"] = record.resourceUrl;
        resourceItem["resourceType"] = record.resourceType;
        resourceItem["resourceSource"] = record.resourceSource;
        resourceItem["resourceLayer"] = record.resourceLayer;
        resourceItem["recommendedPhase"] = record.recommendedPhase;
        resourceItem["sourceContext"] = record.sourceContext;
        resourceItem["sourceContextLabel"] =
            resolveSourceContextLabel(record.sourceContext, record.scopeCode);
        resourceItem["scopeCode"] = record.scopeCode;
        resourceItem["linkedReasonSummary"] = record.linkedReasonSummary;
        resourceItem["interactionType"] = record.interactionType;
        resourceItem["interactionTypeLabel"] =
            resolveInteractionTypeLabel(record.interactionType);
        resourceItem["recordedAt"] = record.recordedAt;
        recentResourceViewItems.append(resourceItem);
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
    payload["summary"]["resourceViewRecordCount"] = resourceViewRecordCount;
    payload["summary"]["averageMasteryPercent"] =
        masteryRecords.empty()
            ? 0
            : static_cast<int>(std::round(
                  masterySum * 100.0 / static_cast<double>(masteryRecords.size())));
    payload["masteryByCode"] = masteryByCode;
    payload["graphMasteryByCode"] = graphMasteryByCode;
    payload["detailMasteryByCode"] = detailMasteryByCode;
    payload["masteryItems"] = masteryItems;
    payload["recentFeedbackItems"] = recentFeedbackItems;
    payload["recentResourceViewItems"] = recentResourceViewItems;

    return payload;
}
}
