#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "Platform/Metal/MetalContext.h"

#include <GLFW/glfw3.h>

#include "Metal/Metal.hpp"
#include "TitaniumLogger/Logger/Log.h"

#define GLFW_NATIVE_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace TitaniumRenderer {

MetalContext::MetalContext(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle) {
    TD_CORE_ASSERT(windowHandle, "Window handle is null!");
}

MetalContext::~MetalContext() {
    if (m_RenderPassDescriptor) {
        m_RenderPassDescriptor->release();
        m_RenderPassDescriptor = nullptr;
    }

    if (m_CommandQueue) {
        m_CommandQueue->release();
        m_CommandQueue = nullptr;
    }

    if (m_Device) {
        m_Device->release();
        m_Device = nullptr;
    }
}

void MetalContext::Init() {
    m_Device = MTL::CreateSystemDefaultDevice();
    TD_CORE_ASSERT(m_Device, "Failed to create Metal System Default Device!");

    TD_RENDERER_INFO("Metal Info:");
    TD_RENDERER_INFO("  Device Name: {0}", m_Device->name()->utf8String());
    TD_RENDERER_INFO("  Low Power: {0}", m_Device->isLowPower() ? "Yes" : "No");

    m_CommandQueue = m_Device->newCommandQueue();

    auto* metalWindow =
        reinterpret_cast<NS::Window*>(glfwGetCocoaWindow(m_WindowHandle));
    TD_CORE_ASSERT(metalWindow, "Failed to retrieve native NS::Window handle!");

    m_MetalLayer = CA::MetalLayer::layer();
    m_MetalLayer->setDevice(m_Device);
    m_MetalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

    auto* contentView = metalWindow->contentView();
    contentView->setLayer(m_MetalLayer);
    contentView->setWantsLayer(true);

    m_RenderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
}

MTL::RenderPassDescriptor* MetalContext::GetCurrentRenderPassDescriptor() {
    if (!m_CurrentDrawable) {
        // Auto-update drawable size to match actual framebuffer pixels
        if (m_MetalLayer && m_WindowHandle) {
            int fbWidth = 0, fbHeight = 0;
            glfwGetFramebufferSize(m_WindowHandle, &fbWidth, &fbHeight);
            m_MetalLayer->setDrawableSize(CGSizeMake(fbWidth, fbHeight));
        }

        m_CurrentDrawable = m_MetalLayer->nextDrawable();
        if (!m_CurrentDrawable) return nullptr;

        auto* colorAttachment =
            m_RenderPassDescriptor->colorAttachments()->object(0);
        colorAttachment->setTexture(m_CurrentDrawable->texture());
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setClearColor(
            MTL::ClearColor::Make(0.1f, 0.1f, 0.1f, 1.0f));
        colorAttachment->setStoreAction(MTL::StoreActionStore);
    } else {
        auto* colorAttachment =
            m_RenderPassDescriptor->colorAttachments()->object(0);
        colorAttachment->setLoadAction(MTL::LoadActionLoad);
    }

    return m_RenderPassDescriptor;
}

MTL::RenderCommandEncoder* MetalContext::GetCommandEncoder() {
    if (!m_CommandEncoder) {
        MTL::RenderPassDescriptor* passDesc = GetCurrentRenderPassDescriptor();
        if (!passDesc) return nullptr;

        m_CommandBuffer = m_CommandQueue->commandBuffer();
        m_CommandEncoder = m_CommandBuffer->renderCommandEncoder(passDesc);
    }

    return m_CommandEncoder;
}

void MetalContext::SwapBuffers() {
    if (m_CommandEncoder) {
        m_CommandEncoder->endEncoding();
        m_CommandEncoder->release();
        m_CommandEncoder = nullptr;
    }

    // 2. Present drawable and commit command buffer
    if (m_CommandBuffer && m_CurrentDrawable) {
        m_CommandBuffer->presentDrawable(m_CurrentDrawable);
        m_CommandBuffer->commit();
    }

    // 3. Clear pass descriptor texture reference to avoid lingering holds
    if (m_RenderPassDescriptor) {
        auto* colorAttachment =
            m_RenderPassDescriptor->colorAttachments()->object(0);
        if (colorAttachment) {
            colorAttachment->setTexture(nullptr);
        }
    }

    // 4. Release retained Metal/Quartz resources
    if (m_CommandBuffer) {
        m_CommandBuffer->release();
        m_CommandBuffer = nullptr;
    }

    if (m_CurrentDrawable) {
        m_CurrentDrawable->release();
        m_CurrentDrawable = nullptr;
    }
}

void MetalContext::SetVSync(bool enabled) {
    m_VSync = enabled;
    if (m_MetalLayer) {
        m_MetalLayer->setDisplaySyncEnabled(enabled);
    }
}

}  // namespace TitaniumRenderer
