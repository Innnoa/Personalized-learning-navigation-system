#include "services/FeedbackService.h"

#include "algorithm/adjuster/LearningPathAdjuster.h"
#include "repositories/DetailLearningRepository.h"
#include "repositories/FeedbackRepository.h"
#include "repositories/LearnerProfileRepository.h"
#include "services/DetailScopeCatalogService.h"
#include "services/KnowledgeGraphService.h"
#include "services/LearnerProfileService.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>

#include <unordered_map>
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

std::string resolveLearnerCode(const Json::Value &requestJson)
{
    if (!requestJson.isObject())
    {
        throw std::invalid_argument("请求体必须是 JSON 对象。");
    }

    if (!requestJson.isMember("learnerCode"))
    {
        return "";
    }

    if (!requestJson["learnerCode"].isString())
    {
        throw std::invalid_argument("learnerCode 必须是字符串。");
    }

    return requestJson["learnerCode"].asString();
}

std::unordered_map<std::string, double> parseMasteryByCode(
    const Json::Value &requestJson)
{
    std::unordered_map<std::string, double> masteryByCode;

    if (!requestJson.isMember("masteryByCode"))
    {
        return masteryByCode;
    }

    if (!requestJson["masteryByCode"].isObject())
    {
        throw std::invalid_argument("masteryByCode 必须是对象。");
    }

    for (const auto &code : requestJson["masteryByCode"].getMemberNames())
    {
        const auto &value = requestJson["masteryByCode"][code];
        if (!value.isNumeric())
        {
            throw std::invalid_argument("masteryByCode 中的掌握度必须是数值。");
        }

        masteryByCode[code] = value.asDouble();
    }

    return masteryByCode;
}

std::vector<algorithm::adjuster::FeedbackItem> parseFeedbackItems(
    const Json::Value &requestJson)
{
    if (!requestJson.isMember("feedbackItems") || !requestJson["feedbackItems"].isArray())
    {
        throw std::invalid_argument("feedbackItems 必须是数组。");
    }

    std::vector<algorithm::adjuster::FeedbackItem> feedbackItems;
    for (const auto &item : requestJson["feedbackItems"])
    {
        if (!item.isObject())
        {
            throw std::invalid_argument("feedbackItems 数组中的元素必须是对象。");
        }

        if (!item.isMember("code") || !item["code"].isString())
        {
            throw std::invalid_argument("feedbackItems.code 必须是字符串。");
        }

        if (!item.isMember("completionStatus") ||
            !item["completionStatus"].isString())
        {
            throw std::invalid_argument(
                "feedbackItems.completionStatus 必须是字符串。");
        }

        if (!item.isMember("selfRatedMastery") ||
            !item["selfRatedMastery"].isNumeric())
        {
            throw std::invalid_argument(
                "feedbackItems.selfRatedMastery 必须是数值。");
        }

        feedbackItems.push_back(algorithm::adjuster::FeedbackItem{
            item["code"].asString(),
            item["completionStatus"].asString(),
            item["selfRatedMastery"].asDouble()});
    }

    if (feedbackItems.empty())
    {
        throw std::invalid_argument("feedbackItems 不能为空。");
    }

    return feedbackItems;
}

std::string parseScopeCode(const Json::Value &requestJson)
{
    if (!requestJson.isMember("scopeCode"))
    {
        return "";
    }

    if (!requestJson["scopeCode"].isString())
    {
        throw std::invalid_argument("scopeCode 必须是字符串。");
    }

    return requestJson["scopeCode"].asString();
}

std::string parseSourcePage(const Json::Value &requestJson)
{
    if (!requestJson.isMember("sourcePage"))
    {
        return "";
    }

    if (!requestJson["sourcePage"].isString())
    {
        throw std::invalid_argument("sourcePage 必须是字符串。");
    }

    return requestJson["sourcePage"].asString();
}

std::string resolveDetailPracticeNodeCode(const std::string &scopeCode,
                                          const std::string &feedbackCode)
{
    if (scopeCode.empty())
    {
        return "";
    }

    const auto &catalog = services::DetailScopeCatalogService::getCatalog();
    const auto scopeIt = catalog.scopesByCode.find(scopeCode);
    if (scopeIt == catalog.scopesByCode.end())
    {
        return "";
    }

    for (const auto &node : scopeIt->second.nodes)
    {
        if (node.code == feedbackCode)
        {
            return node.code;
        }
    }

    for (const auto &node : scopeIt->second.nodes)
    {
        if (node.code.rfind(feedbackCode + "-", 0) == 0)
        {
            return node.code;
        }
    }

    return "";
}
}

namespace services
{
Json::Value FeedbackService::submitFeedback(const Json::Value &requestJson)
{
    const auto learnerCode = resolveLearnerCode(requestJson);
    const auto scopeCode = parseScopeCode(requestJson);
    const auto sourcePage = parseSourcePage(requestJson);
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            learnerCode.empty() ? readDefaultLearnerCode() : learnerCode);

    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法保存反馈。");
    }

    const auto feedbackItems = parseFeedbackItems(requestJson);
    auto masteryByCode = parseMasteryByCode(requestJson);

    if (masteryByCode.empty())
    {
        const auto masteryRecords =
            repositories::LearnerProfileRepository::listMasteryByLearnerAndCourseId(
                learner->id, learner->targetCourseId);
        for (const auto &item : masteryRecords)
        {
            masteryByCode[item.code] = item.masteryScore;
        }
    }

    const auto graph = KnowledgeGraphService::loadKnowledgeGraph();
    const auto feedbackUpdate = algorithm::adjuster::applyFeedbackUpdates(
        graph, masteryByCode, feedbackItems);
    const auto feedbackBatchId = drogon::utils::getUuid();
    std::unordered_map<std::string, double> selfRatedMasteryByCode;
    for (const auto &item : feedbackItems)
    {
        selfRatedMasteryByCode[item.code] = item.selfRatedMastery;
    }

    std::unordered_map<std::string, int> pointIdByCode;
    pointIdByCode.reserve(graph.points.size());
    for (const auto &point : graph.points)
    {
        pointIdByCode[point.code] = point.dbId;
    }

    for (const auto &detail : feedbackUpdate.adjustmentDetails)
    {
        if (pointIdByCode.count(detail.code) == 0)
        {
            throw std::runtime_error("反馈记录引用了不存在的知识点编码。");
        }

        repositories::FeedbackRepository::upsertLearnerMastery(
            learner->id, pointIdByCode[detail.code], detail.updatedMastery);
            repositories::FeedbackRepository::insertFeedbackRecord(
                repositories::FeedbackRecordWrite{
                    learner->id,
                    pointIdByCode[detail.code],
                    detail.completionStatus,
                    selfRatedMasteryByCode[detail.code],
                    detail.previousMastery,
                    detail.updatedMastery,
                    detail.ruleApplied,
                    feedbackBatchId});

        const auto detailNodeCode =
            sourcePage == "detail-learning"
                ? resolveDetailPracticeNodeCode(scopeCode, detail.code)
                : std::string();
        if (!detailNodeCode.empty())
        {
            repositories::DetailLearningRepository::insertDetailFeedbackRecord(
                repositories::DetailFeedbackRecordWrite{
                    learner->id,
                    scopeCode,
                    detailNodeCode,
                    detail.completionStatus,
                    selfRatedMasteryByCode[detail.code],
                    detail.previousMastery,
                    detail.updatedMastery,
                    detail.ruleApplied,
                    feedbackBatchId});
        }
    }

    auto payload = LearnerProfileService::buildProfilePayload(learner->code);
    payload["message"] = "已保存学习反馈并更新学习者画像。";
    payload["savedRecordCount"] =
        static_cast<int>(feedbackUpdate.adjustmentDetails.size());
    payload["feedbackBatchId"] = feedbackBatchId;
    payload["feedbackSummary"]["feedbackItemCount"] =
        static_cast<int>(feedbackUpdate.adjustmentDetails.size());
    payload["feedbackSummary"]["completedCount"] = feedbackUpdate.completedCount;
    payload["feedbackSummary"]["partialCount"] = feedbackUpdate.partialCount;
    payload["feedbackSummary"]["blockedCount"] = feedbackUpdate.blockedCount;

    Json::Value adjustments(Json::arrayValue);
    for (const auto &detail : feedbackUpdate.adjustmentDetails)
    {
        Json::Value item;
        item["code"] = detail.code;
        item["completionStatus"] = detail.completionStatus;
        item["previousMastery"] = detail.previousMastery;
        item["updatedMastery"] = detail.updatedMastery;
        item["ruleApplied"] = detail.ruleApplied;

        Json::Value reasons(Json::arrayValue);
        for (const auto &reason : detail.adjustmentReasons)
        {
            reasons.append(reason);
        }
        item["adjustmentReasons"] = reasons;
        adjustments.append(item);
    }
    payload["adjustments"] = adjustments;

    return payload;
}

Json::Value FeedbackService::rollbackLatestFeedback(
    const Json::Value &requestJson)
{
    const auto learnerCode = resolveLearnerCode(requestJson);
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            learnerCode.empty() ? readDefaultLearnerCode() : learnerCode);

    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法回退反馈。");
    }

    const auto latestBatch =
        repositories::FeedbackRepository::findLatestFeedbackBatchByLearnerId(
            learner->id);
    if (!latestBatch.has_value())
    {
        throw std::invalid_argument("当前没有可回退的学习反馈批次。");
    }

    const auto batchRecords =
        repositories::FeedbackRepository::listFeedbackRecordsByBatchId(
            learner->id, latestBatch->batchId);
    if (batchRecords.empty())
    {
        throw std::runtime_error("最新反馈批次不存在有效记录，无法回退。");
    }

    repositories::FeedbackRepository::rollbackFeedbackBatch(
        learner->id, latestBatch->batchId, batchRecords);

    auto payload = LearnerProfileService::buildProfilePayload(learner->code);
    payload["message"] = "已撤销最近一次学习反馈，并恢复上一版学习者画像。";
    payload["rolledBackRecordCount"] = static_cast<int>(batchRecords.size());
    payload["rolledBackBatchId"] = latestBatch->batchId;
    payload["rollbackSummary"]["batchId"] = latestBatch->batchId;
    payload["rollbackSummary"]["recordedAt"] = latestBatch->recordedAt;
    payload["rollbackSummary"]["feedbackItemCount"] =
        static_cast<int>(batchRecords.size());

    int completedCount = 0;
    int partialCount = 0;
    int blockedCount = 0;
    Json::Value rollbackItems(Json::arrayValue);
    for (const auto &record : batchRecords)
    {
        if (record.completionStatus == "completed")
        {
            ++completedCount;
        }
        else if (record.completionStatus == "partial")
        {
            ++partialCount;
        }
        else if (record.completionStatus == "blocked")
        {
            ++blockedCount;
        }

        Json::Value item;
        item["knowledgePointId"] = record.knowledgePointId;
        item["code"] = record.code;
        item["name"] = record.name;
        item["chapterNo"] = record.chapterNo;
        item["chapterName"] = record.chapterName;
        item["completionStatus"] = record.completionStatus;
        item["selfRatedMastery"] = record.selfRatedMastery;
        item["previousMastery"] = record.previousMastery;
        item["updatedMastery"] = record.updatedMastery;
        item["restoredMastery"] = record.previousMastery;
        item["rolledBackFromMastery"] = record.updatedMastery;
        item["ruleApplied"] = record.ruleApplied;
        item["recordedAt"] = record.recordedAt;
        rollbackItems.append(item);
    }

    payload["rollbackSummary"]["completedCount"] = completedCount;
    payload["rollbackSummary"]["partialCount"] = partialCount;
    payload["rollbackSummary"]["blockedCount"] = blockedCount;
    payload["rollbackItems"] = rollbackItems;

    return payload;
}
}
