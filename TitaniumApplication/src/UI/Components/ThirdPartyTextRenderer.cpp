#include "ThirdPartyTextRenderer.h"

#include "TitaniumRenderer/UI/ThemeManager.h"

namespace TitaniumRenderer {
ThirdPartyTextRenderer::ThirdPartyTextRenderer() {
    m_Editor.SetReadOnly(true);
    m_Editor.SetShowWhitespaces(false);
}

void ThirdPartyTextRenderer::SetText(const std::string& text) {
    m_Editor.SetText(text);
}

void ThirdPartyTextRenderer::Render(const char* title) {
    m_Editor.Render(title);
}

void ThirdPartyTextRenderer::SetCursorToLine(int lineNumber) {
    if (lineNumber >= 0 && lineNumber < m_Editor.GetTotalLines()) {
        m_Editor.SetCursorPosition({lineNumber, 0});
    }
}

void ThirdPartyTextRenderer::ApplyTheme() {
    auto& theme = ThemeManager::Get();
    TextEditor::Palette palette = TextEditor::GetDarkPalette();

    // Map global backgrounds
    palette[(int)TextEditor::PaletteIndex::Background] =
        ImGui::ColorConvertFloat4ToU32(theme.GetImGuiColor(ImGuiCol_ChildBg));
    palette[(int)TextEditor::PaletteIndex::Selection] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetImGuiColor(ImGuiCol_TextSelectedBg));
    palette[(int)TextEditor::PaletteIndex::Cursor] =
        ImGui::ColorConvertFloat4ToU32(theme.GetImGuiColor(ImGuiCol_TabActive));

    // Map tokens
    palette[(int)TextEditor::PaletteIndex::Default] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetTokenStyle(TokenType::DefaultText).Color);
    palette[(int)TextEditor::PaletteIndex::Keyword] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetTokenStyle(TokenType::Keyword).Color);
    palette[(int)TextEditor::PaletteIndex::KnownIdentifier] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetTokenStyle(TokenType::Type).Color);
    palette[(int)TextEditor::PaletteIndex::Number] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetTokenStyle(TokenType::Constant).Color);
    palette[(int)TextEditor::PaletteIndex::Comment] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetTokenStyle(TokenType::Comment).Color);
    palette[(int)TextEditor::PaletteIndex::LineNumber] =
        ImGui::ColorConvertFloat4ToU32(
            theme.GetTokenStyle(TokenType::Address).Color);

    m_Editor.SetPalette(palette);
}
}  // namespace TitaniumRenderer
