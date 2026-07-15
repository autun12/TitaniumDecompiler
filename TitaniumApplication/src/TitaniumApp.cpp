#include <TitaniumRenderer.h>
#include <TitaniumRenderer/Core/EntryPoint.h>

#include "TitaniumApplication/src/UI/ThemeManager.h"
#include "TitaniumLayer.h"

namespace TitaniumRenderer {
class TitaniumDecompilerApplication : public Application {
public:
    TitaniumDecompilerApplication() : Application("Titanium Decompiler") {
        bool themeLoaded = ThemeManager::Get().LoadTheme("TitaniumApplication/assets/themes/dark.yaml");
        if (!themeLoaded) {
            TD_WARN("Failed to load theme asset. Falling back to default layout.");
        }
        PushLayer(new TitaniumLayer());
    }

    ~TitaniumDecompilerApplication() {}
};

Application* CreateApplication() { return new TitaniumDecompilerApplication(); }

}  // namespace TitaniumRenderer
