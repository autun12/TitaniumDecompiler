#pragma once
#include "TitaniumRenderer/ImGui/ImGuiRenderer.h"

namespace TitaniumRenderer {
class Window;
class MetalImGuiRenderer : public ImGuiRenderer {
public:
    void Init(Window& windowHandle) override;
    void Shutdown() override;
    void NewFrame() override;
    void RenderDrawData(ImDrawData* drawData) override;
};

}  // namespace TitaniumRenderer
