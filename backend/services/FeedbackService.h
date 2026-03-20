#pragma once

#include <json/json.h>

namespace services
{
class FeedbackService
{
  public:
    static Json::Value submitFeedback(const Json::Value &requestJson);
    static Json::Value rollbackLatestFeedback(const Json::Value &requestJson);
};
}
