#include "SettingsWidget.h"

#include "TitaniumLogger/Logger/Log.h"
#include "TitaniumRenderer/UI/ThemeManager.h"

namespace TitaniumRenderer {
void SettingsWidget::OnImGuiRender() {
    if (!m_IsOpen) return;

    // Force this window to be non-dockable and adapt to standard dialog rules
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize;

    // Passing GetOpenPtr() adds the 'X' close button to the top right of the
    // window automatically
    if (ImGui::Begin(m_Name.c_str(), GetOpenPtr(), windowFlags)) {
        m_FileBrowser.SetTitle("Load Themes");
        ImGui::Text("Application Configurations");
        ImGui::Separator();

        static bool showLineNumbers = true;
        ImGui::Checkbox("Show Line Numbers", &showLineNumbers);

        static int tabSize = 4;
        ImGui::SliderInt("Tab Size", &tabSize, 2, 8);

        ImGui::Separator();
        if (ImGui::Button("Load Theme")) {
            m_FileBrowser.SetTitle("Load Themes");
            m_FileBrowser.SetTypeFilters({".yaml"});
            m_FileBrowser.Open();
        }

        ImGui::Separator();
        if (ImGui::Button("Close")) {
            Close();
        }

        LoadTheme();
    }
    ImGui::End();
}

void SettingsWidget::LoadTheme() {
    m_FileBrowser.Display();
    if (m_FileBrowser.HasSelected()) {
        std::filesystem::path path = m_FileBrowser.GetSelected();
        bool success = ThemeManager::Get().LoadTheme(path);
        if (!success) {
            TD_ERROR("Failed to load chosen theme file");
        }

        m_FileBrowser.ClearSelected();
        m_FileBrowser.Close();
    }
}
}  // namespace TitaniumRenderer
