#include "HexViewerPanel.h"

#include <stdint.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

namespace TitaniumRenderer {
void HexViewerPanel::OnImGuiRender(bool* open) {
    ImGui::Begin("Hex Viewer", open);
    if(m_FileData.empty()) {
        static char data[0x10000];
        size_t data_size = 0x10000;
        m_MemEditor.DrawContents(data, data_size);
    } else {
        m_MemEditor.DrawContents(m_FileData.data(), m_FileSize);
    }
    ImGui::End();
}

void HexViewerPanel::ReadFile(const std::filesystem::path& path) {
    std::ifstream fileStream(path, std::ios::binary);
    if(!fileStream) {
        std::cerr << "Failed to open file: " << path << std::endl;
    }

    fileStream.seekg(0, std::ios::end);
    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    m_FileSize = fileSize;
    
    m_FileData.resize(fileSize);
    fileStream.read(reinterpret_cast<char*>(m_FileData.data()), fileSize);
    fileStream.close();
}

}