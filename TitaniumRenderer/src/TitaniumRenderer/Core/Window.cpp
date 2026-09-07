#include "TitaniumRenderer/Core/Window.h"

#include "TitaniumRenderer/Core/Base.h"

#if defined(TD_PLATFORM_WINDOWS) || defined(TD_PLATFORM_LINUX)
#include "Platform/Windows/WindowsWindow.h"
#elif defined(TD_PLATFORM_MACOS)
#include "Platform/Mac/MacWindow.h"
#endif

namespace TitaniumRenderer {
Scope<Window> Window::Create(const WindowProps& props) {
#if defined(TD_PLATFORM_WINDOWS) || defined(TD_PLATFORM_LINUX)
    return CreateScope<WindowsWindow>(props);
#elif defined(TD_PLATFORM_MACOS)
    return CreateScope<MacWindow>(props);
#else
    TD_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}
}  // namespace TitaniumRenderer
