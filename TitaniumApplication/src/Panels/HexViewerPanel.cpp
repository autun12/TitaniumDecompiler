#include "HexViewerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_memory_editor.h>
#include <stdint.h>

namespace TitaniumRenderer {
void HexViewerPanel::OnImGuiRender() {
    static MemoryEditor memEditor;
    static char data[0x10000];
    size_t data_size = 0x10000;
    // ImGui::Begin("Hex Viewer");
    memEditor.DrawWindow("Hex Viewer", data, data_size);
    // ImGui::End();
}

}