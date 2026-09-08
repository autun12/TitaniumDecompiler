#include "Platform/Metal/MetalImGuiRenderer.h"

#include <GLFW/glfw3.h>

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
    MetalContext& metalCtx = static_cast<MetalContext&>(window.GetContext());
    TD_CORE_ASSERT(metalCtx, "Failed to get context");
    MTL::Device* device = metalCtx.GetDevice();
    TD_CORE_ASSERT(device != nullptr, "Metal device is null!");
    ImGui_ImplGlfw_InitForOther(nativeWindow, true);
    bool initSuccess = ImGui_ImplMetal_Init(device);
    TD_CORE_ASSERT(
        initSuccess,
        "ImGui_ImplMetal_Init failed to initialize Metal shading objects");
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
    if (!passDesc) return;
    ImGui_ImplMetal_NewFrame(passDesc);
    ImGui_ImplGlfw_NewFrame();
}

void MetalImGuiRenderer::RenderDrawData(ImDrawData* drawData) {
    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());

    MTL::CommandBuffer* cmdBuffer = metalCtx.GetCommandBuffer();
    MTL::RenderCommandEncoder* cmdEncoder = metalCtx.GetCommandEncoder();

    if (cmdBuffer && cmdEncoder) {
        // Ensure scale is correct for Retina displays when merged back into
        // main window
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(
            static_cast<GLFWwindow*>(appWindow.GetNativeWindow()), &fbWidth,
            &fbHeight);
        if (fbWidth > 0 && fbHeight > 0) {
            drawData->ScaleClipRects(
                ImVec2((float)fbWidth / appWindow.GetWidth(),
                       (float)fbHeight / appWindow.GetHeight()));
        }
        ImGui_ImplMetal_RenderDrawData(drawData, cmdBuffer, cmdEncoder);
    }
}

}  // namespace TitaniumRenderer
