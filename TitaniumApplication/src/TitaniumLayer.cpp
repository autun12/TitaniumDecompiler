#include "TitaniumLayer.h"
#include <TitaniumRenderer/Utils/PlatformUtils.h>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include "Panels/ConsoleSink.h"

namespace TitaniumRenderer {

TitaniumLayer::TitaniumLayer() : Layer("TitaniumLayer") {
    Application::Get().GetImGuiLayer()->BlockEvents(false);
}

void TitaniumLayer::OnAttach() {
    auto consoleSink = std::make_shared<ConsoleSink_mt>(&m_ConsolePanel);
    consoleSink->set_pattern("%v");
    Log::GetCoreLogger()->sinks().push_back(consoleSink);
    Log::GetClientLogger()->sinks().push_back(consoleSink);
}

void TitaniumLayer::OnDetach() {}

void TitaniumLayer::OnUpdate(Timestep ts) {
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();
}

void TitaniumLayer::OnImGuiRender() {
    // Panel open / close variables
    static bool hexViewerOpen = true;
    static bool sectionPanelOpen = true;
    static bool disassemblyViewOpen = true;
    static bool consolePanelOpen = true;

    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if(opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
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

    if(opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;
    m_FileBrowser.SetTitle("Browse Files");

    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows, 
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
            if(ImGui::MenuItem("New", "Ctrl+N")) { }
                // NewScene();

            if(ImGui::MenuItem("Open...", "Ctrl+O")) {
                // OpenAnalyzeFile();
                m_FileBrowser.Open();
            }
              
            if(ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) { }
                // SaveSceneAs();

            if(ImGui::MenuItem("Exit")) {
                Application::Get().Close();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Panels")) {
            if(ImGui::MenuItem("Console View Panel")) {
                if(consolePanelOpen) {
                    consolePanelOpen = false;
                } else if(!consolePanelOpen) {
                    consolePanelOpen = true;
                }
            }
    
            if(ImGui::MenuItem("Disassembly View Panel")) {
                if(disassemblyViewOpen) {
                    disassemblyViewOpen = false;
                } else if(!disassemblyViewOpen) {
                    disassemblyViewOpen = true;
                }
            }
            
            if(ImGui::MenuItem("Hex View Panel")) {
                if(hexViewerOpen) {
                    hexViewerOpen = false;
                } else if(!hexViewerOpen) {
                    hexViewerOpen = true;
                }
            }

            if(ImGui::MenuItem("Sections View Panel")) {
                if(sectionPanelOpen) {
                    sectionPanelOpen = false;
                } else if(!sectionPanelOpen) {
                    sectionPanelOpen = true;
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if(consolePanelOpen) {
        m_ConsolePanel.OnImGuiRender(&consolePanelOpen);
    }

    if(disassemblyViewOpen) {
        m_DisassemblyViewerPanel.OnImGuiRender(&disassemblyViewOpen);
    }
    
    if(hexViewerOpen) {
        m_HexViewerPanel.OnImGuiRender(&hexViewerOpen);
    }

    if(sectionPanelOpen) {
        m_SectionsPanel.OnImGuiRender(&sectionPanelOpen);
    }
    
    ImGui::End();
    m_FileBrowser.Display();
    if(m_FileBrowser.HasSelected()) {
        // TD_INFO(m_FileBrowser.GetSelected().string());
        m_elfFile.GetElfFile(m_FileBrowser.GetSelected().c_str());
        m_FileBrowser.ClearSelected();
    }
}

void TitaniumLayer::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(TD_BIND_EVENT_FN(TitaniumLayer::OnKeyPressed));
}

bool TitaniumLayer::OnKeyPressed(KeyPressedEvent& e) {
    // Shortcuts
    if(e.GetRepeatCount() > 0) {
        return false;
    }

    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
    switch (e.GetKeyCode()) {
        case Key::N: {
            if(control)
                TD_WARN("New File hasn't been Implemented Yet");
                // NewScene();
            break;
        }
        case Key::O: {
            if(control)
                // OpenAnalyzeFile();
            break;
        }
        case Key::S: {
            if(control && shift)
                TD_WARN("Save File hasn't been Implemented Yet");
                // SaveSceneAs();
            break;
        }
	}
}

void TitaniumLayer::OpenAnalyzeFile() {
    // std::string filepath = FileDialogs::OpenFile("");
    
    // std::filesystem::path fileExtension(filepath);
    
    // m_elfFile.GetElfFile(filepath.c_str());
    // TODO: fix PE Parser
    // if(fileExtension.extension() == ".exe" || fileExtension.extension() == ".EXE") {
        // TD_WARN("PE Parser (Work In Progress)\n");
    // m_peFile.GetPeFile(filepath.c_str());
    // } else if(fileExtension.extension() == "") {
        // TD_INFO(std::filesystem::path(filepath).stem());
        // m_elfFile.GetElfFile(filepath.c_str());
    // }
}
}