#pragma once

#include "TitaniumRenderer/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace TitaniumRenderer {

class OpenGLContext : public GraphicsContext {
public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;
    virtual void SetVSync(bool enabled) override;
    virtual bool IsVSync() const override { return m_VSync; }

private:
    GLFWwindow* m_WindowHandle;
    bool m_VSync;
};

}  // namespace TitaniumRenderer
