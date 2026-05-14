const PRACTICE_CHECK_QUESTION_SETS = {
  queue: [
    {
      id: "queue-1",
      type: "single-choice",
      prompt: "队列的主要特性是什么？",
      options: ["后进先出（LIFO）", "先进先出（FIFO）", "只能随机访问", "必须使用递归"],
    },
    {
      id: "queue-2",
      type: "single-choice",
      prompt: "在顺序队列中，通常表示队头位置的指针是？",
      options: ["front", "top", "root", "tailPrev"],
    },
    {
      id: "queue-3",
      type: "single-choice",
      prompt: "以下哪个场景更适合使用队列？",
      options: ["函数调用栈", "撤销操作记录", "打印任务排队", "二叉树递归回溯"],
    },
  ],
};

function resolvePracticeQuestionTargetCode(targetCode) {
  if (!targetCode) {
    return "";
  }

  const normalizedCode = String(targetCode);

  if (PRACTICE_CHECK_QUESTION_SETS[normalizedCode]) {
    return normalizedCode;
  }

  const [baseCode] = normalizedCode.split("-");
  return PRACTICE_CHECK_QUESTION_SETS[baseCode] ? baseCode : normalizedCode;
}

export function getPracticeCheckQuestionSet(targetCode) {
  const resolvedTargetCode = resolvePracticeQuestionTargetCode(targetCode);

  if (!resolvedTargetCode) {
    return null;
  }

  return PRACTICE_CHECK_QUESTION_SETS[resolvedTargetCode] || null;
}

export { resolvePracticeQuestionTargetCode };
