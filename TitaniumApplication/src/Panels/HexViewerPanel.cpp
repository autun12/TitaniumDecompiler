#include "HexViewerPanel.h"

#include <stdint.h>

namespace TitaniumRenderer {
void HexViewerPanel::OnImGuiRender(bool* open) {
    static char data[0x10000];
    size_t data_size = 0x10000;
    ImGui::Begin("Hex Viewer", open);
    memEditor.DrawContents(data, data_size);
    ImGui::End();
}

}