#include <drogon/drogon_test.h>

#include "services/LearningResourceService.h"
#include "services/PathPlanningService.h"

#include <json/json.h>
#include <string>

DROGON_TEST(LearningResourceServiceReturnsConfiguredResources)
{
    const auto payload =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "queue");

    REQUIRE(payload.isArray());
    CHECK(payload.empty() == false);
    CHECK(payload[0]["type"].asString() == "video");
    CHECK(payload[0]["url"].asString().empty() == false);
    CHECK(payload[0]["recommendedPhase"].asString().empty() == false);
    CHECK(payload[0]["focusTags"].isArray());

    bool foundQueueCircularResource = false;
    for (const auto &item : payload)
    {
        if (item["focusNodeCode"].asString() == "queue-circular")
        {
            foundQueueCircularResource = true;
            CHECK(item["focusNodeLabel"].asString() == "循环队列");
        }
    }
    CHECK(foundQueueCircularResource == true);

    const auto missingPayload =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "unknown-code");
    CHECK(missingPayload.isArray());
    CHECK(missingPayload.empty() == true);
}

DROGON_TEST(LearningResourceServiceSortsResourcesByLearningStage)
{
    algorithm::planner::LearningPathItem scheduledItem;
    scheduledItem.point.code = "sequence-list";
    scheduledItem.point.name = "顺序表";
    scheduledItem.status = "scheduled";
    scheduledItem.masteryScore = 0.25;
    scheduledItem.reasonTrace.knowledgePointCode = "sequence-list";
    scheduledItem.reasonTrace.relevanceScore = 0.82;
    scheduledItem.reasonTrace.masteryGap = 0.75;
    scheduledItem.reasonTrace.timeCostPenalty = 0.28;
    scheduledItem.reasonTrace.triggerReasons = {"当前节点尚未掌握，需要先建立核心操作直觉。"};

    const auto scheduledResources =
        services::LearningResourceService::buildResourcesForLearningPathItem(
            scheduledItem);

    REQUIRE(scheduledResources.isArray());
    REQUIRE(scheduledResources.size() >= 2);
    CHECK(scheduledResources[0]["recommendedPhase"].asString() == "主学");
    CHECK(scheduledResources[0]["isPrimaryRecommendation"].asBool() == true);
    CHECK(scheduledResources[0]["priorityLabel"].asString() == "优先看");
    CHECK(scheduledResources[0]["recommendationRank"].asInt() == 1);
    CHECK(scheduledResources[0]["resourceLayer"].asString() == "课程风格优先");
    CHECK(scheduledResources[0]["resourceLayerHint"].asString().empty() == false);
    CHECK(scheduledResources[1]["recommendationRank"].asInt() == 2);

    algorithm::planner::LearningPathItem deferredItem;
    deferredItem.point.code = "queue";
    deferredItem.point.name = "队列";
    deferredItem.status = "deferred";
    deferredItem.masteryScore = 0.2;
    deferredItem.reasonTrace.knowledgePointCode = "queue";
    deferredItem.reasonTrace.relevanceScore = 0.74;
    deferredItem.reasonTrace.masteryGap = 0.8;
    deferredItem.reasonTrace.timeCostPenalty = 0.24;
    deferredItem.reasonTrace.triggerReasons = {"进入下一轮前需要先统一基本概念。"};

    const auto deferredResources =
        services::LearningResourceService::buildResourcesForLearningPathItem(
            deferredItem);

    REQUIRE(deferredResources.isArray());
    REQUIRE(deferredResources.size() >= 3);
    CHECK(deferredResources[0]["recommendedPhase"].asString() == "预习");
    CHECK(deferredResources[0]["type"].asString() == "video");
    CHECK(deferredResources[0]["isPrimaryRecommendation"].asBool() == true);
    CHECK(deferredResources[0]["priorityLabel"].asString() == "优先看");
    CHECK(deferredResources[0]["resourceLayer"].asString() == "课程风格优先");
}

DROGON_TEST(PathPlanningServiceBuildsLearningResourcesIntoResponse)
{
    Json::Value request(Json::objectValue);
    request["targetCodes"].append("topological-sort");
    request["availableMinutes"] = 120;
    request["masteryByCode"]["ds-intro"] = 0.9;
    request["masteryByCode"]["linear-list"] = 0.85;
    request["masteryByCode"]["queue"] = 0.2;
    request["masteryByCode"]["graph-basic"] = 0.15;
    request["masteryByCode"]["topological-sort"] = 0.0;

    const auto payload = services::PathPlanningService::buildPathPayload(request);

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["resourceRecommendations"].isArray());
    CHECK(payload["resourceRecommendations"].empty() == false);

    bool foundQueueResource = false;
    bool foundTopologicalSortSection = false;
    bool foundCriticalPathSolveResource = false;
    for (const auto &item : payload["path"])
    {
        if (item["code"].asString() == "queue")
        {
            foundQueueResource = true;
            CHECK(item["learningResources"].isArray());
            CHECK(item["learningResources"].empty() == false);
            CHECK(item["learningResources"][0]["recommendedPhase"].asString().empty() ==
                  false);
            CHECK(item["learningResources"][0]["focusTags"].isArray());
            CHECK(item["learningResources"][0]["selectionReason"].asString().empty() ==
                  false);
            CHECK(item["learningResources"][0]["selectionReason"]
                      .asString()
                      .find("规划依据显示") != std::string::npos);
        }
    }

    for (const auto &item : payload["resourceRecommendations"])
    {
        if (item["code"].asString() == "topological-sort")
        {
            foundTopologicalSortSection = true;
            CHECK(item["resources"].isArray());
            CHECK(item["resourceCount"].asInt() >= 1);
            CHECK(item["primaryResourceTitle"].asString().empty() == false);
            CHECK(item["primaryResourcePriorityLabel"].asString() == "优先看");
            CHECK(item["resourceLayerSummary"].isObject());
            CHECK(item["resourceLayerSummary"]["课程风格优先"].asInt() >= 1);
            CHECK(item["resources"][0]["isPrimaryRecommendation"].asBool() == true);
            CHECK(item["resources"][0]["recommendationRank"].asInt() == 1);

            for (const auto &resource : item["resources"])
            {
                CHECK(resource["selectionReason"].asString().empty() == false);
                CHECK(resource["resourceLayer"].asString().empty() == false);
                CHECK(resource["resourceLayerHint"].asString().empty() == false);

                if (resource["focusNodeCode"].asString() ==
                    "topological-sort-critical-path-solve")
                {
                    foundCriticalPathSolveResource = true;
                    CHECK(resource["focusNodeLabel"].asString() == "关键路径求解");
                }
            }
        }
    }

    CHECK(foundQueueResource == true);
    CHECK(foundTopologicalSortSection == true);
    CHECK(foundCriticalPathSolveResource == true);
}

DROGON_TEST(PathPlanningServiceBuildsDetailScopePathResponse)
{
    Json::Value request(Json::objectValue);
    request["scopeCode"] = "queue";
    request["targetNodeCode"] = "queue-circular";
    request["availableMinutes"] = 40;
    request["masteryByCode"]["queue-definition"] = 0.7;
    request["masteryByCode"]["queue-adt"] = 0.55;
    request["masteryByCode"]["queue-seq"] = 0.35;
    request["masteryByCode"]["queue-circular"] = 0.1;

    const auto payload =
        services::PathPlanningService::buildDetailPathPayload(request);

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["scope"]["scopeCode"].asString() == "queue");
    CHECK(payload["scope"]["isRoot"].asBool() == false);
    CHECK(payload["breadcrumbs"].isArray());
    CHECK(payload["meta"]["knowledgePointCount"].asInt() > 0);
    CHECK(payload["path"].isArray());

    bool foundTargetItem = false;
    bool foundQueueCircularResource = false;
    for (const auto &item : payload["path"])
    {
        if (item["code"].asString() == "queue-circular")
        {
            foundTargetItem = true;
            CHECK(item["isTarget"].asBool() == true);
            CHECK(item["learningResources"].isArray());
            CHECK(item["learningResources"].empty() == false);

            for (const auto &resource : item["learningResources"])
            {
                if (resource["focusNodeCode"].asString() == "queue-circular")
                {
                    foundQueueCircularResource = true;
                    CHECK(resource["selectionReason"].asString().empty() == false);
                }
            }
        }
    }

    CHECK(foundTargetItem == true);
    CHECK(foundQueueCircularResource == true);
}

DROGON_TEST(PathPlanningServiceUsesDefaultDetailScopeTargetWhenMissing)
{
    Json::Value request(Json::objectValue);
    request["scopeCode"] = "kmp";
    request["availableMinutes"] = 45;

    const auto payload =
        services::PathPlanningService::buildDetailPathPayload(request);

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["targetCodes"].isArray());
    CHECK(payload["targetCodes"].size() == 1);
    CHECK(payload["targetCodes"][0].asString() == "kmp-complexity");
}

DROGON_TEST(PathPlanningServiceAdjustsDetailScopePathWithFeedback)
{
    Json::Value request(Json::objectValue);
    request["scopeCode"] = "queue";
    request["targetNodeCode"] = "queue-circular";
    request["availableMinutes"] = 40;
    request["masteryByCode"]["queue-definition"] = 0.6;
    request["masteryByCode"]["queue-adt"] = 0.45;
    request["masteryByCode"]["queue-seq"] = 0.2;
    request["masteryByCode"]["queue-circular"] = 0.1;
    request["feedbackItems"].append(Json::Value(Json::objectValue));
    request["feedbackItems"][0]["code"] = "queue-circular";
    request["feedbackItems"][0]["completionStatus"] = "completed";
    request["feedbackItems"][0]["selfRatedMastery"] = 0.9;

    const auto payload =
        services::PathPlanningService::buildAdjustedDetailPathPayload(request);

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["scope"]["scopeCode"].asString() == "queue");
    CHECK(payload["feedbackSummary"]["feedbackItemCount"].asInt() == 1);
    CHECK(payload["feedbackSummary"]["completedCount"].asInt() == 1);
    CHECK(payload["updatedMasteryByCode"]["queue-circular"].asDouble() >= 0.85);
    CHECK(payload["adjustments"].isArray());
    CHECK(payload["adjustments"].size() == 1);
    CHECK(payload["adjustments"][0]["code"].asString() == "queue-circular");
    CHECK(payload["adjustments"][0]["adjustmentReasons"].isArray());
}
