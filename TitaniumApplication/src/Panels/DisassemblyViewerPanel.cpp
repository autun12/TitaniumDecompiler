#include "DisassemblyViewerPanel.h"

#include "imgui.h"

namespace TitaniumRenderer {
void DisassemblyViewerPanel::OnImGuiRender(bool* open) {
    ImGui::Begin("Disassembly View", open);
    static std::string lastDisassembly;
    if (lastDisassembly != m_Disassembly) {
        m_TextEditor.SetText(m_Disassembly);
        lastDisassembly = m_Disassembly;
    }
    m_TextEditor.SetReadOnly(true);
    m_TextEditor.SetShowWhitespaces(false);
    m_TextEditor.Render("HELLO");
    ImGui::End();
}

void DisassemblyViewerPanel::SetDisassemblyText(const std::string& dis) {
    m_Disassembly = dis;
}

void DisassemblyViewerPanel::SetCursorPositionToFunction(int lineNumber) {
    // Ensure the line number is within the valid range
    if (lineNumber >= 0 && lineNumber < m_TextEditor.GetTotalLines()) {
        // Set column to 0 to move to the beginning of the line
        TextEditor::Coordinates position(lineNumber, 0); 
        m_TextEditor.SetCursorPosition(position);
    }
}
}