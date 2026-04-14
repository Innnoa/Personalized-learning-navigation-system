#include <drogon/drogon.h>
#include <drogon/drogon_test.h>

#include "services/ResourceViewService.h"
#include "services/LearnerProfileService.h"
#include "services/LearningResourceService.h"
#include "services/PathPlanningService.h"

#include <json/json.h>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace
{
drogon::orm::DbClientPtr getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 测试数据库连接。");
    }

    return client;
}

int findLearnerId(const std::string &learnerCode)
{
    const auto result = getClient()->execSqlSync(
        "select id from learners where code = ? limit 1",
        learnerCode);
    if (result.empty())
    {
        throw std::runtime_error("测试学习者不存在: " + learnerCode);
    }

    return result.front()["id"].as<int>();
}

int findKnowledgePointId(const std::string &knowledgePointCode)
{
    const auto result = getClient()->execSqlSync(
        "select id from knowledge_points where code = ? limit 1",
        knowledgePointCode);
    if (result.empty())
    {
        throw std::runtime_error("测试知识点不存在: " + knowledgePointCode);
    }

    return result.front()["id"].as<int>();
}

void createIsolatedLearnerFromDemo(const std::string &learnerCode,
                                   const std::string &learnerName)
{
    const auto existing = getClient()->execSqlSync(
        "select id from learners where code = ? limit 1",
        learnerCode);
    if (existing.empty())
    {
        getClient()->execSqlSync(
            "insert into learners (code, name, major, grade_label, target_course_id, note) "
            "select ?, ?, major, grade_label, target_course_id, ? "
            "from learners where code = 'demo-learner' limit 1",
            learnerCode,
            learnerName,
            "路径规划测试专用学习者，用于验证资源行为排序。");

        const int learnerId = findLearnerId(learnerCode);
        getClient()->execSqlSync(
            "insert into learner_mastery (learner_id, knowledge_point_id, mastery_score) "
            "select ?, knowledge_point_id, mastery_score "
            "from learner_mastery "
            "where learner_id = (select id from learners where code = 'demo-learner')",
            learnerId);
    }

    getClient()->execSqlSync(
        "delete from detail_learning_feedback_records where learner_id = ?",
        findLearnerId(learnerCode));
    getClient()->execSqlSync(
        "delete from learner_detail_mastery where learner_id = ?",
        findLearnerId(learnerCode));
    getClient()->execSqlSync(
        "delete from learning_resource_view_records where learner_id = ?",
        findLearnerId(learnerCode));
}

double queryCourseMasteryScore(const std::string &learnerCode,
                               const std::string &knowledgePointCode)
{
    const auto result = getClient()->execSqlSync(
        "select lm.mastery_score "
        "from learner_mastery lm "
        "join learners l on l.id = lm.learner_id "
        "join knowledge_points kp on kp.id = lm.knowledge_point_id "
        "where l.code = ? and kp.code = ? limit 1",
        learnerCode,
        knowledgePointCode);
    if (result.empty())
    {
        throw std::runtime_error("未找到课程节点掌握度记录。");
    }

    return result.front()["mastery_score"].as<double>();
}

double queryDetailMasteryScore(const std::string &learnerCode,
                               const std::string &scopeCode,
                               const std::string &nodeCode)
{
    const auto result = getClient()->execSqlSync(
        "select dm.mastery_score "
        "from learner_detail_mastery dm "
        "join learners l on l.id = dm.learner_id "
        "where l.code = ? and dm.scope_code = ? and dm.node_code = ? "
        "limit 1",
        learnerCode,
        scopeCode,
        nodeCode);
    if (result.empty())
    {
        throw std::runtime_error("未找到细化节点掌握度记录。");
    }

    return result.front()["mastery_score"].as<double>();
}
}

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

DROGON_TEST(LearningResourceServiceEnrichesDetailNodeResourcesFromMultipleScopes)
{
    const auto queueDefinitionResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "queue-definition");

    REQUIRE(queueDefinitionResources.isArray());
    CHECK(queueDefinitionResources.empty() == false);

    bool foundFocusedResource = false;
    bool foundInheritedResource = false;
    bool foundRelatedResource = false;
    for (const auto &item : queueDefinitionResources)
    {
        if (item["resourceCoverageType"].asString() == "focused" &&
            item["focusNodeCode"].asString() == "queue-definition")
        {
            foundFocusedResource = true;
        }

        if (item["resourceCoverageType"].asString() == "inherited" &&
            item["inheritedFromCode"].asString() == "queue" &&
            item["inheritedFromLabel"].asString() == "队列")
        {
            foundInheritedResource = true;
        }

        if (item["resourceCoverageType"].asString() == "related" &&
            item["inheritedFromCode"].asString().empty() == false &&
            item["inheritedFromCode"].asString() != "queue-definition")
        {
            foundRelatedResource = true;
        }
    }
    CHECK(foundFocusedResource == true);
    CHECK(foundInheritedResource == true);
    CHECK(foundRelatedResource == true);

    const auto topoDefinitionResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "topological-sort-definition");
    REQUIRE(topoDefinitionResources.isArray());
    CHECK(topoDefinitionResources.empty() == false);

    bool foundFocusedTopoResource = false;
    bool foundInheritedTopoResource = false;
    for (const auto &item : topoDefinitionResources)
    {
        if (item["resourceCoverageType"].asString() == "focused" &&
            item["focusNodeCode"].asString() == "topological-sort-definition")
        {
            foundFocusedTopoResource = true;
        }

        if (item["inheritedFromCode"].asString() == "topological-sort")
        {
            foundInheritedTopoResource = true;
        }
    }
    CHECK(foundFocusedTopoResource == true);
    CHECK(foundInheritedTopoResource == true);
}

DROGON_TEST(LearningResourceServiceProvidesMultipleFocusedResourcesForKeyDetailNodes)
{
    const auto countFocusedResources =
        [](const Json::Value &resources, const std::string &nodeCode) {
            int count = 0;
            for (const auto &item : resources)
            {
                if (item["resourceCoverageType"].asString() == "focused" &&
                    item["focusNodeCode"].asString() == nodeCode)
                {
                    count += 1;
                }
            }
            return count;
        };

    const auto stackResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "stack-push");
    REQUIRE(stackResources.isArray());
    CHECK(countFocusedResources(stackResources, "stack-push") >= 2);

    const auto queueResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "queue-enqueue");
    REQUIRE(queueResources.isArray());
    CHECK(countFocusedResources(queueResources, "queue-enqueue") >= 2);

    const auto stringResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "string-matching-problem");
    REQUIRE(stringResources.isArray());
    CHECK(countFocusedResources(stringResources, "string-matching-problem") >= 2);

    const auto treeResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "tree-basic-binary-storage");
    REQUIRE(treeResources.isArray());
    CHECK(countFocusedResources(treeResources, "tree-basic-binary-storage") >= 2);

    const auto graphResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "graph-basic-definition");
    REQUIRE(graphResources.isArray());
    CHECK(countFocusedResources(graphResources, "graph-basic-definition") >= 2);

    const auto topoResources =
        services::LearningResourceService::buildResourcesForKnowledgePoint(
            "topological-sort-algorithm");
    REQUIRE(topoResources.isArray());
    CHECK(countFocusedResources(topoResources, "topological-sort-algorithm") >= 2);
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
    CHECK(scheduledResources[0]["linkedReasonLabels"].isArray());
    CHECK(scheduledResources[0]["linkedReasonLabels"].size() >= 1);
    CHECK(scheduledResources[0]["linkedReasonLabelSummary"].asString().find(
              "对应推荐理由") != std::string::npos);
    CHECK(scheduledResources[0]["linkedReasonLabels"][0].asString() ==
          "本轮学习");
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
    CHECK(deferredResources[0]["linkedReasonLabels"].isArray());
    CHECK(deferredResources[0]["linkedReasonLabels"][0].asString() ==
          "下一轮建议");
}

DROGON_TEST(LearningResourceServiceDiversifiesTopRecommendations)
{
    algorithm::planner::LearningPathItem scheduledItem;
    scheduledItem.point.code = "queue";
    scheduledItem.point.name = "队列";
    scheduledItem.status = "scheduled";
    scheduledItem.masteryScore = 0.2;
    scheduledItem.reasonTrace.knowledgePointCode = "queue";
    scheduledItem.reasonTrace.relevanceScore = 0.74;
    scheduledItem.reasonTrace.masteryGap = 0.8;
    scheduledItem.reasonTrace.timeCostPenalty = 0.24;
    scheduledItem.reasonTrace.triggerReasons = {"当前节点尚未掌握，需要先建立队列的整体概念。"};

    const auto resources =
        services::LearningResourceService::buildResourcesForLearningPathItem(
            scheduledItem);

    REQUIRE(resources.isArray());
    REQUIRE(resources.size() >= 4);
    CHECK(resources[0]["isPrimaryRecommendation"].asBool() == true);

    std::unordered_set<std::string> uniqueSources;
    std::unordered_set<std::string> uniquePhases;
    for (Json::ArrayIndex index = 0; index < 4; ++index)
    {
        uniqueSources.insert(resources[index]["source"].asString());
        uniquePhases.insert(resources[index]["recommendedPhase"].asString());
    }

    CHECK(uniqueSources.size() >= 3);
    CHECK(uniquePhases.size() >= 2);
}

DROGON_TEST(LearningResourceServiceReordersResourcesByLatestInteraction)
{
    const std::string learnerCode = "path-resource-behavior-test-learner";
    createIsolatedLearnerFromDemo(learnerCode, "路径资源行为测试学习者");

    Json::Value recordRequest(Json::objectValue);
    recordRequest["learnerCode"] = learnerCode;
    recordRequest["knowledgePointCode"] = "sequence-list";
    recordRequest["resourceTitle"] = "数据结构和算法入门课";
    recordRequest["resourceUrl"] = "https://www.bilibili.com/video/BV1URFfegEBC/";
    recordRequest["resourceType"] = "video";
    recordRequest["resourceSource"] = "哔哩哔哩 · 纸飞机旅行家";
    recordRequest["resourceLayer"] = "课程风格优先";
    recordRequest["recommendedPhase"] = "主学";
    recordRequest["sourceContext"] = "main_path_resource_panel";
    recordRequest["scopeCode"] = "root";
    recordRequest["linkedReasonLabelSummary"] =
        "对应推荐理由：本轮学习 / 掌握度待提升";
    recordRequest["interactionType"] = "completed";
    services::ResourceViewService::recordResourceView(recordRequest);

    algorithm::planner::LearningPathItem scheduledItem;
    scheduledItem.point.dbId = findKnowledgePointId("sequence-list");
    scheduledItem.point.code = "sequence-list";
    scheduledItem.point.name = "顺序表";
    scheduledItem.status = "scheduled";
    scheduledItem.masteryScore = 0.25;
    scheduledItem.reasonTrace.knowledgePointCode = "sequence-list";
    scheduledItem.reasonTrace.relevanceScore = 0.82;
    scheduledItem.reasonTrace.masteryGap = 0.75;
    scheduledItem.reasonTrace.timeCostPenalty = 0.28;
    scheduledItem.reasonTrace.triggerReasons = {"当前节点尚未掌握，需要先建立核心操作直觉。"};

    const auto resources =
        services::LearningResourceService::buildResourcesForLearningPathItem(
            scheduledItem, learnerCode);

    REQUIRE(resources.isArray());
    REQUIRE(resources.size() >= 2);

    bool foundCompletedResource = false;
    for (const auto &resource : resources)
    {
        if (resource["title"].asString() != "数据结构和算法入门课")
        {
            continue;
        }

        foundCompletedResource = true;
        CHECK(resource["recommendationRank"].asInt() >= 2);
        CHECK(resource["lastInteractionType"].asString() == "completed");
        CHECK(resource["lastInteractionLabel"].asString() == "已学完");
        CHECK(resource["behaviorAdjustmentHint"].asString().find(
                  "已将这条资源记为学完") != std::string::npos);
    }

    CHECK(foundCompletedResource == true);
}

DROGON_TEST(PathPlanningServiceBuildsLearningResourcesIntoResponse)
{
    const std::string learnerCode = "path-planning-stable-learner";
    createIsolatedLearnerFromDemo(learnerCode, "路径规划稳定测试学习者");

    Json::Value request(Json::objectValue);
    request["learnerCode"] = learnerCode;
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
    CHECK(payload["overallExplanation"].isObject());
    CHECK(payload["overallExplanation"]["summary"].asString().empty() == false);
    CHECK(payload["overallExplanation"]["bullets"].isArray());
    CHECK(payload["overallExplanation"]["bullets"].size() >= 3);
    CHECK(payload["overallExplanation"]["labels"].isArray());
    CHECK(payload["overallExplanation"]["summary"]
              .asString()
              .find("当前目标为") != std::string::npos);

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
                CHECK(resource["linkedReasonLabels"].isArray());
                CHECK(resource["linkedReasonLabelSummary"].asString().empty() ==
                      false);

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

DROGON_TEST(PathPlanningServiceIncludesLatestResourceBehaviorInResponse)
{
    const std::string learnerCode = "path-response-resource-behavior-learner";
    createIsolatedLearnerFromDemo(learnerCode, "路径响应资源行为测试学习者");

    Json::Value recordRequest(Json::objectValue);
    recordRequest["learnerCode"] = learnerCode;
    recordRequest["knowledgePointCode"] = "sequence-list";
    recordRequest["resourceTitle"] = "数据结构和算法入门课";
    recordRequest["resourceUrl"] = "https://www.bilibili.com/video/BV1URFfegEBC/";
    recordRequest["resourceType"] = "video";
    recordRequest["resourceSource"] = "哔哩哔哩 · 纸飞机旅行家";
    recordRequest["resourceLayer"] = "课程风格优先";
    recordRequest["recommendedPhase"] = "主学";
    recordRequest["sourceContext"] = "main_path_resource_panel";
    recordRequest["scopeCode"] = "root";
    recordRequest["linkedReasonLabelSummary"] =
        "对应推荐理由：本轮学习 / 掌握度待提升";
    recordRequest["interactionType"] = "completed";
    services::ResourceViewService::recordResourceView(recordRequest);

    Json::Value request(Json::objectValue);
    request["learnerCode"] = learnerCode;
    request["targetCodes"].append("sequence-list");
    request["availableMinutes"] = 90;
    request["masteryByCode"]["ds-intro"] = 0.85;
    request["masteryByCode"]["linear-list"] = 0.7;
    request["masteryByCode"]["sequence-list"] = 0.2;

    const auto payload = services::PathPlanningService::buildPathPayload(request);

    CHECK(payload["status"].asString() == "ok");
    REQUIRE(payload["resourceRecommendations"].isArray());

    bool foundSequenceList = false;
    for (const auto &item : payload["resourceRecommendations"])
    {
        if (item["code"].asString() != "sequence-list")
        {
            continue;
        }

        foundSequenceList = true;
        REQUIRE(item["resources"].isArray());
        REQUIRE(item["resources"].size() >= 2);

        bool foundCompletedResource = false;
        for (const auto &resource : item["resources"])
        {
            if (resource["title"].asString() != "数据结构和算法入门课")
            {
                continue;
            }

            foundCompletedResource = true;
            CHECK(resource["recommendationRank"].asInt() >= 2);
            CHECK(resource["lastInteractionType"].asString() == "completed");
            CHECK(resource["lastInteractionLabel"].asString() == "已学完");
        }

        CHECK(foundCompletedResource == true);
    }

    CHECK(foundSequenceList == true);
}

DROGON_TEST(PathPlanningServiceBuildsDetailScopePathResponse)
{
    const std::string learnerCode = "detail-path-stable-learner";
    createIsolatedLearnerFromDemo(learnerCode, "细化路径稳定测试学习者");

    Json::Value request(Json::objectValue);
    request["learnerCode"] = learnerCode;
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
    const std::string learnerCode = "detail-default-target-stable-learner";
    createIsolatedLearnerFromDemo(learnerCode, "细化默认目标测试学习者");

    Json::Value request(Json::objectValue);
    request["learnerCode"] = learnerCode;
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
    const std::string learnerCode = "detail-adjust-stable-learner";
    createIsolatedLearnerFromDemo(learnerCode, "细化反馈调整测试学习者");
    const double originalQueueMastery =
        queryCourseMasteryScore(learnerCode, "queue");

    Json::Value request(Json::objectValue);
    request["learnerCode"] = learnerCode;
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
    CHECK(payload["feedbackSummary"]["persistedToLearnerProfile"].asBool() ==
          true);
    CHECK(payload["feedbackBatchId"].asString().empty() == false);
    CHECK(payload["updatedMasteryByCode"]["queue-circular"].asDouble() >= 0.85);
    CHECK(payload["adjustments"].isArray());
    CHECK(payload["adjustments"].size() == 1);
    CHECK(payload["adjustments"][0]["code"].asString() == "queue-circular");
    CHECK(payload["adjustments"][0]["adjustmentReasons"].isArray());
    CHECK(queryDetailMasteryScore(learnerCode, "queue", "queue-circular") >= 0.85);
    CHECK(queryDetailMasteryScore(learnerCode, "queue", "queue-definition") ==
          0.6);
    CHECK(queryCourseMasteryScore(learnerCode, "queue") > originalQueueMastery);

    const auto learnerPayload =
        services::LearnerProfileService::buildProfilePayload(learnerCode);
    CHECK(learnerPayload["graphMasteryByCode"]["queue-circular"].asDouble() >=
          0.85);
}
