#include "utils/TimeUtils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace utils
{
std::string TimeUtils::nowIso8601()
{
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime {};
#if defined(_WIN32)
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S");
    return stream.str();
}
}
