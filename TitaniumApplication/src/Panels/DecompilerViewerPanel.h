#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <TitaniumDecompiler.h>
#include "TextEditor.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

namespace TitaniumRenderer {
class DecompilerViewerPanel{
public:
    DecompilerViewerPanel() = default;
    void OnImGuiRender(bool* open);
    void SetDecompilerText(const std::string& dis);
    void SetCursorPositionToFunction(int lineNumber);

public:
    inline std::string GetDecompilerText() { return m_DecompilerText; }

private:
    TextEditor m_TextEditor;
    std::string m_DecompilerText;
    int m_LastLineNumber;
};
}
