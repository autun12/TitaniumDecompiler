#include "GraphUtils.h"

namespace TitaniumDecompiler {
void dfs(std::shared_ptr<BasicBlock> u, std::map<uint32_t, std::shared_ptr<BasicBlock>>& graph, std::map<uint32_t, int>& visited, int& time,
         std::map<uint32_t, int>& dfn, std::map<uint32_t, int>& semi, std::map<uint32_t, int>& label, std::map<uint32_t, int>& ancestor) {
    visited[u->m_ID] = 1;
    dfn[u->m_ID] = time++;
    label[u->m_ID] = u->m_ID;
    semi[u->m_ID] = u->m_ID;
    ancestor[u->m_ID] = 0;

    for (auto& v : u->GetSuccessor()) {
        if (!visited[v->m_ID]) {
            dfs(v, graph, visited, time, dfn, semi, label, ancestor);
        }
    }
}

int find(uint32_t u, std::map<uint32_t, int>& ancestor, std::map<uint32_t, int>& label, std::map<uint32_t, int>& semi) {
    if (ancestor[u] == 0) {
        return u;
    }

    int v = find(ancestor[u], ancestor, label, semi);

    if (semi[label[u]] > semi[label[v]]) {
        label[u] = v;
    }

    ancestor[u] = ancestor[v];
    return label[u];
}

void link(uint32_t u, uint32_t v, std::map<uint32_t, int>& ancestor) { ancestor[v] = u; }

std::map<uint32_t, uint32_t> lengauerTarjan(std::map<uint32_t, std::shared_ptr<BasicBlock>>& graph, uint32_t startNodeId) {
    int n = graph.size();
    std::map<uint32_t, int> visited;
    std::vector<uint32_t> vertex(n + 1);  // 1-based indexing
    int time = 1;

    std::map<uint32_t, int> dfn;
    std::map<uint32_t, int> semi;
    std::map<uint32_t, int> label;
    std::map<uint32_t, int> ancestor;
    std::map<uint32_t, std::vector<uint32_t>> bucket;
    std::map<uint32_t, uint32_t> idom;

    // Initialize visited map
    for (const auto& [nodeId, node] : graph) {
        visited[nodeId] = 0;
        dfn[nodeId] = 0;  // Important:  Initialize dfn, semi, label, ancestor.
        semi[nodeId] = 0;
        label[nodeId] = 0;
        ancestor[nodeId] = 0;
    }

    // Step 1: Perform Depth-First Search
    dfs(graph[startNodeId], graph, visited, time, dfn, semi, label, ancestor);

    // Populate the vertex array.  Only include nodes that were visited in the DFS.
    int vertexIndex = 1;
    for (const auto& [nodeId, node] : graph) {
        if (dfn[nodeId] != 0) {  // If the node was visited
            vertex[dfn[nodeId]] = nodeId;
            vertexIndex++;
        }
    }

    // Step 2 & 3: Compute semi-dominators and intermediate dominators
    for (int i = n; i >= 2; --i) {
        uint32_t wId = vertex[i];
        auto w = graph[wId];
        uint32_t pId = w->GetPreds()[0]->m_ID;  // Get any parent.  graph[w].parents should not be empty.

        for (auto& v : w->GetPreds()) {
            uint32_t vId = v->m_ID;
            if (dfn[vId] < dfn[wId]) {
                if (dfn[wId] == 0) continue;
                semi[wId] = std::min(semi[wId], dfn[vId]);
            } else {
                find(vId, ancestor, label, semi);
                semi[wId] = std::min(semi[wId], semi[label[vId]]);
            }
        }
        semi[wId] = vertex[semi[wId]];
        bucket[semi[wId]].push_back(wId);
        link(pId, wId, ancestor);

        for (uint32_t vId : bucket[pId]) {
            int x = find(vId, ancestor, label, semi);
            if (semi[x] == pId) {
                idom[vId] = pId;
            } else {
                idom[vId] = vertex[x];
            }
        }
        bucket[pId].clear();
    }

    // Step 4: Compute the immediate dominators
    for (int i = 2; i <= n; ++i) {
        uint32_t wId = vertex[i];
        if (idom[wId] != semi[wId]) {
            idom[wId] = idom[idom[wId]];
        }
    }
    idom[startNodeId] = startNodeId;

    return idom;
}

}  // namespace TitaniumDecompiler