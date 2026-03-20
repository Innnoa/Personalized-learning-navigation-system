export const demoGraphElements = [
  {
    data: {
      id: "intro",
      label: "绪论",
      group: "chapter",
    },
  },
  {
    data: {
      id: "linear-list",
      label: "线性表",
      group: "chapter",
    },
  },
  {
    data: {
      id: "stack-queue",
      label: "栈和队列",
      group: "chapter",
    },
  },
  {
    data: {
      id: "tree-graph",
      label: "树和图",
      group: "chapter",
    },
  },
  {
    data: {
      id: "edge-1",
      source: "intro",
      target: "linear-list",
    },
  },
  {
    data: {
      id: "edge-2",
      source: "linear-list",
      target: "stack-queue",
    },
  },
  {
    data: {
      id: "edge-3",
      source: "stack-queue",
      target: "tree-graph",
    },
  },
];
