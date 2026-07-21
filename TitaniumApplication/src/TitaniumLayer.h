#pragma once

#include <TitaniumDecompiler.h>
#include <TitaniumLogger/Logger/Log.h>
#include <TitaniumRenderer.h>

#include <memory>
#include <string>
#include <vector>

#include "Core/ProjectContext.h"
#include "TitaniumRenderer/UI/WidgetManager.h"
#include "UI/Components/SettingsWidget.h"
#include "UI/Panels/ConsolePanel.h"
#include "UI/Panels/DecompilerViewerPanel.h"
#include "UI/Panels/DisassemblyViewerPanel.h"
#include "UI/Panels/FunctionListingPanel.h"
#include "UI/Panels/GraphViewPanel.h"
#include "UI/Panels/HexViewerPanel.h"
#include "UI/Panels/PanelManager.h"
#include "imfilebrowser.h"
#include "imgui.h"

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
    void HandleFileBrowserResolution();

private:
    std::filesystem::path m_FilePath;
    std::unique_ptr<PanelManager> m_PanelManager;
    std::unique_ptr<WidgetManager> m_WidgetManager;
    std::unique_ptr<ProjectContext> m_ProjectContext;

    // Panels
    DisassemblyViewerPanel* m_DisassemblyViewerPanel;
    HexViewerPanel* m_HexViewerPanel;
    ConsolePanel* m_ConsolePanel;
    FunctionListingPanel* m_FunctionListPanel;
    DecompilerViewerPanel* m_DecompilerViewerPanel;
    GraphViewPanel* m_GraphViewPanel;

    // Widgets
    SettingsWidget* m_SettingsWidget;
    // ImGui file browser
    ImGui::FileBrowser m_FileBrowser;
};
}  // namespace TitaniumRenderer
