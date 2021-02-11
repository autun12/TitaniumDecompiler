#include "tdpch.h"
#include "TitaniumRenderer/Core/Window.h"

#if defined(TD_PLATFORM_WINDOWS) || defined(TD_PLATFORM_LINUX)
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace TitaniumRenderer {
Scope<Window> Window::Create(const WindowProps& props) {
    #if defined(TD_PLATFORM_WINDOWS) || defined(TD_PLATFORM_LINUX)
        return CreateScope<WindowsWindow>(props);
    #else
        TD_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
}
}