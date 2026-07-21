#include "HexViewerPanel.h"

#include <stdint.h>

#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

#include "TitaniumLogger/Logger/Log.h"

namespace TitaniumRenderer {
void HexViewerPanel::OnImGuiRender() {
    if (!m_IsOpen) return;

    if (ImGui::Begin(GetName().c_str(), GetOpenPtr())) {
        if (m_FileData.empty()) {
            static char data[0x10000];
            size_t data_size = 0x10000;
            m_MemEditor.DrawContents(data, data_size);
        } else {
            m_MemEditor.DrawContents(m_FileData.data(), m_FileSize);
        }
    }
    ImGui::End();
}

void HexViewerPanel::ReadFile(const std::filesystem::path& path) {
    std::ifstream fileStream(path, std::ios::binary);
    if (!fileStream) {
        TD_ERROR("HexViewer failed to open file: {0}", path.string());
        return;
    }

    fileStream.seekg(0, std::ios::end);
    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    m_FileSize = fileSize;

    m_FileData.resize(fileSize);
    fileStream.read(reinterpret_cast<char*>(m_FileData.data()), fileSize);
    fileStream.close();

    TD_INFO("HexViewer successfully cached {0} bytes from: {1}", m_FileSize,
            path.filename().string());
}

}  // namespace TitaniumRenderer
