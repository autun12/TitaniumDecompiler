#include "GraphViewPanel.h"

#include <stdint.h>

#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

namespace TitaniumRenderer {
void GraphViewPanel::OnImGuiRender() {
    if (!m_IsOpen) return;

    if (ImGui::Begin(GetName().c_str(), GetOpenPtr())) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        // ImVec2 canvas_size = ImGui::GetContentRegionAvail();

        ImVec2 origin = canvas_pos + m_GraphOffset;

        // Draw edges
        for (const auto& edge : m_Edges) {
            ImVec2 from_pos =
                (ImVec2(edge->from->m_Pos.x + edge->from->m_Size.x / 2,
                        edge->from->m_Pos.y + edge->from->m_Size.y / 2) *
                 m_GraphZoom) +
                origin;
            ImVec2 to_pos =
                (ImVec2(edge->to->m_Pos.x + edge->to->m_Size.x / 2,
                        edge->to->m_Pos.y + edge->to->m_Size.y / 2) *
                 m_GraphZoom) +
                origin;
            draw_list->AddLine(from_pos, to_pos, IM_COL32(255, 255, 0, 255),
                               2.0f);  // Default edge color
        }

        // Draw nodes
        for (const auto& node : m_Nodes) {
            ImVec2 node_pos = (node->m_Pos * m_GraphZoom) + origin;
            ImVec2 node_size = (node->m_Size * m_GraphZoom);
            draw_list->AddRectFilled(node_pos, node_pos + node_size,
                                     IM_COL32(128, 128, 128, 255));
            draw_list->AddRect(node_pos, node_pos + node_size,
                               IM_COL32(0, 0, 0, 255));
            ImVec2 text_pos = node_pos + ImVec2(5.0f, 5.0f);
            draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255),
                               node->m_Label.c_str());

            // Interaction (Selection example - using block address as ID)
            ImGui::SetCursorScreenPos(node_pos);
            ImGui::InvisibleButton(
                std::to_string(reinterpret_cast<uintptr_t>(node->block.get()))
                    .c_str(),
                node_size);
            if (ImGui::IsItemHovered()) {
                draw_list->AddRect(node_pos - ImVec2(2, 2),
                                   node_pos + node_size + ImVec2(2, 2),
                                   IM_COL32(255, 255, 255, 255));
                if (ImGui::IsMouseClicked(0)) {
                    // Handle node selection
                }
            }
        }

        // Panning
        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
            m_GraphOffset = m_GraphOffset + ImGui::GetIO().MouseDelta;
        }

        // Zooming
        if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0.0f) {
            float zoom_delta = ImGui::GetIO().MouseWheel * 0.1f;
            m_GraphZoom += zoom_delta;
            if (m_GraphZoom < 0.1f) m_GraphZoom = 0.1f;
        }

        ImGui::End();
    }
}

void GraphViewPanel::CreateGraphDataFromCFG(TitaniumDecompiler::CFG& cfg) {
    m_Nodes.clear();
    m_Edges.clear();
    m_BlockToNode.clear();

    const auto& basicBlocksMap = cfg.GetMapOfBlocks();
    std::vector<std::shared_ptr<TitaniumDecompiler::BasicBlock>> basicBlocks;
    for (const auto& pair : basicBlocksMap) {
        basicBlocks.push_back(pair.second);
    }

    if (basicBlocks.empty()) return;

    std::map<std::shared_ptr<TitaniumDecompiler::BasicBlock>, int> nodeDepths;
    std::map<std::shared_ptr<TitaniumDecompiler::BasicBlock>, float>
        nodeXPositions;
    std::map<int, std::vector<std::shared_ptr<TitaniumDecompiler::BasicBlock>>>
        blocksByDepth;

    // 1. Determine Node Depths (Simple BFS from the first block)
    std::vector<std::shared_ptr<TitaniumDecompiler::BasicBlock>> queue;
    std::set<std::shared_ptr<TitaniumDecompiler::BasicBlock>> visited;
    std::shared_ptr<TitaniumDecompiler::BasicBlock> startBlock =
        cfg.GetFirst();  // Assuming you have a way to get the starting block

    if (startBlock) {
        queue.push_back(startBlock);
        nodeDepths[startBlock] = 0;
        visited.insert(startBlock);
        blocksByDepth[0].push_back(startBlock);

        while (!queue.empty()) {
            std::shared_ptr<TitaniumDecompiler::BasicBlock> currentBlock =
                queue.front();
            queue.erase(queue.begin());
            int depth = nodeDepths[currentBlock];

            for (const auto& successor : currentBlock->GetSuccessor()) {
                if (visited.find(successor) == visited.end()) {
                    visited.insert(successor);
                    nodeDepths[successor] = depth + 1;
                    blocksByDepth[depth + 1].push_back(successor);
                    queue.push_back(successor);
                }
            }
            for (const auto& successorEx :
                 currentBlock->GetSuccessorExceptions()) {
                if (visited.find(successorEx) == visited.end()) {
                    visited.insert(successorEx);
                    nodeDepths[successorEx] = depth + 1;
                    blocksByDepth[depth + 1].push_back(successorEx);
                    queue.push_back(successorEx);
                }
            }
        }
    } else {
        // If no starting block, just assign depth 0 to all (will likely
        // overlap)
        for (const auto& blockPtr : basicBlocks) {
            nodeDepths[blockPtr] = 0;
            blocksByDepth[0].push_back(blockPtr);
        }
    }

    // 2. Assign X Positions within each depth level (Simple Averaging)
    float y_offset = 50.0f;
    float depth_spacing = 100.0f;
    for (const auto& depthPair : blocksByDepth) {
        int depth = depthPair.first;
        const auto& blocksAtDepth = depthPair.second;
        float total_width = 0.0f;
        std::vector<ImVec2> nodeSizesAtDepth;

        // Create nodes and calculate total width for centering
        for (const auto& blockPtr : blocksAtDepth) {
            std::shared_ptr<Node> node = std::make_shared<Node>();
            node->block = blockPtr;
            std::stringstream ss;
            ss << "BB_" << blockPtr->m_ID << "\n";
            for (const auto& insn : blockPtr->GetInstructions()) {
                ss << insn.opcodeName << "\n";
            }
            node->m_Label = ss.str();
            node->m_Size = ImVec2(
                150.0f, std::max(40.0f, static_cast<float>(
                                            blockPtr->GetInstructions().size() *
                                            15.0f)));
            nodeSizesAtDepth.push_back(node->m_Size);
            total_width += node->m_Size.x + 50.0f;  // Add some spacing
            m_Nodes.push_back(node);
            m_BlockToNode[blockPtr] = node;
        }

        float current_x = -total_width / 2.0f;  // Center the level
        for (size_t i = 0; i < blocksAtDepth.size(); ++i) {
            nodeXPositions[blocksAtDepth[i]] =
                current_x + nodeSizesAtDepth[i].x / 2.0f;
            current_x += nodeSizesAtDepth[i].x + 50.0f;
        }

        for (const auto& blockPtr : blocksAtDepth) {
            if (m_BlockToNode.count(blockPtr)) {
                m_BlockToNode[blockPtr]->m_Pos = ImVec2(
                    nodeXPositions[blockPtr] * 1.5f,
                    y_offset +
                        depth * depth_spacing);  // Adjust multipliers as needed
            }
        }
        y_offset += depth_spacing;
    }

    // 3. Create Edges
    for (const auto& blockPtr : basicBlocks) {
        if (m_BlockToNode.count(blockPtr)) {
            std::shared_ptr<Node> fromNode = m_BlockToNode[blockPtr];
            for (const auto& successorPtr : blockPtr->GetSuccessor()) {
                if (m_BlockToNode.count(successorPtr)) {
                    std::shared_ptr<Edge> edge = std::make_shared<Edge>();
                    edge->from = fromNode;
                    edge->to = m_BlockToNode[successorPtr];
                    m_Edges.push_back(edge);
                }
            }
            for (const auto& successorExceptionPtr :
                 blockPtr->GetSuccessorExceptions()) {
                if (m_BlockToNode.count(successorExceptionPtr)) {
                    std::shared_ptr<Edge> edge = std::make_shared<Edge>();
                    edge->from = fromNode;
                    edge->to = m_BlockToNode[successorExceptionPtr];
                    m_Edges.push_back(edge);
                }
            }
        }
    }
}

void GraphViewPanel::OnProjectLoaded(const ProjectContext& context) {
    // for (auto& classPair : context.GetClassesToFunctions()) {
    //     for (auto& function : classPair.second) {
    //         if (function.GetFunctionCFG()) {
    //             CreateGraphDataFromCFG(*function.GetFunctionCFG());
    //         }
    //     }
    // }
}
}  // namespace TitaniumRenderer
