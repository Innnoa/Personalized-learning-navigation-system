import { PRACTICE_CHECK_BANKS } from "./practiceCheckQuestionBankData";

const PRACTICE_CHECK_TARGET_ALIASES = {
  "linear-list-sequential-representation": "sequence-list",
  "linear-list-linked-representation": "linked-list",
};

const LOW_MASTERY_DIFFICULTY_PLAN = ["easy", "easy", "easy", "medium", "medium"];
const MID_MASTERY_DIFFICULTY_PLAN = ["easy", "easy", "medium", "medium", "hard"];
const HIGH_MASTERY_DIFFICULTY_PLAN = ["easy", "medium", "medium", "hard", "hard"];

function normalizePreviousMasteryPercent(value) {
  const numericValue = Number(value);

  if (!Number.isFinite(numericValue)) {
    return 0;
  }

  return Math.max(0, Math.min(100, Math.round(numericValue)));
}

function resolveDifficultyPlan(previousMasteryPercent) {
  const normalizedMasteryPercent = normalizePreviousMasteryPercent(previousMasteryPercent);

  if (normalizedMasteryPercent <= 30) {
    return LOW_MASTERY_DIFFICULTY_PLAN;
  }

  if (normalizedMasteryPercent <= 70) {
    return MID_MASTERY_DIFFICULTY_PLAN;
  }

  return HIGH_MASTERY_DIFFICULTY_PLAN;
}

function pickQuestionFromPool(groupedPool, usedQuestionIds, difficulty) {
  const primaryPool = groupedPool[difficulty] || [];
  const fallbackOrder = {
    easy: ["easy", "medium", "hard"],
    medium: ["medium", "easy", "hard"],
    hard: ["hard", "medium", "easy"],
  };

  for (const difficultyCode of fallbackOrder[difficulty] || [difficulty]) {
    const candidate = (groupedPool[difficultyCode] || []).find(
      (question) => !usedQuestionIds.has(question.id),
    );

    if (candidate) {
      return candidate;
    }
  }

  return primaryPool.find((question) => !usedQuestionIds.has(question.id)) || null;
}

function selectQuestionsFromPool(pool, previousMasteryPercent) {
  const groupedPool = pool.reduce(
    (groups, question) => {
      const difficulty = question.difficulty || "medium";
      return {
        ...groups,
        [difficulty]: [...(groups[difficulty] || []), question],
      };
    },
    {},
  );
  const usedQuestionIds = new Set();
  const selectedQuestions = [];

  resolveDifficultyPlan(previousMasteryPercent).forEach((difficulty) => {
    const nextQuestion = pickQuestionFromPool(groupedPool, usedQuestionIds, difficulty);

    if (!nextQuestion) {
      return;
    }

    usedQuestionIds.add(nextQuestion.id);
    selectedQuestions.push(nextQuestion);
  });

  return selectedQuestions;
}

function hasQuestionBank(targetCode) {
  return Boolean(PRACTICE_CHECK_BANKS[targetCode]);
}

function resolveHierarchicalQuestionTargetCode(normalizedCode) {
  const segments = normalizedCode.split("-");

  for (let length = segments.length - 1; length >= 1; length -= 1) {
    const candidateCode = segments.slice(0, length).join("-");
    if (hasQuestionBank(candidateCode)) {
      return candidateCode;
    }
  }

  return normalizedCode;
}

function resolvePracticeQuestionTargetCode(targetCode) {
  if (!targetCode) {
    return "";
  }

  const normalizedCode = String(targetCode);
  const aliasedCode = PRACTICE_CHECK_TARGET_ALIASES[normalizedCode];

  if (aliasedCode && hasQuestionBank(aliasedCode)) {
    return aliasedCode;
  }

  if (hasQuestionBank(normalizedCode)) {
    return normalizedCode;
  }

  return resolveHierarchicalQuestionTargetCode(normalizedCode);
}

export function getPracticeCheckQuestionSet(targetCode, options = {}) {
  const resolvedTargetCode = resolvePracticeQuestionTargetCode(targetCode);

  if (!resolvedTargetCode) {
    return null;
  }

  const pool = PRACTICE_CHECK_BANKS[resolvedTargetCode]?.pool || null;

  if (!pool) {
    return null;
  }

  return selectQuestionsFromPool(pool, options.previousMasteryPercent);
}

export function getPracticeCheckAnswerKey(targetCode, options = {}) {
  const questions = getPracticeCheckQuestionSet(targetCode, options);

  if (!questions) {
    return {};
  }

  return Object.fromEntries(
    questions.map((question) => [question.id, question.correctIndex]),
  );
}

export { resolvePracticeQuestionTargetCode };
