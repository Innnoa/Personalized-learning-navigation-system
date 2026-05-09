<template>
  <PageLayout
    :eyebrow="pageEyebrow"
    :title="pageTitle"
    :description="pageDescription"
  >
    <section class="resource-page-layout">
      <article class="card surface-panel resource-summary-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">页面层级</p>
            <h3>{{ resourcePageHeading }}</h3>
          </div>
          <p class="caption">
            {{ resourcePageCaption }}
          </p>
        </div>

        <div
          v-if="resourcePageTabs.length > 0"
          class="resource-page-tabs"
        >
          <button
            v-for="tab in resourcePageTabs"
            :key="`resource-page-tab-${tab.mode}`"
            type="button"
            class="resource-page-tab"
            :class="{
              'resource-page-tab--active': activeResourcePageLevel === tab.mode,
            }"
            @click="goToResourcePageLevel(tab.mode)"
          >
            <strong>{{ tab.label }}</strong>
            <span>{{ tab.caption }}</span>
          </button>
        </div>

        <div class="section-headline section-headline--resource-scope">
          <div>
            <p class="label page-section-label">资源范围</p>
            <h3>{{ resourceSectionHeading }}</h3>
          </div>
          <p class="caption">
            {{ resourceSectionCaption }}
          </p>
        </div>

        <div v-if="availableSections.length === 0" class="empty-tip">
          当前还没有可用的资源推荐快照，请先回到首页生成学习路径。
        </div>

        <div v-else class="sibling-sections">
          <button
            v-for="section in availableSections"
            :key="`section-link-${section.code}`"
            type="button"
            class="sibling-section-button"
            :class="{
              'sibling-section-button--active':
                currentSection && currentSection.code === section.code,
            }"
            @click="goToSection(section.code)"
          >
            <strong>{{ section.name }}</strong>
            <span>{{ formatResourceSectionStatus(section.status) }}</span>
            <span>{{ section.resourceCount }} 条资源</span>
          </button>
        </div>
      </article>

      <article v-if="currentSection" class="card surface-panel resource-detail-card">
        <div class="section-headline">
          <div>
            <p class="label page-section-label">资源详情</p>
            <h3>{{ currentSection.name }}推荐资源</h3>
          </div>
          <p class="caption">
            {{ currentSectionCoverageNote || currentSection.recommendedUsage }}
          </p>
        </div>

        <div class="resource-guidance-panel">
          <p class="resource-guidance-title">当前智能推荐策略</p>
          <p class="resource-guidance-copy">
            {{ displayGuidanceText }}
          </p>
          <button
            v-if="hasHiddenResources"
            type="button"
            class="ghost-button"
            @click="toggleShowAllResources"
          >
            {{ showAllResources ? "收起额外资源" : `展开全部资源（+${hiddenResourceCount}）` }}
          </button>
        </div>

        <dl class="summary-grid">
          <div>
            <dt>资源总数</dt>
            <dd>{{ currentSection.resourceCount }} 条</dd>
          </div>
          <div>
            <dt>{{ statusOrParentLabel }}</dt>
            <dd>{{ statusOrParentValue }}</dd>
          </div>
          <div>
            <dt>优先资源</dt>
            <dd>{{ currentSection.primaryResourceTitle || "暂无" }}</dd>
          </div>
          <div>
            <dt>{{ budgetOrScopeLabel }}</dt>
            <dd>{{ budgetOrScopeValue }}</dd>
          </div>
        </dl>

        <ul
          v-if="getSectionResourceLayerSummary(currentSection).length > 0"
          class="resource-layer-summary"
        >
          <li
            v-for="item in getSectionResourceLayerSummary(currentSection)"
            :key="`${currentSection.code}-${item.layer}`"
            class="resource-layer-chip"
            :class="resourceLayerClass(item.layer)"
          >
            <strong>{{ item.layer }}</strong>
            <span>{{ item.count }}</span>
          </li>
        </ul>

        <div v-if="actionMessage" class="state state--success">
          {{ actionMessage }}
        </div>
        <div v-if="actionError" class="state state--error">
          {{ actionError }}
        </div>

        <ul class="resource-list">
          <li
            v-for="resource in displayedResources"
            :key="`${currentSection.code}-${resource.url}`"
            class="resource-item"
          >
            <div class="resource-item-head">
              <a
                :href="resource.url"
                target="_blank"
                rel="noreferrer"
                class="resource-link"
                @click="handleResourceClick(currentSection, resource)"
              >
                {{ resource.title }}
              </a>
              <div class="resource-item-badges">
                <span
                  v-if="resource.isPrimaryRecommendation"
                  class="resource-top-badge"
                >
                  优先看
                </span>
                <span
                  v-if="resource.resourceLayer"
                  class="resource-layer-badge"
                  :class="resourceLayerClass(resource.resourceLayer)"
                >
                  {{ resource.resourceLayer }}
                </span>
                <span
                  v-if="resource.inheritedFromLabel"
                  class="resource-origin-badge"
                >
                  {{ formatResourceOriginLabel(resource) }}
                </span>
                <span
                  v-if="resource.lastInteractionLabel"
                  class="resource-state-badge"
                  :class="resourceBehaviorBadgeClass(resource.lastInteractionType)"
                >
                  {{ resource.lastInteractionLabel }}
                </span>
                <span class="resource-type">
                  {{ formatResourceType(resource.type) }}
                </span>
              </div>
            </div>

            <div
              v-if="resource.recommendedPhase || resource.focusNodeLabel || (resource.focusTags || []).length > 0"
              class="resource-tag-stack"
            >
              <div
                v-if="resource.recommendedPhase || resource.focusNodeLabel"
                class="resource-tag-row"
              >
                <span v-if="resource.recommendedPhase" class="resource-phase">
                  {{ resource.recommendedPhase }}
                </span>
                <span v-if="resource.focusNodeLabel" class="resource-focus-node">
                  对应细化节点：{{ resource.focusNodeLabel }}
                </span>
              </div>

              <ul
                v-if="(resource.focusTags || []).length > 0"
                class="resource-focus-tags"
              >
                <li
                  v-for="tag in resource.focusTags || []"
                  :key="`${currentSection.code}-${resource.url}-${tag}`"
                  class="resource-focus-tag"
                >
                  {{ tag }}
                </li>
              </ul>
            </div>

            <div
              v-if="(resource.linkedReasonLabels || []).length > 0"
              class="resource-reason-links"
            >
              <span class="resource-reason-caption">对应推荐理由</span>
              <ul class="resource-reason-tags">
                <li
                  v-for="label in resource.linkedReasonLabels || []"
                  :key="`${currentSection.code}-${resource.url}-${label}`"
                  class="resource-reason-tag"
                >
                  {{ label }}
                </li>
              </ul>
            </div>

            <p class="resource-meta">{{ resource.source }}</p>
            <p class="resource-description">{{ resource.description }}</p>
            <p
              v-if="resource.linkedReasonLabelSummary"
              class="resource-reason-summary"
            >
              {{ resource.linkedReasonLabelSummary }}
            </p>
            <p
              v-if="resource.selectionReason"
              class="resource-selection-reason"
            >
              选择原因：{{ resource.selectionReason }}
            </p>
            <p
              v-if="resource.behaviorAdjustmentHint"
              class="resource-behavior-note"
            >
              排序调整：{{ resource.behaviorAdjustmentHint }}
            </p>
            <p
              v-if="resource.resourceLayerHint"
              class="resource-layer-hint"
            >
              分层说明：{{ resource.resourceLayerHint }}
            </p>
            <p class="resource-usage">
              使用建议：{{ resource.recommendedUsage }}
            </p>

            <div class="resource-action-row">
              <span class="resource-action-caption">
                点击标题会记为“已打开”，也可直接补充资源反馈：
              </span>
              <div class="resource-action-buttons">
                <button
                  v-for="action in resourceInteractionButtons"
                  :key="`${currentSection.code}-${resource.url}-${action.value}`"
                  type="button"
                  class="resource-action-button"
                  :class="resourceActionButtonClass(action.value)"
                  @click="handleResourceAction(currentSection, resource, action.value)"
                >
                  {{ action.label }}
                </button>
              </div>
            </div>
          </li>
        </ul>
      </article>

      <article
        v-else-if="hasResourceSnapshot"
        class="card surface-panel resource-detail-card resource-detail-card--empty"
      >
        <div class="section-headline">
          <div>
            <p class="label page-section-label">资源兜底</p>
            <h3>当前节点资源快照已失效</h3>
          </div>
          <p class="caption">
            你访问的节点不在最近一次资源快照中，通常是因为首页重新生成了路径，或当前链接来自旧会话。
          </p>
        </div>

        <div class="empty-tip resource-fallback-tip">
          当前请求节点：{{ missingSectionCodeLabel }}
          <span v-if="summary.targetLabel">
            · 最近一次目标：{{ summary.targetLabel }}
          </span>
        </div>

        <div class="hero-actions">
          <button type="button" class="ghost-button" @click="goToPlannerHome">
            回首页重新生成路径
          </button>
          <button
            type="button"
            class="ghost-button"
            @click="goToSection(availableSections[0].code)"
          >
            查看最近缓存资源
          </button>
        </div>
      </article>
    </section>
  </PageLayout>
</template>

<script setup>
import { computed, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import PageLayout from "../components/PageLayout.vue";
import { recordResourceView } from "../api/resource";
import { useNavigationStore } from "../stores/navigationStore";
import {
  buildTimeBudgetTierHint,
  resolveTimeBudgetResourceLimitBonus,
  resolveTimeBudgetTier,
} from "../utils/resourceRecommendationPolicy";

// ResourceRecommendationView is the main learning resource recommendation page.
// It renders the learning resource recommendation snapshot produced on the home page,
// lets the learner switch between sections, and shows why each recommendation was chosen.
// Search terms kept here on purpose: learning resource recommendation, resource
// recommendation flow, recommendation detail page.
const route = useRoute();
const router = useRouter();
const navigationStore = useNavigationStore();

const actionMessage = ref("");
const actionError = ref("");
const showAllResources = ref(false);

const resourceInteractionButtons = [
  { value: "viewed", label: "记为已看" },
  { value: "completed", label: "记为学完" },
  { value: "save_for_later", label: "稍后再看" },
];

const resourceTypeLabelMap = {
  video: "视频教程",
  article: "文本教程",
  document: "参考资料",
};

const currentSectionCode = computed(() => String(route.params.code || ""));
const requestedResourcePageLevel = computed(() => String(route.query.level || ""));
const mainResourceSections = computed(() =>
  navigationStore.resourceRecommendationSectionsByMode("main") || [],
);
const detailResourceSections = computed(() =>
  navigationStore.resourceRecommendationSectionsByMode("detail") || [],
);
const resourcePageTabs = computed(() => {
  const tabs = [];
  if (mainResourceSections.value.length > 0) {
    tabs.push({
      mode: "main",
      label: "一级页面",
      caption: `${mainResourceSections.value.length} 个一级资源节点`,
    });
  }
  if (detailResourceSections.value.length > 0) {
    tabs.push({
      mode: "detail",
      label: "二级页面",
      caption: `${detailResourceSections.value.length} 个细化资源节点`,
    });
  }
  return tabs;
});
const activeResourcePageLevel = computed(() => {
  if (
    requestedResourcePageLevel.value === "detail" &&
    detailResourceSections.value.length > 0
  ) {
    return "detail";
  }

  if (
    requestedResourcePageLevel.value === "main" &&
    mainResourceSections.value.length > 0
  ) {
    return "main";
  }

  if (
    detailResourceSections.value.some((item) => item.code === currentSectionCode.value)
  ) {
    return "detail";
  }

  if (
    mainResourceSections.value.some((item) => item.code === currentSectionCode.value)
  ) {
    return "main";
  }

  if (
    navigationStore.resourceRecommendationDetailSections.length > 0 &&
    navigationStore.resourceRecommendationSummary.contextMode === "detail"
  ) {
    return "detail";
  }

  if (mainResourceSections.value.length > 0) {
    return "main";
  }

  if (detailResourceSections.value.length > 0) {
    return "detail";
  }

  return "main";
});
const availableSections = computed(() =>
  activeResourcePageLevel.value === "detail"
    ? detailResourceSections.value
    : mainResourceSections.value,
);
const hasResourceSnapshot = computed(
  () =>
    mainResourceSections.value.length > 0 || detailResourceSections.value.length > 0,
);
const currentSection = computed(() =>
  navigationStore.resourceSectionByModeAndCode(
    activeResourcePageLevel.value,
    currentSectionCode.value,
  ),
);
const currentSectionResourceCount = computed(
  () => currentSection.value?.resources?.length || 0,
);
const currentSectionHasInheritedResources = computed(() =>
  (currentSection.value?.resources || []).some(
    (item) => item.resourceCoverageType === "inherited",
  ),
);
const currentSectionHasRelatedResources = computed(() =>
  (currentSection.value?.resources || []).some(
    (item) => item.resourceCoverageType === "related",
  ),
);
const missingSectionCodeLabel = computed(() =>
  currentSectionCode.value ? currentSectionCode.value : "未指定节点",
);
const summary = computed(
  () =>
    navigationStore.resourceRecommendationSummaryByMode(
      activeResourcePageLevel.value,
    ) || {
      contextMode: activeResourcePageLevel.value,
      sourcePage: "home",
      targetLabel: "",
      availableMinutes: 0,
      scheduledCount: 0,
      deferredCount: 0,
      scopeCode: "root",
      scopeLabel: "",
      parentNodeCode: "",
      parentNodeLabel: "",
      generatedAt: "",
    },
);
const isDetailMode = computed(() => activeResourcePageLevel.value === "detail");

const timeBudgetTier = computed(() =>
  resolveTimeBudgetTier(summary.value.availableMinutes),
);

const timeBudgetTierHint = computed(() =>
  buildTimeBudgetTierHint(timeBudgetTier.value),
);

const maxRecommendedResources = computed(() => {
  const section = currentSection.value;
  if (!section) {
    return 0;
  }

  const availableMinutes = Number(summary.value.availableMinutes || 0);
  const masteryPercent = Number(section.masteryPercent || 0);
  const difficultyLevel = Number(section.difficultyLevel || 0);
  const isScheduled = section.status === "scheduled";

  let limit = 2;
  limit += resolveTimeBudgetResourceLimitBonus(availableMinutes);

  if (isScheduled) {
    limit += 1;
  }

  if (masteryPercent < 40) {
    limit += 1;
  }

  if (difficultyLevel >= 4) {
    limit += 1;
  }

  if (isDetailMode.value) {
    limit += 1;
  }

  if (section.status === "deferred") {
    limit -= 1;
  }

  const boundedLimit = Math.max(2, Math.min(5, limit));
  return Math.min(currentSectionResourceCount.value, boundedLimit);
});

const displayedResources = computed(() => {
  const resources = currentSection.value?.resources || [];
  if (showAllResources.value) {
    return resources;
  }

  return resources.slice(0, maxRecommendedResources.value);
});

const hiddenResourceCount = computed(() =>
  Math.max(0, currentSectionResourceCount.value - maxRecommendedResources.value),
);

const hasHiddenResources = computed(() => hiddenResourceCount.value > 0);

const displayGuidanceText = computed(() => {
  if (!currentSection.value) {
    return "当前节点暂无可展示资源。";
  }

  const budgetIntro = `当前时间预算 ${summary.value.availableMinutes || 0} 分钟（${timeBudgetTier.value}）。${timeBudgetTierHint.value}`;
  const base = `系统已结合当前学习进度、能力状态与时间预算，优先展示 ${displayedResources.value.length} 条最匹配资源。`;
  if (!hasHiddenResources.value) {
    return `${budgetIntro} ${base}`;
  }

  return `${budgetIntro} ${base} 其余 ${hiddenResourceCount.value} 条已折叠，可按需展开。`;
});

const resourcePageHeading = computed(() =>
  isDetailMode.value ? "二级资源页面" : "一级资源页面",
);

const resourcePageCaption = computed(() =>
  isDetailMode.value
    ? "二级页面只展示当前细化学习所属一级节点内部的二级资源，适合聚焦局部学习。"
    : "一级页面展示课程主线路径里的一级资源入口，适合先总览本轮主学习资源。",
);

const pageEyebrow = computed(() =>
  isDetailMode.value ? "资源建议 · 二级页面" : "资源建议 · 一级页面",
);

const pageTitle = computed(() =>
  currentSection.value
    ? `${currentSection.value.name}推荐资源`
    : "单节点推荐资源",
);

const pageDescription = computed(() => {
  if (!currentSection.value) {
    return isDetailMode.value
      ? "单独查看当前细化学习节点的配套资源。"
      : "单独查看当前主路径一级节点的配套资源。";
  }

  if (isDetailMode.value) {
    return `${currentSection.value.name}当前有 ${currentSection.value.resourceCount} 条推荐资源，资源范围限定在“${summary.value.parentNodeLabel || "当前一级节点"}”这一细化学习分支内。`;
  }

  return `${currentSection.value.name}当前有 ${currentSection.value.resourceCount} 条推荐资源，排序会参考最近的资源行为。`;
});

const currentSectionHeroCopy = computed(() => {
  if (!currentSection.value) {
    return "";
  }

  if (isDetailMode.value) {
    return `${formatResourceSectionStatus(currentSection.value.status)} · ${currentSection.value.resourceCount} 条资源 · 所属一级节点：${summary.value.parentNodeLabel || "当前分支"}`;
  }

  return `${formatResourceSectionStatus(currentSection.value.status)} · ${currentSection.value.resourceCount} 条资源 · ${summary.value.targetLabel ? `当前目标：${summary.value.targetLabel}` : "来自最近一次生成的学习路径"}`;
});

function toggleShowAllResources() {
  showAllResources.value = !showAllResources.value;
}

const returnButtonLabel = computed(() =>
  isDetailMode.value ? "返回细化学习" : "返回首页路径规划",
);

const resourceSectionHeading = computed(() =>
  isDetailMode.value ? "当前一级节点下的二级资源导航" : "课程主线一级资源导航",
);

const resourceSectionCaption = computed(() =>
  isDetailMode.value
    ? "先切到“二级页面”，再在当前一级节点下选择一个二级节点查看其专属资源。"
    : "先切到“一级页面”，再在课程主线路径中选择一个一级节点查看资源。",
);

const statusOrParentLabel = computed(() =>
  isDetailMode.value ? "所属一级节点" : "当前状态",
);

const statusOrParentValue = computed(() =>
  isDetailMode.value
    ? summary.value.parentNodeLabel || "未指定"
    : formatResourceSectionStatus(currentSection.value?.status),
);

const budgetOrScopeLabel = computed(() =>
  isDetailMode.value ? "细化范围" : "时间预算",
);

const budgetOrScopeValue = computed(() =>
  isDetailMode.value
    ? summary.value.scopeLabel || "当前细化范围"
    : `${summary.value.availableMinutes || 0} 分钟（${timeBudgetTier.value}）`,
);

const currentSectionCoverageNote = computed(() => {
  if (
    currentSectionHasInheritedResources.value &&
    currentSectionHasRelatedResources.value
  ) {
    return "当前节点的专属资源仍在补齐中，以下内容会混合复用上层节点材料与同层关联节点材料，并继续按当前路径依据排序。";
  }

  if (currentSectionHasInheritedResources.value) {
    return "当前节点暂无完全专属资源，以下内容会优先复用上层节点材料，并继续按当前路径依据排序。";
  }

  if (currentSectionHasRelatedResources.value) {
    return "当前节点会补充同一细化范围内关联节点的材料，帮助你从相邻知识点切入理解。";
  }

  return "";
});

function formatResourceSectionStatus(status) {
  if (status === "scheduled") {
    return "本轮推荐学习";
  }

  if (status === "deferred") {
    return "下一轮建议";
  }

  return "资源建议";
}

watch(
  () => currentSectionCode.value,
  () => {
    showAllResources.value = false;
  },
);

watch(
  () => activeResourcePageLevel.value,
  () => {
    showAllResources.value = false;
  },
);

function formatResourceType(type) {
  return resourceTypeLabelMap[type] || "学习资源";
}

function formatResourceOriginLabel(resource) {
  if (!resource?.inheritedFromLabel) {
    return "";
  }

  if (resource.resourceCoverageType === "related") {
    return `来自关联节点：${resource.inheritedFromLabel}`;
  }

  return `来自上层：${resource.inheritedFromLabel}`;
}

function resourceLayerClass(layer) {
  return {
    "resource-layer--primary": layer === "课程风格优先",
    "resource-layer--supplement": layer === "图文补充",
    "resource-layer--review": layer === "答辩复盘",
  };
}

function resourceActionButtonClass(interactionType) {
  return {
    "resource-action-button--viewed": interactionType === "viewed",
    "resource-action-button--completed": interactionType === "completed",
    "resource-action-button--later": interactionType === "save_for_later",
  };
}

function resourceBehaviorBadgeClass(interactionType) {
  return {
    "resource-state-badge--opened": interactionType === "opened",
    "resource-state-badge--viewed": interactionType === "viewed",
    "resource-state-badge--completed": interactionType === "completed",
    "resource-state-badge--later": interactionType === "save_for_later",
  };
}

function getSectionResourceLayerSummary(section) {
  return ["课程风格优先", "图文补充", "答辩复盘"]
    .map((layer) => ({
      layer,
      count: Number(section?.resourceLayerSummary?.[layer] || 0),
    }))
    .filter((item) => item.count > 0);
}

function goToSection(code, level = activeResourcePageLevel.value) {
  navigationStore.setActiveResourceRecommendationMode(level);
  router.push({
    name: "resource-recommendation",
    params: { code },
    query: {
      level,
    },
  });
}

function goToResourcePageLevel(level) {
  const targetSections =
    level === "detail" ? detailResourceSections.value : mainResourceSections.value;
  if (targetSections.length === 0) {
    return;
  }

  const targetCode =
    targetSections.find((item) => item.code === currentSectionCode.value)?.code ||
    targetSections[0]?.code ||
    "";
  if (!targetCode) {
    return;
  }

  goToSection(targetCode, level);
}

function goToPlannerHome() {
  if (isDetailMode.value && summary.value.scopeCode) {
    router.push({
      name: "detail-learning",
      query: {
        scope: summary.value.scopeCode,
      },
    });
    return;
  }

  const targetCode = currentSection.value?.code || currentSectionCode.value || "";
  router.push({
    name: "home",
    query: targetCode
      ? {
          target: targetCode,
        }
      : {},
  });
}

function goToSiblingSection(currentCode) {
  const targetSection =
    availableSections.value.find((section) => section.code !== currentCode) ||
    availableSections.value[0];
  if (!targetSection) {
    return;
  }

  goToSection(targetSection.code, activeResourcePageLevel.value);
}

function buildBehaviorHint(interactionType, sectionStatus) {
  if (interactionType === "completed") {
    return "你已将这条资源记为学完，返回首页重新生成路径后，它会自动后移。";
  }

  if (interactionType === "viewed") {
    return "你已将这条资源记为已看，返回首页重新生成路径后，系统会优先展示未接触资源。";
  }

  if (interactionType === "opened") {
    return "你已打开过这条资源，返回首页重新生成路径后，系统会略微降低它的优先级。";
  }

  if (interactionType === "save_for_later") {
    return sectionStatus === "deferred"
      ? "你已将这条资源标记为稍后再看，下一轮建议中仍会保留它。"
      : "你已将这条资源标记为稍后再看，当前轮次会优先展示更适合立即使用的材料。";
  }

  return "";
}

function updateLocalResourceState(sectionCode, resourceUrl, interactionType) {
  const section = navigationStore.resourceSectionByCode(sectionCode);
  if (!section?.resources) {
    return;
  }

  const targetResource = section.resources.find((item) => item.url === resourceUrl);
  if (!targetResource) {
    return;
  }

  const interactionLabelMap = {
    opened: "已打开",
    viewed: "已看过",
    completed: "已学完",
    save_for_later: "稍后再看",
  };

  targetResource.lastInteractionType = interactionType;
  targetResource.lastInteractionLabel = interactionLabelMap[interactionType] || "";
  targetResource.behaviorAdjustmentHint = buildBehaviorHint(
    interactionType,
    section.status,
  );
  navigationStore.persistResourceContext();
}

async function recordResourceInteraction(section, resource, interactionType) {
  actionMessage.value = "";
  actionError.value = "";

  const payload = await recordResourceView({
    learnerCode: navigationStore.activeLearnerCode || "demo-learner",
    knowledgePointCode: section.recordingKnowledgePointCode || section.code,
    resourceTitle: resource.title,
    resourceUrl: resource.url,
    resourceType: resource.type || "",
    resourceSource: resource.source || "",
    resourceLayer: resource.resourceLayer || "",
    recommendedPhase: resource.recommendedPhase || "",
    sourceContext: isDetailMode.value
      ? "detail_scope_resource_panel"
      : "resource_recommendation_page",
    scopeCode: section.scopeCode || summary.value.scopeCode || "root",
    linkedReasonLabelSummary: resource.linkedReasonLabelSummary || "",
    interactionType,
  });

  updateLocalResourceState(section.code, resource.url, interactionType);
  actionMessage.value = isDetailMode.value
    ? `${payload.recordedResource.interactionTypeLabel}已记录。返回细化学习或首页重新生成路径后，可看到资源排序的最新变化。`
    : `${payload.recordedResource.interactionTypeLabel}已记录。返回首页重新生成路径后，可看到资源排序的最新变化。`;
}

async function handleResourceClick(section, resource) {
  try {
    await recordResourceInteraction(section, resource, "opened");
  } catch (error) {
    actionError.value =
      error?.response?.data?.detail || "记录资源行为失败，请检查后端日志。";
    console.error(error);
  }
}

async function handleResourceAction(section, resource, interactionType) {
  try {
    await recordResourceInteraction(section, resource, interactionType);
  } catch (error) {
    actionError.value =
      error?.response?.data?.detail || "记录资源行为失败，请检查后端日志。";
    console.error(error);
  }
}

watch(
  activeResourcePageLevel,
  (level) => {
    navigationStore.setActiveResourceRecommendationMode(level);
  },
  { immediate: true },
);
</script>

<style scoped>
.resource-page-layout {
  display: grid;
  gap: 20px;
}

h2,
h3 {
  margin: 0;
}

.caption,
.hero-note-copy {
  margin: 0;
  color: #44515c;
  line-height: 1.6;
}

.hero-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-top: 18px;
}

.ghost-button {
  border: none;
  border-radius: 999px;
  padding: 9px 14px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-weight: 700;
  cursor: pointer;
}

.resource-page-tabs {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 12px;
  margin-top: 18px;
}

.resource-page-tab {
  display: grid;
  gap: 6px;
  align-items: start;
  text-align: left;
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.9);
  padding: 14px;
  cursor: pointer;
  color: #24323b;
}

.resource-page-tab strong {
  font-size: 1rem;
}

.resource-page-tab span {
  color: #5a6974;
  font-size: 0.88rem;
}

.resource-page-tab--active {
  border-color: rgba(12, 106, 113, 0.22);
  background: rgba(12, 106, 113, 0.08);
}

.section-headline {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: start;
}

.section-headline--resource-scope {
  margin-top: 18px;
}

.sibling-sections {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 12px;
  margin-top: 18px;
}

.sibling-section-button {
  display: grid;
  gap: 6px;
  align-items: start;
  text-align: left;
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.9);
  padding: 14px;
  cursor: pointer;
  color: #24323b;
}

.sibling-section-button strong {
  font-size: 1rem;
}

.sibling-section-button span {
  color: #5a6974;
  font-size: 0.88rem;
}

.sibling-section-button--active {
  border-color: rgba(12, 106, 113, 0.22);
  background: rgba(12, 106, 113, 0.08);
}

.summary-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
  margin-top: 18px;
}

.summary-grid div {
  padding: 14px;
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.9);
}

dt {
  font-size: 0.82rem;
  color: #6d7781;
}

dd {
  margin: 4px 0 0;
  font-weight: 700;
  color: #24323b;
}

.resource-layer-summary,
.resource-list,
.resource-focus-tags,
.resource-reason-tags {
  margin: 0;
  padding: 0;
  list-style: none;
}

.resource-layer-summary {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 18px;
}

.resource-guidance-panel {
  margin-top: 16px;
  padding: 14px;
  border-radius: 16px;
  background: rgba(12, 106, 113, 0.06);
  border: 1px solid rgba(12, 106, 113, 0.12);
  display: grid;
  gap: 10px;
}

.resource-guidance-title {
  margin: 0;
  font-weight: 700;
  color: #0c5960;
}

.resource-guidance-copy {
  margin: 0;
  color: #44515c;
  line-height: 1.55;
}

.resource-layer-chip,
.resource-layer-badge,
.resource-origin-badge,
.resource-phase,
.resource-focus-node,
.resource-focus-tag,
.resource-reason-tag,
.resource-top-badge,
.resource-type,
.resource-state-badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 4px 10px;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 700;
}

.resource-layer--primary,
.resource-phase {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.resource-layer--supplement,
.resource-focus-tag,
.resource-type,
.resource-state-badge--opened,
.resource-state-badge--viewed {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.resource-layer--review,
.resource-focus-node,
.resource-reason-tag,
.resource-state-badge--later {
  background: rgba(55, 82, 129, 0.12);
  color: #27456f;
}

.resource-state-badge--completed {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.resource-top-badge {
  background: rgba(225, 147, 35, 0.16);
  color: #8a4d10;
}

.resource-origin-badge {
  background: rgba(121, 74, 153, 0.12);
  color: #68418a;
}

.resource-list {
  display: grid;
  gap: 14px;
  margin-top: 20px;
}

.resource-item {
  border-radius: 20px;
  background: rgba(247, 250, 249, 0.9);
  padding: 18px;
  contain: paint;
}

.resource-item-head {
  display: flex;
  justify-content: space-between;
  gap: 14px;
  align-items: start;
}

.resource-item-badges,
.resource-focus-tags,
.resource-action-buttons {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.resource-tag-stack {
  display: grid;
  gap: 10px;
  margin-top: 12px;
}

.resource-tag-row {
  display: flex;
  flex-wrap: wrap;
  align-items: flex-start;
  gap: 8px;
}

.resource-focus-tags {
  align-items: flex-start;
  row-gap: 10px;
}

.resource-item-badges {
  justify-content: end;
}

.resource-link {
  color: #0c5960;
  font-weight: 700;
  text-decoration: none;
}

.resource-link:hover {
  text-decoration: underline;
}

.resource-reason-links,
.resource-action-row {
  display: grid;
  gap: 8px;
  margin-top: 10px;
}

.resource-meta,
.resource-description,
.resource-reason-summary,
.resource-selection-reason,
.resource-behavior-note,
.resource-layer-hint,
.resource-usage {
  margin: 8px 0 0;
  color: #44515c;
  line-height: 1.55;
}

.resource-selection-reason {
  color: #214861;
  font-weight: 600;
}

.resource-behavior-note {
  color: #6b4b14;
}

.resource-reason-caption,
.resource-action-caption {
  color: #53616c;
  font-size: 0.84rem;
  font-weight: 700;
}

.resource-action-row {
  padding-top: 12px;
  border-top: 1px dashed rgba(22, 32, 42, 0.12);
}

.resource-action-button {
  border: 1px solid rgba(12, 106, 113, 0.14);
  background: rgba(12, 106, 113, 0.06);
  color: #0c5960;
  font-weight: 700;
  padding: 7px 12px;
  border-radius: 999px;
  cursor: pointer;
}

.resource-action-button--completed {
  background: rgba(28, 113, 58, 0.1);
  border-color: rgba(28, 113, 58, 0.16);
  color: #14542d;
}

.resource-action-button--later {
  background: rgba(55, 82, 129, 0.1);
  border-color: rgba(55, 82, 129, 0.16);
  color: #27456f;
}

.empty-tip,
.state {
  margin-top: 18px;
  border-radius: 18px;
  padding: 18px;
}

.empty-tip {
  background: rgba(247, 250, 249, 0.9);
  color: #44515c;
}

.resource-detail-card--empty {
  align-content: start;
}

.resource-fallback-tip {
  margin-top: 0;
}

.state--success {
  background: rgba(28, 113, 58, 0.12);
  color: #14542d;
}

.state--error {
  background: rgba(176, 47, 47, 0.1);
  color: #8d2323;
}

@supports (content-visibility: auto) {
  .resource-item {
    content-visibility: auto;
    contain-intrinsic-size: 180px;
  }
}

@media (max-width: 980px) {
  .section-headline,
  .resource-item-head {
    flex-direction: column;
  }

  .resource-item-badges {
    justify-content: start;
  }

  .summary-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}

@media (max-width: 720px) {
  .summary-grid {
    grid-template-columns: 1fr;
  }
}
</style>
