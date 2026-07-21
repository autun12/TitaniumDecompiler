#pragma once

#include <imgui.h>

#include "TitaniumRenderer/UI/IWidget.h"
#include "imfilebrowser.h"

namespace TitaniumRenderer {

class SettingsWidget : public IWidget {
public:
    SettingsWidget() : IWidget("Preferences", false) {}

    virtual void OnImGuiRender() override;
    void LoadTheme();

private:
    ImGui::FileBrowser m_FileBrowser;
};
}  // namespace TitaniumRenderer
