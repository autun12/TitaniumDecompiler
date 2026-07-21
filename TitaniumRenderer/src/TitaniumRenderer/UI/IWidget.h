
#pragma once

#include <string>

namespace TitaniumRenderer {

class IWidget {
public:
    IWidget(const std::string& name, bool defaultOpen = false)
        : m_Name(name), m_IsOpen(defaultOpen) {}

    virtual ~IWidget() = default;

    // Force subclasses to implement their specific ImGui layout
    virtual void OnImGuiRender() = 0;

    // Utility controls for toggling visibility from menus
    void Open() { m_IsOpen = true; }
    void Close() { m_IsOpen = false; }
    void Toggle() { m_IsOpen = !m_IsOpen; }

    bool IsOpen() const { return m_IsOpen; }
    const std::string& GetName() const { return m_Name; }
    bool* GetOpenPtr() { return &m_IsOpen; }

protected:
    std::string m_Name;
    bool m_IsOpen;
};

}  // namespace TitaniumRenderer
