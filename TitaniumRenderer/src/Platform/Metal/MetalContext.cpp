#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "Platform/Metal/MetalContext.h"

#include <GLFW/glfw3.h>

#include "Metal/Metal.hpp"
#include "TitaniumLogger/Logger/Log.h"
#include "TitaniumRenderer/Core/Base.h"

#define GLFW_NATIVE_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace TitaniumRenderer {

MetalContext::MetalContext(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle) {
    TD_CORE_ASSERT(windowHandle, "Window handle is null!");
}

MetalContext::~MetalContext() {
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

    auto* metalWindow = glfwGetCocoaWindow(m_WindowHandle);
}

void MetalContext::SwapBuffers() {}

void MetalContext::SetVSync(bool enabled) {
    m_VSync = enabled;
    if (m_MetalLayer) {
        m_MetalLayer->setDisplaySyncEnabled(enabled);
    }
}

}  // namespace TitaniumRenderer
