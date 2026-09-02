#include "TitaniumLayer.h"

#include <TitaniumRenderer/Utils/PlatformUtils.h>
#include <stdio.h>
#include <stdlib.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

#include "UI/Panels/ConsoleSink.h"

namespace TitaniumRenderer {

TitaniumLayer::TitaniumLayer() : Layer("TitaniumLayer") {
    Application::Get().GetImGuiLayer()->BlockEvents(false);
}

void TitaniumLayer::OnAttach() {
    // Initialize Project Context
    m_ProjectContext = std::make_unique<TitaniumRenderer::ProjectContext>();

    // Panel Management
    m_PanelManager = std::make_unique<PanelManager>();
    auto hexViewer = m_PanelManager->AddPanel<HexViewerPanel>(false);
    m_HexViewerPanel = hexViewer.get();
    auto consoleViewer = m_PanelManager->AddPanel<ConsolePanel>(true);
    m_ConsolePanel = consoleViewer.get();
    auto funcListViewer = m_PanelManager->AddPanel<FunctionListingPanel>(true);
    m_FunctionListPanel = funcListViewer.get();
    auto graphViewer = m_PanelManager->AddPanel<GraphViewPanel>(false);
    m_GraphViewPanel = graphViewer.get();
    auto disasmViewer = m_PanelManager->AddPanel<DisassemblyViewerPanel>(true);
    m_DisassemblyViewerPanel = disasmViewer.get();
    auto decompViewer = m_PanelManager->AddPanel<DecompilerViewerPanel>(false);
    m_DecompilerViewerPanel = decompViewer.get();

    // console output creation
    auto consoleSink = std::make_shared<ConsoleSink_mt>(m_ConsolePanel);
    consoleSink->set_pattern("%v");
    TitaniumLogger::Log::GetRendererLogger()->sinks().push_back(consoleSink);
    TitaniumLogger::Log::GetClientLogger()->sinks().push_back(consoleSink);
    TitaniumLogger::Log::GetDecompilerLogger()->sinks().push_back(consoleSink);

    m_WidgetManager = std::make_unique<WidgetManager>();
    m_WidgetManager->RegisterWidget<SettingsWidget>();
}

void TitaniumLayer::OnDetach() {}

void TitaniumLayer::OnUpdate(Timestep ts) {
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();
}

void TitaniumLayer::OnImGuiRender() {
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin() to
    // not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    // Important: note that we proceed even if Begin() returns false (aka window
    // is collapsed). This is because we want to keep our DockSpace() active. If
    // a DockSpace() is inactive, all active windows docked into it will lose
    // their parent and become undocked. We cannot preserve the docking
    // relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in
    // limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("TitaniumWorkspace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;
    m_FileBrowser.SetTitle("Browse Files");

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                m_FileBrowser.Open();
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
            }
            if (ImGui::MenuItem("Exit")) {
                Application::Get().Close();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Panels")) {
            for (auto& panel : m_PanelManager->GetPanels()) {
                ImGui::MenuItem(panel->GetName().c_str(), nullptr,
                                panel->GetOpenPtr());
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::MenuItem("Preferences...", "Ctrl+,")) {
                auto* settings = m_WidgetManager->GetWidget<SettingsWidget>();
                if (settings) {
                    settings->Toggle();
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    m_PanelManager->OnImGuiRender();
    m_FileBrowser.Display();
    m_WidgetManager->OnImGuiRender();
    ImGui::End();
    HandleFileBrowserResolution();
}

void TitaniumLayer::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(
        TD_BIND_EVENT_FN(TitaniumLayer::OnKeyPressed));
}

bool TitaniumLayer::OnKeyPressed(KeyPressedEvent& e) {
    // Shortcuts
    if (e.GetRepeatCount() > 0) {
        return false;
    }

    bool control = Input::IsKeyPressed(Key::LeftControl) ||
                   Input::IsKeyPressed(Key::RightControl);
    bool shift = Input::IsKeyPressed(Key::LeftShift) ||
                 Input::IsKeyPressed(Key::RightShift);
    switch (e.GetKeyCode()) {
        case Key::N: {
            if (control) TD_WARN("New File hasn't been Implemented Yet");
            break;
        }
        case Key::O: {
            if (control) {
                m_FileBrowser.Open();
                HandleFileBrowserResolution();
                break;
            }
        }
        case Key::S: {
            if (control && shift)
                TD_WARN("Save File hasn't been Implemented Yet");
            // SaveProjectAs();
            break;
        }
    }
    return false;
}

void TitaniumLayer::AnalyzeFile(const std::filesystem::path& path) {}

void TitaniumLayer::OpenFile() {}

void TitaniumLayer::HandleFileBrowserResolution() {
    m_FileBrowser.SetTypeFilters({".class", ".jar"});
    if (!m_FileBrowser.HasSelected()) return;

    m_FilePath = m_FileBrowser.GetSelected().c_str();

    if (!m_ProjectContext->LoadTargetFile(m_FilePath)) {
        TD_WARN("Failed to load target file into project context: {0}",
                m_FilePath.string());
        m_FileBrowser.ClearSelected();
        return;
    }

    // Display the binary in a hex viewer.
    if (m_HexViewerPanel) {
        m_HexViewerPanel->ReadFile(m_FilePath);
    }

    m_FileBrowser.ClearSelected();
    m_PanelManager->BroadcastProjectLoad(*m_ProjectContext);
}
}  // namespace TitaniumRenderer
