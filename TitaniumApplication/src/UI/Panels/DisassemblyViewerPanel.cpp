#include "DisassemblyViewerPanel.h"

#include "TitaniumRenderer/UI/ThemeManager.h"
#include "imgui.h"

namespace TitaniumRenderer {
void DisassemblyViewerPanel::OnImGuiRender() {
    if (!m_IsOpen) return;

    static std::string lastThemeName;
    std::string currentTheme = ThemeManager::Get().GetCurrentThemeName();

    if (lastThemeName != currentTheme) {
        m_TextRenderer->ApplyTheme();
        lastThemeName = currentTheme;
    }

    bool showContent = ImGui::Begin(GetName().c_str(), GetOpenPtr());
    if (showContent) {
        m_TextRenderer->Render(GetName().c_str());
    }
    ImGui::End();
}

void DisassemblyViewerPanel::SetDisassemblyText(const std::string& dis) {
    m_Disassembly = dis;
    m_TextRenderer->SetText(m_Disassembly);
}

void DisassemblyViewerPanel::SetCursorPositionToFunction(int lineNumber) {
    m_TextRenderer->SetCursorToLine(lineNumber);
}

void DisassemblyViewerPanel::OnProjectLoaded(const ProjectContext& context) {
    m_Disassembly = context.GetDisassemblyText();
    if (m_TextRenderer) {
        SetDisassemblyText(m_Disassembly);
    }
}

}  // namespace TitaniumRenderer
