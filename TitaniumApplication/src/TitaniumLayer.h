#pragma once

#include <TitaniumRenderer.h>
#include <TitaniumDecompiler.h>
#include "Panels/DisassemblyViewerPanel.h"
#include "Panels/HexViewerPanel.h"
#include "Panels/SectionsPanel.h"
#include "Panels/ConsolePanel.h"

#include <imgui/imgui.h>
#include <imgui/imfilebrowser.h>

namespace TitaniumRenderer {
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
    void OpenAnalyzeFile();

private:
    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

    // Panels
    DisassemblyViewerPanel m_DisassemblyViewerPanel;
    HexViewerPanel m_HexViewerPanel;
    SectionsPanel m_SectionsPanel;
    ConsolePanel m_ConsolePanel;

    TitaniumDecompiler::ELF m_elfFile;
    // TitaniumDecompiler::PE m_peFile;

    ImGui::FileBrowser m_FileBrowser;
};
}