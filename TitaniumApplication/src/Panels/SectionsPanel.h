#pragma once

#include <TitaniumRenderer/Base.h>
#include <TitaniumDecompiler.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

namespace TitaniumRenderer {
class SectionsPanel {
public:
    SectionsPanel() = default;
    void OnImGuiRender();
private:
    TitaniumDecompiler::ELF m_elfFile;

};
}