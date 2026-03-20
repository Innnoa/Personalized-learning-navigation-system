#include "services/SystemStatusService.h"

#include "utils/TimeUtils.h"

namespace services
{
Json::Value SystemStatusService::buildHealthPayload()
{
    Json::Value payload;
    payload["status"] = "ok";
    payload["service"] = "personalized-learning-navigation-backend";
    payload["course"] = "数据结构";
    payload["timestamp"] = utils::TimeUtils::nowIso8601();
    payload["message"] = "后端服务已启动，可继续接入知识图谱与路径规划接口。";
    return payload;
}
}
