#include <drogon/drogon.h>

int main(int argc, char *argv[])
{
    // 默认从 backend/build 目录运行，支持命令行覆盖配置文件路径。
    const std::string configPath =
        argc > 1 ? argv[1] : "../config/config.json";

    drogon::app().loadConfigFile(configPath);
    drogon::app().run();
    return 0;
}
