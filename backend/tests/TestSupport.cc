#include "tests/TestSupport.h"

#include <sqlite3.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace
{
std::string readTextFile(const std::filesystem::path &filePath)
{
    std::ifstream input(filePath);
    if (!input.is_open())
    {
        throw std::runtime_error("无法读取测试所需文件: " + filePath.string());
    }

    return std::string(std::istreambuf_iterator<char>(input),
                       std::istreambuf_iterator<char>());
}

void executeSqlScript(sqlite3 *db,
                      const std::string &script,
                      const std::filesystem::path &scriptPath)
{
    char *errorMessage = nullptr;
    const int result = sqlite3_exec(db, script.c_str(), nullptr, nullptr, &errorMessage);
    if (result == SQLITE_OK)
    {
        return;
    }

    std::string message =
        errorMessage != nullptr ? errorMessage : "未知 SQLite 错误";
    if (errorMessage != nullptr)
    {
        sqlite3_free(errorMessage);
    }

    throw std::runtime_error("执行 SQL 脚本失败: " + scriptPath.string() +
                             "，错误: " + message);
}
}

namespace testsupport
{
std::filesystem::path locateProjectRoot()
{
    auto currentPath = std::filesystem::current_path();

    while (true)
    {
        if (std::filesystem::exists(currentPath / "database/init/001_schema.sql") &&
            std::filesystem::exists(currentPath / "database/seed/course_data_structures.sql"))
        {
            return currentPath;
        }

        if (currentPath == currentPath.root_path())
        {
            break;
        }

        currentPath = currentPath.parent_path();
    }

    throw std::runtime_error("未找到项目根目录，无法定位测试数据库脚本。");
}

std::filesystem::path prepareIsolatedTestDatabase()
{
    const auto projectRoot = locateProjectRoot();
    const auto schemaPath = projectRoot / "database/init/001_schema.sql";
    const auto seedPath = projectRoot / "database/seed/course_data_structures.sql";
    const auto questionBankSeedPath =
        projectRoot / "database/seed/question_banks_data_structures.sql";
    const auto resourceSeedPath =
        projectRoot / "database/seed/learning_resources_data_structures.sql";
    const auto dbPath = projectRoot / "backend/build/tests/test_app.db";

    std::filesystem::create_directories(dbPath.parent_path());
    std::filesystem::remove(dbPath);

    sqlite3 *db = nullptr;
    if (sqlite3_open(dbPath.string().c_str(), &db) != SQLITE_OK)
    {
        const std::string message =
            db != nullptr ? sqlite3_errmsg(db) : "未知 SQLite 打开错误";
        if (db != nullptr)
        {
            sqlite3_close(db);
        }
        throw std::runtime_error("无法创建测试数据库: " + message);
    }

    try
    {
        executeSqlScript(db, readTextFile(schemaPath), schemaPath);
        executeSqlScript(db, readTextFile(seedPath), seedPath);
        if (std::filesystem::exists(questionBankSeedPath))
        {
            executeSqlScript(db, readTextFile(questionBankSeedPath), questionBankSeedPath);
        }
        if (std::filesystem::exists(resourceSeedPath))
        {
            executeSqlScript(db, readTextFile(resourceSeedPath), resourceSeedPath);
        }
    }
    catch (...)
    {
        sqlite3_close(db);
        throw;
    }

    sqlite3_close(db);
    // 复位 SQLite 全局状态，避免后续 Drogon 初始化连接时再次配置线程模式报错。
    const int shutdownResult = sqlite3_shutdown();
    if (shutdownResult != SQLITE_OK)
    {
        throw std::runtime_error("测试数据库初始化后 sqlite3_shutdown 失败，错误码: " +
                                 std::to_string(shutdownResult));
    }
    return dbPath;
}
}
