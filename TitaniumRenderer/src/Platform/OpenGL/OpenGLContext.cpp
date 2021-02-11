
#include "tdpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace TitaniumRenderer {

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {
    TD_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGLContext::Init() {
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    TD_CORE_ASSERT(status, "Failed to initialize Glad!");
    
    TD_CORE_INFO("OpenGL Info:");
    TD_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    TD_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    TD_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

    TD_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Titanium Decompiler requires at least OpenGL version 4.5!");
}

void OpenGLContext::SwapBuffers() { 
    glfwSwapBuffers(m_WindowHandle); 
}

}  // namespace TitaniumRenderer