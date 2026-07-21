#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <stdint.h>

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <TitaniumDecompiler/Disassembler/BasicBlock.h>
#include <TitaniumDecompiler/Disassembler/CFG.h>

#include <map>
#include <string>
#include <vector>

#include "../../Core/ProjectContext.h"
#include "IPanel.h"
#include "imgui_internal.h"

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

class GraphViewPanel : public IPanel {
public:
    GraphViewPanel(bool defaultOpen = false)
        : IPanel("Flow Graph", defaultOpen) {}
    virtual void OnImGuiRender() override;
    void CreateGraphDataFromCFG(TitaniumDecompiler::CFG& cfg);
    virtual void OnProjectLoaded(const ProjectContext& context) override;

private:
    std::vector<std::shared_ptr<Node>> m_Nodes;
    std::vector<std::shared_ptr<Edge>> m_Edges;
    ImVec2 m_GraphOffset = ImVec2(0.0f, 0.0f);
    float m_GraphZoom = 1.0f;
    std::map<std::shared_ptr<TitaniumDecompiler::BasicBlock>,
             std::shared_ptr<Node>>
        m_BlockToNode;
};
}  // namespace TitaniumRenderer
