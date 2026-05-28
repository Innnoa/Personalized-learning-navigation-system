#!/usr/bin/env node
/**
 * 题库同步脚本：将后端 JSON 题库文件转换为前端 JS 静态题库文件
 *
 * 用法：
 *   node scripts/sync-question-banks.js [--course=courseCode]
 *
 * 读取 backend/config/question_bank_*.json
 * 写入 frontend/src/utils/practiceCheckQuestionBankData.js
 *
 * 若指定 --course，只同步该课程；默认同步所有找到的 JSON 题库文件。
 */

const fs = require("fs");
const path = require("path");

const BACKEND_DIR = path.resolve(__dirname, "..", "backend", "config");
const FRONTEND_FILE = path.resolve(
  __dirname,
  "..",
  "frontend",
  "src",
  "utils",
  "practiceCheckQuestionBankData.js",
);

function loadJsonFiles(courseFilter) {
  const files = fs.readdirSync(BACKEND_DIR).filter(
    (f) => f.startsWith("question_bank_") && f.endsWith(".json"),
  );

  if (files.length === 0) {
    console.log("未找到题库 JSON 文件。请在教师页面新增题目并保存后重试。");
    process.exit(0);
  }

  const banks = [];
  for (const file of files) {
    const content = fs.readFileSync(path.join(BACKEND_DIR, file), "utf-8");
    const data = JSON.parse(content);
    if (Array.isArray(data.banks)) {
      for (const kpr of data.banks) {
        if (courseFilter && kpr.knowledgePointCode !== courseFilter) continue;
        banks.push(kpr);
      }
    }
  }

  return banks;
}

function generateJs(banks) {
  const entryMap = {};
  for (const bank of banks) {
    if (!bank.knowledgePointCode || !Array.isArray(bank.questions)) continue;
    entryMap[bank.knowledgePointCode] = { pool: bank.questions };
  }

  let output = "";
  output += "// 由 scripts/sync-question-banks.js 自动生成\n";
  output += "// 生成时间：" + new Date().toISOString() + "\n";
  output += `// 已同步知识点数：${Object.keys(entryMap).length}\n\n`;

  output += "function q(id, difficulty, prompt, options, correctIndex, explanation) {\n";
  output += "  return {\n";
  output += "    id,\n";
  output += '    type: "single-choice",\n';
  output += "    difficulty,\n";
  output += "    prompt,\n";
  output += "    options,\n";
  output += "    correctIndex,\n";
  output += "    explanation,\n";
  output += "  };\n";
  output += "}\n\n";

  output += "export const PRACTICE_CHECK_BANKS = {\n";
  const codes = Object.keys(entryMap).sort();
  for (const code of codes) {
    const pool = entryMap[code].pool;
    output += `  "${code}": {\n`;
    output += "    pool: [\n";
    for (const q of pool) {
      const options = JSON.stringify(q.options || ["", "", "", ""]);
      output += `      q("${q.id}", "${q.difficulty}", ${JSON.stringify(q.prompt)}, ${options}, ${q.correctIndex ?? 0}, ${JSON.stringify(q.explanation || "")}),\n`;
    }
    output += "    ],\n";
    output += "  },\n";
  }

  // Preserve existing aliases
  output += `  // -- aliases (preserved from original) --
  // "linear-list" uses "sequence-list" bank
};

// Aliases: map of aliasCode -> bankCode
PRACTICE_CHECK_BANKS["linear-list"] = PRACTICE_CHECK_BANKS["sequence-list"];
`;

  return output;
}

function main() {
  const args = process.argv.slice(2);
  let courseFilter = null;
  for (const arg of args) {
    if (arg.startsWith("--course=")) {
      courseFilter = arg.replace("--course=", "");
    }
  }

  const banks = loadJsonFiles(courseFilter);
  const js = generateJs(banks);

  // Write to a temp file first to compare
  let existingContent = "";
  try {
    existingContent = fs.readFileSync(FRONTEND_FILE, "utf-8");
  } catch {
    // file doesn't exist yet
  }

  fs.writeFileSync(FRONTEND_FILE, js, "utf-8");

  const codes = new Set();
  for (const b of banks) codes.add(b.knowledgePointCode);
  console.log(`同步完成: ${banks.length} 个知识点分组 → ${FRONTEND_FILE}`);
  console.log(`知识点: ${[...codes].sort().join(", ")}`);
}

main();
