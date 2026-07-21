#pragma once

#include <TitaniumDecompiler.h>
#include <TitaniumRenderer/Core/Base.h>
#include <stdlib.h>

#include <memory>
#include <string>

#include "../../Core/ProjectContext.h"
#include "../Components/ITextRenderer.h"
#include "../Components/ThirdPartyTextRenderer.h"
#include "IPanel.h"

namespace TitaniumRenderer {
class DisassemblyViewerPanel : public IPanel {
public:
    DisassemblyViewerPanel(bool defaultOpen)
        : IPanel("Disassembly View", defaultOpen) {
        m_TextRenderer = std::make_unique<ThirdPartyTextRenderer>();
    }
    virtual void OnImGuiRender() override;
    void SetDisassemblyText(const std::string& dis);
    void SetCursorPositionToFunction(int lineNumber);
    virtual void OnProjectLoaded(const ProjectContext& context) override;

public:
    inline std::string GetDisassemblyText() { return m_Disassembly; }

private:
    std::unique_ptr<ITextRenderer> m_TextRenderer;
    std::string m_Disassembly;
    int m_LastLineNumber;
};
}  // namespace TitaniumRenderer
