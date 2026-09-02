#include <memory>
#include <vector>

#include "../../Core/ProjectContext.h"
#include "IPanel.h"

namespace TitaniumRenderer {

class PanelManager {
public:
    PanelManager() = default;

    // Add any panel derived from the base interface
    template <typename T, typename... Args>
    std::shared_ptr<T> AddPanel(Args&&... args) {
        auto panel = std::make_shared<T>(std::forward<Args>(args)...);
        m_Panels.push_back(panel);
        return panel;
    }

    // Loop through and render every active viewport automatically
    void OnImGuiRender() {
        for (auto& panel : m_Panels) {
            if (panel->IsOpen()) {
                panel->OnImGuiRender();
            }
        }
    }

    const std::vector<std::shared_ptr<IPanel>>& GetPanels() const {
        return m_Panels;
    }

    void BroadcastProjectLoad(const ProjectContext& context) {
        for (auto& panel : m_Panels) {
            panel->OnProjectLoaded(context);
        }
    }

private:
    std::vector<std::shared_ptr<IPanel>> m_Panels;
};

}  // namespace TitaniumRenderer
