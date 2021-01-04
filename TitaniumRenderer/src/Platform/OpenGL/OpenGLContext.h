#pragma once

#include "TitaniumRenderer/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace TitaniumRenderer {

class OpenGLContext : public GraphicsContext {
public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* m_WindowHandle;
};

}  // namespace TitaniumRenderer