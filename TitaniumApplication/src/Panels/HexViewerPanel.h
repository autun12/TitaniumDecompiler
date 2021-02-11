#pragma once

#include <TitaniumRenderer/Core/Base.h>

#include <imgui/imgui.h>
#include <imgui/imgui_memory_editor.h>

namespace TitaniumRenderer {
class HexViewerPanel {
public:
    HexViewerPanel() = default;
    void OnImGuiRender(bool* open);
private:
    MemoryEditor memEditor;
    bool m_HexViewer;
};
}