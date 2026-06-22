#include "services/TeacherCourseEditService.h"

#include <drogon/drogon.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <optional>
#include <stdexcept>

namespace
{
auto getClient()
{
    auto client = drogon::app().getDbClient("sqlite_client");
    if (!client)
    {
        throw std::runtime_error("未找到 sqlite_client 数据库连接。");
    }
    return client;
}

void verifyTeacherCourseAssignment(const std::string &username,
                                   const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select 1 "
        "from teacher_course_assignments tca "
        "join users u on u.id = tca.user_id "
        "join courses c on c.id = tca.course_id "
        "where u.username = ? and c.code = ? limit 1",
        username,
        courseCode);

    if (result.empty())
    {
        throw std::invalid_argument("教师未分配到指定课程。");
    }
}

int findCourseId(const std::string &courseCode)
{
    const auto result = getClient()->execSqlSync(
        "select id from courses where code = ? limit 1", courseCode);
    if (result.empty())
    {
        throw std::invalid_argument("未找到指定课程。");
    }
    return result.front()["id"].as<int>();
}

void validateKnowledgePointCode(const std::string &code)
{
    if (code.empty() || code.size() < 2 || code.size() > 50)
    {
        throw std::invalid_argument(
            "知识点代码长度应为 2-50 个字符。");
    }
    if (code[0] < 'a' || code[0] > 'z')
    {
        throw std::invalid_argument(
            "知识点代码必须以小写字母开头。");
    }
    for (char ch : code)
    {
        if (!((ch >= 'a' && ch <= 'z') ||
              (ch >= '0' && ch <= '9') ||
              ch == '-'))
        {
            throw std::invalid_argument(
                "知识点代码只能包含小写字母、数字和连字符。");
        }
    }
}

std::string getResourceFilePath(const std::string &courseCode)
{
    return "../config/learning_resources_" + courseCode + ".json";
}

Json::Value readResourceFile(const std::string &courseCode)
{
    const auto path = getResourceFilePath(courseCode);
    std::ifstream file(path);
    if (!file.is_open())
    {
        Json::Value empty;
        empty["knowledgePointResources"] = Json::arrayValue;
        return empty;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    std::istringstream stream(content);
    if (!Json::parseFromStream(builder, stream, &root, &errors))
    {
        throw std::runtime_error("学习资源文件 JSON 解析失败: " + errors);
    }
    return root;
}

void writeResourceFile(const std::string &courseCode, const Json::Value &root)
{
    const auto path = getResourceFilePath(courseCode);
    std::ofstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("无法写入学习资源文件: " + path);
    }
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    file << Json::writeString(builder, root);
    file.close();
}

std::optional<int> findKnowledgePointIdByCode(int courseId, const std::string &code)
{
    if (code.empty())
    {
        return std::nullopt;
    }

    const auto result = getClient()->execSqlSync(
        "select id from knowledge_points where code = ? and course_id = ? limit 1",
        code,
        std::to_string(courseId));
    if (result.empty())
    {
        return std::nullopt;
    }

    return result.front()["id"].as<int>();
}

double clampMasteryValue(double value)
{
    return std::clamp(value, 0.0, 1.0);
}

Json::Value parseFocusTagsJson(const std::string &value)
{
    if (value.empty())
    {
        return Json::Value(Json::arrayValue);
    }

    Json::CharReaderBuilder builder;
    Json::Value parsed;
    std::string errors;
    std::istringstream stream(value);
    if (Json::parseFromStream(builder, stream, &parsed, &errors) && parsed.isArray())
    {
        return parsed;
    }

    return Json::Value(Json::arrayValue);
}

Json::Value buildResourcePayloadRow(const drogon::orm::Row &row)
{
    Json::Value resource;
    resource["id"] = row["id"].as<int>();
    resource["knowledgePointCode"] =
        row["knowledge_point_code"].isNull() ? "" : row["knowledge_point_code"].as<std::string>();
    resource["knowledgePointName"] =
        row["knowledge_point_name"].isNull() ? "" : row["knowledge_point_name"].as<std::string>();
    resource["title"] = row["title"].as<std::string>();
    resource["resourceType"] = row["resource_type"].as<std::string>();
    resource["source"] = row["source"].as<std::string>();
    resource["url"] = row["url"].as<std::string>();
    resource["description"] = row["description"].as<std::string>();
    resource["recommendedUsage"] = row["recommended_usage"].as<std::string>();
    resource["recommendedPhase"] = row["recommended_phase"].as<std::string>();
    resource["importanceWeight"] = row["importance_weight"].as<double>();
    resource["estimatedMinutes"] = row["estimated_minutes"].as<int>();
    resource["minMastery"] = row["min_mastery"].as<double>();
    resource["maxMastery"] = row["max_mastery"].as<double>();
    resource["focusTags"] = parseFocusTagsJson(row["focus_tags_json"].as<std::string>());
    resource["status"] = row["status"].as<std::string>();
    resource["displayOrder"] = row["display_order"].as<int>();
    return resource;
}

Json::Value buildGroupedLegacyResourcePayload(
    const Json::Value &resourceRows)
{
    Json::Value groups(Json::arrayValue);
    std::vector<std::string> orderedCodes;
    std::map<std::string, Json::Value> grouped;

    for (const auto &resource : resourceRows)
    {
        const auto knowledgePointCode = resource["knowledgePointCode"].asString();
        const auto groupCode = knowledgePointCode;
        if (grouped.find(groupCode) == grouped.end())
        {
            Json::Value group;
            group["knowledgePointCode"] = knowledgePointCode;
            group["resources"] = Json::arrayValue;
            grouped[groupCode] = group;
            orderedCodes.push_back(groupCode);
        }

        Json::Value legacy;
        legacy["title"] = resource["title"];
        legacy["type"] = resource["resourceType"];
        legacy["source"] = resource["source"];
        legacy["url"] = resource["url"];
        legacy["description"] = resource["description"];
        legacy["recommendedUsage"] = resource["recommendedUsage"];
        legacy["recommendedPhase"] = resource["recommendedPhase"];
        legacy["importanceWeight"] = resource["importanceWeight"];
        legacy["estimatedMinutes"] = resource["estimatedMinutes"];
        legacy["minMastery"] = resource["minMastery"];
        legacy["maxMastery"] = resource["maxMastery"];
        legacy["focusTags"] = resource["focusTags"];
        grouped[groupCode]["resources"].append(legacy);
    }

    for (const auto &code : orderedCodes)
    {
        groups.append(grouped[code]);
    }

    return groups;
}
}

namespace services
{
Json::Value TeacherCourseEditService::createKnowledgePoint(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const std::string code = body.get("code", "").asString();
    if (code.empty())
    {
        throw std::invalid_argument("知识点 code 不能为空。");
    }
    validateKnowledgePointCode(code);

    const std::string name = body.get("name", "").asString();
    if (name.empty())
    {
        throw std::invalid_argument("知识点 name 不能为空。");
    }

    const int chapterNo = body.get("chapterNo", 1).asInt();
    const std::string chapterName = body.get("chapterName", "").asString();
    const std::string nodeType = body.get("nodeType", "concept").asString();
    const int difficultyLevel = body.get("difficultyLevel", 1).asInt();
    const int estimatedMinutes = body.get("estimatedMinutes", 20).asInt();
    int displayOrder = body.get("displayOrder", -1).asInt();

    // Auto-assign display_order if not explicitly provided
    if (displayOrder < 0)
    {
        const auto maxResult = getClient()->execSqlSync(
            "select coalesce(max(display_order), -1) + 1 as next_order "
            "from knowledge_points where course_id = ?",
            std::to_string(courseId));
        displayOrder = maxResult.front()["next_order"].as<int>();
    }

    getClient()->execSqlSync(
        "insert into knowledge_points "
        "(course_id, code, name, chapter_no, chapter_name, node_type, "
        "difficulty_level, estimated_minutes, display_order) "
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?)",
        std::to_string(courseId),
        code,
        name,
        std::to_string(chapterNo),
        chapterName,
        nodeType,
        std::to_string(difficultyLevel),
        std::to_string(estimatedMinutes),
        std::to_string(displayOrder));

    Json::Value payload;
    payload["code"] = code;
    payload["created"] = true;
    return payload;
}

Json::Value TeacherCourseEditService::updateKnowledgePoint(
    const std::string &username,
    const std::string &courseCode,
    const std::string &pointCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto existing = getClient()->execSqlSync(
        "select id from knowledge_points where code = ? and course_id = ? limit 1",
        pointCode,
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定知识点。");
    }

    const int pointId = existing.front()["id"].as<int>();

    if (body.isMember("name"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set name = ? where id = ?",
            body["name"].asString(),
            std::to_string(pointId));
    }
    if (body.isMember("chapterNo"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set chapter_no = ? where id = ?",
            std::to_string(body["chapterNo"].asInt()),
            std::to_string(pointId));
    }
    if (body.isMember("chapterName"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set chapter_name = ? where id = ?",
            body["chapterName"].asString(),
            std::to_string(pointId));
    }
    if (body.isMember("nodeType"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set node_type = ? where id = ?",
            body["nodeType"].asString(),
            std::to_string(pointId));
    }
    if (body.isMember("difficultyLevel"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set difficulty_level = ? where id = ?",
            std::to_string(body["difficultyLevel"].asInt()),
            std::to_string(pointId));
    }
    if (body.isMember("estimatedMinutes"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set estimated_minutes = ? where id = ?",
            std::to_string(body["estimatedMinutes"].asInt()),
            std::to_string(pointId));
    }
    if (body.isMember("displayOrder"))
    {
        getClient()->execSqlSync(
            "update knowledge_points set display_order = ? where id = ?",
            std::to_string(body["displayOrder"].asInt()),
            std::to_string(pointId));
    }

    Json::Value payload;
    payload["updated"] = true;
    return payload;
}

Json::Value TeacherCourseEditService::deleteKnowledgePoint(
    const std::string &username,
    const std::string &courseCode,
    const std::string &pointCode)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto existing = getClient()->execSqlSync(
        "select id from knowledge_points where code = ? and course_id = ? limit 1",
        pointCode,
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定知识点。");
    }

    getClient()->execSqlSync(
        "delete from knowledge_points where code = ? and course_id = ?",
        pointCode,
        std::to_string(courseId));

    Json::Value payload;
    payload["deleted"] = true;
    return payload;
}

Json::Value TeacherCourseEditService::readDependencies(
    const std::string &username,
    const std::string &courseCode)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto result = getClient()->execSqlSync(
        "select kp_target.code as target_code, kp_pre.code as prerequisite_code "
        "from knowledge_dependencies kd "
        "join knowledge_points kp_target on kp_target.id = kd.target_point_id "
        "join knowledge_points kp_pre on kp_pre.id = kd.prerequisite_point_id "
        "where kd.course_id = ?",
        std::to_string(courseId));

    Json::Value payload;
    payload["courseCode"] = courseCode;
    Json::Value deps(Json::arrayValue);

    for (const auto &row : result)
    {
        Json::Value dep;
        dep["knowledgePointCode"] = row["target_code"].as<std::string>();
        dep["prerequisiteCode"] = row["prerequisite_code"].as<std::string>();
        deps.append(dep);
    }

    payload["dependencies"] = deps;
    return payload;
}

Json::Value TeacherCourseEditService::updateDependencies(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    // Delete all existing dependencies for this course
    getClient()->execSqlSync(
        "delete from knowledge_dependencies where course_id = ?",
        std::to_string(courseId));

    const auto &deps = body["dependencies"];
    if (!deps.isArray())
    {
        throw std::invalid_argument("dependencies 必须为数组。");
    }

    int count = 0;
    for (const auto &dep : deps)
    {
        const std::string targetCode = dep["knowledgePointCode"].asString();
        const std::string preCode = dep["prerequisiteCode"].asString();

        const auto targetResult = getClient()->execSqlSync(
            "select id from knowledge_points where code = ? and course_id = ? limit 1",
            targetCode,
            std::to_string(courseId));
        const auto preResult = getClient()->execSqlSync(
            "select id from knowledge_points where code = ? and course_id = ? limit 1",
            preCode,
            std::to_string(courseId));

        if (!targetResult.empty() && !preResult.empty())
        {
            getClient()->execSqlSync(
                "insert into knowledge_dependencies "
                "(course_id, prerequisite_point_id, target_point_id) "
                "values (?, ?, ?)",
                std::to_string(courseId),
                std::to_string(preResult.front()["id"].as<int>()),
                std::to_string(targetResult.front()["id"].as<int>()));
            ++count;
        }
    }

    Json::Value payload;
    payload["updated"] = true;
    payload["count"] = count;
    return payload;
}

Json::Value TeacherCourseEditService::readResources(
    const std::string &username,
    const std::string &courseCode)
{
    const auto listed = listResources(username, courseCode, Json::Value(Json::objectValue));

    Json::Value payload;
    payload["courseCode"] = courseCode;
    payload["resources"] = listed["resources"];
    payload["knowledgePointResources"] =
        buildGroupedLegacyResourcePayload(listed["resources"]);
    return payload;
}

Json::Value TeacherCourseEditService::listResources(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &query)
{
    (void)query;
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto result = getClient()->execSqlSync(
        "select lr.id, lr.title, lr.resource_type, lr.source, lr.url, lr.description, "
        "lr.recommended_usage, lr.recommended_phase, lr.importance_weight, "
        "lr.estimated_minutes, lr.min_mastery, lr.max_mastery, lr.focus_tags_json, "
        "lr.status, lr.display_order, kp.code as knowledge_point_code, kp.name as knowledge_point_name "
        "from learning_resources lr "
        "left join knowledge_points kp on kp.id = lr.knowledge_point_id "
        "where lr.course_id = ? "
        "order by lr.display_order asc, lr.id asc",
        std::to_string(courseId));

    Json::Value payload;
    payload["courseCode"] = courseCode;
    Json::Value resources(Json::arrayValue);
    for (const auto &row : result)
    {
        resources.append(buildResourcePayloadRow(row));
    }
    payload["resources"] = resources;
    return payload;
}

Json::Value TeacherCourseEditService::createResource(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto title = body.get("title", "").asString();
    if (title.empty())
    {
        throw std::invalid_argument("资源标题不能为空。");
    }

    const auto pointId =
        findKnowledgePointIdByCode(courseId, body.get("knowledgePointCode", "").asString());

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    const auto focusTagsJson = Json::writeString(builder, body["focusTags"]);
    const auto minMastery = clampMasteryValue(body.get("minMastery", 0.0).asDouble());
    const auto maxMastery = clampMasteryValue(body.get("maxMastery", 1.0).asDouble());

    const auto nextOrderResult = getClient()->execSqlSync(
        "select coalesce(max(display_order), -1) + 1 as next_order "
        "from learning_resources where course_id = ?",
        std::to_string(courseId));
    const auto displayOrder = nextOrderResult.front()["next_order"].as<int>();

    getClient()->execSqlSync(
        "insert into learning_resources "
        "(course_id, knowledge_point_id, title, resource_type, source, url, description, "
        "recommended_usage, recommended_phase, importance_weight, estimated_minutes, "
        "min_mastery, max_mastery, focus_tags_json, display_order, status) "
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
        std::to_string(courseId),
        pointId.has_value() ? std::to_string(*pointId) : nullptr,
        title,
        body.get("resourceType", "article").asString(),
        body.get("source", "").asString(),
        body.get("url", "").asString(),
        body.get("description", "").asString(),
        body.get("recommendedUsage", "").asString(),
        body.get("recommendedPhase", "learn").asString(),
        std::to_string(body.get("importanceWeight", 1.0).asDouble()),
        std::to_string(std::max(1, body.get("estimatedMinutes", 20).asInt())),
        std::to_string(std::min(minMastery, maxMastery)),
        std::to_string(std::max(minMastery, maxMastery)),
        focusTagsJson,
        std::to_string(displayOrder),
        body.get("status", "active").asString());

    const auto inserted = getClient()->execSqlSync(
        "select lr.id, lr.title, lr.resource_type, lr.source, lr.url, lr.description, "
        "lr.recommended_usage, lr.recommended_phase, lr.importance_weight, "
        "lr.estimated_minutes, lr.min_mastery, lr.max_mastery, lr.focus_tags_json, "
        "lr.status, lr.display_order, kp.code as knowledge_point_code, kp.name as knowledge_point_name "
        "from learning_resources lr "
        "left join knowledge_points kp on kp.id = lr.knowledge_point_id "
        "where lr.id = last_insert_rowid() limit 1");

    Json::Value payload;
    payload["created"] = true;
    payload["resource"] = buildResourcePayloadRow(inserted.front());
    return payload;
}

Json::Value TeacherCourseEditService::updateResource(
    const std::string &username,
    const std::string &courseCode,
    int resourceId,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto existing = getClient()->execSqlSync(
        "select id from learning_resources where id = ? and course_id = ? limit 1",
        std::to_string(resourceId),
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定学习资源。");
    }

    if (body.isMember("knowledgePointCode"))
    {
        const auto pointId =
            findKnowledgePointIdByCode(courseId, body["knowledgePointCode"].asString());
        getClient()->execSqlSync(
            "update learning_resources set knowledge_point_id = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            pointId.has_value() ? std::to_string(*pointId) : nullptr,
            std::to_string(resourceId));
    }
    if (body.isMember("title"))
    {
        getClient()->execSqlSync(
            "update learning_resources set title = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["title"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("resourceType"))
    {
        getClient()->execSqlSync(
            "update learning_resources set resource_type = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["resourceType"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("source"))
    {
        getClient()->execSqlSync(
            "update learning_resources set source = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["source"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("url"))
    {
        getClient()->execSqlSync(
            "update learning_resources set url = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["url"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("description"))
    {
        getClient()->execSqlSync(
            "update learning_resources set description = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["description"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("recommendedUsage"))
    {
        getClient()->execSqlSync(
            "update learning_resources set recommended_usage = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["recommendedUsage"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("recommendedPhase"))
    {
        getClient()->execSqlSync(
            "update learning_resources set recommended_phase = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["recommendedPhase"].asString(),
            std::to_string(resourceId));
    }
    if (body.isMember("importanceWeight"))
    {
        getClient()->execSqlSync(
            "update learning_resources set importance_weight = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(std::max(0.0, body["importanceWeight"].asDouble())),
            std::to_string(resourceId));
    }
    if (body.isMember("estimatedMinutes"))
    {
        getClient()->execSqlSync(
            "update learning_resources set estimated_minutes = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(std::max(1, body["estimatedMinutes"].asInt())),
            std::to_string(resourceId));
    }
    if (body.isMember("minMastery"))
    {
        getClient()->execSqlSync(
            "update learning_resources set min_mastery = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(clampMasteryValue(body["minMastery"].asDouble())),
            std::to_string(resourceId));
    }
    if (body.isMember("maxMastery"))
    {
        getClient()->execSqlSync(
            "update learning_resources set max_mastery = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            std::to_string(clampMasteryValue(body["maxMastery"].asDouble())),
            std::to_string(resourceId));
    }
    if (body.isMember("focusTags"))
    {
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        getClient()->execSqlSync(
            "update learning_resources set focus_tags_json = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            Json::writeString(builder, body["focusTags"]),
            std::to_string(resourceId));
    }
    if (body.isMember("status"))
    {
        getClient()->execSqlSync(
            "update learning_resources set status = ?, updated_at = CURRENT_TIMESTAMP where id = ?",
            body["status"].asString(),
            std::to_string(resourceId));
    }

    const auto updated = getClient()->execSqlSync(
        "select lr.id, lr.title, lr.resource_type, lr.source, lr.url, lr.description, "
        "lr.recommended_usage, lr.recommended_phase, lr.importance_weight, "
        "lr.estimated_minutes, lr.min_mastery, lr.max_mastery, lr.focus_tags_json, "
        "lr.status, lr.display_order, kp.code as knowledge_point_code, kp.name as knowledge_point_name "
        "from learning_resources lr "
        "left join knowledge_points kp on kp.id = lr.knowledge_point_id "
        "where lr.id = ? limit 1",
        std::to_string(resourceId));

    Json::Value payload;
    payload["updated"] = true;
    payload["resource"] = buildResourcePayloadRow(updated.front());
    return payload;
}

Json::Value TeacherCourseEditService::deleteResource(
    const std::string &username,
    const std::string &courseCode,
    int resourceId)
{
    verifyTeacherCourseAssignment(username, courseCode);
    const int courseId = findCourseId(courseCode);

    const auto existing = getClient()->execSqlSync(
        "select id from learning_resources where id = ? and course_id = ? limit 1",
        std::to_string(resourceId),
        std::to_string(courseId));
    if (existing.empty())
    {
        throw std::invalid_argument("未找到指定学习资源。");
    }

    getClient()->execSqlSync(
        "delete from learning_resources where id = ? and course_id = ?",
        std::to_string(resourceId),
        std::to_string(courseId));

    Json::Value payload;
    payload["deleted"] = true;
    return payload;
}

Json::Value TeacherCourseEditService::writeResources(
    const std::string &username,
    const std::string &courseCode,
    const Json::Value &body)
{
    verifyTeacherCourseAssignment(username, courseCode);

    writeResourceFile(courseCode, body);

    Json::Value payload;
    payload["saved"] = true;
    int count = 0;
    if (body.isMember("knowledgePointResources") && body["knowledgePointResources"].isArray())
    {
        for (const auto &kpr : body["knowledgePointResources"])
        {
            if (kpr.isMember("resources") && kpr["resources"].isArray())
            {
                count += static_cast<int>(kpr["resources"].size());
            }
        }
    }
    payload["count"] = count;
    return payload;
}

Json::Value TeacherCourseEditService::createCourse(
    const std::string &username,
    const Json::Value &body)
{
    const std::string code = body.get("code", "").asString();
    const std::string name = body.get("name", "").asString();
    const std::string description = body.get("description", "").asString();
    const std::string targetAudience = body.get("targetAudience", "").asString();

    if (code.empty() || name.empty())
    {
        throw std::invalid_argument("课程代码和名称不能为空。");
    }

    // Validate code format
    for (char ch : code)
    {
        if (!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '-'))
        {
            throw std::invalid_argument("课程代码只能包含小写字母、数字和连字符。");
        }
    }

    // Check duplicate
    const auto existing = getClient()->execSqlSync(
        "select 1 from courses where code = ? limit 1", code);
    if (!existing.empty())
    {
        throw std::invalid_argument("课程代码 " + code + " 已存在。");
    }

    getClient()->execSqlSync(
        "insert into courses (code, name, description, target_audience, created_at) "
        "values (?, ?, ?, ?, datetime('now','localtime'))",
        code, name, description, targetAudience);

    // Assign the creating teacher to this course
    const auto userResult = getClient()->execSqlSync(
        "select id from users where username = ? limit 1", username);
    if (!userResult.empty())
    {
        const auto courseResult = getClient()->execSqlSync(
            "select id from courses where code = ? limit 1", code);
        if (!courseResult.empty())
        {
            getClient()->execSqlSync(
                "insert into teacher_course_assignments (user_id, course_id) "
                "values (?, ?)",
                std::to_string(userResult.front()["id"].as<int>()),
                std::to_string(courseResult.front()["id"].as<int>()));
        }
    }

    Json::Value payload;
    payload["code"] = code;
    payload["created"] = true;
    return payload;
}
}
