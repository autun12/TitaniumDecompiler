#pragma once

#include <TitaniumDecompiler.h>
#include <TitaniumRenderer/Core/Base.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "../Components/ITextRenderer.h"
#include "../Components/ThirdPartyTextRenderer.h"
#include "IPanel.h"

namespace TitaniumRenderer {
class DecompilerViewerPanel : public IPanel {
public:
    DecompilerViewerPanel(bool defaultOpen)
        : IPanel("Decompiler View", defaultOpen) {
        m_TextRenderer = std::make_unique<ThirdPartyTextRenderer>();
    }

    virtual void OnImGuiRender() override;
    void SetDecompilerText(const std::string& dis);
    void SetCursorPositionToFunction(int lineNumber);

public:
    inline std::string GetDecompilerText() { return m_DecompilerText; }

private:
    std::unique_ptr<ITextRenderer> m_TextRenderer;
    std::string m_DecompilerText;
    int m_LastLineNumber;
};
}  // namespace TitaniumRenderer
