#include "SectionsPanel.h"

#include <imgui/imgui.h>

namespace TitaniumRenderer {
void SectionsPanel::OnImGuiRender() {
    
    ImGui::Begin("Section Table");
    // DisplayElfFileSections(m_FileBrowser.GetSelected().c_str());
    ImGui::End();
}
}