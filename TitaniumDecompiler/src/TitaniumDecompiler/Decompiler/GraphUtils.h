#pragma once

#include <stdint.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

#include "TitaniumDecompiler/Disassembler/BasicBlock.h"

namespace TitaniumDecompiler {

void dfs(std::shared_ptr<TitaniumDecompiler::BasicBlock> u, std::map<uint32_t, std::shared_ptr<TitaniumDecompiler::BasicBlock>>& graph,
         std::map<uint32_t, int>& visited, int& time, std::map<uint32_t, int>& dfn, std::map<uint32_t, int>& semi, std::map<uint32_t, int>& label,
         std::map<uint32_t, int>& ancestor);

// Function to find the ancestor with the minimum semi-dominator, adapted
int find(uint32_t u, std::map<uint32_t, int>& ancestor, std::map<uint32_t, int>& label, std::map<uint32_t, int>& semi);

// Function to link nodes in the dominator tree, adapted
void link(uint32_t u, uint32_t v, std::map<uint32_t, int>& ancestor);

// Function to implement the Lengauer-Tarjan algorithm, adapted for your CFG
std::map<uint32_t, uint32_t> lengauerTarjan(std::map<uint32_t, std::shared_ptr<TitaniumDecompiler::BasicBlock>>& graph, uint32_t startNodeId);
}  // namespace TitaniumDecompiler