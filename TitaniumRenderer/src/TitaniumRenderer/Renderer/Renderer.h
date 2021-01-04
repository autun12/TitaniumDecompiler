#pragma once

#include "TitaniumRenderer/Renderer/RenderCommand.h"

namespace TitaniumRenderer {
class Renderer {
public:
    static void Init();
    static void OnWindowResize(uint32_t width, uint32_t height);
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

}