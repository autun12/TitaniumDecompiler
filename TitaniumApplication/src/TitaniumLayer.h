#pragma once

#include <TitaniumRenderer.h>
#include <TitaniumLogger/Logger/Log.h>
#include <TitaniumDecompiler.h>
#include "Panels/DecompilerViewerPanel.h"
#include "Panels/DisassemblyViewerPanel.h"
#include "Panels/HexViewerPanel.h"
#include "Panels/SectionsPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/FunctionListingPanel.h"
#include "Panels/GraphViewPanel.h"
// #include "Panels/Decompiler.h"

#include "imgui.h"
#include "imfilebrowser.h"
#include <vector>
#include <string>

namespace TitaniumRenderer {
struct Viewport {
    float x;
    float y;
};

class TitaniumLayer : public Layer {
public:
    TitaniumLayer();
    virtual ~TitaniumLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Event& e) override;

private:
    bool OnKeyPressed(KeyPressedEvent& e);
    void AnalyzeFile(const std::filesystem::path& path);
    void OpenFile();

private:
    std::filesystem::path m_FilePath;
    std::vector<std::string> m_funcNames;

    // Panels
    DisassemblyViewerPanel m_DisassemblyViewerPanel;
    HexViewerPanel m_HexViewerPanel;
    SectionsPanel m_SectionsPanel;
    ConsolePanel m_ConsolePanel;
    FunctionListingPanel m_FunctionListPanel;
    DecompilerViewerPanel m_DecompilerViewerPanel;
    GraphViewPanel m_GraphViewPanel;
    
    // Disassembler
    TitaniumDecompiler::JVMDisassembler m_JVMDisassembler;
    TitaniumDecompiler::Disassembler m_Disassembler;

    // Decompiler
    TitaniumDecompiler::Decompiler m_Decompiler;

    // ImGui file browser
    ImGui::FileBrowser m_FileBrowser;

};
}
