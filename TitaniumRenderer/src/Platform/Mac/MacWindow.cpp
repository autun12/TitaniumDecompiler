#include "Platform/Mac/MacWindow.h"

#include "Platform/Metal/MetalContext.h"
#include "TitaniumLogger/Logger/Log.h"
#include "TitaniumRenderer/Core/Base.h"
#include "TitaniumRenderer/Core/Input.h"
#include "TitaniumRenderer/Events/ApplicationEvent.h"
#include "TitaniumRenderer/Events/KeyEvent.h"
#include "TitaniumRenderer/Events/MouseEvent.h"
#include "TitaniumRenderer/Renderer/Renderer.h"

namespace TitaniumRenderer {

static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char* description) {
    TD_RENDERER_ERROR("GLFW Error ({0}): {1}", error, description);
}

MacWindow::MacWindow(const WindowProps& props) { Init(props); }

MacWindow::~MacWindow() { Shutdown(); }

void MacWindow::Init(const WindowProps& props) {
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    TD_RENDERER_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width,
                     props.Height);

    if (s_GLFWWindowCount == 0) {
        // TODO: glfwTerminate on system shutdown
        int success = glfwInit();
        TD_CORE_ASSERT(success, "Could not intialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    {
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height,
                                    m_Data.Title.c_str(), nullptr, nullptr);
        ++s_GLFWWindowCount;
    }

    m_Context = GraphicsContext::Create(m_Window);
    m_Context->Init();

    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(
        m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode,
                                    int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, true);
                data.EventCallback(event);
                break;
            }
        }
    });

    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        KeyTypedEvent event(keycode);
        data.EventCallback(event);
    });

    glfwSetMouseButtonCallback(
        m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

    glfwSetScrollCallback(
        m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

    glfwSetCursorPosCallback(
        m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
}

void MacWindow::Shutdown() {
    glfwDestroyWindow(m_Window);
    --s_GLFWWindowCount;

    if (s_GLFWWindowCount == 0) {
        glfwTerminate();
    }
}

void MacWindow::OnUpdate() {
    glfwPollEvents();
    m_Context->SwapBuffers();
}

void MacWindow::SetVSync(bool enabled) {
    if (m_Context) {
        m_Context->SetVSync(enabled);
    }

    m_Data.VSync = enabled;
}

bool MacWindow::IsVSync() const { return m_Data.VSync; }

}  // namespace TitaniumRenderer
