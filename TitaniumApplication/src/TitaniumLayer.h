#pragma once

#include <TitaniumRenderer.h>
#include "Panels/HexViewerPanel.h"

namespace TitaniumRenderer {
class TitaniumLayer : public Layer {
public:
    TitaniumLayer();
    virtual ~TitaniumLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Event& e) override;
private:
    bool OnKeyPressed(KeyPressedEvent& e);
private:
    
    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

    // Panels
    HexViewerPanel m_HexViewerPanel;
};
}