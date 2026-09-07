// TitaniumRenderer/Platform/OpenGL/OpenGLImGuiRenderer.h
#pragma once
#include "TitaniumRenderer/Core/Window.h"
#include "TitaniumRenderer/ImGui/ImGuiRenderer.h"

namespace TitaniumRenderer {
class Window;
class OpenGLImGuiRenderer : public ImGuiRenderer {
public:
    void Init(Window& window) override;
    void Shutdown() override;
    void NewFrame() override;
    void RenderDrawData(ImDrawData* drawData) override;
};

}  // namespace TitaniumRenderer
