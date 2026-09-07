#include "Platform/Metal/MetalImGuiRenderer.h"

#include "Metal/Metal.hpp"
#include "Platform/Metal/MetalContext.h"  // Internal context holding native Metal state
#include "TitaniumRenderer/Core/Application.h"
#include "TitaniumRenderer/Core/Assert.h"
#include "TitaniumRenderer/Core/Window.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_metal.h"

namespace TitaniumRenderer {
void MetalImGuiRenderer::Init(Window& window) {
    GLFWwindow* nativeWindow =
        static_cast<GLFWwindow*>(window.GetNativeWindow());
    ImGui_ImplGlfw_InitForOther(nativeWindow, true);
    MetalContext& metalCtx = static_cast<MetalContext&>(window.GetContext());
    TD_CORE_ASSERT(metalCtx, "Failed to get context");
    MTL::Device* device = metalCtx.GetDevice();

    ImGui_ImplMetal_Init(device);
}

void MetalImGuiRenderer::Shutdown() {
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void MetalImGuiRenderer::NewFrame() {
    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());
    MTL::RenderPassDescriptor* passDesc =
        metalCtx.GetCurrentRenderPassDescriptor();
    ImGui_ImplMetal_NewFrame(passDesc);
    ImGui_ImplGlfw_NewFrame();
}

void MetalImGuiRenderer::RenderDrawData(ImDrawData* drawData) {
    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());

    MTL::CommandBuffer* cmdBuffer = metalCtx.GetCommandBuffer();
    MTL::RenderCommandEncoder* cmdEncoder = metalCtx.GetCommandEncoder();

    if (cmdBuffer && cmdEncoder) {
        ImGui_ImplMetal_RenderDrawData(drawData, cmdBuffer, cmdEncoder);
    }
}

}  // namespace TitaniumRenderer
