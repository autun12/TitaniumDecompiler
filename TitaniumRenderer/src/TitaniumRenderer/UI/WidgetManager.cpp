#include "WidgetManager.h"

namespace TitaniumRenderer {

void WidgetManager::OnImGuiRender() {
    for (auto& widget : m_Widgets) {
        widget->OnImGuiRender();
    }
}
}  // namespace TitaniumRenderer
