#include <algorithm>

#include <drogon/drogon_test.h>

#include "algorithm/explainer/LearningPathExplainer.h"

DROGON_TEST(LearningPathExplainerBuildsExplanationFromReasonTrace)
{
    algorithm::planner::LearningPathItem item;
    item.order = 1;
    item.point = {1,
                  "graph-basic",
                  "图的存储与遍历",
                  6,
                  "图",
                  "algorithm",
                  1,
                  45,
                  35,
                  true,
                  "图结构基础内容"};
    item.status = "scheduled";
    item.scheduledNow = true;
    item.isTarget = false;
    item.isPrerequisite = true;
    item.masteryScore = 0.2;
    item.reasonTrace = algorithm::planner::PathReasonTrace{
        "graph-basic",
        false,
        true,
        0.2,
        0.8,
        0.85,
        0.7,
        0.5,
        {"该知识点是目标知识点的前置依赖。",
         "当前掌握度较低，建议优先学习。",
         "已纳入当前时间预算内的学习路径。"}};

    const auto explanation =
        algorithm::explainer::explainLearningPathItem(item);

    CHECK(explanation.summary.find("前置") != std::string::npos);
    CHECK(explanation.summary.find("20%") != std::string::npos);
    CHECK(std::find(explanation.labels.begin(), explanation.labels.end(),
                    "前置依赖") != explanation.labels.end());
    CHECK(std::find(explanation.labels.begin(), explanation.labels.end(),
                    "本轮学习") != explanation.labels.end());
    CHECK(std::find(explanation.labels.begin(), explanation.labels.end(),
                    "掌握度待提升") != explanation.labels.end());
    CHECK(explanation.bullets.size() >= 3);
}
