#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <TitaniumDecompiler.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

namespace TitaniumRenderer {
class DisassemblyViewerPanel {
public:
    DisassemblyViewerPanel() = default;
    void OnImGuiRender(bool* open);
private:
    TitaniumDecompiler::ELF m_elfFile;

};
}