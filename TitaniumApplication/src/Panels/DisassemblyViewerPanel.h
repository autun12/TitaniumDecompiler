#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <TitaniumDecompiler.h>
#include "TextEditor.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

namespace TitaniumRenderer {
class DisassemblyViewerPanel {
public:
    DisassemblyViewerPanel() = default;
    void OnImGuiRender(bool* open);
    void SetDisassemblyText(const std::string& dis);
    void SetCursorPositionToFunction(int lineNumber);

public:
    inline std::string GetDisassemblyText() { return m_Disassembly; }

private:
    TextEditor m_TextEditor;
    std::string m_Disassembly;
    int m_LastLineNumber;
};
}