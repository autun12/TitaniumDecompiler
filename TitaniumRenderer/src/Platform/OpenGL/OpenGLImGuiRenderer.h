// TitaniumRenderer/Platform/OpenGL/OpenGLImGuiRenderer.h
#pragma once
#include "TitaniumRenderer/ImGui/ImGuiRenderer.h"

namespace TitaniumRenderer {

class OpenGLImGuiRenderer : public ImGuiRenderer {
public:
    void Init(GLFWwindow* windowHandle) override;
    void Shutdown() override;
    void NewFrame() override;
    void RenderDrawData(ImDrawData* drawData) override;
};

}  // namespace TitaniumRenderer
