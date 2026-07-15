#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <stdint.h>
#include "imgui.h"
#include "imgui_memory_editor.h"
#include <filesystem>
#include <vector>

namespace TitaniumRenderer {
class HexViewerPanel {
public:
    HexViewerPanel() = default;
    void OnImGuiRender(bool* open);
    void ReadFile(const std::filesystem::path& file);
private:
    MemoryEditor m_MemEditor;
    bool m_HexViewer;
    size_t m_FileSize = 0;
    std::vector<uint8_t> m_FileData; 
};
}