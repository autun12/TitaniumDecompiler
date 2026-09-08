#include "TitaniumRenderer/Core/Window.h"

#include "TitaniumRenderer/Core/Base.h"

#if defined(TD_PLATFORM_MACOS)
#include "Platform/Mac/MacWindow.h"
#else
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace TitaniumRenderer {
Scope<Window> Window::Create(const WindowProps& props) {
#if defined(TD_PLATFORM_MACOS)
    return CreateScope<MacWindow>(props);
#else
    return CreateScope<WindowsWindow>(props);
#endif
}
}  // namespace TitaniumRenderer
