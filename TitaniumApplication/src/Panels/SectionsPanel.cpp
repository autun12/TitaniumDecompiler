#include "SectionsPanel.h"
#include <TitaniumRenderer/Utils/PlatformUtils.h>
#include <filesystem>

namespace TitaniumRenderer {
void SectionsPanel::OnImGuiRender(bool* open) {
    static bool displaySection = false;
    
    ImGui::Begin("Section Table", open);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f });
    ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        if(m_FileBrowser.HasSelected()) {
            displaySection = true;
            DisplaySections(m_FileBrowser.GetSelected().string(), &displaySection);
        } else {
            displaySection = false;
        }
    }
    ImGui::EndChild();
    // ImGui::Text(GetFileName(m_FileName).c_str());
    // DisplayElfFileSections(m_FileBrowser.GetSelected().c_str());

    ImGui::PopStyleColor();
    ImGui::End();

}

std::string SectionsPanel::GetFileName(std::string fileName) {
    return std::filesystem::path(fileName).stem().string();
}

void SectionsPanel::DisplaySections(const std::string& fileName, bool* display) {
    Elf64Hdr header64;
    Elf64SHdr* sectionHeaderTable64;
    int fileDescriptor;

    fileDescriptor = open(fileName.c_str(), O_RDONLY | O_SYNC);

    if(!m_elfFile.IsElfFile(header64)) {
        printf("is not elf file");
        return;
    }
    m_elfFile.GetElfHeader(header64);

    sectionHeaderTable64 = (Elf64SHdr*)malloc(header64.e_shentsize * header64.e_shnum);
    
    if(!sectionHeaderTable64) {
        printf("Failed to allocate % bytes\n", (header64.e_shentsize * header64.e_shnum));
    }

    m_elfFile.GetSectionHeaderForImGui(fileDescriptor, header64, sectionHeaderTable64);
    printf(m_elfFile.GetSectionHeaderForImGui(fileDescriptor, header64, sectionHeaderTable64));
    
    // ImGui::Text("&s", GetSectionHeaderForImGui(fileDescriptor, header64, sectionHeaderTable64));
}
}