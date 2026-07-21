#pragma once

#include <memory>
#include <string>
#include <vector>

#include "IWidget.h"

namespace TitaniumRenderer {

class WidgetManager {
public:
    WidgetManager() = default;
    ~WidgetManager() = default;

    // Add a widget to the manager and take ownership of its lifetime
    template <typename T, typename... Args>
    T* RegisterWidget(Args&&... args) {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = widget.get();
        m_Widgets.push_back(std::move(widget));
        return ptr;
    }

    // Retrieve a specific widget instance by its compiled class type
    template <typename T>
    T* GetWidget() {
        for (auto& widget : m_Widgets) {
            if (T* targeted = dynamic_cast<T*>(widget.get())) {
                return targeted;
            }
        }
        return nullptr;
    }

    // Sequentially render every registered active widget
    void OnImGuiRender();

    const std::vector<std::unique_ptr<IWidget>>& GetWidgets() const {
        return m_Widgets;
    }

private:
    std::vector<std::unique_ptr<IWidget>> m_Widgets;
};

}  // namespace TitaniumRenderer
