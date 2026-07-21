#include "DecompilerViewerPanel.h"

#include "imgui.h"

namespace TitaniumRenderer {
void DecompilerViewerPanel::OnImGuiRender() {
    bool showContent = ImGui::Begin(GetName().c_str(), GetOpenPtr());
    if (showContent) {
        m_TextRenderer->Render(GetName().c_str());
    }
    ImGui::End();
}

void DecompilerViewerPanel::SetDecompilerText(const std::string& dis) {
    m_DecompilerText = dis;
    m_TextRenderer->SetText(dis);
}

void DecompilerViewerPanel::SetCursorPositionToFunction(int lineNumber) {
    m_TextRenderer->SetCursorToLine(lineNumber);
}
}  // namespace TitaniumRenderer
