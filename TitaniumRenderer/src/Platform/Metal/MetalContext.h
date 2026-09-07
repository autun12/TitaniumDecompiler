#pragma once

#include "TitaniumRenderer/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace MTL {
class Device;
}

namespace CA {
class MetalLayer;
}

namespace TitaniumRenderer {

class MetalContext : public GraphicsContext {
public:
    MetalContext(GLFWwindow* windowHandle);
    virtual ~MetalContext();

    virtual void Init() override;
    virtual void SwapBuffers() override;

    inline MTL::Device* GetDevice() const { return m_Device; }
    virtual void SetVSync(bool enabled) override;
    virtual bool IsVSync() const override { return m_VSync; }

private:
    GLFWwindow* m_WindowHandle;
    MTL::Device* m_Device = nullptr;
    CA::MetalLayer* m_MetalLayer = nullptr;
    bool m_VSync;
};

}  // namespace TitaniumRenderer
