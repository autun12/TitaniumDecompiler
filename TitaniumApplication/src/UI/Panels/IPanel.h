#pragma once
#include <string>

#include "../../Core/ProjectContext.h"

namespace TitaniumRenderer {

class IPanel {
public:
    IPanel(const std::string& name, bool defaultOpen = true)
        : m_Name(name), m_IsOpen(defaultOpen) {}

    virtual ~IPanel() = default;

    // Core rendering hook executed every ImGui loop pass
    virtual void OnImGuiRender() = 0;

    // Getters and setters for structural lifecycle management
    const std::string& GetName() const { return m_Name; }
    bool IsOpen() const { return m_IsOpen; }
    void SetOpen(bool open) { m_IsOpen = open; }
    bool* GetOpenPtr() { return &m_IsOpen; }
    virtual void OnProjectLoaded(const ProjectContext& context) {}

protected:
    std::string m_Name;
    bool m_IsOpen;
};

}  // namespace TitaniumRenderer
