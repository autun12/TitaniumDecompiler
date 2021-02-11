#pragma once

#include "TitaniumRenderer/Core/Layer.h"

#include "TitaniumRenderer/Events/ApplicationEvent.h"
#include "TitaniumRenderer/Events/KeyEvent.h"
#include "TitaniumRenderer/Events/MouseEvent.h"

namespace TitaniumRenderer {

class ImGuiLayer : public Layer {
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& e) override;

    void Begin();
    void End();

    void BlockEvents(bool block) { m_BlockEvents = block; }

    void SetDarkThemeColors();

private:
    bool m_BlockEvents = true;
    float m_Time = 0.0f;
};

}  // namespace TitaniumRenderer