#include "DomHelper.h"

#include <algorithm>
#include <iostream>
#include <stack>

namespace TitaniumDecompiler {
// DomHelper::DomHelper() {}
// Function to compute the dominator set for each basic block
void DomHelper::Compute() {
    ComputeDominatorTree();
    ComputeImmediateDominator();

    auto backEdges = FindBackEdges();
    for (const auto& [src, target] : backEdges) {
        std::cout << "Loop detected: back edge from Block " << src << " to Block " << target << std::endl;
        FindLoop(target, src);
    }
}

void DomHelper::ComputeDominatorTree() {
    std::vector<std::shared_ptr<BasicBlock>> blocks = m_CFG.GetBlocks();
    if (blocks.empty()) return;

    size_t entryID = blocks[0]->m_ID;

    for (const auto& block : blocks) {
        for (const auto& other : blocks) {
            dom[block->m_ID].insert(other->m_ID);
        }
    }

    dom[entryID] = {entryID};

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& block : blocks) {
            if (block->m_ID == entryID) continue;

            std::set<size_t> newDom;
            bool firstPred = true;

            for (const auto& pred : block->GetPreds()) {
                if (firstPred) {
                    newDom = dom[pred->m_ID];
                    firstPred = false;
                } else {
                    std::set<size_t> temp;
                    std::set_intersection(newDom.begin(), newDom.end(), dom[pred->m_ID].begin(), dom[pred->m_ID].end(), std::inserter(temp, temp.begin()));

                    // for (size_t id : dom[pred->m_ID]) {
                    //     if (newDom.count(id)) {
                    //         temp.insert(id);
                    //     }
                    // }

                    newDom = temp;
                }
            }

            newDom.insert(block->m_ID);
            if (newDom != dom[block->m_ID]) {
                dom[block->m_ID] = newDom;
                changed = true;
            }
        }
    }
}

void DomHelper::ComputeImmediateDominator() {
    std::cout << "Dominator sets:\n";
    for (const auto& [b, domSet] : dom) {
        std::cout << "Block " << b << " dominates: ";
        for (size_t id : domSet) {
            std::cout << id << " ";
        }
        std::cout << "\n";
    }

    // Now, calculate the immediate dominators
    for (const auto& [b, domSet] : dom) {
        if (b == *domSet.begin()) continue;

        // Initialize the candidates to all dominators
        std::set<size_t> candidates = domSet;
        candidates.erase(b);  // Remove the block itself

        std::cout << "Finding immediate dominator for Block " << b << "...\n";

        // Simplified check for immediate dominator: find the closest dominator
        size_t immediateDominator = -1;
        for (size_t d : candidates) {
            bool isClosest = true;
            // Check if any other dominator strictly dominates 'd' and is dominated by 'b'
            for (size_t other : candidates) {
                if (other != d && dom.at(other).count(d)) {
                    isClosest = false;
                    break;
                }
            }

            if (isClosest) {
                immediateDominator = d;
                break;
            }
        }

        if (immediateDominator != static_cast<size_t>(-1)) {
            idom[b] = immediateDominator;
            std::cout << "Block " << b << " is immediately dominated by Block " << immediateDominator << "\n";
        } else {
            std::cout << "No immediate dominator found for Block " << b << "\n";
        }
    }
}

std::set<size_t> DomHelper::FindLoop(size_t header, size_t backEdgeSource) {
    std::set<size_t> loopBlocks;
    std::stack<size_t> stack;

    stack.push(backEdgeSource);
    loopBlocks.insert(header);
    loopBlocks.insert(backEdgeSource);

    std::cout << "Finding loop: Header = " << header << ", BackEdgeSource = " << backEdgeSource << std::endl;

    while (!stack.empty()) {
        size_t blockID = stack.top();
        stack.pop();

        std::cout << "  Processing block: " << blockID << std::endl;

        for (const auto& pred : m_CFG.m_Blocks[blockID]->GetPreds()) {
            size_t predID = pred->m_ID;

            if (loopBlocks.find(predID) == loopBlocks.end()) {
                loopBlocks.insert(predID);
                stack.push(predID);
                std::cout << "    Adding block to loop: " << predID << std::endl;
            }
        }
    }

    std::cout << "Loop blocks for header " << header << ": ";
    for (size_t block : loopBlocks) {
        std::cout << block << " ";
    }
    std::cout << std::endl;

    return loopBlocks;
}

std::vector<std::pair<size_t, size_t>> DomHelper::FindBackEdges() {
    std::vector<std::pair<size_t, size_t>> backEdges;

    for (const auto& [blockAddr, block] : m_CFG.GetMapOfBlocks()) {
        for (const auto& succ : block->GetSuccessor()) {
            size_t succIndex = succ->m_ID;

            if (dom[succIndex].find(blockAddr) != dom[succIndex].end()) {
                backEdges.push_back({blockAddr, succIndex});
            }
        }
    }

    return backEdges;
}

std::shared_ptr<ASTNode> DomHelper::BuildASTFromDomTree(std::shared_ptr<BasicBlock> block, std::map<size_t, std::set<size_t>>& domTree,
                                                        std::unordered_map<int, std::shared_ptr<BasicBlock>>& basicBlocks) {
    std::unordered_map<int, std::shared_ptr<ASTNode>> astNodes;
    auto backEdges = FindBackEdges();
    std::map<size_t, std::set<size_t>> loops;
    for (const auto& [src, target] : backEdges) {
        loops[target] = FindLoop(target, src);
    }

    // Step 1: Create an AST node for each BasicBlock
    for (const auto& [blockID, block] : basicBlocks) {
        if (!block) continue;

        std::cout << "Processing Block: " << blockID << "\n";
        std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>(ASTNodeType::SEQUENCE, "Block " + std::to_string(blockID));

        for (const Insn& instruction : block->GetInstructions()) {
            std::string opcode = instruction.opcodeName;
            std::shared_ptr<ASTNode> instNode;

            switch (instruction.group) {
                case GROUP_GENERAL:
                    instNode = std::make_shared<ASTNode>(opcode.find("store") != std::string::npos ? ASTNodeType::ASSIGNMENT : ASTNodeType::EXPRESSION, opcode);
                    break;
                case GROUP_JUMP:
                    instNode = std::make_shared<ASTNode>(opcode.find("if_") != std::string::npos ? ASTNodeType::CONDITIONAL : ASTNodeType::LOOP, opcode);
                    break;
                case GROUP_SWITCH:
                    instNode = std::make_shared<ASTNode>(ASTNodeType::SWITCH_CASE, opcode);
                    break;
                case GROUP_INVOCATION:
                    instNode = std::make_shared<ASTNode>(ASTNodeType::FUNCTION_CALL, opcode);
                    break;
                case GROUP_FIELDACCESS:
                    instNode = std::make_shared<ASTNode>(ASTNodeType::VARIABLE_ACCESS, opcode);
                    break;
                case GROUP_RETURN:
                    instNode = std::make_shared<ASTNode>(ASTNodeType::RETURN, opcode);
                    break;
                default:
                    instNode = std::make_shared<ASTNode>(ASTNodeType::EXPRESSION, opcode);
                    break;
            }

            node->addChild(instNode);
        }

        astNodes[blockID] = node;
    }

    // Step 2: Connect AST nodes based on the dominator tree
    std::shared_ptr<ASTNode> root = nullptr;

    for (const auto& [blockID, block] : basicBlocks) {
        if (!block || domTree.find(blockID) == domTree.end()) continue;

        for (int childID : domTree.at(blockID)) {
            if (childID != blockID && astNodes.count(childID)) {  // Avoid self-loop
                astNodes[blockID]->addChild(astNodes[childID]);
            }
        }

        if (!root) root = astNodes[blockID];  // Set the first node as root
    }

    return root;
}

std::string DomHelper::GenerateStructuredCode(std::shared_ptr<ASTNode> node, int indentLevel) {
    if (!node) return "";

    std::string indent(indentLevel * 4, ' ');  // 4 spaces per indent level
    std::string code;

    switch (node->m_Type) {
        case ASTNodeType::ASSIGNMENT:
            code += indent + node->m_Value + ";\n";
            break;

        case ASTNodeType::EXPRESSION:
            code += indent + node->m_Value + ";\n";
            break;

        case ASTNodeType::FUNCTION_CALL:
            code += indent + node->m_Value + "();\n";
            break;

        case ASTNodeType::RETURN:
            code += indent + "return " + node->m_Value + ";\n";
            break;

        case ASTNodeType::CONDITIONAL: {
            code += indent + "if (" + node->m_Value + ") {\n";
            for (const auto& child : node->m_Children) {
                code += GenerateStructuredCode(child, indentLevel + 1);
            }
            code += indent + "}\n";
            break;
        }

        case ASTNodeType::LOOP: {
            code += indent + "while (" + node->m_Value + ") {\n";
            for (const auto& child : node->m_Children) {
                code += GenerateStructuredCode(child, indentLevel + 1);
            }
            code += indent + "}\n";
            break;
        }

        case ASTNodeType::SWITCH_CASE: {
            code += indent + "switch (" + node->m_Value + ") {\n";
            for (const auto& child : node->m_Children) {
                code += GenerateStructuredCode(child, indentLevel + 1);
            }
            code += indent + "}\n";
            break;
        }

        case ASTNodeType::VARIABLE_ACCESS:
            code += indent + "// Access variable: " + node->m_Value + "\n";
            break;

        case ASTNodeType::SEQUENCE:
            for (const auto& child : node->m_Children) {
                code += GenerateStructuredCode(child, indentLevel);
            }
            break;

        default:
            code += indent + "// Unknown Node Type: " + node->m_Value + "\n";
            break;
    }

    return code;
}

void DomHelper::Print() const {
    std::cout << "Dominator Tree:\n";
    for (const auto& [b, d] : idom) {
        std::cout << "Block " << b << " is immediately dominated by Block " << d << "\n";
    }
}

void DomHelper::PrintAST(const std::shared_ptr<ASTNode>& node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; ++i) std::cout << "  ";  // Indentation
    std::cout << node->m_Value << " (" << static_cast<int>(node->m_Type) << ")\n";

    for (const auto& child : node->m_Children) {
        PrintAST(child, depth + 1);
    }
}

std::vector<std::set<uint32_t>> FindLoops(CFG& cfg, const DominatorTree& domTree) {
    std::vector<std::set<uint32_t>> loops;  // Vector of sets, where each set is a loop

    // 1. Iterate through all edges in the CFG
    for (const auto& [nodeId, block] : cfg.GetMapOfBlocks()) {  // Get blocks from CFG
        for (const auto& successor : block->GetSuccessor()) {
            uint32_t sourceId = nodeId;
            uint32_t targetId = successor->m_ID;

            // 2. Check for back edge: target dominates source
            if (domTree.dominates(targetId, sourceId)) {
                // We found a back edge!  targetId is the loop header.
                std::set<uint32_t> loopNodes;
                loopNodes.insert(targetId);  // Add header
                loopNodes.insert(sourceId);  // Add source of back edge

                // 3. Find loop members (conceptual DFS)
                std::vector<uint32_t> toVisit;
                toVisit.push_back(sourceId);
                std::set<uint32_t> visited;

                while (!toVisit.empty()) {
                    uint32_t currentId = toVisit.back();
                    toVisit.pop_back();
                    visited.insert(currentId);

                    // Get predecessors of currentId
                    auto currentBlock = cfg.GetMapOfBlocks().at(currentId);
                    for (auto& predecessor : currentBlock->GetPreds()) {
                        uint32_t predId = predecessor->m_ID;
                        if (predId != targetId && visited.find(predId) == visited.end()) {
                            loopNodes.insert(predId);
                            toVisit.push_back(predId);
                        }
                    }
                }
                loops.push_back(loopNodes);
            }
        }
    }
    return loops;
}

}  // namespace TitaniumDecompiler
