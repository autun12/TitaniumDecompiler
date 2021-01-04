#include "tdpch.h"
#include "TitaniumRenderer/Renderer/Renderer.h"

namespace TitaniumRenderer {
void Renderer::Init() {
    RenderCommand::Init();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
    RenderCommand::SetViewport(0, 0, width, height);
}
}