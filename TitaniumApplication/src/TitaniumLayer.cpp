#include "TitaniumLayer.h"
#include <imgui/imgui.h>
#include <TitaniumRenderer/PlatformUtils.h>
#include <stdio.h>
#include <stdlib.h>

namespace TitaniumRenderer {
TitaniumLayer::TitaniumLayer() : Layer("TitaniumLayer") {}

void TitaniumLayer::OnAttach() {}

void TitaniumLayer::OnDetach() {}

void TitaniumLayer::OnUpdate(Timestep ts) {
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();
}

void TitaniumLayer::OnImGuiRender() {
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

    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows, 
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
            if(ImGui::MenuItem("New", "Ctrl+N")) { }
                // NewScene();

            if(ImGui::MenuItem("Open...", "Ctrl+O")) {
                OpenAnalyzeFile();
            }
              
            if(ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) { }
                // SaveSceneAs();

            if(ImGui::MenuItem("Exit")) {
                Application::Get().Close();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    
    m_HexViewerPanel.OnImGuiRender();
    m_SectionsPanel.OnImGuiRender();

    ImGui::End();

    // if(m_FileBrowser.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN)) {
    //     m_SectionsPanel.DisplayElfFileSections(m_FileBrowser.selected_path.c_str());
    //     elfFile.GetElfFile(m_FileBrowser.selected_path.c_str());
    // }

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
                TD_CORE_INFO("New File hasn't been Implemented Yet");
                // NewScene();
            break;
        }
        case Key::O: {
            if(control)
                TD_CORE_INFO("Open File hasn't been Implemented Yet");
                // OpenFile();
            break;
        }
        case Key::S: {
            if(control && shift)
                TD_CORE_INFO("Save File hasn't been Implemented Yet");
                // SaveSceneAs();
            break;
        }
	}
}

void TitaniumLayer::OpenAnalyzeFile() {
    std::optional<std::string> filepath = FileDialogs::OpenFile("");
    
    if(filepath) {
        m_elfFile.GetElfFile(*filepath);
    }
}
}