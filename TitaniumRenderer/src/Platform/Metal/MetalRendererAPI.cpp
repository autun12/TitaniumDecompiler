#include "Metal/Metal.hpp"
#include "Platform/Metal/MetalContext.h"
#include "TitaniumLogger/Logger/Log.h"
#include "TitaniumRenderer/Core/Application.h"
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "MetalRendererAPI.h"
namespace TitaniumRenderer {
void MetalRendererAPI::Init() {
    TD_RENDERER_INFO("Initializing Metal Renderer API");

    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());
    MTL::Device* device = metalCtx.GetDevice();

    if (!device) {
        TD_RENDERER_ERROR(
            "Failed to initialize MetalRendererAPI: MTL::Device is null!");
        return;
    }

    // 1. Replicate glEnable(GL_DEPTH_TEST) & glDepthFunc(GL_LESS)
    auto* depthDesc = MTL::DepthStencilDescriptor::alloc()->init();
    depthDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
    depthDesc->setDepthWriteEnabled(true);

    m_DepthStencilState = device->newDepthStencilState(depthDesc);
    depthDesc->release();
}

void MetalRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width,
                                   uint32_t height) {
    m_Viewport.x = static_cast<double>(x);
    m_Viewport.y = static_cast<double>(y);
    m_Viewport.width = static_cast<double>(width);
    m_Viewport.height = static_cast<double>(height);

    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());

    if (MTL::RenderCommandEncoder* encoder = metalCtx.GetCommandEncoder()) {
        MTL::Viewport vp{m_Viewport.x,      m_Viewport.y,     m_Viewport.width,
                         m_Viewport.height, m_Viewport.znear, m_Viewport.zfar};
        encoder->setViewport(vp);
    }
}

void MetalRendererAPI::SetClearColor(const Color& color) {
    m_ClearColor[0] = static_cast<double>(color.r);
    m_ClearColor[1] = static_cast<double>(color.g);
    m_ClearColor[2] = static_cast<double>(color.b);
    m_ClearColor[3] = static_cast<double>(color.a);

    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());

    if (MTL::RenderPassDescriptor* passDesc =
            metalCtx.GetCurrentRenderPassDescriptor()) {
        if (auto* colorAttachment = passDesc->colorAttachments()->object(0)) {
            colorAttachment->setClearColor(
                MTL::ClearColor::Make(m_ClearColor[0], m_ClearColor[1],
                                      m_ClearColor[2], m_ClearColor[3]));
        }
    }
}

void MetalRendererAPI::Clear() {
    auto& appWindow = Application::Get().GetWindow();
    auto& metalCtx = static_cast<MetalContext&>(appWindow.GetContext());

    MTL::RenderCommandEncoder* encoder = metalCtx.GetCommandEncoder();
    if (!encoder) return;
    MTL::Viewport vp{m_Viewport.x,      m_Viewport.y,     m_Viewport.width,
                     m_Viewport.height, m_Viewport.znear, m_Viewport.zfar};
    encoder->setViewport(vp);

    MTL::ScissorRect scissor = {static_cast<NS::UInteger>(m_Viewport.x),
                                static_cast<NS::UInteger>(m_Viewport.y),
                                static_cast<NS::UInteger>(m_Viewport.width),
                                static_cast<NS::UInteger>(m_Viewport.height)};
    encoder->setScissorRect(scissor);
}
}  // namespace TitaniumRenderer
