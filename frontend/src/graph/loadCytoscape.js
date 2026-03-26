export async function loadCytoscape() {
  return (await import("cytoscape")).default;
}
