#include "tdpch.h"
#include "TitaniumRenderer/Renderer/GraphicsContext.h"
#include "TitaniumRenderer/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace TitaniumRenderer {
Scope<GraphicsContext> GraphicsContext::Create(void* window) {
    switch(Renderer::GetAPI()) {
        case RendererAPI::API::None:
            TD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
            return nullptr;
        case RendererAPI::API::OpenGL:  
            return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
    }

    TD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}