#pragma once

#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "ASTNodes.h"
#include "Platform/Linux/FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/Disassembler/BasicBlock.h"
#include "TitaniumDecompiler/Disassembler/CFG.h"

namespace TitaniumDecompiler {
struct DomHelper {
public:
    explicit DomHelper(CFG& cfg) : m_CFG(cfg) {}
    // DomHelper() = default;
    ~DomHelper() = default;

    void Compute();
    void Print() const;
    std::map<size_t, size_t> GetImmediateDominators() const { return idom; }
    std::map<size_t, std::set<size_t>> GetDoms() const { return dom; }
    // std::shared_ptr<RootStatement> createStatement(const std::shared_ptr<CFG>& cfg, const Function& func);
    std::shared_ptr<ASTNode> BuildASTFromDomTree(std::shared_ptr<BasicBlock> block, std::map<size_t, std::set<size_t>>& domTree,
                                                 std::unordered_map<int, std::shared_ptr<BasicBlock>>& basicBlocks);
    void PrintAST(const std::shared_ptr<ASTNode>& node, int depth = 0);
    std::string GenerateStructuredCode(std::shared_ptr<ASTNode> node, int indentLevel = 0);
    std::vector<std::pair<size_t, size_t>> FindBackEdges();
    std::set<size_t> FindLoop(size_t header, size_t backEdgeSource);

private:
    CFG& m_CFG;
    std::map<size_t, std::set<size_t>> dom;
    std::map<size_t, size_t> idom;

    void ComputeDominatorTree();
    void ComputeImmediateDominator();
};

class DominatorTree {
public:
    DominatorTree(const std::map<uint32_t, uint32_t>& dominators, const std::map<uint32_t, std::shared_ptr<BasicBlock>>& cfgBlocks, uint32_t startNodeId)
        : m_Dominators(dominators), m_CFGBlocks(cfgBlocks), m_StartNodeId(startNodeId) {
        buildDominatorTree();
    }

    // Function to get the immediate dominator of a given node.
    uint32_t getImmediateDominator(uint32_t nodeId) const {
        auto it = m_Dominators.find(nodeId);
        if (it != m_Dominators.end()) {
            return it->second;
        }
        return nodeId;  // Or throw an exception, depending on your error handling policy
    }

    // Function to check if node A dominates node B
    bool dominates(uint32_t a, uint32_t b) const {
        if (a == b) return true;  // A node dominates itself

        uint32_t current = b;
        while (current != m_StartNodeId) {
            current = getImmediateDominator(current);
            if (current == a) return true;
        }
        return false;
    }

    // Function to get the dominator children of a given node.
    std::vector<uint32_t> getDominatorChildren(uint32_t nodeId) const {
        std::vector<uint32_t> children;
        for (const auto& [childId, dominatorId] : m_Dominators) {
            if (dominatorId == nodeId) {
                children.push_back(childId);
            }
        }
        return children;
    }

    // Prints the dominator tree in a simple format.
    void printDominatorTree() const {
        std::cout << "Dominator Tree:\n";
        for (const auto& [nodeId, dominatorId] : m_Dominators) {
            std::cout << "Node " << nodeId << " is dominated by: " << dominatorId << "\n";
        }
    }

    // Function to get the root of the dominator tree
    uint32_t getRootNodeId() const { return m_StartNodeId; }

private:
    std::map<uint32_t, uint32_t> m_Dominators;                    // Maps node ID to its immediate dominator's ID.
    std::map<uint32_t, std::shared_ptr<BasicBlock>> m_CFGBlocks;  // Copy of the CFG's blocks.
    uint32_t m_StartNodeId;
    std::map<uint32_t, std::vector<uint32_t>> m_DominatorTree;

    void buildDominatorTree() {
        for (const auto& [nodeId, dominatorId] : m_Dominators) {
            m_DominatorTree[dominatorId].push_back(nodeId);
        }
    }
};

std::vector<std::set<uint32_t>> FindLoops(CFG& cfg, const DominatorTree& domTree);
}  // namespace TitaniumDecompiler
