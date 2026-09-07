#include "TitaniumRenderer/Renderer/RendererAPI.h"

#include "TitaniumRenderer/Core/Base.h"

#ifdef TD_PLATFORM_MACOS
#include "Platform/Metal/MetalRendererAPI.h"
#else
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#endif

namespace TitaniumRenderer {
#ifdef TD_PLATFORM_MACOS
RendererAPI::API RendererAPI::s_API = RendererAPI::API::Metal;
#else
RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif

Scope<RendererAPI> RendererAPI::Create() {
    switch (s_API) {
        case RendererAPI::API::None:
            TD_CORE_ASSERT(false,
                           "RendererAPI::None is currently not supported!");
            return nullptr;
#ifndef TD_PLATFORM_MACOS
        case RendererAPI::API::OpenGL:
            return CreateScope<OpenGLRendererAPI>();
        case RendererAPI::API::Metal:
            TD_CORE_ASSERT(
                false,
                "RendererAPI::Metal is not supported on non-Apple devices!");
            return nullptr;
#else
        case RendererAPI::API::Metal:
            return CreateScope<MetalRendererAPI>();
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
