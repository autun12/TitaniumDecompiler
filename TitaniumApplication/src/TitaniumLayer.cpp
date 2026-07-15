#include "TitaniumLayer.h"

#include <TitaniumRenderer/Utils/PlatformUtils.h>
#include <stdio.h>
#include <stdlib.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

#include "Panels/ConsoleSink.h"

namespace TitaniumRenderer {

TitaniumLayer::TitaniumLayer() : Layer("TitaniumLayer") { Application::Get().GetImGuiLayer()->BlockEvents(false); }

void TitaniumLayer::OnAttach() {
    auto consoleSink = std::make_shared<ConsoleSink_mt>(&m_ConsolePanel);
    consoleSink->set_pattern("%v");
    TitaniumLogger::Log::GetRendererLogger()->sinks().push_back(consoleSink);
    TitaniumLogger::Log::GetClientLogger()->sinks().push_back(consoleSink);
    TitaniumLogger::Log::GetDecompilerLogger()->sinks().push_back(consoleSink);
}

void TitaniumLayer::OnDetach() {}

void TitaniumLayer::OnUpdate(Timestep ts) {
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();
}

void TitaniumLayer::OnImGuiRender() {
    // Panel open / close variables
    static bool hexViewerOpen = false;
    static bool sectionPanelOpen = false;
    static bool disassemblyViewOpen = true;
    static bool consolePanelOpen = true;
    static bool functionListPanelOpen = true;
    static bool decompilerPanelOpen = true;
    static bool graphViewPanelOpen = false;

    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render
    // a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
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
            ImGui::MenuItem("Console View Panel", nullptr, &consolePanelOpen);
            ImGui::MenuItem("Disassembly View Panel", nullptr, &disassemblyViewOpen);
            ImGui::MenuItem("Decompiler View Panel", nullptr, &decompilerPanelOpen);
            ImGui::MenuItem("Hex View Panel", nullptr, &hexViewerOpen);
            ImGui::MenuItem("Sections View Panel", nullptr, &sectionPanelOpen);
            ImGui::MenuItem("Function List View Panel", nullptr, &functionListPanelOpen);
            ImGui::MenuItem("Flow Graph Panel", nullptr, &graphViewPanelOpen);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (consolePanelOpen) m_ConsolePanel.OnImGuiRender(&consolePanelOpen);
    if (disassemblyViewOpen) m_DisassemblyViewerPanel.OnImGuiRender(&disassemblyViewOpen);
    if (hexViewerOpen) m_HexViewerPanel.OnImGuiRender(&hexViewerOpen);
    if (sectionPanelOpen) m_SectionsPanel.OnImGuiRender(&sectionPanelOpen);
    if (decompilerPanelOpen) m_DecompilerViewerPanel.OnImGuiRender(&decompilerPanelOpen);
    if (functionListPanelOpen) {
        m_FunctionListPanel.SetDisassemblyViewerPanel(&m_DisassemblyViewerPanel);
        m_FunctionListPanel.OnImGuiRender(&functionListPanelOpen);
    }
    if (graphViewPanelOpen && !m_JVMDisassembler.GetClassesMap().empty()) {
        m_GraphViewPanel.OnImGuiRender(&graphViewPanelOpen);
    }

    ImGui::End();
    m_FileBrowser.SetTypeFilters({".class", ".jar"});
    m_FileBrowser.Display();
    std::string output = "";
    std::string decompiledOutput = "";
    if (m_FileBrowser.HasSelected()) {
        m_FilePath = m_FileBrowser.GetSelected().c_str();
        // Display the java class file binary in a hex viewer.
        m_HexViewerPanel.ReadFile(m_FilePath);

        // Disassemble Class File
        output = m_JVMDisassembler.DisassemblyWriter(m_FilePath);

        if (output.size() == 0) {
            m_FileBrowser.ClearSelected();
        } else {
            std::vector<std::string> classNames = m_JVMDisassembler.GetClassNames();
            std::vector<std::string> functionNames = m_JVMDisassembler.GetFunctionNames();
            TitaniumDecompiler::ClassesMap classMap = m_JVMDisassembler.GetClassesMap();
            m_FunctionListPanel.SetClassesMap(classMap);

            for (auto& classEntry : m_JVMDisassembler.GetClassesToFunctions()) {
                TD_INFO(classEntry.first);
                m_Decompiler.ProcessFunctions(classEntry.second);
            }
            decompiledOutput = m_Decompiler.Decompile();
            for (auto& func : m_Disassembler.GetFunctions()) {
                for (auto& block : func.GetFunctionCFG()->GetBlocks()) {
                    func.GetFunctionCFG()->PrintBlock(block);
                }
            }
            m_DisassemblyViewerPanel.SetDisassemblyText(output);
            m_DecompilerViewerPanel.SetDecompilerText(decompiledOutput);
            // m_FunctionListPanel.SetFunctionNames(functionNames);
            TD_INFO("JVM Class File has been fully read.");

            const auto& classesToFunctionsMap = m_JVMDisassembler.GetClassesToFunctions();

            for (auto& classPair : classesToFunctionsMap) {
                for (auto& function : classPair.second) {
                    if (function.GetFunctionCFG()) {
                        m_GraphViewPanel.CreateGraphDataFromCFG(*function.GetFunctionCFG());
                    }
                }
            }

            m_FileBrowser.ClearSelected();
        }
    }
}

void TitaniumLayer::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(TD_BIND_EVENT_FN(TitaniumLayer::OnKeyPressed));
}

#pragma warning(default : 4716)
bool TitaniumLayer::OnKeyPressed(KeyPressedEvent& e) {
    // Shortcuts
    if (e.GetRepeatCount() > 0) {
        return false;
    }

    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
    switch (e.GetKeyCode()) {
        case Key::N: {
            if (control) TD_WARN("New File hasn't been Implemented Yet");
            break;
        }
        case Key::O: {
            if (control)
                // OpenAnalyzeFile();
                break;
        }
        case Key::S: {
            if (control && shift) TD_WARN("Save File hasn't been Implemented Yet");
            //             // SaveSceneAs();
            break;
        }
    }
    return false;
}

void TitaniumLayer::AnalyzeFile(const std::filesystem::path& path) {}

void TitaniumLayer::OpenFile() {}
}  // namespace TitaniumRenderer
