// TitaniumRenderer/UI/ImGuiRenderer.cpp
#include "TitaniumRenderer/ImGui/ImGuiRenderer.h"

#ifdef TD_PLATFORM_MACOS
#include "Platform/Metal/MetalImGuiRenderer.h"
#else
#include "Platform/OpenGL/OpenGLImGuiRenderer.h"
#endif

namespace TitaniumRenderer {

Scope<ImGuiRenderer> ImGuiRenderer::Create() {
#ifdef TD_PLATFORM_MACOS
    return CreateScope<MetalImGuiRenderer>();
#else
    return CreateScope<OpenGLImGuiRenderer>();
#endif
}

}  // namespace TitaniumRenderer
