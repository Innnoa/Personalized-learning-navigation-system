<template>
  <article class="card">
    <div class="card-head">
      <div>
        <p class="label">当前学习图谱</p>
        <h2>按层浏览当前学习图谱</h2>
      </div>
      <p class="caption">
        当前只展示所在层级的节点；节点颜色表示掌握度，双击可细化节点可进入下一层图谱。
      </p>
    </div>

    <div v-if="loading" class="state state--loading">
      正在读取当前层级图谱...
    </div>

    <div v-else-if="loadError" class="state state--error">
      {{ loadError }}
    </div>

    <template v-else-if="currentScopeGraph.nodes.length > 0">
      <div class="scope-toolbar">
        <div class="breadcrumb-row">
          <button
            v-for="item in breadcrumbs"
            :key="item.scopeCode"
            type="button"
            class="breadcrumb-button"
            :class="{
              'breadcrumb-button--active': item.scopeCode === currentScopeCode,
            }"
            @click="navigateToScope(item.scopeCode)"
          >
            {{ item.label }}
          </button>
        </div>

        <div class="scope-note">
          <p class="scope-label">当前范围</p>
          <strong data-testid="current-scope-name">{{ currentScopeName }}</strong>
          <span>{{ resolveScopeLevelLabel(currentScopeLevel) }}</span>
        </div>
      </div>

      <dl class="summary-grid">
        <div>
          <dt>当前层节点</dt>
          <dd data-testid="graph-node-count">{{ currentScopeGraph.nodes.length }} 个</dd>
        </div>
        <div>
          <dt>已学习节点</dt>
          <dd data-testid="learned-node-count">{{ learnedNodeCount }} 个</dd>
        </div>
        <div>
          <dt>未学习节点</dt>
          <dd>{{ unlearnedNodeCount }} 个</dd>
        </div>
        <div>
          <dt>当前层路径</dt>
          <dd data-testid="graph-edge-count">{{ currentScopeGraph.edges.length }} 条</dd>
        </div>
      </dl>

      <p class="rule-note">
        颜色越深表示掌握度越高；当前图谱只展示所处 scope 的节点结构，二级 / 三级节点通过双击逐层进入。
      </p>

      <div class="legend-row">
        <span class="legend-chip legend-chip--progress-low">浅粉：0% - 35%</span>
        <span class="legend-chip legend-chip--progress-mid">中粉：36% - 70%</span>
        <span class="legend-chip legend-chip--progress-high">深粉：71% - 100%</span>
        <span class="legend-chip legend-chip--selected">描边强调：当前选中节点</span>
        <span class="legend-chip legend-chip--context">描边增强：上下关联节点</span>
      </div>

      <div class="graph-layout">
        <div ref="graphRef" class="graph-canvas"></div>
      </div>

      <aside class="detail-panel">
        <template v-if="selectedNode">
          <div class="detail-head">
            <div>
              <p class="label label--detail">节点详情</p>
              <h3>{{ selectedNode.label }}</h3>
            </div>
            <span
              class="mastery-badge"
              :class="{
                'mastery-badge--mastered': selectedNode.masteryState === 'mastered',
                'mastery-badge--progress': selectedNode.masteryState === 'in-progress',
                'mastery-badge--idle': selectedNode.masteryState === 'not-started',
              }"
            >
              {{ resolveMasteryStateLabel(selectedNode.masteryState) }}
            </span>
          </div>

          <dl class="detail-grid">
            <div>
              <dt>章节</dt>
              <dd>第{{ selectedNode.chapterNo }}章</dd>
            </div>
            <div>
              <dt>掌握度</dt>
              <dd>{{ selectedNode.masteryPercent }}%</dd>
            </div>
            <div>
              <dt>图谱层级</dt>
              <dd>{{ resolveScopeLevelLabel(currentScopeLevel) }}</dd>
            </div>
            <div>
              <dt>所在范围</dt>
              <dd>{{ currentScopeName }}</dd>
            </div>
            <div>
              <dt>类型</dt>
              <dd>{{ resolveNodeTypeLabel(selectedNode.nodeType) }}</dd>
            </div>
            <div>
              <dt>预计学习</dt>
              <dd>{{ selectedNode.estimatedMinutes }} 分钟</dd>
            </div>
          </dl>

          <p class="detail-description">
            {{ selectedNode.description || "当前节点暂未补充说明。" }}
          </p>

          <div class="detail-actions">
            <button
              type="button"
              class="detail-action-button detail-action-button--primary"
              data-testid="graph-set-target-button"
              @click="emit('set-target', { code: selectedNode.code, scopeCode: currentScopeCode })"
            >
              设为当前学习目标
            </button>
          </div>

          <div class="relation-grid">
            <section class="relation-section">
              <div class="relation-head">
                <h4>直接前置节点</h4>
                <span>{{ directPrerequisites.length }} 个</span>
              </div>
              <ul v-if="directPrerequisites.length > 0" class="relation-list">
                <li
                  v-for="item in directPrerequisites"
                  :key="`${selectedNode.code}-pre-${item.code}`"
                  class="relation-item"
                >
                  <div class="relation-main">
                    <strong>{{ item.label }}</strong>
                    <span
                      class="relation-state"
                      :class="`relation-state--${item.masteryState}`"
                    >
                      {{ resolveMasteryStateLabel(item.masteryState) }}
                    </span>
                  </div>
                  <p>掌握度 {{ item.masteryPercent }}%</p>
                </li>
              </ul>
              <p v-else class="empty-tip">该节点当前没有直接前置节点。</p>
            </section>

            <section class="relation-section">
              <div class="relation-head">
                <h4>直接后继节点</h4>
                <span>{{ directDependents.length }} 个</span>
              </div>
              <ul v-if="directDependents.length > 0" class="relation-list">
                <li
                  v-for="item in directDependents"
                  :key="`${selectedNode.code}-next-${item.code}`"
                  class="relation-item"
                >
                  <div class="relation-main">
                    <strong>{{ item.label }}</strong>
                    <span
                      class="relation-state"
                      :class="`relation-state--${item.masteryState}`"
                    >
                      {{ resolveMasteryStateLabel(item.masteryState) }}
                    </span>
                  </div>
                  <p>掌握度 {{ item.masteryPercent }}%</p>
                </li>
              </ul>
              <p v-else class="empty-tip">该节点当前没有直接后继节点。</p>
            </section>
          </div>
        </template>

        <div v-else class="empty-tip empty-tip--panel">
          点击左侧任一节点后，可在这里查看它的掌握度、上下关联结构，以及是否还能继续细化。
        </div>
      </aside>
    </template>

    <div v-else class="empty-tip empty-tip--standalone">
      当前还没有可展示的课程图谱数据。
    </div>
  </article>
</template>

<script setup>
import { computed, nextTick, onBeforeUnmount, onMounted, ref, watch } from "vue";

import { fetchKnowledgeGraph } from "../api/knowledgeGraph";
import { useNavigationStore } from "../stores/navigationStore";

const props = defineProps({
  profile: {
    type: Object,
    default: null,
  },
  preferredSelectedCode: {
    type: String,
    default: "",
  },
});
const emit = defineEmits(["set-target"]);
const navigationStore = useNavigationStore();

const graphRef = ref(null);
const loading = ref(true);
const loadError = ref("");
const graphData = ref(null);
const selectedNodeCode = ref("");
const preferredSelectedCode = ref("");
let cyInstance = null;
let detachZoomWheelGuard = null;

const nodeTypeLabelMap = {
  foundation: "基础概念",
  structure: "核心结构",
  implementation: "实现方法",
  algorithm: "算法方法",
  application: "应用场景",
  abstraction: "抽象建模",
};

function normalizeProfileMastery(value) {
  const numericValue = Number(value || 0);
  if (numericValue <= 1) {
    return numericValue * 100;
  }

  return numericValue;
}

const masteryByCode = computed(() => {
  const graphMasteryByCode = props.profile?.graphMasteryByCode;
  if (graphMasteryByCode && typeof graphMasteryByCode === "object") {
    return new Map(
      Object.entries(graphMasteryByCode).map(([code, masteryScore]) => [
        code,
        normalizeProfileMastery(masteryScore),
      ]),
    );
  }

  const items = props.profile?.masteryItems || [];
  return new Map(
    items.map((item) => [item.code, Number(item.masteryPercent || 0)]),
  );
});

const breadcrumbs = computed(() => graphData.value?.breadcrumbs || []);
const currentScopeCode = computed(() => graphData.value?.view?.scopeCode || "root");
const currentScopeName = computed(() => graphData.value?.view?.scopeName || "课程主图");
const currentScopeLevel = computed(() => resolveScopeLevel(graphData.value));
const activeCourseCode = computed(() => String(props.profile?.course?.code || ""));

const currentScopeGraph = computed(() => normalizePayload(graphData.value, masteryByCode.value));

const learnedNodeCount = computed(
  () => currentScopeGraph.value.nodes.filter((node) => node.isLearned === 1).length,
);

const unlearnedNodeCount = computed(
  () => currentScopeGraph.value.nodes.filter((node) => node.isLearned !== 1).length,
);

const selectedNode = computed(() => {
  if (!selectedNodeCode.value) {
    return null;
  }

  return currentScopeGraph.value.nodeMap.get(selectedNodeCode.value) || null;
});

const directPrerequisites = computed(() => {
  if (!selectedNode.value) {
    return [];
  }

  const prerequisiteCodes = currentScopeGraph.value.edges
    .filter((edge) => edge.target === selectedNode.value.code)
    .map((edge) => edge.source);

  return prerequisiteCodes
    .map((code) => currentScopeGraph.value.nodeMap.get(code))
    .filter(Boolean)
    .sort(sortNodes);
});

const directDependents = computed(() => {
  if (!selectedNode.value) {
    return [];
  }

  const dependentCodes = currentScopeGraph.value.edges
    .filter((edge) => edge.source === selectedNode.value.code)
    .map((edge) => edge.target);

  return dependentCodes
    .map((code) => currentScopeGraph.value.nodeMap.get(code))
    .filter(Boolean)
    .sort(sortNodes);
});

function sortNodes(left, right) {
  if ((left.chapterNo || 0) !== (right.chapterNo || 0)) {
    return (left.chapterNo || 0) - (right.chapterNo || 0);
  }

  if ((left.displayOrder || 0) !== (right.displayOrder || 0)) {
    return (left.displayOrder || 0) - (right.displayOrder || 0);
  }

  return (left.label || left.code).localeCompare(right.label || right.code);
}

function resolveMasteryState(masteryPercent) {
  if (masteryPercent >= 85) {
    return "mastered";
  }

  if (masteryPercent >= 61) {
    return "familiar";
  }

  if (masteryPercent >= 31) {
    return "learning";
  }

  if (masteryPercent > 0) {
    return "beginner";
  }

  return "not-started";
}

function resolveNodeTypeLabel(nodeType) {
  return nodeTypeLabelMap[nodeType] || nodeType || "知识点";
}

function resolveMasteryStateLabel(masteryState) {
  const labelMap = {
    mastered: "已掌握",
    familiar: "基本掌握",
    learning: "学习中",
    beginner: "初步了解",
    "not-started": "未学习",
  };

  return labelMap[masteryState] || "未学习";
}

function resolveScopeLevelLabel(level) {
  if (level === 1) {
    return "一级主图";
  }

  if (level === 2) {
    return "二级细化图";
  }

  return "三级细化图";
}

function resolveScopeLevel(payload) {
  return Math.min(payload?.breadcrumbs?.length || (payload?.view?.isRoot ? 1 : 2), 3);
}

function clampMasteryPercent(value) {
  return Math.max(0, Math.min(100, Math.round(Number(value || 0))));
}

function interpolateChannel(start, end, ratio) {
  return Math.round(start + (end - start) * ratio);
}

function interpolateColor(start, end, ratio) {
  return `rgb(${interpolateChannel(start[0], end[0], ratio)}, ${interpolateChannel(
    start[1],
    end[1],
    ratio,
  )}, ${interpolateChannel(start[2], end[2], ratio)})`;
}

function resolveMasteryNodeColor(percent) {
  const masteryPercent = clampMasteryPercent(percent);

  if (masteryPercent <= 35) {
    return interpolateColor([255, 246, 250], [246, 217, 230], masteryPercent / 35);
  }

  if (masteryPercent <= 70) {
    return interpolateColor([246, 217, 230], [235, 181, 204], (masteryPercent - 35) / 35);
  }

  return interpolateColor([235, 181, 204], [220, 144, 176], (masteryPercent - 70) / 30);
}

function resolveMasteryBorderColor(percent) {
  const masteryPercent = clampMasteryPercent(percent);

  if (masteryPercent <= 35) {
    return interpolateColor([238, 206, 220], [227, 172, 194], masteryPercent / 35);
  }

  if (masteryPercent <= 70) {
    return interpolateColor([227, 172, 194], [211, 136, 167], (masteryPercent - 35) / 35);
  }

  return interpolateColor([211, 136, 167], [190, 104, 139], (masteryPercent - 70) / 30);
}

function resolveMasteryTextColor() {
  return "#63374a";
}

function buildNodeProgressBadgeDataUri(percent) {
  const masteryPercent = clampMasteryPercent(percent);
  const svg = `
    <svg xmlns="http://www.w3.org/2000/svg" width="38" height="18" viewBox="0 0 38 18">
      <rect x="0.75" y="0.75" width="36.5" height="16.5" rx="8.25" fill="rgba(255,255,255,0.88)" stroke="rgba(160,97,127,0.28)" stroke-width="1.5"/>
      <text x="19" y="12.2" text-anchor="middle" font-family="sans-serif" font-size="10" font-weight="700" fill="#7b4760">${masteryPercent}%</text>
    </svg>
  `;

  return `data:image/svg+xml;utf8,${encodeURIComponent(svg)}`;
}

function normalizePayload(payload, masteryLookup) {
  if (!payload?.nodes?.length) {
    return {
      nodes: [],
      edges: [],
      nodeMap: new Map(),
    };
  }

  const scopeLevel = resolveScopeLevel(payload);
  const nodes = payload.nodes
    .map((node) => {
      const masteryPercent = clampMasteryPercent(masteryLookup.get(node.code) || 0);
      return {
        ...node,
        scopeLevel,
        masteryPercent,
        masteryState: resolveMasteryState(masteryPercent),
        isLearned: masteryPercent > 0 ? 1 : 0,
        nodeColor: resolveMasteryNodeColor(masteryPercent),
        borderColor: resolveMasteryBorderColor(masteryPercent),
        textColor: resolveMasteryTextColor(masteryPercent),
        progressBadgeSvg: buildNodeProgressBadgeDataUri(masteryPercent),
      };
    })
    .sort(sortNodes);
  const nodeMap = new Map(nodes.map((node) => [node.code, node]));
  const edges = (payload.edges || []).map((edge) => ({
    ...edge,
    edgeKind: "dependency",
    isLearned:
      nodeMap.get(edge.source)?.isLearned === 1 && nodeMap.get(edge.target)?.isLearned === 1
        ? 1
        : 0,
  }));

  return {
    nodes,
    edges,
    nodeMap,
  };
}

function buildElements(payload) {
  const nodes = payload.nodes.map((node) => ({
    data: {
      id: node.code,
      label: node.label,
      isLearned: node.isLearned,
      scopeLevel: node.scopeLevel,
      detailScopeCode: node.detailScopeCode || "",
      hasDetailGraph: node.hasDetailGraph ? 1 : 0,
      nodeColor: node.nodeColor,
      borderColor: node.borderColor,
      textColor: node.textColor,
      progressBadgeSvg: node.progressBadgeSvg,
    },
  }));

  const edges = payload.edges.map((edge) => ({
    data: {
      id: edge.id,
      source: edge.source,
      target: edge.target,
      isLearned: edge.isLearned,
      edgeKind: edge.edgeKind,
    },
  }));

  return [...nodes, ...edges];
}

function refreshGraphState() {
  if (!cyInstance) {
    return;
  }

  cyInstance.elements().removeClass("is-selected is-context is-context-edge is-dimmed");

  if (!selectedNodeCode.value) {
    return;
  }

  const node = cyInstance.getElementById(selectedNodeCode.value);
  if (!node || node.empty()) {
    return;
  }

  const contextNodes = node
    .predecessors()
    .nodes()
    .union(node.successors().nodes())
    .union(node);
  const contextEdges = node
    .predecessors()
    .edges()
    .union(node.successors().edges())
    .union(node.connectedEdges());

  cyInstance.elements().addClass("is-dimmed");
  contextNodes.removeClass("is-dimmed").addClass("is-context");
  contextEdges.removeClass("is-dimmed").addClass("is-context-edge");
  node.removeClass("is-context");
  node.removeClass("is-dimmed");
  node.addClass("is-selected");
}

function clearZoomWheelGuard() {
  if (detachZoomWheelGuard) {
    detachZoomWheelGuard();
    detachZoomWheelGuard = null;
  }
}

function installCtrlWheelZoomGuard() {
  clearZoomWheelGuard();

  if (!graphRef.value || !cyInstance) {
    return;
  }

  const container = graphRef.value;
  const onWheel = (event) => {
    if (!cyInstance || !(event.ctrlKey || event.metaKey)) {
      return;
    }

    event.preventDefault();

    const currentZoom = cyInstance.zoom();
    const zoomFactor = event.deltaY < 0 ? 1.1 : 1 / 1.1;
    const nextZoom = Math.max(
      cyInstance.minZoom(),
      Math.min(cyInstance.maxZoom(), currentZoom * zoomFactor),
    );

    if (nextZoom === currentZoom) {
      return;
    }

    const rect = container.getBoundingClientRect();
    cyInstance.zoom({
      level: nextZoom,
      renderedPosition: {
        x: event.clientX - rect.left,
        y: event.clientY - rect.top,
      },
    });
  };

  container.addEventListener("wheel", onWheel, { passive: false });
  detachZoomWheelGuard = () => {
    container.removeEventListener("wheel", onWheel);
  };
}

function selectNode(code) {
  selectedNodeCode.value = code;
  refreshGraphState();
  navigationStore.setLearningGraphViewState({
    currentScopeCode: currentScopeCode.value,
    selectedNodeCode: code,
    lastFocusedNodeCode: code,
    lastFocusedScopeCode: currentScopeCode.value,
  });
}

function resolveRememberedScopeCodeForFocus(preferredCode) {
  const focusCode = String(preferredCode || "");
  const rememberedState = navigationStore.learningGraphViewState || {};

  if (
    focusCode &&
    focusCode === rememberedState.lastFocusedNodeCode &&
    rememberedState.lastFocusedScopeCode
  ) {
    return rememberedState.lastFocusedScopeCode;
  }

  return "root";
}

function resolveInitialScopeCode() {
  if (props.preferredSelectedCode) {
    return resolveRememberedScopeCodeForFocus(props.preferredSelectedCode);
  }

  return navigationStore.learningGraphViewState?.currentScopeCode || "root";
}

function resolveInitialPreferredSelectedCode() {
  if (props.preferredSelectedCode) {
    return String(props.preferredSelectedCode || "");
  }

  return (
    navigationStore.learningGraphViewState?.selectedNodeCode ||
    navigationStore.learningGraphViewState?.lastFocusedNodeCode ||
    ""
  );
}

function resolvePreferredNodeCode(payload, candidateCode) {
  const preferredCode = String(candidateCode || "");
  if (preferredCode && payload.nodes.some((node) => node.code === preferredCode)) {
    return preferredCode;
  }

  const rememberedState = navigationStore.learningGraphViewState || {};
  const rememberedCode = rememberedState.selectedNodeCode || rememberedState.lastFocusedNodeCode || "";
  if (rememberedCode && payload.nodes.some((node) => node.code === rememberedCode)) {
    return rememberedCode;
  }

  const recentCode = props.profile?.recentFeedbackItems?.find((item) =>
    payload.nodes.some((node) => node.code === item.code),
  )?.code;
  if (recentCode) {
    return recentCode;
  }

  const learnedCode = payload.nodes.find((node) => node.isLearned === 1)?.code;
  if (learnedCode) {
    return learnedCode;
  }

  return payload.nodes[0]?.code || "";
}

async function renderGraph(payload, preferredCode = "") {
  if (!graphRef.value) {
    return;
  }

  clearZoomWheelGuard();

  if (cyInstance) {
    cyInstance.destroy();
    cyInstance = null;
  }

  if (!payload?.nodes?.length) {
    return;
  }

  const cytoscape = (await import("cytoscape")).default;

  cyInstance = cytoscape({
    container: graphRef.value,
    elements: buildElements(payload),
    autoungrabify: true,
    userZoomingEnabled: false,
    layout: payload.edges.length > 0
      ? {
          name: "breadthfirst",
          directed: true,
          padding: 36,
          spacingFactor: 1.25,
          avoidOverlap: true,
          animate: false,
          nodeDimensionsIncludeLabels: true,
        }
      : {
          name: "grid",
          padding: 36,
          avoidOverlap: true,
          animate: false,
        },
    style: [
      {
        selector: "node",
        style: {
          shape: "round-rectangle",
          label: "data(label)",
          "background-color": "data(nodeColor)",
          color: "data(textColor)",
          "font-size": "10.5px",
          "font-weight": 600,
          "text-valign": "center",
          "text-halign": "center",
          width: 112,
          height: 82,
          "text-wrap": "wrap",
          "text-max-width": 92,
          "border-width": 3,
          "border-color": "data(borderColor)",
          "background-image": "data(progressBadgeSvg)",
          "background-image-containment": "over",
          "background-width": "38px",
          "background-height": "18px",
          "background-fit": "none",
          "background-repeat": "no-repeat",
          "background-position-x": "90%",
          "background-position-y": "90%",
          "overlay-opacity": 0,
        },
      },
      {
        selector: "node[scopeLevel = 1]",
        style: {
          width: 128,
          height: 88,
          "font-size": "11px",
          "text-max-width": 102,
        },
      },
      {
        selector: "node[scopeLevel = 3]",
        style: {
          width: 102,
          height: 76,
          "font-size": "10px",
          "text-max-width": 82,
        },
      },
      {
        selector: "node[hasDetailGraph = 1]",
        style: {
          "border-style": "dashed",
        },
      },
      {
        selector: "edge",
        style: {
          width: 2.8,
          "line-color": "#d6a6bb",
          "target-arrow-color": "#d6a6bb",
          "target-arrow-shape": "triangle",
          "curve-style": "bezier",
          opacity: 0.85,
          "overlay-opacity": 0,
        },
      },
      {
        selector: "edge[isLearned = 1]",
        style: {
          "line-color": "#c87b9d",
          "target-arrow-color": "#c87b9d",
        },
      },
      {
        selector: "node.is-context",
        style: {
          "border-width": 5,
          "border-color": "#2f5fae",
          opacity: 1,
        },
      },
      {
        selector: "edge.is-context-edge",
        style: {
          width: 4,
          opacity: 1,
        },
      },
      {
        selector: "node.is-selected",
        style: {
          "border-width": 5,
          "border-color": "#a64d72",
          opacity: 1,
          "shadow-blur": 16,
          "shadow-color": "#d995b0",
          "shadow-opacity": 0.48,
          "shadow-offset-x": 0,
          "shadow-offset-y": 0,
          "z-index": 999,
        },
      },
      {
        selector: "node.is-dimmed",
        style: {
          opacity: 0.52,
          "text-opacity": 0.92,
        },
      },
      {
        selector: "edge.is-dimmed",
        style: {
          opacity: 0.18,
        },
      },
    ],
  });

  cyInstance.on("tap", "node", (event) => {
    selectNode(event.target.id());
  });

  cyInstance.on("dbltap", "node", (event) => {
    const detailScopeCode = event.target.data("detailScopeCode");
    if (!detailScopeCode) {
      return;
    }

    navigateToScope(detailScopeCode);
  });

  const defaultSelectedCode = resolvePreferredNodeCode(payload, preferredCode);
  if (defaultSelectedCode) {
    selectNode(defaultSelectedCode);
  }

  installCtrlWheelZoomGuard();
}

async function loadGraph(scopeCode = "root", options = {}) {
  loading.value = true;
  loadError.value = "";

  try {
    const params = {};
    if (scopeCode !== "root") params.scopeCode = scopeCode;
    if (activeCourseCode.value) params.courseCode = activeCourseCode.value;
    const payload =
      Object.keys(params).length > 0 ? await fetchKnowledgeGraph(params) : await fetchKnowledgeGraph();

    const normalizedPayload = normalizePayload(payload, masteryByCode.value);
    graphData.value = payload;
    selectedNodeCode.value = resolvePreferredNodeCode(
      normalizedPayload,
      options.preferredSelectedCode,
    );
    preferredSelectedCode.value = String(options.preferredSelectedCode || "");
    navigationStore.setLearningGraphViewState({
      currentScopeCode: payload?.view?.scopeCode || scopeCode || "root",
      selectedNodeCode: selectedNodeCode.value,
      lastFocusedNodeCode:
        selectedNodeCode.value ||
        navigationStore.learningGraphViewState?.lastFocusedNodeCode ||
        "",
      lastFocusedScopeCode: selectedNodeCode.value
        ? payload?.view?.scopeCode || scopeCode || "root"
        : navigationStore.learningGraphViewState?.lastFocusedScopeCode || "root",
    });
    loading.value = false;
  } catch (error) {
    loadError.value =
      "未能读取当前层级图谱。请先启动后端并确认数据库已初始化。";
    loading.value = false;
    console.error(error);
  }
}

function navigateToScope(scopeCode) {
  const rememberedState = navigationStore.learningGraphViewState || {};
  const rememberedNodeCode =
    scopeCode === rememberedState.lastFocusedScopeCode
      ? rememberedState.lastFocusedNodeCode || rememberedState.selectedNodeCode || ""
      : "";

  loadGraph(scopeCode || "root", {
    preferredSelectedCode: rememberedNodeCode,
  });
}

onMounted(async () => {
  await loadGraph(resolveInitialScopeCode(), {
    preferredSelectedCode: resolveInitialPreferredSelectedCode(),
  });
});

watch(
  [currentScopeGraph, loading, loadError],
  async ([payload, isLoading, error]) => {
    if (isLoading || error) {
      return;
    }

    await nextTick();
    await renderGraph(payload, preferredSelectedCode.value);
    preferredSelectedCode.value = "";
  },
  {
    deep: true,
  },
);

watch(
  () => props.preferredSelectedCode,
  async (code) => {
    if (!code || code === selectedNodeCode.value) {
      return;
    }

    await loadGraph(resolveRememberedScopeCodeForFocus(code), {
      preferredSelectedCode: code,
    });
  },
);

watch(
  activeCourseCode,
  async (courseCode, previousCourseCode) => {
    if (!courseCode || courseCode === previousCourseCode) {
      return;
    }

    await loadGraph(currentScopeCode.value || "root", {
      preferredSelectedCode:
        selectedNodeCode.value || props.preferredSelectedCode || "",
    });
  },
);

onBeforeUnmount(() => {
  clearZoomWheelGuard();
  if (cyInstance) {
    cyInstance.destroy();
    cyInstance = null;
  }
});
</script>

<style scoped>
.card {
  background: var(--panel-surface);
  border: var(--panel-border);
  border-radius: 24px;
  padding: 22px;
  box-shadow: var(--panel-shadow);
}

.card-head {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  gap: 16px;
  margin-bottom: 18px;
}

.label {
  margin: 0 0 6px;
  font-size: 0.82rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

.label--detail {
  color: #6a7283;
}

h2,
h3,
h4 {
  margin: 0;
  color: #183042;
}

h2 {
  font-size: 1.28rem;
}

h3 {
  font-size: 1.1rem;
}

h4 {
  font-size: 0.98rem;
}

.caption {
  margin: 0;
  max-width: 460px;
  color: #516170;
  line-height: 1.65;
}

.state,
.empty-tip {
  border-radius: 18px;
  padding: 18px;
  color: #43515f;
}

.state {
  background: #eef5ff;
}

.state--error {
  background: #fff2f2;
  color: #9b3333;
}

.scope-toolbar {
  display: grid;
  gap: 14px;
  margin-bottom: 16px;
}

.breadcrumb-row {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.breadcrumb-button {
  border: 1px solid #d5dce5;
  background: #f4f7fb;
  color: #4f6071;
  border-radius: 999px;
  padding: 8px 14px;
  font-size: 0.88rem;
  font-weight: 600;
  cursor: pointer;
}

.breadcrumb-button--active {
  background: #e7f0ff;
  border-color: #bfd4fb;
  color: #2459b4;
}

.scope-note {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 10px;
  color: #526172;
}

.scope-note strong {
  color: #183042;
}

.scope-label {
  margin: 0;
  color: #6a7682;
}

.summary-grid {
  margin: 0 0 14px;
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
}

.summary-grid div,
.detail-grid div {
  padding: 14px 16px;
  border-radius: 18px;
  background: #f5f8fb;
  border: 1px solid rgba(103, 124, 144, 0.16);
}

.summary-grid dt,
.detail-grid dt {
  margin: 0 0 6px;
  font-size: 0.85rem;
  color: #667381;
}

.summary-grid dd,
.detail-grid dd {
  margin: 0;
  color: #173042;
  font-weight: 700;
}

.rule-note {
  margin: 0 0 16px;
  color: #4d5d6c;
  line-height: 1.7;
}

.legend-row {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-bottom: 18px;
}

.legend-chip {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 8px 14px;
  border-radius: 999px;
  font-size: 0.88rem;
  font-weight: 600;
  border: 1px solid transparent;
}

.legend-chip::before {
  content: "";
  width: 10px;
  height: 10px;
  border-radius: 999px;
  background: currentColor;
}

.legend-chip--progress-low {
  color: #b56c8f;
  background: #fdeef4;
  border-color: #f2cfdd;
}

.legend-chip--progress-mid {
  color: #a45379;
  background: #f5d7e4;
  border-color: #e8bace;
}

.legend-chip--progress-high {
  color: #8d4567;
  background: #ebb6cc;
  border-color: #dc93b4;
}

.legend-chip--selected {
  color: #a64d72;
  background: #fff4f8;
  border-color: #e8bdd0;
}

.legend-chip--context {
  color: #375d95;
  background: #eef4ff;
  border-color: #d5e2fb;
}

.graph-layout {
  margin-bottom: 18px;
}

.graph-canvas {
  width: 100%;
  min-height: 760px;
  border-radius: 24px;
  background:
    radial-gradient(circle at top, rgba(131, 153, 181, 0.1), transparent 38%),
    linear-gradient(180deg, rgba(247, 249, 252, 0.96), rgba(237, 242, 248, 0.9));
  border: 1px solid rgba(99, 119, 141, 0.16);
}

.detail-panel {
  display: grid;
  gap: 16px;
  padding: 18px;
  border-radius: 24px;
  background: #fbfcfe;
  border: 1px solid rgba(100, 120, 140, 0.14);
}

.detail-head,
.relation-head,
.relation-main {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 12px;
}

.mastery-badge,
.relation-state {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  border-radius: 999px;
  padding: 6px 12px;
  font-size: 0.82rem;
  font-weight: 700;
}

.mastery-badge--mastered,
.relation-state--mastered {
  color: #ffffff;
  background: #3f7fe8;
}

.mastery-badge--progress,
.relation-state--familiar {
  color: #1a6b3a;
  background: #d4edda;
}

.relation-state--learning {
  color: #856404;
  background: #fff3cd;
}

.relation-state--beginner {
  color: #0c5460;
  background: #d1ecf1;
}

.mastery-badge--idle,
.relation-state--not-started {
  color: #586778;
  background: #e5eaef;
}

.detail-grid {
  margin: 0;
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
}

.detail-description {
  margin: 0;
  line-height: 1.75;
  color: #4d5d6c;
}

.detail-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.detail-action-button {
  justify-self: start;
  border: 0;
  border-radius: 999px;
  background: #d96c96;
  color: #ffffff;
  padding: 10px 16px;
  font-weight: 700;
  cursor: pointer;
}

.detail-action-button--primary {
  background: linear-gradient(135deg, #c7668f, #d995b0);
}

.relation-section {
  display: grid;
  gap: 10px;
}

.relation-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px;
}

.relation-head span {
  color: #607080;
  font-size: 0.88rem;
}

.relation-list {
  list-style: none;
  margin: 0;
  padding: 0;
  display: grid;
  gap: 10px;
}

.relation-item {
  padding: 12px 14px;
  border-radius: 16px;
  background: #f4f7fb;
  border: 1px solid rgba(103, 124, 144, 0.14);
}

.relation-item strong {
  color: #183042;
}

.relation-item p {
  margin: 6px 0 0;
  color: #5b6a79;
}

.empty-tip--panel,
.empty-tip--standalone {
  background: #f5f8fb;
  border: 1px solid rgba(100, 120, 140, 0.14);
}

@media (max-width: 1200px) {
  .summary-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }

  .detail-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }

  .graph-canvas {
    min-height: 680px;
  }

  .relation-grid {
    grid-template-columns: 1fr;
  }
}

@media (max-width: 760px) {
  .card-head {
    flex-direction: column;
  }

  .summary-grid,
  .detail-grid {
    grid-template-columns: 1fr;
  }

  .graph-canvas {
    min-height: 540px;
  }
}
</style>
