#include "TitaniumRenderer/Renderer/GraphicsContext.h"

#include "Platform/Metal/MetalContext.h"
#include "TitaniumRenderer/Core/Base.h"
#include "TitaniumRenderer/Renderer/Renderer.h"

#ifdef TD_PLATFORM_MACOS
#include "Platform/Metal/MetalContext.h"
#else
#include "Platform/OpenGL/OpenGLContext.h"
#endif

namespace TitaniumRenderer {
Scope<GraphicsContext> GraphicsContext::Create(void* window) {
    switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            TD_CORE_ASSERT(false,
                           "RendererAPI::None is currently not supported!");
            return nullptr;
#ifndef TD_PLATFORM_MACOS
        case RendererAPI::API::OpenGL:
            return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
        case RendererAPI::API::Metal:
            TD_CORE_ASSERT(
                false,
                "RendererAPI::Metal is not supported on non-Apple devices!");
            return nullptr;
#else
        case RendererAPI::API::Metal:
            TD_CORE_ASSERT(false, "Metal implementation pending");
            return CreateScope<MetalContext>(static_cast<GLFWwindow*>(window));
        case RendererAPI::API::OpenGL:
            TD_CORE_ASSERT(false,
                           "Renderer::OpenGL is disabled on macOS builds!");
            return nullptr;
#endif
    }

    TD_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}  // namespace TitaniumRenderer
