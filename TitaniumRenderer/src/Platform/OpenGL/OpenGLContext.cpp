#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "TitaniumLogger/Logger/Log.h"
#include "TitaniumRenderer/Core/Base.h"

namespace TitaniumRenderer {

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle) {
    TD_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGLContext::Init() {
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    TD_CORE_ASSERT(status, "Failed to initialize Glad!");

    TD_RENDERER_INFO("OpenGL Info:");
    TD_RENDERER_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
    TD_RENDERER_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
    TD_RENDERER_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

    TD_CORE_ASSERT(
        GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5),
        "Titanium Decompiler requires at least OpenGL version 4.5!");
}

void OpenGLContext::SwapBuffers() { glfwSwapBuffers(m_WindowHandle); }

void OpenGLContext::SetVSync(bool enabled) {
    m_VSync = enabled;

    if (enabled) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
}

}  // namespace TitaniumRenderer
