#pragma once

#include <TitaniumRenderer/Core/Base.h>
#include <stdint.h>

#include <filesystem>
#include <vector>

#include "IPanel.h"
#include "imgui_memory_editor.h"

namespace TitaniumRenderer {
class HexViewerPanel : public IPanel {
public:
    HexViewerPanel(bool defaultOpen = false)
        : IPanel("Hex View", defaultOpen) {}
    virtual void OnImGuiRender() override;
    void ReadFile(const std::filesystem::path& file);

private:
    MemoryEditor m_MemEditor;
    size_t m_FileSize = 0;
    std::vector<uint8_t> m_FileData;
};
}  // namespace TitaniumRenderer
