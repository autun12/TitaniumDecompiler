#include "DecompilerViewerPanel.h"

#include "imgui.h"

namespace TitaniumRenderer {
void DecompilerViewerPanel::OnImGuiRender(bool* open) {
    ImGui::Begin("Decompiler View", open);
    static std::string lastDisassembly;
    if (lastDisassembly != m_DecompilerText) {
        m_TextEditor.SetText(m_DecompilerText);
        lastDisassembly = m_DecompilerText;
    }
    m_TextEditor.SetReadOnly(true);
    m_TextEditor.SetShowWhitespaces(false);
    m_TextEditor.Render("HELLO2");
    ImGui::End();
}

void DecompilerViewerPanel::SetDecompilerText(const std::string& dis) {
    m_DecompilerText = dis;
}

void DecompilerViewerPanel::SetCursorPositionToFunction(int lineNumber) {
    // Ensure the line number is within the valid range
    if (lineNumber >= 0 && lineNumber < m_TextEditor.GetTotalLines()) {
        // Set column to 0 to move to the beginning of the line
        TextEditor::Coordinates position(lineNumber, 0);
        m_TextEditor.SetCursorPosition(position);
    }
}
}
