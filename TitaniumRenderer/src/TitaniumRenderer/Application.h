#pragma once

#include "TitaniumRenderer/Base.h"
#include "TitaniumRenderer/Events/ApplicationEvent.h"
#include "TitaniumRenderer/Events/Event.h"
#include "TitaniumRenderer/ImGui/ImGuiLayer.h"
#include "TitaniumRenderer/LayerStack.h"
#include "TitaniumRenderer/Window.h"
#include "TitaniumRenderer/Timestep.h"

int main(int argc, char** argv);

namespace TitaniumRenderer {

class Application {
public:
    Application(std::string, const std::string& name = "Titanium Decompiler");
    virtual ~Application();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    inline Window& GetWindow() { return *m_Window; }

    void Close();

    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
    inline static Application& Get() { return *s_Instance; }
    std::string CorrectFilePath(const std::string&);

private:
    void Run();
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;

private:
    static Application* s_Instance;
    std::string m_BaseDirectory;
    friend int ::main(int argc, char** argv);
};

// To be defined in CLIENT
Application* CreateApplication();

}  // namespace TitaniumRenderer