#include "services/DemoResetService.h"

#include "repositories/DemoResetRepository.h"
#include "repositories/LearnerProfileRepository.h"
#include "repositories/ResourceViewRepository.h"
#include "services/LearnerProfileService.h"

#include <drogon/drogon.h>

#include <array>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace
{
struct DemoBaselineSeed
{
    const char *code = "";
    double masteryScore = 0.0;
};

struct DemoBaselineFeedbackSeed
{
    const char *code = "";
    const char *feedbackBatchId = "";
    const char *completionStatus = "";
    double selfRatedMastery = 0.0;
    double previousMastery = 0.0;
    double updatedMastery = 0.0;
    const char *ruleApplied = "";
    const char *recordedAt = "";
};

constexpr std::array<DemoBaselineSeed, 14> kDefaultDemoBaseline = {{
    {"ds-intro", 0.90},
    {"algorithm-analysis", 0.70},
    {"linear-list", 0.82},
    {"sequence-list", 0.58},
    {"linked-list", 0.46},
    {"stack", 0.60},
    {"queue", 0.35},
    {"string", 0.40},
    {"kmp", 0.12},
    {"tree-basic", 0.38},
    {"binary-tree-traversal", 0.18},
    {"huffman-tree", 0.05},
    {"graph-basic", 0.15},
    {"topological-sort", 0.00},
}};

constexpr std::array<DemoBaselineFeedbackSeed, 4> kDefaultDemoFeedbackBaseline = {{
    {"sequence-list", "seed-demo-learner-seq", "partial", 0.34, 0.22, 0.28, "seeded-demo-progress", "2026-06-10 09:30:00"},
    {"linked-list", "seed-demo-learner-linked", "partial", 0.54, 0.31, 0.48, "seeded-demo-progress", "2026-06-12 15:20:00"},
    {"queue", "seed-demo-learner-queue", "completed", 0.61, 0.39, 0.56, "seeded-demo-progress", "2026-06-16 11:40:00"},
    {"tree-basic", "seed-demo-learner-tree", "completed", 0.88, 0.43, 0.92, "seeded-demo-progress", "2026-06-19 18:10:00"},
}};

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
}

namespace services
{
Json::Value DemoResetService::resetDemoState(const Json::Value &requestJson)
{
    const auto learnerCode = resolveLearnerCode(requestJson);
    const auto targetLearnerCode =
        learnerCode.empty() ? readDefaultLearnerCode() : learnerCode;

    const auto learner =
        repositories::LearnerProfileRepository::findLearnerByCode(
            targetLearnerCode);
    if (!learner.has_value())
    {
        throw std::invalid_argument("未找到指定学习者，无法恢复演示初始状态。");
    }

    const auto masteryRecords =
        repositories::LearnerProfileRepository::listMasteryByLearnerAndCourseId(
            learner->id, learner->targetCourseId);
    if (masteryRecords.empty())
    {
        throw std::runtime_error("未找到学习者当前课程的掌握度记录。");
    }

    std::unordered_map<std::string, int> knowledgePointIdByCode;
    knowledgePointIdByCode.reserve(masteryRecords.size());
    for (const auto &record : masteryRecords)
    {
        knowledgePointIdByCode[record.code] = record.knowledgePointId;
    }

    std::vector<repositories::DemoBaselineMasteryWrite> baselineMastery;
    baselineMastery.reserve(kDefaultDemoBaseline.size());
    std::vector<repositories::DemoBaselineFeedbackWrite> baselineFeedbackRecords;
    baselineFeedbackRecords.reserve(kDefaultDemoFeedbackBaseline.size());

    for (const auto &seed : kDefaultDemoBaseline)
    {
        const auto iter = knowledgePointIdByCode.find(seed.code);
        if (iter == knowledgePointIdByCode.end())
        {
            throw std::runtime_error(
                "演示重置基线引用了不存在的知识点编码。");
        }

        baselineMastery.push_back(
            repositories::DemoBaselineMasteryWrite{
                iter->second,
                seed.masteryScore});
    }

    for (const auto &seed : kDefaultDemoFeedbackBaseline)
    {
        const auto iter = knowledgePointIdByCode.find(seed.code);
        if (iter == knowledgePointIdByCode.end())
        {
            throw std::runtime_error(
                "演示重置反馈基线引用了不存在的知识点编码。");
        }

        baselineFeedbackRecords.push_back(
            repositories::DemoBaselineFeedbackWrite{
                iter->second,
                seed.feedbackBatchId,
                seed.completionStatus,
                seed.selfRatedMastery,
                seed.previousMastery,
                seed.updatedMastery,
                seed.ruleApplied,
                seed.recordedAt});
    }

    const int clearedFeedbackRecordCount =
        repositories::LearnerProfileRepository::countFeedbackRecordsByLearnerId(
            learner->id);
    const int clearedResourceViewRecordCount =
        repositories::ResourceViewRepository::countResourceViewRecordsByLearnerId(
            learner->id);

    repositories::DemoResetRepository::resetLearnerState(
        learner->id, baselineMastery, baselineFeedbackRecords);

    auto payload = LearnerProfileService::buildProfilePayload(learner->code);
    payload["message"] = "已恢复演示初始状态。";
    payload["resetSummary"]["learnerCode"] = learner->code;
    payload["resetSummary"]["clearedFeedbackRecordCount"] =
        clearedFeedbackRecordCount;
    payload["resetSummary"]["clearedResourceViewRecordCount"] =
        clearedResourceViewRecordCount;
    payload["resetSummary"]["restoredMasteryPointCount"] =
        static_cast<int>(baselineMastery.size());

    return payload;
}
}
