#pragma once

#include "TitaniumRenderer/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace MTL {
class Device;
class RenderPassDescriptor;
class CommandBuffer;
class RenderCommandEncoder;
class CommandQueue;
}  // namespace MTL

namespace CA {
class MetalLayer;
class MetalDrawable;
}  // namespace CA

namespace TitaniumRenderer {

class MetalContext : public GraphicsContext {
public:
    MetalContext(GLFWwindow* windowHandle);
    virtual ~MetalContext();

    virtual void Init() override;
    virtual void SwapBuffers() override;

    inline MTL::Device* GetDevice() const { return m_Device; }
    MTL::RenderPassDescriptor* GetCurrentRenderPassDescriptor();
    MTL::RenderCommandEncoder* GetCommandEncoder();
    MTL::CommandBuffer* GetCommandBuffer() const { return m_CommandBuffer; }

    virtual void SetVSync(bool enabled) override;
    virtual bool IsVSync() const override { return m_VSync; }

private:
    GLFWwindow* m_WindowHandle;
    MTL::Device* m_Device = nullptr;

    MTL::CommandQueue* m_CommandQueue = nullptr;
    MTL::RenderPassDescriptor* m_RenderPassDescriptor = nullptr;
    CA::MetalDrawable* m_CurrentDrawable = nullptr;
    MTL::CommandBuffer* m_CommandBuffer = nullptr;
    MTL::RenderCommandEncoder* m_CommandEncoder = nullptr;
    CA::MetalLayer* m_MetalLayer = nullptr;
    bool m_VSync;
};

}  // namespace TitaniumRenderer
