#pragma once
#include "ITextRenderer.h"
#include "TextEditor.h"  // ImGuiColorTextEdit header

namespace TitaniumRenderer {
class ThirdPartyTextRenderer : public ITextRenderer {
public:
    ThirdPartyTextRenderer();
    virtual void SetText(const std::string& text) override;
    virtual void Render(const char* title) override;
    virtual void SetCursorToLine(int lineNumber) override;
    virtual void ApplyTheme() override;

private:
    TextEditor m_Editor;
};
}  // namespace TitaniumRenderer
