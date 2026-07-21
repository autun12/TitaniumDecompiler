#pragma once
#include <string>

namespace TitaniumRenderer {
class ITextRenderer {
public:
    virtual ~ITextRenderer() = default;
    virtual void SetText(const std::string& text) = 0;
    virtual void Render(const char* title) = 0;
    virtual void SetCursorToLine(int lineNumber) = 0;
    virtual void ApplyTheme() = 0;
};
}  // namespace TitaniumRenderer
