#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <TitaniumDecompiler.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <imgui/imgui.h>
#include <imgui/imfilebrowser.h>

namespace TitaniumRenderer {
class SectionsPanel {
public:
    SectionsPanel() = default;
    void OnImGuiRender(bool* open);
    std::string GetFileName(std::string fileName);
    void DisplaySections(const std::string& fileName, bool* display);

private:
    ImGui::FileBrowser m_FileBrowser;
    TitaniumDecompiler::ELF m_elfFile;
    Elf64Hdr m_elfHeader;
    Elf64SHdr m_sectionHeader;
    std::string m_Sections;
};
}