#include "Platform/Metal/MetalImGuiRenderer.h"

#include "Metal/Metal.hpp"
#include "Platform/Metal/MetalContext.h"  // Internal context holding native Metal state
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_metal.h"

namespace TitaniumRenderer {

void MetalImGuiRenderer::Init(GLFWwindow* windowHandle) {
    ImGui_ImplGlfw_InitForOther(windowHandle, true);
    // MTL::Device* device = MetalContext::GetNativeDevice();

    // id<MTLDevice> device = MetalContext::GetNativeDevice();
    // ImGui_ImplMetal_Init(device);
}

void MetalImGuiRenderer::Shutdown() {
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void MetalImGuiRenderer::NewFrame() {
    // MTL::Private::Class::s_kMTLMeshRenderPipelineDescriptor* passDescriptor =
    //       MetalContext::GetCurrentRenderPassDescriptor();
    //   ImGui_ImplMetal_NewFrame(passDescriptor);
    ImGui_ImplGlfw_NewFrame();
}

void MetalImGuiRenderer::RenderDrawData(ImDrawData* drawData) {
    // id<MTLCommandBuffer> commandBuffer =
    //     MetalContext::GetCurrentCommandBuffer();
    // id<MTLRenderCommandEncoder> commandEncoder =
    //     MetalContext::GetCurrentCommandEncoder();
    //
    // ImGui_ImplMetal_RenderDrawData(drawData, commandBuffer, commandEncoder);
}

}  // namespace TitaniumRenderer
