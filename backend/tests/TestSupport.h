#pragma once

#include <filesystem>

namespace testsupport
{
std::filesystem::path locateProjectRoot();
std::filesystem::path prepareIsolatedTestDatabase();
}
