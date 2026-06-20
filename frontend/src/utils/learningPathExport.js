function normalizeArray(value) {
  return Array.isArray(value) ? value : [];
}

function padNumber(value) {
  return String(value).padStart(2, "0");
}

function formatTimestamp(date = new Date()) {
  const year = date.getFullYear();
  const month = padNumber(date.getMonth() + 1);
  const day = padNumber(date.getDate());
  const hours = padNumber(date.getHours());
  const minutes = padNumber(date.getMinutes());

  return `${year}-${month}-${day} ${hours}:${minutes}`;
}

function formatFileTimestamp(date = new Date()) {
  const year = date.getFullYear();
  const month = padNumber(date.getMonth() + 1);
  const day = padNumber(date.getDate());
  const hours = padNumber(date.getHours());
  const minutes = padNumber(date.getMinutes());

  return `${year}${month}${day}-${hours}${minutes}`;
}

function sanitizeFileNameSegment(value) {
  return String(value || "")
    .trim()
    .replace(/[^a-zA-Z0-9_-]+/g, "-")
    .replace(/-+/g, "-")
    .replace(/^-|-$/g, "");
}

function resolveBudgetMessage(summary = {}) {
  if (summary.targetReachableWithinBudget) {
    return "当前时间预算可以覆盖目标链路。";
  }

  return "当前时间预算不足以覆盖目标链路，系统已自动顺延部分节点。";
}

function resolveExplanationSummary(item) {
  if (item?.explanation?.summary) {
    return item.explanation.summary;
  }

  const reasons = normalizeArray(item?.reasonTrace?.triggerReasons);
  if (reasons.length > 0) {
    return reasons.join(" ");
  }

  return "暂无推荐理由说明。";
}

function resolveExplanationLabels(item) {
  return normalizeArray(item?.explanation?.labels);
}

function resolveSectionStatusLabel(status) {
  if (status === "scheduled") {
    return "本轮推荐";
  }

  if (status === "deferred") {
    return "下一轮建议";
  }

  if (status === "mastered") {
    return "已掌握";
  }

  return status || "未标注";
}

function buildPathSection(title, items) {
  const lines = [title];

  if (items.length === 0) {
    lines.push("（无）");
    return lines;
  }

  items.forEach((item, index) => {
    lines.push(
      `${index + 1}. ${item.name}（第${item.chapterNo}章，${item.estimatedMinutes}分钟，掌握度${item.masteryPercent}%）`,
    );
    lines.push(`   推荐理由：${resolveExplanationSummary(item)}`);

    const explanationLabels = resolveExplanationLabels(item);
    if (explanationLabels.length > 0) {
      lines.push(`   理由标签：${explanationLabels.join(" / ")}`);
    }
  });

  return lines;
}

function buildResourceSection(resourceRecommendations) {
  const lines = ["三、资源建议"];

  if (resourceRecommendations.length === 0) {
    lines.push("当前结果没有可导出的资源建议。");
    return lines;
  }

  resourceRecommendations.forEach((section) => {
    lines.push(
      `- ${section.name}：${section.resourceCount || 0} 条资源，状态：${resolveSectionStatusLabel(section.status)}`,
    );

    if (section.primaryResourceTitle) {
      lines.push(`  优先资源：${section.primaryResourceTitle}`);
    }

    if (section.recommendedUsage) {
      lines.push(`  节点建议：${section.recommendedUsage}`);
    }

    normalizeArray(section.resources)
      .slice(0, 2)
      .forEach((resource, index) => {
        const phaseText = resource.recommendedPhase
          ? `，阶段：${resource.recommendedPhase}`
          : "";
        lines.push(
          `  资源${index + 1}：${resource.title}（${resource.type || "学习资源"}，${resource.source || "来源未标注"}${phaseText}）`,
        );
      });
  });

  return lines;
}

function buildOverallExplanationSection(overallExplanation) {
  if (!overallExplanation?.summary) {
    return [];
  }

  const lines = [
    "一-补充、路径总说明",
    overallExplanation.summary,
  ];

  normalizeArray(overallExplanation.bullets).forEach((bullet) => {
    lines.push(`- ${bullet}`);
  });

  return [...lines, ""];
}

export function buildLearningPathExportText({
  learnerCode = "demo-learner",
  targetCode = "",
  targetLabel = "未命名目标",
  parentTargetCode = "",
  parentTargetLabel = "",
  availableMinutes = 0,
  planResult = null,
  generatedAt = new Date(),
  scopeLabel = "",
  documentTitle = "学习路径导出结果",
} = {}) {
  const summary = planResult?.summary || {};
  const scheduledItems = normalizeArray(planResult?.path).filter(
    (item) => item.status === "scheduled",
  );
  const deferredItems = normalizeArray(planResult?.path).filter(
    (item) => item.status === "deferred",
  );
  const resourceRecommendations = normalizeArray(planResult?.resourceRecommendations);

  const lines = [
    "基于知识点依赖图的个性化学习导航系统",
    documentTitle,
    "",
    `导出时间：${formatTimestamp(generatedAt)}`,
    `学习者：${learnerCode}`,
    ...(scopeLabel ? [`当前范围：${scopeLabel}`] : []),
    ...(parentTargetLabel
      ? [`一级目标：${parentTargetLabel}${parentTargetCode ? `（${parentTargetCode}）` : ""}`]
      : []),
    `目标节点：${targetLabel}${targetCode ? `（${targetCode}）` : ""}`,
    `时间预算：${availableMinutes} 分钟`,
    "",
    "一、规划摘要",
    `本轮安排：${summary.scheduledCount || 0} 个节点`,
    `下一轮建议：${summary.deferredCount || 0} 个节点`,
    `已掌握节点：${summary.masteredCount || 0} 个`,
    `本轮预计耗时：${summary.scheduledMinutes || 0} 分钟`,
    `目标链路总耗时：${summary.totalRequiredMinutes || 0} 分钟`,
    `预算判断：${resolveBudgetMessage(summary)}`,
    "",
    ...buildOverallExplanationSection(planResult?.overallExplanation),
    ...buildPathSection("二、本轮推荐学习", scheduledItems),
    "",
    ...buildPathSection("二-补充、下一轮建议", deferredItems),
    "",
    ...buildResourceSection(resourceRecommendations),
  ];

  return `${lines.join("\n")}\n`;
}

export function buildLearningPathExportFileName({
  targetCode = "",
  generatedAt = new Date(),
  filePrefix = "learning-path",
} = {}) {
  const fileTarget = sanitizeFileNameSegment(targetCode) || "learning-path";
  const normalizedPrefix = sanitizeFileNameSegment(filePrefix) || "learning-path";
  return `${normalizedPrefix}-${fileTarget}-${formatFileTimestamp(generatedAt)}.txt`;
}

export function downloadLearningPathExport(payload = {}) {
  if (typeof window === "undefined" || typeof document === "undefined") {
    throw new Error("当前环境不支持文件导出。");
  }

  const generatedAt = payload.generatedAt || new Date();
  const content = buildLearningPathExportText({
    ...payload,
    generatedAt,
  });
  const fileName = buildLearningPathExportFileName({
    targetCode: payload.targetCode,
    generatedAt,
    filePrefix: payload.filePrefix,
  });

  const blob = new Blob([content], { type: "text/plain;charset=utf-8" });
  const objectUrl = window.URL.createObjectURL(blob);
  const anchor = document.createElement("a");
  anchor.href = objectUrl;
  anchor.download = fileName;
  anchor.style.display = "none";
  document.body.appendChild(anchor);
  anchor.click();
  anchor.remove();
  window.URL.revokeObjectURL(objectUrl);

  return {
    fileName,
    content,
  };
}
