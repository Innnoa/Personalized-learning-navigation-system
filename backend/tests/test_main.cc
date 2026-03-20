#define DROGON_TEST_MAIN
#include <drogon/drogon_test.h>
#include <drogon/drogon.h>

#include "tests/TestSupport.h"

#include <json/json.h>

int main(int argc, char** argv) 
{
    using namespace drogon;

    const auto projectRoot = testsupport::locateProjectRoot();
    const auto testDbPath = testsupport::prepareIsolatedTestDatabase();

    Json::Value config;
    Json::Value dbClient(Json::objectValue);
    dbClient["name"] = "sqlite_client";
    dbClient["rdbms"] = "sqlite3";
    dbClient["filename"] = testDbPath.string();
    dbClient["is_fast"] = false;
    dbClient["number_of_connections"] = 1;
    dbClient["timeout"] = -1.0;
    config["db_clients"].append(dbClient);
    config["app"]["number_of_threads"] = 1;
    config["app"]["enable_session"] = false;
    config["app"]["document_root"] = "./public";
    config["app"]["use_implicit_page"] = false;
    config["app"]["load_dynamic_views"] = false;
    config["custom_config"]["selected_course"] = "数据结构";
    config["custom_config"]["default_learner_code"] = "demo-learner";
    config["custom_config"]["knowledge_graph_detail_file"] =
        (projectRoot / "backend/config/knowledge_graph_details.json").string();
    config["custom_config"]["learning_resource_file"] =
        (projectRoot / "backend/config/learning_resources.json").string();
    app().loadConfigJson(config);

    std::promise<void> p1;
    std::future<void> f1 = p1.get_future();

    // Start the main loop on another thread
    std::thread thr([&]() {
        // Queues the promise to be fulfilled after starting the loop
        app().getLoop()->queueInLoop([&p1]() { p1.set_value(); });
        app().run();
    });

    // The future is only satisfied after the event loop started
    f1.get();
    int status = test::run(argc, argv);

    // Ask the event loop to shutdown and wait
    app().getLoop()->queueInLoop([]() { app().quit(); });
    thr.join();
    return status;
}
