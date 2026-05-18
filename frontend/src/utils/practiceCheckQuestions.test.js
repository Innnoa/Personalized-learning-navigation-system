import { describe, expect, it } from "vitest";

import {
  getPracticeCheckQuestionSet,
  getPracticeCheckAnswerKey,
  resolvePracticeQuestionTargetCode,
} from "./practiceCheckQuestions";

describe("practiceCheckQuestions", () => {
  it("returns five selected questions for every first-level knowledge point", () => {
    const coveredTargetCodes = [
      "ds-intro",
      "algorithm-analysis",
      "linear-list",
      "sequence-list",
      "linked-list",
      "stack",
      "queue",
      "string",
      "kmp",
      "tree-basic",
      "binary-tree-traversal",
      "huffman-tree",
      "graph-basic",
      "topological-sort",
    ];

    coveredTargetCodes.forEach((code) => {
      const options = { previousMasteryPercent: 50 };
      const questions = getPracticeCheckQuestionSet(code, options);
      const answerKey = getPracticeCheckAnswerKey(code, options);

      expect(questions).toHaveLength(5);
      expect(Object.keys(answerKey)).toHaveLength(5);
      expect(questions.every((question) => Array.isArray(question.options))).toBe(true);
      expect(questions.every((question) => question.options.length === 4)).toBe(true);
      expect(questions.every((question) => Object.hasOwn(answerKey, question.id))).toBe(true);
    });
  });

  it("returns five selected questions for every second-level scope", () => {
    const secondLevelCodes = [
      "ds-intro",
      "algorithm-analysis",
      "linear-list",
      "sequence-list",
      "linked-list",
      "stack",
      "queue",
      "string",
      "kmp",
      "tree-basic",
      "binary-tree-traversal",
      "huffman-tree",
      "graph-basic",
      "topological-sort",
    ];

    secondLevelCodes.forEach((code) => {
      const questions = getPracticeCheckQuestionSet(code, { previousMasteryPercent: 80 });
      expect(questions).toHaveLength(5);
    });
  });

  it("selects easier questions for low mastery and harder questions for high mastery", () => {
    const lowMasteryQuestions = getPracticeCheckQuestionSet("queue", { previousMasteryPercent: 20 });
    const highMasteryQuestions = getPracticeCheckQuestionSet("queue", { previousMasteryPercent: 85 });

    const countByDifficulty = (questions) =>
      questions.reduce(
        (summary, question) => ({
          ...summary,
          [question.difficulty]: (summary[question.difficulty] || 0) + 1,
        }),
        {},
      );

    expect(countByDifficulty(lowMasteryQuestions)).toEqual({
      easy: 3,
      medium: 2,
    });
    expect(countByDifficulty(highMasteryQuestions)).toEqual({
      easy: 1,
      medium: 2,
      hard: 2,
    });
  });

  it("maps detail-learning target codes back to their configured parent question bank", () => {
    expect(resolvePracticeQuestionTargetCode("linear-list")).toBe("linear-list");
    expect(resolvePracticeQuestionTargetCode("linear-list-linked-representation")).toBe(
      "linked-list",
    );
    expect(resolvePracticeQuestionTargetCode("queue-linked")).toBe("queue-linked");
    expect(resolvePracticeQuestionTargetCode("queue-circular")).toBe("queue-circular");
    expect(resolvePracticeQuestionTargetCode("queue-enqueue")).toBe("queue-enqueue");
    expect(resolvePracticeQuestionTargetCode("queue-applications")).toBe("queue-applications");
    expect(resolvePracticeQuestionTargetCode("stack-applications")).toBe("stack-applications");
    expect(resolvePracticeQuestionTargetCode("stack-recursion")).toBe("stack-recursion");
    expect(resolvePracticeQuestionTargetCode("string-matching-problem")).toBe("string-matching-problem");
    expect(resolvePracticeQuestionTargetCode("string-naive-match")).toBe("string-naive-match");
    expect(resolvePracticeQuestionTargetCode("kmp-next-definition")).toBe("kmp-next-definition");
    expect(resolvePracticeQuestionTargetCode("kmp-main-process")).toBe("kmp-main-process");
    expect(resolvePracticeQuestionTargetCode("sequence-list-insert")).toBe("sequence-list-insert");
    expect(resolvePracticeQuestionTargetCode("sequence-list-delete")).toBe("sequence-list-delete");
    expect(resolvePracticeQuestionTargetCode("sequence-list-merge")).toBe("sequence-list-merge");
    expect(resolvePracticeQuestionTargetCode("linked-list-head-insert")).toBe("linked-list-head-insert");
    expect(resolvePracticeQuestionTargetCode("linked-list-tail-insert")).toBe("linked-list-tail-insert");
    expect(resolvePracticeQuestionTargetCode("linked-list-merge")).toBe("linked-list-merge");
    expect(resolvePracticeQuestionTargetCode("tree-basic-binary-storage")).toBe("tree-basic-binary-storage");
    expect(resolvePracticeQuestionTargetCode("binary-tree-traversal-recursive")).toBe("binary-tree-traversal-recursive");
    expect(resolvePracticeQuestionTargetCode("binary-tree-traversal-iterative")).toBe("binary-tree-traversal-iterative");
    expect(resolvePracticeQuestionTargetCode("binary-tree-traversal-level-order")).toBe("binary-tree-traversal-level-order");
    expect(resolvePracticeQuestionTargetCode("graph-basic-adjacency-matrix")).toBe("graph-basic-adjacency-matrix");
    expect(resolvePracticeQuestionTargetCode("graph-basic-adjacency-list")).toBe("graph-basic-adjacency-list");
    expect(resolvePracticeQuestionTargetCode("graph-basic-dfs")).toBe("graph-basic-dfs");
    expect(resolvePracticeQuestionTargetCode("graph-basic-bfs")).toBe("graph-basic-bfs");
    expect(resolvePracticeQuestionTargetCode("huffman-tree-construction-process")).toBe("huffman-tree-construction-process");
    expect(resolvePracticeQuestionTargetCode("huffman-tree-prefix-code")).toBe("huffman-tree-prefix-code");
    expect(resolvePracticeQuestionTargetCode("huffman-tree-code-decode")).toBe("huffman-tree-code-decode");
    expect(resolvePracticeQuestionTargetCode("topological-sort-aoe")).toBe("topological-sort-aoe");
    expect(resolvePracticeQuestionTargetCode("topological-sort-critical-path-solve")).toBe("topological-sort-critical-path-solve");
  });

  it("uses dedicated question banks for high-frequency third-level demo nodes", () => {
    expect(
      getPracticeCheckQuestionSet("queue-linked", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("链式队列");
    expect(
      getPracticeCheckQuestionSet("queue-circular", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("循环队列");
    expect(
      getPracticeCheckQuestionSet("queue-enqueue", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("入队");
    expect(
      getPracticeCheckQuestionSet("queue-applications", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("队列应用");
    expect(
      getPracticeCheckQuestionSet("stack-applications", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("栈的应用");
    expect(
      getPracticeCheckQuestionSet("sequence-list-insert", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("插入");
    expect(
      getPracticeCheckQuestionSet("sequence-list-delete", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("删除");
    expect(
      getPracticeCheckQuestionSet("sequence-list-merge", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("合并");
    expect(
      getPracticeCheckQuestionSet("linked-list-head-insert", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("头插法");
    expect(
      getPracticeCheckQuestionSet("linked-list-tail-insert", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("尾插法");
    expect(
      getPracticeCheckQuestionSet("linked-list-merge", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("链表合并");
    expect(
      getPracticeCheckQuestionSet("tree-basic-binary-storage", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("顺序存储");
    expect(
      getPracticeCheckQuestionSet("stack-recursion", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("递归");
    expect(
      getPracticeCheckQuestionSet("string-matching-problem", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("模式匹配");
    expect(
      getPracticeCheckQuestionSet("string-naive-match", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("朴素模式匹配");
    expect(
      getPracticeCheckQuestionSet("graph-basic-bfs", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("广度优先遍历");
    expect(
      getPracticeCheckQuestionSet("kmp-next-definition", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("next 数组");
    expect(
      getPracticeCheckQuestionSet("kmp-main-process", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("KMP 主过程");
    expect(
      getPracticeCheckQuestionSet("binary-tree-traversal-recursive", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("递归遍历");
    expect(
      getPracticeCheckQuestionSet("binary-tree-traversal-iterative", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("非递归遍历");
    expect(
      getPracticeCheckQuestionSet("binary-tree-traversal-level-order", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("层序遍历");
    expect(
      getPracticeCheckQuestionSet("huffman-tree-construction-process", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("构造过程");
    expect(
      getPracticeCheckQuestionSet("huffman-tree-prefix-code", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("前缀码");
    expect(
      getPracticeCheckQuestionSet("huffman-tree-code-decode", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("译码");
    expect(
      getPracticeCheckQuestionSet("graph-basic-adjacency-matrix", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("邻接矩阵");
    expect(
      getPracticeCheckQuestionSet("graph-basic-adjacency-list", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("邻接表");
    expect(
      getPracticeCheckQuestionSet("graph-basic-dfs", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("深度优先遍历");
    expect(
      getPracticeCheckQuestionSet("topological-sort-aoe", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("AOE 网");
    expect(
      getPracticeCheckQuestionSet("topological-sort-critical-path-solve", { previousMasteryPercent: 20 })?.[0]?.prompt,
    ).toContain("关键路径");
  });

  it("keeps unsupported targets unresolved and without a question set", () => {
    expect(resolvePracticeQuestionTargetCode("heap")).toBe("heap");
    expect(getPracticeCheckQuestionSet("heap", { previousMasteryPercent: 50 })).toBeNull();
    expect(getPracticeCheckAnswerKey("heap")).toEqual({});
  });

  it("reuses linear-list entry questions from the sequential representation bank", () => {
    const questions = getPracticeCheckQuestionSet("linear-list", { previousMasteryPercent: 30 });

    expect(questions).toHaveLength(5);
    expect(questions[0].prompt).toContain("线性表");
  });
});
