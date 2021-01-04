#include "tdpch.h"
#include "TitaniumRenderer/Application.h"

#include "TitaniumRenderer/Input.h"
#include "TitaniumRenderer/Log.h"
#include "TitaniumRenderer/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace TitaniumRenderer {

Application* Application::s_Instance = nullptr;

Application::Application(std::string base_directory, const std::string& name) {
    TD_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_BaseDirectory = base_directory;

    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(TD_BIND_EVENT_FN(OnEvent));

    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

Application::~Application() {}

void Application::PushLayer(Layer* layer) {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer) {
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close() {
    m_Running = false;
}

void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(TD_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(TD_BIND_EVENT_FN(Application::OnWindowResize));
    
    for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
        if(e.Handled) { 
            break;
        }
       
        (*it)->OnEvent(e);
    }
}

void Application::Run() {
    while (m_Running) {
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        if(!m_Minimized) {
            {
                for(Layer* layer : m_LayerStack) {
                    layer->OnUpdate(timestep);
                }
            }

            m_ImGuiLayer->Begin();
            {
                for(Layer* layer : m_LayerStack) {
                    layer->OnImGuiRender();
                }
            }
            m_ImGuiLayer->End();
        }
        
        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e) {
    if(e.GetWidth() == 0 || e.GetHeight() == 0) {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
    return false;
}

}  // namespace TitaniumRenderer