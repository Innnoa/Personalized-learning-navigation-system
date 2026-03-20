#include "algorithm/explainer/LearningPathExplainer.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace
{
int toPercent(double score)
{
    return static_cast<int>(std::round(std::clamp(score, 0.0, 1.0) * 100.0));
}

void appendUnique(std::vector<std::string> &values, const std::string &value)
{
    if (value.empty())
    {
        return;
    }

    if (std::find(values.begin(), values.end(), value) == values.end())
    {
        values.push_back(value);
    }
}

std::string buildRolePhrase(const algorithm::planner::LearningPathItem &item)
{
    if (item.isTarget)
    {
        return "当前学习目标";
    }

    if (item.isPrerequisite)
    {
        return "目标前置知识点";
    }

    return "当前学习链路中的相关节点";
}

std::string buildStatusPhrase(
    const algorithm::planner::LearningPathItem &item)
{
    if (item.status == "mastered")
    {
        return "当前已作为已掌握节点保留";
    }

    if (item.status == "scheduled")
    {
        return "已纳入本轮学习安排";
    }

    if (item.status == "deferred")
    {
        return "受当前时间预算限制，建议放入下一轮学习";
    }

    return "已纳入当前学习路径";
}
}

namespace algorithm::explainer
{
PathExplanation explainLearningPathItem(
    const planner::LearningPathItem &item)
{
    PathExplanation explanation;

    appendUnique(explanation.labels, item.isTarget ? "目标节点" : "");
    appendUnique(explanation.labels,
                 item.isPrerequisite ? "前置依赖" : "");
    appendUnique(explanation.labels,
                 item.status == "mastered"
                     ? "已掌握"
                     : (item.status == "scheduled" ? "本轮学习" : ""));
    appendUnique(explanation.labels,
                 item.status == "deferred" ? "下一轮建议" : "");

    if (item.reasonTrace.masteryGap >= 0.5 && item.status != "mastered")
    {
        appendUnique(explanation.labels, "掌握度待提升");
    }

    if (item.reasonTrace.relevanceScore >= 0.8)
    {
        appendUnique(explanation.labels, "高目标相关");
    }

    std::ostringstream summary;
    summary << item.point.name << "属于" << buildRolePhrase(item) << "，当前掌握度约为 "
            << toPercent(item.masteryScore) << "%，" << buildStatusPhrase(item);

    if (item.status != "mastered" && item.reasonTrace.masteryGap >= 0.5)
    {
        summary << "，仍有较大的补强空间";
    }

    summary << "。";
    explanation.summary = summary.str();

    for (const auto &reason : item.reasonTrace.triggerReasons)
    {
        appendUnique(explanation.bullets, reason);
    }

    {
        std::ostringstream bullet;
        bullet << "与当前目标的相关度约为 "
               << toPercent(item.reasonTrace.relevanceScore) << "%。";
        appendUnique(explanation.bullets, bullet.str());
    }

    {
        std::ostringstream bullet;
        bullet << "当前掌握度约为 " << toPercent(item.masteryScore)
               << "%，仍有 " << toPercent(item.reasonTrace.masteryGap)
               << "% 的提升空间。";
        appendUnique(explanation.bullets, bullet.str());
    }

    {
        std::ostringstream bullet;
        bullet << "预计学习用时 " << item.point.estimatedMinutes
               << " 分钟，约占当前时间预算的 "
               << toPercent(item.reasonTrace.timeCostPenalty) << "%。";
        appendUnique(explanation.bullets, bullet.str());
    }

    return explanation;
}
}
