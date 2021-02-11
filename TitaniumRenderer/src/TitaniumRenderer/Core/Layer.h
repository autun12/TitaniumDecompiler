#pragma once

#include "TitaniumRenderer/Core/Base.h"
#include "TitaniumRenderer/Core/Timestep.h"
#include "TitaniumRenderer/Events/Event.h"

namespace TitaniumRenderer {

class Layer {
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};

}  // namespace TitaniumRenderer