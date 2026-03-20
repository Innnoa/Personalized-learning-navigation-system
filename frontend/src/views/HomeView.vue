<template>
  <PageLayout>
    <template #hero-side>
      <div class="hero-side">
        <HealthStatusCard :health="health" :error="healthError" />
      </div>
    </template>

    <section class="grid">
      <KnowledgeGraphPreview />
      <PathPlannerPanel
        :learner-code="learnerProfile?.learner?.code || 'demo-learner'"
        :initial-mastery-by-code="learnerProfile?.masteryByCode || {}"
        :feedback-record-count="learnerProfile?.summary?.feedbackRecordCount || 0"
        :profile-loading="learnerProfileLoading"
        @feedback-saved="handleLearnerProfileUpdated"
      />

      <article class="card">
        <p class="label">当前实现进度</p>
        <h2>第一版反馈闭环与学习记录已接入</h2>
        <ul class="list">
          <li>课程范围已固定为“数据结构”</li>
          <li>后端已接入真实课程图谱、学习者画像与反馈记录</li>
          <li>学习者画像已拆分为独立页面，可通过顶部导航切换查看</li>
          <li>知识图谱已支持多层面包屑导航与双击下钻</li>
          <li>当前已完成十四个细化节点：“数据结构基本概念”“算法复杂度分析”“线性表”“顺序表”“链表”“栈”“队列”“串”“KMP 模式匹配”“树与二叉树基础”“二叉树遍历”“哈夫曼树与编码”“图的存储与遍历”“拓扑排序与关键路径”</li>
          <li>第三层图谱当前已支持 36 个关键子节点：“逻辑结构”“存储结构”“时间复杂度”“复杂度案例解析”“顺序表示（线性表）”“链式表示（线性表）”“插入操作”“删除操作”“合并操作（顺序表）”“头插法建表”“尾插法建表”“有序表合并（链表）”“递归工作栈”“典型应用（栈）”“循环队列”“入队操作（队列）”“队列典型应用”“模式匹配问题”“简单模式匹配”“next 数组含义”“KMP 主过程”“二叉树存储结构”“递归遍历思想”“层序遍历”“非递归遍历思想”“构造过程”“前缀编码约束”“哈夫曼编码与译码”“邻接矩阵存储”“邻接表存储”“深度优先遍历”“广度优先遍历”“AOE 网”“拓扑排序算法”“关键路径概念”“关键路径求解”</li>
          <li>前端路径规划默认使用学习者画像中的掌握度</li>
          <li>反馈提交后会同步更新画像，并重新生成学习路径</li>
          <li>学习反馈现已支持按提交批次逐步回退，便于纠正误点或误判</li>
          <li>学习者画像页面已支持展示最近学习记录与逐步回退后的画像恢复结果</li>
          <li>路径规划面板已支持展示调整前后路径对比</li>
        </ul>
      </article>

      <article class="card">
        <p class="label">下一步建议</p>
        <h2>可以继续补强论文配套材料与答辩演示脚本</h2>
        <ol class="list ordered">
          <li>继续按单元节点逐个补充细化图谱</li>
          <li>整理数据库 ER 图与系统流程图</li>
          <li>整理接口说明和答辩演示步骤</li>
          <li>整理实验结果截图与核心功能演示话术</li>
        </ol>
      </article>
    </section>
  </PageLayout>
</template>

<script setup>
import { onMounted, ref } from "vue";

import HealthStatusCard from "../components/HealthStatusCard.vue";
import KnowledgeGraphPreview from "../components/KnowledgeGraphPreview.vue";
import PageLayout from "../components/PageLayout.vue";
import PathPlannerPanel from "../components/PathPlannerPanel.vue";
import { fetchHealth } from "../api/health";
import { fetchLearnerProfile } from "../api/learnerProfile";

const health = ref(null);
const healthError = ref("");
const learnerProfile = ref(null);
const learnerProfileLoading = ref(true);
const learnerProfileError = ref("");

function handleLearnerProfileUpdated(payload) {
  learnerProfile.value = payload;
  learnerProfileError.value = "";
}

onMounted(async () => {
  const [healthResult, learnerResult] = await Promise.allSettled([
    fetchHealth(),
    fetchLearnerProfile(),
  ]);

  if (healthResult.status === "fulfilled") {
    health.value = healthResult.value;
  } else {
    healthError.value =
      "未能连接后端，请先启动 Drogon 服务，或检查 WSL 中的代理环境变量。";
    console.error(healthResult.reason);
  }

  if (learnerResult.status === "fulfilled") {
    learnerProfile.value = learnerResult.value;
  } else {
    learnerProfileError.value =
      "未能读取学习者画像，将回退到前端默认掌握度预设。";
    console.error(learnerResult.reason);
  }

  learnerProfileLoading.value = false;
});
</script>

<style scoped>
.hero-side {
  display: grid;
  gap: 18px;
}

.grid {
  display: grid;
  grid-template-columns: minmax(0, 1.5fr) minmax(320px, 1fr);
  gap: 20px;
  align-items: start;
}

.card {
  background: rgba(255, 255, 255, 0.8);
  border: 1px solid rgba(22, 32, 42, 0.08);
  border-radius: 24px;
  padding: 22px;
  backdrop-filter: blur(10px);
  box-shadow: 0 18px 50px rgba(22, 32, 42, 0.08);
}

.label {
  margin: 0 0 6px;
  font-size: 0.84rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #6d7781;
}

h2 {
  margin: 0 0 16px;
  font-size: 1.3rem;
}

.list {
  margin: 0;
  padding-left: 18px;
  color: #32404a;
  display: grid;
  gap: 10px;
}

.ordered {
  padding-left: 22px;
}

@media (max-width: 980px) {
  .grid {
    grid-template-columns: 1fr;
  }
}
</style>
