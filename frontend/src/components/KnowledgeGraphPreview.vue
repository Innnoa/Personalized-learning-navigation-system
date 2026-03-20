<template>
  <section class="card">
    <div class="card-head">
      <div>
        <p class="label">图谱预览</p>
        <h2>{{ title }}</h2>
      </div>
      <p class="caption">{{ caption }}</p>
    </div>

    <div v-if="isLoading" class="state state--loading">
      正在从 SQLite 加载数据结构课程知识图谱...
    </div>

    <div v-else-if="loadError" class="state state--error">
      {{ loadError }}
    </div>

    <template v-else>
      <dl class="meta-grid">
        <div>
          <dt>知识点</dt>
          <dd>{{ graphData.meta.knowledgePointCount }}</dd>
        </div>
        <div>
          <dt>依赖边</dt>
          <dd>{{ graphData.meta.dependencyCount }}</dd>
        </div>
        <div>
          <dt>章节数</dt>
          <dd>{{ graphData.meta.chapterCount }}</dd>
        </div>
        <div>
          <dt>核心节点</dt>
          <dd>{{ graphData.meta.coreKnowledgePointCount }}</dd>
        </div>
      </dl>

      <div class="scope-bar">
        <div class="breadcrumb-list">
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
        <p class="scope-note">{{ scopeNote }}</p>
      </div>

      <div class="graph-layout">
        <div ref="graphRef" class="graph-canvas"></div>

        <aside class="detail-panel">
          <template v-if="selectedNode">
            <div class="detail-head">
              <div>
                <p class="label label--detail">节点详情</p>
                <h3>{{ selectedNode.label }}</h3>
              </div>
              <span class="node-badge">{{ selectedNode.chapterName }}</span>
            </div>

            <dl class="detail-grid">
              <div>
                <dt>章节</dt>
                <dd>第{{ selectedNode.chapterNo }}章</dd>
              </div>
              <div>
                <dt>类型</dt>
                <dd>{{ resolveNodeTypeLabel(selectedNode.nodeType) }}</dd>
              </div>
              <div>
                <dt>难度</dt>
                <dd>{{ selectedNode.difficultyLevel }}/5</dd>
              </div>
              <div>
                <dt>预计学习</dt>
                <dd>{{ selectedNode.estimatedMinutes }} 分钟</dd>
              </div>
            </dl>

            <p class="detail-description">
              {{ selectedNode.description || "当前节点暂未补充说明。" }}
            </p>

            <section
              v-if="selectedNode.hasDetailGraph"
              class="relation-section relation-section--drilldown"
            >
              <div class="relation-head">
                <h4>细化图谱</h4>
                <span>可继续下钻</span>
              </div>
              <p class="detail-description detail-description--compact">
                当前节点已经整理出更细粒度图谱。可直接双击左侧节点进入，也可点击下方按钮跳转。
              </p>
              <button
                type="button"
                class="drilldown-button"
                @click="navigateToScope(selectedNode.detailScopeCode)"
              >
                进入该节点细化图谱
              </button>
            </section>

            <section class="relation-section">
              <div class="relation-head">
                <h4>直接前置依赖</h4>
                <span>{{ directPrerequisites.length }} 个</span>
              </div>
              <ul v-if="directPrerequisites.length > 0" class="relation-list">
                <li
                  v-for="item in directPrerequisites"
                  :key="`${selectedNode.code}-pre-${item.code}`"
                  class="relation-item"
                >
                  <strong>{{ item.label }}</strong>
                  <p>第{{ item.chapterNo }}章 · {{ resolveNodeTypeLabel(item.nodeType) }}</p>
                </li>
              </ul>
              <p v-else class="empty-tip">
                当前节点没有直接前置依赖，可作为一条子链路的起点。
              </p>
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
                  <strong>{{ item.label }}</strong>
                  <p>第{{ item.chapterNo }}章 · {{ resolveNodeTypeLabel(item.nodeType) }}</p>
                </li>
              </ul>
              <p v-else class="empty-tip">
                当前节点已经位于一条学习链路的末端或暂无后继节点。
              </p>
            </section>
          </template>

          <div v-else class="empty-tip empty-tip--panel">
            点击左侧知识点节点后，可在这里查看它的章节位置、难度、说明和直接依赖关系。
          </div>
        </aside>
      </div>
    </template>
  </section>
</template>

<script setup>
import { computed, nextTick, onBeforeUnmount, onMounted, ref } from "vue";

import { fetchKnowledgeGraph } from "../api/knowledgeGraph";

const graphRef = ref(null);
const isLoading = ref(true);
const loadError = ref("");
const graphData = ref(null);
const selectedNodeCode = ref("");
const currentScopeCode = ref("root");
let cyInstance = null;

const title = computed(() => {
  if (graphData.value?.view?.scopeCode && graphData.value.view.scopeCode !== "root") {
    return `${graphData.value.view.scopeName}细化图谱`;
  }

  if (graphData.value?.course?.name) {
    return `${graphData.value.course.name}课程主线`;
  }

  return "数据结构课程主线";
});

const caption = computed(() => {
  if (isLoading.value) {
    return "真实课程数据加载中";
  }

  if (loadError.value) {
    return "接口加载失败";
  }

  if (graphData.value?.view?.sourceReference) {
    return graphData.value.view.sourceReference;
  }

  return `${graphData.value.meta.knowledgePointCount} 个知识点 / ${graphData.value.meta.dependencyCount} 条依赖边`;
});

const breadcrumbs = computed(() => graphData.value?.breadcrumbs || []);

const scopeNote = computed(() => {
  if (!graphData.value?.view) {
    return "";
  }

  if (graphData.value.view.scopeCode === "root") {
    return "双击带“可细化”标记的节点，可进入下一层知识图谱。";
  }

  return (
    graphData.value.view.description ||
    "当前正在查看细化图谱，可通过上方面包屑返回任意上层。"
  );
});

const selectedNode = computed(() => {
  if (!graphData.value?.nodes?.length || !selectedNodeCode.value) {
    return null;
  }

  return (
    graphData.value.nodes.find((node) => node.code === selectedNodeCode.value) ||
    null
  );
});

const directPrerequisites = computed(() => {
  if (!selectedNode.value || !graphData.value?.edges?.length) {
    return [];
  }

  const prerequisiteCodes = graphData.value.edges
    .filter((edge) => edge.target === selectedNode.value.code)
    .map((edge) => edge.source);

  return graphData.value.nodes.filter((node) => prerequisiteCodes.includes(node.code));
});

const directDependents = computed(() => {
  if (!selectedNode.value || !graphData.value?.edges?.length) {
    return [];
  }

  const dependentCodes = graphData.value.edges
    .filter((edge) => edge.source === selectedNode.value.code)
    .map((edge) => edge.target);

  return graphData.value.nodes.filter((node) => dependentCodes.includes(node.code));
});

const nodeTypeLabelMap = {
  foundation: "基础概念",
  structure: "核心结构",
  implementation: "实现方法",
  algorithm: "算法方法",
  application: "应用场景",
  abstraction: "抽象建模",
};

function buildElements(payload) {
  const nodes = payload.nodes.map((node) => ({
    data: {
      id: node.id,
      label: node.label,
      chapterLabel: `第${node.chapterNo}章`,
      nodeType: node.nodeType,
      difficultyLevel: node.difficultyLevel,
      hasDetailGraph: node.hasDetailGraph ? 1 : 0,
      detailScopeCode: node.detailScopeCode || "",
    },
  }));

  const edges = payload.edges.map((edge) => ({
    data: {
      id: edge.id,
      source: edge.source,
      target: edge.target,
      relationType: edge.relationType,
    },
  }));

  return [...nodes, ...edges];
}

function highlightNodeContext(code) {
  if (!cyInstance) {
    return;
  }

  cyInstance.elements().removeClass("is-selected is-context is-context-edge is-muted");

  const targetNode = cyInstance.getElementById(code);
  if (!targetNode || targetNode.empty()) {
    return;
  }

  const contextElements = targetNode
    .union(targetNode.connectedEdges())
    .union(targetNode.predecessors())
    .union(targetNode.successors());

  cyInstance.elements().addClass("is-muted");
  contextElements.removeClass("is-muted");
  contextElements.nodes().addClass("is-context");
  contextElements.edges().addClass("is-context-edge");
  targetNode.removeClass("is-muted");
  targetNode.removeClass("is-context");
  targetNode.addClass("is-selected");
}

async function renderGraph(payload) {
  if (!graphRef.value) {
    return;
  }

  if (cyInstance) {
    cyInstance.destroy();
  }

  const elements = buildElements(payload);
  const cytoscape = (await import("cytoscape")).default;

  cyInstance = cytoscape({
    container: graphRef.value,
    elements,
    layout: {
      name: "breadthfirst",
      directed: true,
      padding: 28,
      spacingFactor: 1.45,
      avoidOverlap: true,
      nodeDimensionsIncludeLabels: true,
    },
    style: [
      {
        selector: "node",
        style: {
          label: "data(label)",
          "background-color": "#0c6a71",
          color: "#ffffff",
          "font-size": "12px",
          "text-valign": "center",
          "text-halign": "center",
          width: 92,
          height: 92,
          "text-wrap": "wrap",
          "text-max-width": 78,
          "border-width": 3,
          "border-color": "#d9f1ef",
          "overlay-opacity": 0,
        },
      },
      {
        selector: "node[nodeType = 'foundation']",
        style: {
          "background-color": "#205072",
        },
      },
      {
        selector: "node[nodeType = 'structure']",
        style: {
          "background-color": "#0c6a71",
        },
      },
      {
        selector: "node[nodeType = 'implementation']",
        style: {
          "background-color": "#3b7d3a",
        },
      },
      {
        selector: "node[nodeType = 'algorithm']",
        style: {
          "background-color": "#d97706",
        },
      },
      {
        selector: "node[nodeType = 'application']",
        style: {
          "background-color": "#b45309",
        },
      },
      {
        selector: "node[nodeType = 'abstraction']",
        style: {
          "background-color": "#6b5ca5",
        },
      },
      {
        selector: "node[hasDetailGraph = 1]",
        style: {
          "border-style": "dashed",
          "border-width": 4,
          "border-color": "#f7b267",
        },
      },
      {
        selector: "edge",
        style: {
          width: 3,
          "line-color": "#ff9c48",
          "target-arrow-color": "#ff9c48",
          "target-arrow-shape": "triangle",
          "curve-style": "round-taxi",
          "taxi-direction": "downward",
          "taxi-turn": "38%",
          "taxi-turn-min-distance": 26,
          "edge-distances": "node-position",
          "overlay-opacity": 0,
          "arrow-scale": 1.05,
        },
      },
      {
        selector: "node.is-selected",
        style: {
          "border-width": 5,
          "border-color": "#ff9c48",
          "border-opacity": 1,
          opacity: 1,
          "text-opacity": 1,
          "z-index": 999,
        },
      },
      {
        selector: "node.is-context",
        style: {
          opacity: 0.96,
          "text-opacity": 1,
          "border-width": 4,
          "border-color": "#8fd6cc",
          "border-opacity": 0.95,
        },
      },
      {
        selector: "edge.is-context-edge",
        style: {
          width: 4,
          opacity: 0.92,
          "line-color": "#f08c2e",
          "target-arrow-color": "#f08c2e",
          "z-index": 998,
        },
      },
      {
        selector: "node.is-muted",
        style: {
          opacity: 0.22,
          "text-opacity": 0.35,
          "border-opacity": 0.18,
        },
      },
      {
        selector: "edge.is-muted",
        style: {
          opacity: 0.12,
        },
      },
    ],
  });

  cyInstance.on("tap", "node", (event) => {
    const selectedCode = event.target.id();
    selectNode(selectedCode);
  });

  cyInstance.on("dbltap", "node", (event) => {
    const targetNode = event.target;
    const detailScopeCode = targetNode.data("detailScopeCode");
    if (!detailScopeCode) {
      return;
    }

    navigateToScope(detailScopeCode);
  });

  if (payload.nodes.length > 0) {
    selectNode(payload.nodes[0].code);
  }
}

function selectNode(code) {
  selectedNodeCode.value = code;

  if (!cyInstance) {
    return;
  }

  highlightNodeContext(code);
}

function resolveNodeTypeLabel(nodeType) {
  return nodeTypeLabelMap[nodeType] || nodeType;
}

async function loadGraph(scopeCode = "root") {
  isLoading.value = true;
  loadError.value = "";

  try {
    const payload = await fetchKnowledgeGraph(
      scopeCode && scopeCode !== "root" ? { scopeCode } : {},
    );
    graphData.value = payload;
    currentScopeCode.value = payload?.view?.scopeCode || "root";
    selectedNodeCode.value = "";
    isLoading.value = false;
    await nextTick();
    await renderGraph(payload);
  } catch (error) {
    isLoading.value = false;
    loadError.value =
      "未能加载真实知识图谱，请先执行数据库初始化脚本并确认后端服务已启动。";
    console.error(error);
  }
}

function navigateToScope(scopeCode) {
  loadGraph(scopeCode || "root");
}

onMounted(async () => {
  await loadGraph();
});

onBeforeUnmount(() => {
  if (cyInstance) {
    cyInstance.destroy();
    cyInstance = null;
  }
});
</script>

<style scoped>
.card {
  background: rgba(255, 255, 255, 0.8);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
}

.card-head {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: baseline;
}

.label {
  margin: 0 0 6px;
  font-size: 0.84rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

h2 {
  margin: 0;
  font-size: 1.3rem;
}

.caption {
  margin: 0;
  color: #66727d;
  font-size: 0.9rem;
}

.meta-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
  margin-top: 18px;
}

.scope-bar {
  display: grid;
  gap: 12px;
  margin-top: 18px;
  padding: 16px 18px;
  border-radius: 18px;
  background: rgba(244, 248, 247, 0.86);
}

.breadcrumb-list {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.breadcrumb-button,
.drilldown-button {
  border: none;
  border-radius: 999px;
  cursor: pointer;
  transition: transform 0.15s ease, opacity 0.15s ease;
}

.breadcrumb-button {
  padding: 8px 14px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-weight: 700;
}

.breadcrumb-button--active {
  background: #0c6a71;
  color: #ffffff;
}

.scope-note {
  margin: 0;
  color: #44515c;
  line-height: 1.6;
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

.state {
  margin-top: 18px;
  border-radius: 18px;
  padding: 18px;
  font-weight: 600;
}

.state--loading {
  background: rgba(12, 106, 113, 0.1);
  color: #0c5960;
}

.state--error {
  background: rgba(176, 47, 47, 0.1);
  color: #8d2323;
}

.graph-canvas {
  width: 100%;
  min-height: clamp(420px, 44vw, 620px);
  border-radius: 18px;
  background:
    linear-gradient(180deg, rgba(12, 106, 113, 0.08), rgba(12, 106, 113, 0.02)),
    rgba(244, 248, 247, 0.7);
}

.graph-layout {
  display: grid;
  grid-template-columns: minmax(0, 1.4fr) minmax(280px, 0.9fr);
  gap: 16px;
  margin-top: 18px;
}

.detail-panel {
  border-radius: 18px;
  background: rgba(247, 250, 249, 0.86);
  padding: 18px;
}

.detail-head,
.relation-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  align-items: start;
}

.label--detail {
  margin-bottom: 4px;
}

h3,
h4 {
  margin: 0;
}

.node-badge {
  display: inline-flex;
  align-items: center;
  padding: 6px 12px;
  border-radius: 999px;
  background: rgba(12, 106, 113, 0.09);
  color: #0c5960;
  font-size: 0.88rem;
  font-weight: 700;
}

.detail-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 10px;
  margin-top: 16px;
}

.detail-grid div {
  padding: 12px 14px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.92);
}

.detail-description {
  margin: 16px 0 0;
  color: #44515c;
  line-height: 1.65;
}

.detail-description--compact {
  margin-top: 12px;
}

.relation-section {
  margin-top: 18px;
}

.relation-section--drilldown {
  padding: 14px 16px;
  border-radius: 18px;
  background: rgba(255, 255, 255, 0.92);
}

.relation-head span {
  color: #66727d;
  font-size: 0.88rem;
}

.relation-list {
  display: grid;
  gap: 10px;
  margin: 14px 0 0;
  padding: 0;
  list-style: none;
}

.relation-item {
  padding: 12px 14px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.92);
}

.relation-item p,
.empty-tip {
  margin: 6px 0 0;
  color: #44515c;
}

.empty-tip--panel {
  margin-top: 0;
  line-height: 1.6;
}

.drilldown-button {
  margin-top: 14px;
  padding: 10px 16px;
  background: linear-gradient(135deg, #0c6a71, #1b828b);
  color: #ffffff;
  font-weight: 700;
}

.breadcrumb-button:hover,
.drilldown-button:hover {
  transform: translateY(-1px);
}

@media (max-width: 640px) {
  .card-head {
    flex-direction: column;
    align-items: start;
  }

  .meta-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}

@media (max-width: 980px) {
  .graph-layout,
  .detail-grid {
    grid-template-columns: 1fr;
  }
}
</style>
