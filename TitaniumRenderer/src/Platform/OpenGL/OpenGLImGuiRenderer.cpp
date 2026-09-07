#include "Platform/OpenGL/OpenGLImGuiRenderer.h"

#include "TitaniumRenderer/Core/Window.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace TitaniumRenderer {

void OpenGLImGuiRenderer::Init(Window& window) {
    GLFWwindow* windowHandle =
        static_cast<GLFWwindow*>(window.GetNativeWindow());
    ImGui_ImplGlfw_InitForOpenGL(windowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void OpenGLImGuiRenderer::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void OpenGLImGuiRenderer::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void OpenGLImGuiRenderer::RenderDrawData(ImDrawData* drawData) {
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
}

}  // namespace TitaniumRenderer
