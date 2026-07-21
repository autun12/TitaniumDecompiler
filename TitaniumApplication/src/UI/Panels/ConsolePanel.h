#pragma once

#include <TitaniumRenderer/Core/Base.h>

#include <string>
#include <vector>

#include "IPanel.h"

namespace TitaniumRenderer {
class ConsolePanel : public IPanel {
public:
    enum class Level : int8_t {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Critical = 5,
        Off = 6
    };

    struct Message {
        std::string LoggerName;
        std::string MessageString;
        Level MessageLevel;
    };

public:
    ConsolePanel(bool defaultOpen = true) : IPanel("Console", defaultOpen) {}
    virtual void OnImGuiRender() override;
    void AddMessage(const Ref<Message>& message);

private:
    bool m_ShowInfo = true;
    bool m_ShowWarning = true;
    bool m_ShowError = true;
    bool m_ShowCritical = true;
    bool m_ShowTrace = true;

    std::vector<Ref<Message>> m_Messages;
};
}  // namespace TitaniumRenderer
