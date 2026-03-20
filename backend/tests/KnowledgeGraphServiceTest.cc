#include <drogon/drogon_test.h>

#include "services/KnowledgeGraphService.h"

DROGON_TEST(KnowledgeGraphRootPayloadMarksDrilldownNodes)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload();

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "root");
    CHECK(payload["breadcrumbs"].size() == 1U);
    CHECK(payload["meta"]["detailScopeCount"].asInt() >= 14);

    bool foundDsIntroNode = false;
    bool foundAlgorithmAnalysisNode = false;
    bool foundLinearListNode = false;
    bool foundSequenceListNode = false;
    bool foundLinkedListNode = false;
    bool foundStackNode = false;
    bool foundQueueNode = false;
    bool foundStringNode = false;
    bool foundKmpNode = false;
    bool foundTreeBasicNode = false;
    bool foundBinaryTreeTraversalNode = false;
    bool foundHuffmanTreeNode = false;
    bool foundGraphBasicNode = false;
    bool foundTopologicalSortNode = false;
    for (const auto &node : payload["nodes"])
    {
        const auto nodeCode = node["code"].asString();
        if (nodeCode == "ds-intro")
        {
            foundDsIntroNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "ds-intro");
        }
        else if (nodeCode == "algorithm-analysis")
        {
            foundAlgorithmAnalysisNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "algorithm-analysis");
        }
        else if (nodeCode == "linear-list")
        {
            foundLinearListNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "linear-list");
        }
        else if (nodeCode == "sequence-list")
        {
            foundSequenceListNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "sequence-list");
        }
        else if (nodeCode == "linked-list")
        {
            foundLinkedListNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "linked-list");
        }
        else if (nodeCode == "stack")
        {
            foundStackNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "stack");
        }
        else if (nodeCode == "queue")
        {
            foundQueueNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "queue");
        }
        else if (nodeCode == "string")
        {
            foundStringNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "string");
        }
        else if (nodeCode == "kmp")
        {
            foundKmpNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "kmp");
        }
        else if (nodeCode == "tree-basic")
        {
            foundTreeBasicNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "tree-basic");
        }
        else if (nodeCode == "binary-tree-traversal")
        {
            foundBinaryTreeTraversalNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "binary-tree-traversal");
        }
        else if (nodeCode == "huffman-tree")
        {
            foundHuffmanTreeNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "huffman-tree");
        }
        else if (nodeCode == "graph-basic")
        {
            foundGraphBasicNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "graph-basic");
        }
        else if (nodeCode == "topological-sort")
        {
            foundTopologicalSortNode = true;
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "topological-sort");
        }
    }

    CHECK(foundDsIntroNode == true);
    CHECK(foundAlgorithmAnalysisNode == true);
    CHECK(foundLinearListNode == true);
    CHECK(foundSequenceListNode == true);
    CHECK(foundLinkedListNode == true);
    CHECK(foundStackNode == true);
    CHECK(foundQueueNode == true);
    CHECK(foundStringNode == true);
    CHECK(foundKmpNode == true);
    CHECK(foundTreeBasicNode == true);
    CHECK(foundBinaryTreeTraversalNode == true);
    CHECK(foundHuffmanTreeNode == true);
    CHECK(foundGraphBasicNode == true);
    CHECK(foundTopologicalSortNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsDsIntroRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload("ds-intro");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "ds-intro");
    CHECK(payload["view"]["scopeName"].asString() == "数据结构基本概念");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "ds-intro");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 9);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 2);

    bool foundAdtNode = false;
    bool foundLogicalStructureNode = false;
    bool foundStorageStructureNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "ds-intro-adt")
        {
            foundAdtNode = true;
            CHECK(node["label"].asString() == "抽象数据类型");
            CHECK(node["chapterNo"].asInt() == 1);
        }

        if (node["code"].asString() == "ds-intro-logical-structure")
        {
            foundLogicalStructureNode = true;
            CHECK(node["label"].asString() == "逻辑结构");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "ds-intro-logical-structure");
        }

        if (node["code"].asString() == "ds-intro-storage-structure")
        {
            foundStorageStructureNode = true;
            CHECK(node["label"].asString() == "存储结构");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "ds-intro-storage-structure");
        }
    }

    CHECK(foundAdtNode == true);
    CHECK(foundLogicalStructureNode == true);
    CHECK(foundStorageStructureNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsAlgorithmAnalysisRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("algorithm-analysis");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "algorithm-analysis");
    CHECK(payload["view"]["scopeName"].asString() == "算法复杂度分析");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "algorithm-analysis");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 10);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 12);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 2);

    bool foundTimeComplexityNode = false;
    bool foundCaseStudyNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "algorithm-analysis-time-complexity")
        {
            foundTimeComplexityNode = true;
            CHECK(node["label"].asString() == "时间复杂度");
            CHECK(node["nodeType"].asString() == "algorithm");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "algorithm-analysis-time-complexity");
        }

        if (node["code"].asString() == "algorithm-analysis-case-study")
        {
            foundCaseStudyNode = true;
            CHECK(node["label"].asString() == "复杂度案例解析");
            CHECK(node["chapterNo"].asInt() == 1);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "algorithm-analysis-case-study");
        }
    }

    CHECK(foundTimeComplexityNode == true);
    CHECK(foundCaseStudyNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsLinearListRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload("linear-list");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "linear-list");
    CHECK(payload["view"]["scopeName"].asString() == "线性表");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linear-list");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 9);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 11);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 2);

    bool foundAdtNode = false;
    bool foundLinkedRepresentationNode = false;
    bool foundSequentialRepresentationNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "linear-list-adt")
        {
            foundAdtNode = true;
            CHECK(node["label"].asString() == "抽象数据类型");
            CHECK(node["nodeType"].asString() == "abstraction");
        }

        if (node["code"].asString() == "linear-list-linked-representation")
        {
            foundLinkedRepresentationNode = true;
            CHECK(node["label"].asString() == "链式表示");
            CHECK(node["chapterNo"].asInt() == 2);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "linear-list-linked-representation");
        }

        if (node["code"].asString() == "linear-list-sequential-representation")
        {
            foundSequentialRepresentationNode = true;
            CHECK(node["label"].asString() == "顺序表示");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "linear-list-sequential-representation");
        }
    }

    CHECK(foundAdtNode == true);
    CHECK(foundLinkedRepresentationNode == true);
    CHECK(foundSequentialRepresentationNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsSequenceListRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("sequence-list");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "sequence-list");
    CHECK(payload["view"]["scopeName"].asString() == "顺序表");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "sequence-list");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 9);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 12);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 3);

    bool foundInsertNode = false;
    bool foundDeleteNode = false;
    bool foundMergeNode = false;
    bool foundScenarioNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "sequence-list-insert")
        {
            foundInsertNode = true;
            CHECK(node["label"].asString() == "插入操作");
            CHECK(node["nodeType"].asString() == "algorithm");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "sequence-list-insert");
        }

        if (node["code"].asString() == "sequence-list-delete")
        {
            foundDeleteNode = true;
            CHECK(node["label"].asString() == "删除操作");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "sequence-list-delete");
        }

        if (node["code"].asString() == "sequence-list-merge")
        {
            foundMergeNode = true;
            CHECK(node["label"].asString() == "合并操作");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "sequence-list-merge");
        }

        if (node["code"].asString() == "sequence-list-scenarios")
        {
            foundScenarioNode = true;
            CHECK(node["label"].asString() == "适用场景");
            CHECK(node["chapterNo"].asInt() == 2);
        }
    }

    CHECK(foundInsertNode == true);
    CHECK(foundDeleteNode == true);
    CHECK(foundMergeNode == true);
    CHECK(foundScenarioNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsLinkedListRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("linked-list");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "linked-list");
    CHECK(payload["view"]["scopeName"].asString() == "链表");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linked-list");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 10);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 13);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 3);

    bool foundHeadInsertNode = false;
    bool foundTailInsertNode = false;
    bool foundMergeNode = false;
    bool foundCompareNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "linked-list-head-insert")
        {
            foundHeadInsertNode = true;
            CHECK(node["label"].asString() == "头插法建表");
            CHECK(node["nodeType"].asString() == "algorithm");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "linked-list-head-insert");
        }

        if (node["code"].asString() == "linked-list-tail-insert")
        {
            foundTailInsertNode = true;
            CHECK(node["label"].asString() == "尾插法建表");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "linked-list-tail-insert");
        }

        if (node["code"].asString() == "linked-list-merge")
        {
            foundMergeNode = true;
            CHECK(node["label"].asString() == "有序表合并");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "linked-list-merge");
        }

        if (node["code"].asString() == "linked-list-vs-sequence")
        {
            foundCompareNode = true;
            CHECK(node["label"].asString() == "与顺序表比较");
            CHECK(node["chapterNo"].asInt() == 2);
        }
    }

    CHECK(foundHeadInsertNode == true);
    CHECK(foundTailInsertNode == true);
    CHECK(foundMergeNode == true);
    CHECK(foundCompareNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsStackRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("stack");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "stack");
    CHECK(payload["view"]["scopeName"].asString() == "栈");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "stack");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 9);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 11);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 2);

    bool foundPushNode = false;
    bool foundRecursionNode = false;
    bool foundApplicationsNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "stack-push")
        {
            foundPushNode = true;
            CHECK(node["label"].asString() == "入栈操作");
            CHECK(node["nodeType"].asString() == "algorithm");
        }

        if (node["code"].asString() == "stack-recursion")
        {
            foundRecursionNode = true;
            CHECK(node["label"].asString() == "递归工作栈");
            CHECK(node["chapterNo"].asInt() == 3);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "stack-recursion");
        }

        if (node["code"].asString() == "stack-applications")
        {
            foundApplicationsNode = true;
            CHECK(node["label"].asString() == "典型应用");
            CHECK(node["chapterNo"].asInt() == 3);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "stack-applications");
        }
    }

    CHECK(foundPushNode == true);
    CHECK(foundRecursionNode == true);
    CHECK(foundApplicationsNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsQueueRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("queue");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "queue");
    CHECK(payload["view"]["scopeName"].asString() == "队列");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "queue");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 9);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 12);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 3);

    bool foundCircularNode = false;
    bool foundEnqueueNode = false;
    bool foundApplicationNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "queue-circular")
        {
            foundCircularNode = true;
            CHECK(node["label"].asString() == "循环队列");
            CHECK(node["nodeType"].asString() == "implementation");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "queue-circular");
        }

        if (node["code"].asString() == "queue-enqueue")
        {
            foundEnqueueNode = true;
            CHECK(node["label"].asString() == "入队操作");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "queue-enqueue");
        }

        if (node["code"].asString() == "queue-applications")
        {
            foundApplicationNode = true;
            CHECK(node["label"].asString() == "典型应用");
            CHECK(node["chapterNo"].asInt() == 3);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "queue-applications");
        }
    }

    CHECK(foundCircularNode == true);
    CHECK(foundEnqueueNode == true);
    CHECK(foundApplicationNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsStringRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("string");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "string");
    CHECK(payload["view"]["scopeName"].asString() == "串");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "string");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 2);

    bool foundMatchProblemNode = false;
    bool foundNaiveMatchNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "string-matching-problem")
        {
            foundMatchProblemNode = true;
            CHECK(node["label"].asString() == "模式匹配问题");
            CHECK(node["nodeType"].asString() == "application");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "string-matching-problem");
        }

        if (node["code"].asString() == "string-naive-match")
        {
            foundNaiveMatchNode = true;
            CHECK(node["label"].asString() == "简单模式匹配");
            CHECK(node["chapterNo"].asInt() == 4);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "string-naive-match");
        }
    }

    CHECK(foundMatchProblemNode == true);
    CHECK(foundNaiveMatchNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsKmpRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("kmp");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "kmp");
    CHECK(payload["view"]["scopeName"].asString() == "KMP 模式匹配");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "kmp");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 9);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 2);

    bool foundNextNode = false;
    bool foundMainProcessNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "kmp-next-definition")
        {
            foundNextNode = true;
            CHECK(node["label"].asString() == "next 数组含义");
            CHECK(node["nodeType"].asString() == "abstraction");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "kmp-next-definition");
        }

        if (node["code"].asString() == "kmp-main-process")
        {
            foundMainProcessNode = true;
            CHECK(node["label"].asString() == "KMP 主过程");
            CHECK(node["chapterNo"].asInt() == 4);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "kmp-main-process");
        }
    }

    CHECK(foundNextNode == true);
    CHECK(foundMainProcessNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsTreeBasicRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("tree-basic");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "tree-basic");
    CHECK(payload["view"]["scopeName"].asString() == "树与二叉树基础");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "tree-basic");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 1);

    bool foundPropertyNode = false;
    bool foundStorageNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "tree-basic-binary-properties")
        {
            foundPropertyNode = true;
            CHECK(node["label"].asString() == "二叉树性质");
            CHECK(node["nodeType"].asString() == "foundation");
        }

        if (node["code"].asString() == "tree-basic-binary-storage")
        {
            foundStorageNode = true;
            CHECK(node["label"].asString() == "二叉树存储结构");
            CHECK(node["chapterNo"].asInt() == 5);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "tree-basic-binary-storage");
        }
    }

    CHECK(foundPropertyNode == true);
    CHECK(foundStorageNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsDsIntroLogicalStructureRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "ds-intro-logical-structure");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "ds-intro-logical-structure");
    CHECK(payload["view"]["scopeName"].asString() == "逻辑结构");
    CHECK(payload["view"]["parentScopeCode"].asString() == "ds-intro");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "ds-intro");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "ds-intro-logical-structure");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundGraphNode = false;
    bool foundCourseMapNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "ds-intro-logical-structure-graph")
        {
            foundGraphNode = true;
            CHECK(node["label"].asString() == "图状结构");
        }

        if (node["code"].asString() == "ds-intro-logical-structure-course-map")
        {
            foundCourseMapNode = true;
            CHECK(node["label"].asString() == "课程章节映射");
        }
    }

    CHECK(foundGraphNode == true);
    CHECK(foundCourseMapNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsDsIntroStorageStructureRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "ds-intro-storage-structure");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "ds-intro-storage-structure");
    CHECK(payload["view"]["scopeName"].asString() == "存储结构");
    CHECK(payload["view"]["parentScopeCode"].asString() == "ds-intro");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "ds-intro");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "ds-intro-storage-structure");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundLinkedNode = false;
    bool foundChoiceNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "ds-intro-storage-structure-linked")
        {
            foundLinkedNode = true;
            CHECK(node["label"].asString() == "链式存储");
        }

        if (node["code"].asString() == "ds-intro-storage-structure-choice")
        {
            foundChoiceNode = true;
            CHECK(node["label"].asString() == "结构选择因素");
        }
    }

    CHECK(foundLinkedNode == true);
    CHECK(foundChoiceNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsLinearListSequentialRepresentationRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "linear-list-sequential-representation");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "linear-list-sequential-representation");
    CHECK(payload["view"]["scopeName"].asString() == "顺序表示");
    CHECK(payload["view"]["parentScopeCode"].asString() == "linear-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linear-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "linear-list-sequential-representation");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundRandomAccessNode = false;
    bool foundScenarioNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() ==
            "linear-list-sequential-representation-random-access")
        {
            foundRandomAccessNode = true;
            CHECK(node["label"].asString() == "随机访问能力");
        }

        if (node["code"].asString() ==
            "linear-list-sequential-representation-scenario")
        {
            foundScenarioNode = true;
            CHECK(node["label"].asString() == "适用场景判断");
        }
    }

    CHECK(foundRandomAccessNode == true);
    CHECK(foundScenarioNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsLinearListLinkedRepresentationRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "linear-list-linked-representation");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "linear-list-linked-representation");
    CHECK(payload["view"]["scopeName"].asString() == "链式表示");
    CHECK(payload["view"]["parentScopeCode"].asString() == "linear-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linear-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "linear-list-linked-representation");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundInsertDeleteNode = false;
    bool foundHeadPointerNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() ==
            "linear-list-linked-representation-insert-delete")
        {
            foundInsertDeleteNode = true;
            CHECK(node["label"].asString() == "插入删除便捷");
        }

        if (node["code"].asString() ==
            "linear-list-linked-representation-head-pointer")
        {
            foundHeadPointerNode = true;
            CHECK(node["label"].asString() == "头指针入口");
        }
    }

    CHECK(foundInsertDeleteNode == true);
    CHECK(foundHeadPointerNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsQueueCircularRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("queue-circular");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "queue-circular");
    CHECK(payload["view"]["scopeName"].asString() == "循环队列");
    CHECK(payload["view"]["parentScopeCode"].asString() == "queue");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "queue");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "queue-circular");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundFullNode = false;
    bool foundEnqueueNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "queue-circular-full")
        {
            foundFullNode = true;
            CHECK(node["label"].asString() == "判满条件");
        }

        if (node["code"].asString() == "queue-circular-enqueue-update")
        {
            foundEnqueueNode = true;
            CHECK(node["label"].asString() == "入队取模更新");
        }
    }

    CHECK(foundFullNode == true);
    CHECK(foundEnqueueNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsStringNaiveMatchRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("string-naive-match");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "string-naive-match");
    CHECK(payload["view"]["scopeName"].asString() == "简单模式匹配");
    CHECK(payload["view"]["parentScopeCode"].asString() == "string");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "string");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "string-naive-match");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundShiftNode = false;
    bool foundCostNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "string-naive-match-shift-one")
        {
            foundShiftNode = true;
            CHECK(node["label"].asString() == "模式串右移一位");
        }

        if (node["code"].asString() == "string-naive-match-repeat-cost")
        {
            foundCostNode = true;
            CHECK(node["label"].asString() == "重复比较代价");
        }
    }

    CHECK(foundShiftNode == true);
    CHECK(foundCostNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsKmpMainProcessRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("kmp-main-process");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "kmp-main-process");
    CHECK(payload["view"]["scopeName"].asString() == "KMP 主过程");
    CHECK(payload["view"]["parentScopeCode"].asString() == "kmp");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "kmp");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "kmp-main-process");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundFallbackNode = false;
    bool foundStableNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "kmp-main-process-fallback-next")
        {
            foundFallbackNode = true;
            CHECK(node["label"].asString() == "按 next 回退 j");
        }

        if (node["code"].asString() == "kmp-main-process-main-stable")
        {
            foundStableNode = true;
            CHECK(node["label"].asString() == "主串指针不回退");
        }
    }

    CHECK(foundFallbackNode == true);
    CHECK(foundStableNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsTreeBasicBinaryStorageRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "tree-basic-binary-storage");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "tree-basic-binary-storage");
    CHECK(payload["view"]["scopeName"].asString() == "二叉树存储结构");
    CHECK(payload["view"]["parentScopeCode"].asString() == "tree-basic");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "tree-basic");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "tree-basic-binary-storage");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundSeqNode = false;
    bool foundCompareNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "tree-basic-binary-storage-seq-index")
        {
            foundSeqNode = true;
            CHECK(node["label"].asString() == "数组下标关系");
        }

        if (node["code"].asString() == "tree-basic-binary-storage-compare")
        {
            foundCompareNode = true;
            CHECK(node["label"].asString() == "顺序与链式比较");
        }
    }

    CHECK(foundSeqNode == true);
    CHECK(foundCompareNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsAlgorithmAnalysisTimeComplexityRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "algorithm-analysis-time-complexity");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "algorithm-analysis-time-complexity");
    CHECK(payload["view"]["scopeName"].asString() == "时间复杂度");
    CHECK(payload["view"]["parentScopeCode"].asString() == "algorithm-analysis");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "algorithm-analysis");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "algorithm-analysis-time-complexity");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundBigONode = false;
    bool foundGrowthNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "algorithm-analysis-time-complexity-big-o")
        {
            foundBigONode = true;
            CHECK(node["label"].asString() == "大 O 记号");
        }

        if (node["code"].asString() ==
            "algorithm-analysis-time-complexity-growth-compare")
        {
            foundGrowthNode = true;
            CHECK(node["label"].asString() == "渐进增长比较");
        }
    }

    CHECK(foundBigONode == true);
    CHECK(foundGrowthNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsSequenceListInsertRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("sequence-list-insert");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "sequence-list-insert");
    CHECK(payload["view"]["scopeName"].asString() == "插入操作");
    CHECK(payload["view"]["parentScopeCode"].asString() == "sequence-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "sequence-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "sequence-list-insert");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundShiftNode = false;
    bool foundCostNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "sequence-list-insert-shift-backward")
        {
            foundShiftNode = true;
            CHECK(node["label"].asString() == "末尾到前逐位后移");
        }

        if (node["code"].asString() == "sequence-list-insert-time-cost")
        {
            foundCostNode = true;
            CHECK(node["label"].asString() == "时间代价理解");
        }
    }

    CHECK(foundShiftNode == true);
    CHECK(foundCostNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsLinkedListHeadInsertRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "linked-list-head-insert");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "linked-list-head-insert");
    CHECK(payload["view"]["scopeName"].asString() == "头插法建表");
    CHECK(payload["view"]["parentScopeCode"].asString() == "linked-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linked-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "linked-list-head-insert");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundUpdateNode = false;
    bool foundReverseNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "linked-list-head-insert-head-update")
        {
            foundUpdateNode = true;
            CHECK(node["label"].asString() == "头结点改指向新结点");
        }

        if (node["code"].asString() == "linked-list-head-insert-reverse-feature")
        {
            foundReverseNode = true;
            CHECK(node["label"].asString() == "逆序特征理解");
        }
    }

    CHECK(foundUpdateNode == true);
    CHECK(foundReverseNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsStackApplicationsRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("stack-applications");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "stack-applications");
    CHECK(payload["view"]["scopeName"].asString() == "典型应用");
    CHECK(payload["view"]["parentScopeCode"].asString() == "stack");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "stack");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "stack-applications");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundMatchingNode = false;
    bool foundRestoreNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "stack-applications-parenthesis-matching")
        {
            foundMatchingNode = true;
            CHECK(node["label"].asString() == "括号匹配");
        }

        if (node["code"].asString() == "stack-applications-state-restore")
        {
            foundRestoreNode = true;
            CHECK(node["label"].asString() == "状态恢复场景");
        }
    }

    CHECK(foundMatchingNode == true);
    CHECK(foundRestoreNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsAlgorithmAnalysisCaseStudyRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "algorithm-analysis-case-study");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "algorithm-analysis-case-study");
    CHECK(payload["view"]["scopeName"].asString() == "复杂度案例解析");
    CHECK(payload["view"]["parentScopeCode"].asString() == "algorithm-analysis");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "algorithm-analysis");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "algorithm-analysis-case-study");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundBranchNode = false;
    bool foundCompareNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "algorithm-analysis-case-study-branch-impact")
        {
            foundBranchNode = true;
            CHECK(node["label"].asString() == "条件分支影响");
        }

        if (node["code"].asString() == "algorithm-analysis-case-study-cross-compare")
        {
            foundCompareNode = true;
            CHECK(node["label"].asString() == "案例结果比较");
        }
    }

    CHECK(foundBranchNode == true);
    CHECK(foundCompareNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsSequenceListDeleteRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("sequence-list-delete");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "sequence-list-delete");
    CHECK(payload["view"]["scopeName"].asString() == "删除操作");
    CHECK(payload["view"]["parentScopeCode"].asString() == "sequence-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "sequence-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "sequence-list-delete");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundShiftNode = false;
    bool foundCostNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "sequence-list-delete-shift-forward")
        {
            foundShiftNode = true;
            CHECK(node["label"].asString() == "后继元素逐位前移");
        }

        if (node["code"].asString() == "sequence-list-delete-time-cost")
        {
            foundCostNode = true;
            CHECK(node["label"].asString() == "时间代价理解");
        }
    }

    CHECK(foundShiftNode == true);
    CHECK(foundCostNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsLinkedListTailInsertRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "linked-list-tail-insert");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "linked-list-tail-insert");
    CHECK(payload["view"]["scopeName"].asString() == "尾插法建表");
    CHECK(payload["view"]["parentScopeCode"].asString() == "linked-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linked-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "linked-list-tail-insert");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundMoveNode = false;
    bool foundKeepNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "linked-list-tail-insert-tail-move")
        {
            foundMoveNode = true;
            CHECK(node["label"].asString() == "尾指针后移");
        }

        if (node["code"].asString() == "linked-list-tail-insert-order-keep")
        {
            foundKeepNode = true;
            CHECK(node["label"].asString() == "保持输入顺序");
        }
    }

    CHECK(foundMoveNode == true);
    CHECK(foundKeepNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsQueueApplicationsRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("queue-applications");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "queue-applications");
    CHECK(payload["view"]["scopeName"].asString() == "队列典型应用");
    CHECK(payload["view"]["parentScopeCode"].asString() == "queue");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "queue");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "queue-applications");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundBfsNode = false;
    bool foundWindowNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "queue-applications-bfs")
        {
            foundBfsNode = true;
            CHECK(node["label"].asString() == "广度优先搜索");
        }

        if (node["code"].asString() == "queue-applications-service-window")
        {
            foundWindowNode = true;
            CHECK(node["label"].asString() == "服务窗口模型");
        }
    }

    CHECK(foundBfsNode == true);
    CHECK(foundWindowNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsSequenceListMergeRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("sequence-list-merge");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "sequence-list-merge");
    CHECK(payload["view"]["scopeName"].asString() == "合并操作");
    CHECK(payload["view"]["parentScopeCode"].asString() == "sequence-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "sequence-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "sequence-list-merge");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundPointerNode = false;
    bool foundCostNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "sequence-list-merge-double-pointer")
        {
            foundPointerNode = true;
            CHECK(node["label"].asString() == "双指针比较");
        }

        if (node["code"].asString() == "sequence-list-merge-time-space")
        {
            foundCostNode = true;
            CHECK(node["label"].asString() == "时间与空间代价");
        }
    }

    CHECK(foundPointerNode == true);
    CHECK(foundCostNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsLinkedListMergeRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("linked-list-merge");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "linked-list-merge");
    CHECK(payload["view"]["scopeName"].asString() == "有序表合并");
    CHECK(payload["view"]["parentScopeCode"].asString() == "linked-list");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "linked-list");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "linked-list-merge");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundLinkNode = false;
    bool foundAdvantageNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "linked-list-merge-link-smaller")
        {
            foundLinkNode = true;
            CHECK(node["label"].asString() == "较小结点接到结果链");
        }

        if (node["code"].asString() == "linked-list-merge-no-move-element")
        {
            foundAdvantageNode = true;
            CHECK(node["label"].asString() == "不搬移元素优势");
        }
    }

    CHECK(foundLinkNode == true);
    CHECK(foundAdvantageNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsQueueEnqueueRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("queue-enqueue");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "queue-enqueue");
    CHECK(payload["view"]["scopeName"].asString() == "入队操作");
    CHECK(payload["view"]["parentScopeCode"].asString() == "queue");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "queue");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "queue-enqueue");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundFullCheckNode = false;
    bool foundRearUpdateNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "queue-enqueue-full-check")
        {
            foundFullCheckNode = true;
            CHECK(node["label"].asString() == "判满检查");
        }

        if (node["code"].asString() == "queue-enqueue-update-rear")
        {
            foundRearUpdateNode = true;
            CHECK(node["label"].asString() == "rear 后移更新");
        }
    }

    CHECK(foundFullCheckNode == true);
    CHECK(foundRearUpdateNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsStringMatchingProblemRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "string-matching-problem");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "string-matching-problem");
    CHECK(payload["view"]["scopeName"].asString() == "模式匹配问题");
    CHECK(payload["view"]["parentScopeCode"].asString() == "string");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "string");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "string-matching-problem");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundCompareNode = false;
    bool foundScenarioNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "string-matching-problem-char-compare")
        {
            foundCompareNode = true;
            CHECK(node["label"].asString() == "逐字符比较思想");
        }

        if (node["code"].asString() ==
            "string-matching-problem-application-scenarios")
        {
            foundScenarioNode = true;
            CHECK(node["label"].asString() == "应用场景");
        }
    }

    CHECK(foundCompareNode == true);
    CHECK(foundScenarioNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsBinaryTreeTraversalRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("binary-tree-traversal");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "binary-tree-traversal");
    CHECK(payload["view"]["scopeName"].asString() == "二叉树遍历");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 3);

    bool foundRecursiveNode = false;
    bool foundLevelOrderNode = false;
    bool foundIterativeNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "binary-tree-traversal-recursive")
        {
            foundRecursiveNode = true;
            CHECK(node["label"].asString() == "递归遍历思想");
            CHECK(node["nodeType"].asString() == "abstraction");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "binary-tree-traversal-recursive");
        }

        if (node["code"].asString() == "binary-tree-traversal-iterative")
        {
            foundIterativeNode = true;
            CHECK(node["label"].asString() == "非递归遍历思想");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "binary-tree-traversal-iterative");
        }

        if (node["code"].asString() == "binary-tree-traversal-level-order")
        {
            foundLevelOrderNode = true;
            CHECK(node["label"].asString() == "层序遍历");
            CHECK(node["chapterNo"].asInt() == 5);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "binary-tree-traversal-level-order");
        }
    }

    CHECK(foundRecursiveNode == true);
    CHECK(foundIterativeNode == true);
    CHECK(foundLevelOrderNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsHuffmanTreeRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("huffman-tree");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "huffman-tree");
    CHECK(payload["view"]["scopeName"].asString() == "哈夫曼树与编码");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "huffman-tree");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 3);

    bool foundConstructionNode = false;
    bool foundCodingNode = false;
    bool foundPrefixNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "huffman-tree-construction-process")
        {
            foundConstructionNode = true;
            CHECK(node["label"].asString() == "构造过程");
            CHECK(node["nodeType"].asString() == "algorithm");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "huffman-tree-construction-process");
        }

        if (node["code"].asString() == "huffman-tree-prefix-code")
        {
            foundPrefixNode = true;
            CHECK(node["label"].asString() == "前缀编码约束");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "huffman-tree-prefix-code");
        }

        if (node["code"].asString() == "huffman-tree-code-decode")
        {
            foundCodingNode = true;
            CHECK(node["label"].asString() == "哈夫曼编码与译码");
            CHECK(node["chapterNo"].asInt() == 5);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "huffman-tree-code-decode");
        }
    }

    CHECK(foundConstructionNode == true);
    CHECK(foundPrefixNode == true);
    CHECK(foundCodingNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsGraphBasicRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("graph-basic");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "graph-basic");
    CHECK(payload["view"]["scopeName"].asString() == "图的存储与遍历");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "graph-basic");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 12);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 4);

    bool foundDfsNode = false;
    bool foundBfsNode = false;
    bool foundAdjacencyListNode = false;
    bool foundAdjacencyMatrixNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "graph-basic-adjacency-matrix")
        {
            foundAdjacencyMatrixNode = true;
            CHECK(node["label"].asString() == "邻接矩阵存储");
            CHECK(node["nodeType"].asString() == "implementation");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "graph-basic-adjacency-matrix");
        }

        if (node["code"].asString() == "graph-basic-dfs")
        {
            foundDfsNode = true;
            CHECK(node["label"].asString() == "深度优先遍历");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "graph-basic-dfs");
        }

        if (node["code"].asString() == "graph-basic-adjacency-list")
        {
            foundAdjacencyListNode = true;
            CHECK(node["label"].asString() == "邻接表存储");
            CHECK(node["nodeType"].asString() == "implementation");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "graph-basic-adjacency-list");
        }

        if (node["code"].asString() == "graph-basic-bfs")
        {
            foundBfsNode = true;
            CHECK(node["label"].asString() == "广度优先遍历");
            CHECK(node["chapterNo"].asInt() == 6);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "graph-basic-bfs");
        }
    }

    CHECK(foundDfsNode == true);
    CHECK(foundBfsNode == true);
    CHECK(foundAdjacencyMatrixNode == true);
    CHECK(foundAdjacencyListNode == true);
}

DROGON_TEST(KnowledgeGraphDetailScopeReturnsTopologicalSortRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("topological-sort");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "topological-sort");
    CHECK(payload["view"]["scopeName"].asString() == "拓扑排序与关键路径");
    CHECK(payload["breadcrumbs"].size() == 2U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "topological-sort");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);
    CHECK(payload["meta"]["detailScopeCount"].asInt() == 4);

    bool foundTopoAlgoNode = false;
    bool foundCriticalSolveNode = false;
    bool foundAoeNode = false;
    bool foundCriticalConceptNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "topological-sort-algorithm")
        {
            foundTopoAlgoNode = true;
            CHECK(node["label"].asString() == "拓扑排序算法");
            CHECK(node["nodeType"].asString() == "algorithm");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "topological-sort-algorithm");
        }

        if (node["code"].asString() == "topological-sort-aoe")
        {
            foundAoeNode = true;
            CHECK(node["label"].asString() == "AOE 网");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() == "topological-sort-aoe");
        }

        if (node["code"].asString() == "topological-sort-critical-path-concept")
        {
            foundCriticalConceptNode = true;
            CHECK(node["label"].asString() == "关键路径概念");
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "topological-sort-critical-path-concept");
        }

        if (node["code"].asString() == "topological-sort-critical-path-solve")
        {
            foundCriticalSolveNode = true;
            CHECK(node["label"].asString() == "关键路径求解");
            CHECK(node["chapterNo"].asInt() == 6);
            CHECK(node["hasDetailGraph"].asBool() == true);
            CHECK(node["detailScopeCode"].asString() ==
                  "topological-sort-critical-path-solve");
        }
    }

    CHECK(foundTopoAlgoNode == true);
    CHECK(foundAoeNode == true);
    CHECK(foundCriticalConceptNode == true);
    CHECK(foundCriticalSolveNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsBinaryTreeTraversalLevelOrderRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "binary-tree-traversal-level-order");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "binary-tree-traversal-level-order");
    CHECK(payload["view"]["scopeName"].asString() == "层序遍历");
    CHECK(payload["view"]["parentScopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "binary-tree-traversal-level-order");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundQueueNode = false;
    bool foundLayerNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "binary-tree-traversal-level-order-queue")
        {
            foundQueueNode = true;
            CHECK(node["label"].asString() == "队列辅助结构");
        }

        if (node["code"].asString() ==
            "binary-tree-traversal-level-order-layer-progress")
        {
            foundLayerNode = true;
            CHECK(node["label"].asString() == "按层推进理解");
        }
    }

    CHECK(foundQueueNode == true);
    CHECK(foundLayerNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsHuffmanTreePrefixCodeRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "huffman-tree-prefix-code");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "huffman-tree-prefix-code");
    CHECK(payload["view"]["scopeName"].asString() == "前缀编码约束");
    CHECK(payload["view"]["parentScopeCode"].asString() == "huffman-tree");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "huffman-tree");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "huffman-tree-prefix-code");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundDefinitionNode = false;
    bool foundDecodeNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "huffman-tree-prefix-code-definition")
        {
            foundDefinitionNode = true;
            CHECK(node["label"].asString() == "前缀编码定义");
        }

        if (node["code"].asString() == "huffman-tree-prefix-code-no-separator")
        {
            foundDecodeNode = true;
            CHECK(node["label"].asString() == "译码无需分隔符");
        }
    }

    CHECK(foundDefinitionNode == true);
    CHECK(foundDecodeNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsGraphBasicAdjacencyMatrixRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "graph-basic-adjacency-matrix");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "graph-basic-adjacency-matrix");
    CHECK(payload["view"]["scopeName"].asString() == "邻接矩阵存储");
    CHECK(payload["view"]["parentScopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "graph-basic-adjacency-matrix");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundCellNode = false;
    bool foundQueryNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "graph-basic-adjacency-matrix-cell")
        {
            foundCellNode = true;
            CHECK(node["label"].asString() == "矩阵单元含义");
        }

        if (node["code"].asString() == "graph-basic-adjacency-matrix-query-traverse")
        {
            foundQueryNode = true;
            CHECK(node["label"].asString() == "查边与遍历便利性");
        }
    }

    CHECK(foundCellNode == true);
    CHECK(foundQueryNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsCriticalPathConceptRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "topological-sort-critical-path-concept");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "topological-sort-critical-path-concept");
    CHECK(payload["view"]["scopeName"].asString() == "关键路径概念");
    CHECK(payload["view"]["parentScopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "topological-sort-critical-path-concept");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundLongestNode = false;
    bool foundSolveRelationNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() ==
            "topological-sort-critical-path-concept-longest-path")
        {
            foundLongestNode = true;
            CHECK(node["label"].asString() == "最长路径含义");
        }

        if (node["code"].asString() ==
            "topological-sort-critical-path-concept-to-solve")
        {
            foundSolveRelationNode = true;
            CHECK(node["label"].asString() == "与关键路径求解关系");
        }
    }

    CHECK(foundLongestNode == true);
    CHECK(foundSolveRelationNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsBinaryTreeTraversalIterativeRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "binary-tree-traversal-iterative");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "binary-tree-traversal-iterative");
    CHECK(payload["view"]["scopeName"].asString() == "非递归遍历思想");
    CHECK(payload["view"]["parentScopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "binary-tree-traversal-iterative");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundInorderNode = false;
    bool foundCompareNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "binary-tree-traversal-iterative-inorder")
        {
            foundInorderNode = true;
            CHECK(node["label"].asString() == "中序非递归框架");
        }

        if (node["code"].asString() == "binary-tree-traversal-iterative-compare")
        {
            foundCompareNode = true;
            CHECK(node["label"].asString() == "递归与非递归对比");
        }
    }

    CHECK(foundInorderNode == true);
    CHECK(foundCompareNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsHuffmanTreeCodeDecodeRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("huffman-tree-code-decode");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "huffman-tree-code-decode");
    CHECK(payload["view"]["scopeName"].asString() == "哈夫曼编码与译码");
    CHECK(payload["view"]["parentScopeCode"].asString() == "huffman-tree");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "huffman-tree");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "huffman-tree-code-decode");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundGenerateNode = false;
    bool foundDecodeNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "huffman-tree-code-decode-generate-code")
        {
            foundGenerateNode = true;
            CHECK(node["label"].asString() == "根到叶生成编码");
        }

        if (node["code"].asString() == "huffman-tree-code-decode-decode-bitstream")
        {
            foundDecodeNode = true;
            CHECK(node["label"].asString() == "按位译码过程");
        }
    }

    CHECK(foundGenerateNode == true);
    CHECK(foundDecodeNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsGraphBasicAdjacencyListRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "graph-basic-adjacency-list");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "graph-basic-adjacency-list");
    CHECK(payload["view"]["scopeName"].asString() == "邻接表存储");
    CHECK(payload["view"]["parentScopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "graph-basic-adjacency-list");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundBuildNode = false;
    bool foundTraverseNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "graph-basic-adjacency-list-build")
        {
            foundBuildNode = true;
            CHECK(node["label"].asString() == "建表过程");
        }

        if (node["code"].asString() ==
            "graph-basic-adjacency-list-traverse-neighbor")
        {
            foundTraverseNode = true;
            CHECK(node["label"].asString() == "遍历邻接点");
        }
    }

    CHECK(foundBuildNode == true);
    CHECK(foundTraverseNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsTopologicalSortAoeRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("topological-sort-aoe");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "topological-sort-aoe");
    CHECK(payload["view"]["scopeName"].asString() == "AOE 网");
    CHECK(payload["view"]["parentScopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "topological-sort-aoe");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundWeightNode = false;
    bool foundCompareNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "topological-sort-aoe-edge-weight")
        {
            foundWeightNode = true;
            CHECK(node["label"].asString() == "边权表示持续时间");
        }

        if (node["code"].asString() == "topological-sort-aoe-vs-aov")
        {
            foundCompareNode = true;
            CHECK(node["label"].asString() == "与 AOV 网区别");
        }
    }

    CHECK(foundWeightNode == true);
    CHECK(foundCompareNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsBinaryTreeTraversalRecursiveRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "binary-tree-traversal-recursive");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "binary-tree-traversal-recursive");
    CHECK(payload["view"]["scopeName"].asString() == "递归遍历思想");
    CHECK(payload["view"]["parentScopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() ==
          "binary-tree-traversal");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "binary-tree-traversal-recursive");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundCallStackNode = false;
    bool foundInorderFrameNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "binary-tree-traversal-recursive-call-stack")
        {
            foundCallStackNode = true;
            CHECK(node["label"].asString() == "调用栈理解");
        }

        if (node["code"].asString() ==
            "binary-tree-traversal-recursive-inorder-frame")
        {
            foundInorderFrameNode = true;
            CHECK(node["label"].asString() == "中序递归框架");
        }
    }

    CHECK(foundCallStackNode == true);
    CHECK(foundInorderFrameNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsHuffmanTreeConstructionProcessRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "huffman-tree-construction-process");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "huffman-tree-construction-process");
    CHECK(payload["view"]["scopeName"].asString() == "构造过程");
    CHECK(payload["view"]["parentScopeCode"].asString() == "huffman-tree");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "huffman-tree");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "huffman-tree-construction-process");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundMinSelectionNode = false;
    bool foundTreeFormedNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() ==
            "huffman-tree-construction-process-min-selection")
        {
            foundMinSelectionNode = true;
            CHECK(node["label"].asString() == "最小权值选择");
        }

        if (node["code"].asString() ==
            "huffman-tree-construction-process-tree-formed")
        {
            foundTreeFormedNode = true;
            CHECK(node["label"].asString() == "哈夫曼树形成");
        }
    }

    CHECK(foundMinSelectionNode == true);
    CHECK(foundTreeFormedNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsGraphBasicBfsRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("graph-basic-bfs");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "graph-basic-bfs");
    CHECK(payload["view"]["scopeName"].asString() == "广度优先遍历");
    CHECK(payload["view"]["parentScopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "graph-basic-bfs");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundQueueNode = false;
    bool foundListNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "graph-basic-bfs-queue")
        {
            foundQueueNode = true;
            CHECK(node["label"].asString() == "队列支持");
        }

        if (node["code"].asString() == "graph-basic-bfs-list")
        {
            foundListNode = true;
            CHECK(node["label"].asString() == "邻接表下 BFS");
        }
    }

    CHECK(foundQueueNode == true);
    CHECK(foundListNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsTopologicalSortAlgorithmRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "topological-sort-algorithm");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "topological-sort-algorithm");
    CHECK(payload["view"]["scopeName"].asString() == "拓扑排序算法");
    CHECK(payload["view"]["parentScopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "topological-sort-algorithm");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundInitTableNode = false;
    bool foundCycleCheckNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "topological-sort-algorithm-init-table")
        {
            foundInitTableNode = true;
            CHECK(node["label"].asString() == "初始化入度表");
        }

        if (node["code"].asString() == "topological-sort-algorithm-cycle-check")
        {
            foundCycleCheckNode = true;
            CHECK(node["label"].asString() == "环检测与合法性判断");
        }
    }

    CHECK(foundInitTableNode == true);
    CHECK(foundCycleCheckNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsStackRecursionRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("stack-recursion");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "stack-recursion");
    CHECK(payload["view"]["scopeName"].asString() == "递归工作栈");
    CHECK(payload["view"]["parentScopeCode"].asString() == "stack");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "stack");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "stack-recursion");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundSystemStackNode = false;
    bool foundConversionNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "stack-recursion-system-stack")
        {
            foundSystemStackNode = true;
            CHECK(node["label"].asString() == "系统工作栈");
        }

        if (node["code"].asString() == "stack-recursion-conversion")
        {
            foundConversionNode = true;
            CHECK(node["label"].asString() == "递归与非递归转换");
        }
    }

    CHECK(foundSystemStackNode == true);
    CHECK(foundConversionNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsKmpNextDefinitionRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("kmp-next-definition");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "kmp-next-definition");
    CHECK(payload["view"]["scopeName"].asString() == "next 数组含义");
    CHECK(payload["view"]["parentScopeCode"].asString() == "kmp");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "kmp");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "kmp-next-definition");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundMeaningNode = false;
    bool foundExampleNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "kmp-next-definition-nextj-meaning")
        {
            foundMeaningNode = true;
            CHECK(node["label"].asString() == "next[j] 含义");
        }

        if (node["code"].asString() == "kmp-next-definition-example")
        {
            foundExampleNode = true;
            CHECK(node["label"].asString() == "示例推导");
        }
    }

    CHECK(foundMeaningNode == true);
    CHECK(foundExampleNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsGraphBasicDfsRefinement)
{
    const auto payload =
        services::KnowledgeGraphService::buildGraphPayload("graph-basic-dfs");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() == "graph-basic-dfs");
    CHECK(payload["view"]["scopeName"].asString() == "深度优先遍历");
    CHECK(payload["view"]["parentScopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "graph-basic");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() == "graph-basic-dfs");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundRecursiveNode = false;
    bool foundTreeNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() == "graph-basic-dfs-recursive-frame")
        {
            foundRecursiveNode = true;
            CHECK(node["label"].asString() == "递归实现框架");
        }

        if (node["code"].asString() == "graph-basic-dfs-tree")
        {
            foundTreeNode = true;
            CHECK(node["label"].asString() == "DFS 树与搜索轨迹");
        }
    }

    CHECK(foundRecursiveNode == true);
    CHECK(foundTreeNode == true);
}

DROGON_TEST(KnowledgeGraphThirdScopeReturnsCriticalPathSolveRefinement)
{
    const auto payload = services::KnowledgeGraphService::buildGraphPayload(
        "topological-sort-critical-path-solve");

    CHECK(payload["status"].asString() == "ok");
    CHECK(payload["view"]["scopeCode"].asString() ==
          "topological-sort-critical-path-solve");
    CHECK(payload["view"]["scopeName"].asString() == "关键路径求解");
    CHECK(payload["view"]["parentScopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"].size() == 3U);
    CHECK(payload["breadcrumbs"][1]["scopeCode"].asString() == "topological-sort");
    CHECK(payload["breadcrumbs"][2]["scopeCode"].asString() ==
          "topological-sort-critical-path-solve");
    CHECK(payload["meta"]["knowledgePointCount"].asInt() == 8);
    CHECK(payload["meta"]["dependencyCount"].asInt() == 10);

    bool foundVeNode = false;
    bool foundPathNode = false;
    for (const auto &node : payload["nodes"])
    {
        if (node["code"].asString() ==
            "topological-sort-critical-path-solve-ve")
        {
            foundVeNode = true;
            CHECK(node["label"].asString() == "正向计算 ve");
        }

        if (node["code"].asString() ==
            "topological-sort-critical-path-solve-path")
        {
            foundPathNode = true;
            CHECK(node["label"].asString() == "关键路径形成");
        }
    }

    CHECK(foundVeNode == true);
    CHECK(foundPathNode == true);
}
