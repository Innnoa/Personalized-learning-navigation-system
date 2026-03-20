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

            for (const auto &resource : item["resources"])
            {
                CHECK(resource["selectionReason"].asString().empty() == false);

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
