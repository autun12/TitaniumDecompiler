#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <stdint.h>
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <vector>
#include <map>
#include <string>
#include <TitaniumDecompiler/Disassembler/CFG.h>
#include <TitaniumDecompiler/Disassembler/BasicBlock.h>

namespace TitaniumRenderer {

struct Node {
    ImVec2 m_Pos;
    ImVec2 m_Size;
    std::string m_Label;
    std::shared_ptr<TitaniumDecompiler::BasicBlock> block;
};

struct Edge {
    std::shared_ptr<Node> from;
    std::shared_ptr<Node> to;
};

class GraphViewPanel {
public:
    GraphViewPanel() = default;
    void OnImGuiRender(bool* open);
    void CreateGraphDataFromCFG(TitaniumDecompiler::CFG& cfg);
private:
    std::vector<std::shared_ptr<Node>> m_Nodes;
    std::vector<std::shared_ptr<Edge>> m_Edges;
    ImVec2 m_GraphOffset = ImVec2(0.0f, 0.0f);
    float m_GraphZoom = 1.0f;
    std::map<std::shared_ptr<TitaniumDecompiler::BasicBlock>, std::shared_ptr<Node>> m_BlockToNode;
};
}