#include "services/ResourceViewService.h"

#include "repositories/KnowledgeGraphRepository.h"
#include "repositories/LearnerProfileRepository.h"
#include "repositories/ResourceViewRepository.h"
#include "services/LearnerProfileService.h"

#include <drogon/drogon.h>

#include <array>
#include <stdexcept>
#include <string>

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

std::string parseOptionalStringField(const Json::Value &requestJson,
                                     const std::string &fieldName)
{
    if (!requestJson.isMember(fieldName))
    {
        return "";
    }

    if (!requestJson[fieldName].isString())
    {
        throw std::invalid_argument(fieldName + " 必须是字符串。");
    }

    return requestJson[fieldName].asString();
}

std::string parseRequiredStringField(const Json::Value &requestJson,
                                     const std::string &fieldName)
{
    const auto value = parseOptionalStringField(requestJson, fieldName);
    if (value.empty())
    {
        throw std::invalid_argument(fieldName + " 不能为空。");
    }

    return value;
}

std::string resolveInteractionType(const Json::Value &requestJson)
{
    const auto interactionType =
        parseOptionalStringField(requestJson, "interactionType");
    const auto effectiveInteractionType =
        interactionType.empty() ? "opened" : interactionType;

    static const std::array<std::string, 4> allowedTypes = {
        "opened",
        "viewed",
        "completed",
        "save_for_later"};

    for (const auto &allowedType : allowedTypes)
    {
        if (effectiveInteractionType == allowedType)
        {
            return effectiveInteractionType;
        }
    }

    throw std::invalid_argument(
        "interactionType 仅支持 opened、viewed、completed、save_for_later。");
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
Json::Value ResourceViewService::recordResourceView(const Json::Value &requestJson)
{
    if (!requestJson.isObject())
    {
        throw std::invalid_argument("请求体必须是 JSON 对象。");
    }

    const auto learnerCode = parseOptionalStringField(requestJson, "learnerCode");
    const auto effectiveLearnerCode =
        learnerCode.empty() ? readDefaultLearnerCode() : learnerCode;
    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            effectiveLearnerCode);
    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法记录资源行为。");
    }

    const auto knowledgePointCode =
        parseRequiredStringField(requestJson, "knowledgePointCode");
    const auto knowledgePoint =
        repositories::KnowledgeGraphRepository::findKnowledgePointByCourseIdAndCode(
            learner->targetCourseId, knowledgePointCode);
    if (!knowledgePoint.has_value())
    {
        throw std::invalid_argument("未找到指定知识点，无法记录资源行为。");
    }

    const auto interactionType = resolveInteractionType(requestJson);

    repositories::ResourceViewRepository::insertResourceViewRecord(
        repositories::ResourceViewRecordWrite{
            learner->id,
            knowledgePoint->id,
            parseRequiredStringField(requestJson, "resourceTitle"),
            parseRequiredStringField(requestJson, "resourceUrl"),
            parseOptionalStringField(requestJson, "resourceType"),
            parseOptionalStringField(requestJson, "resourceSource"),
            parseOptionalStringField(requestJson, "resourceLayer"),
            parseOptionalStringField(requestJson, "recommendedPhase"),
            parseOptionalStringField(requestJson, "sourceContext"),
            parseOptionalStringField(requestJson, "scopeCode"),
            parseOptionalStringField(requestJson, "linkedReasonLabelSummary"),
            interactionType});

    auto payload = LearnerProfileService::buildProfilePayload(learner->code);
    payload["message"] = "已记录资源行为，并更新最近资源轨迹。";
    payload["savedResourceViewCount"] = 1;
    payload["recordedResource"]["knowledgePointCode"] = knowledgePoint->code;
    payload["recordedResource"]["knowledgePointName"] = knowledgePoint->name;
    payload["recordedResource"]["resourceTitle"] =
        requestJson["resourceTitle"].asString();
    payload["recordedResource"]["resourceUrl"] =
        requestJson["resourceUrl"].asString();
    payload["recordedResource"]["sourceContext"] =
        parseOptionalStringField(requestJson, "sourceContext");
    payload["recordedResource"]["interactionType"] = interactionType;
    payload["recordedResource"]["interactionTypeLabel"] =
        resolveInteractionTypeLabel(interactionType);

    return payload;
}
}
