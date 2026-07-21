#pragma once

#include <TitaniumRenderer.h>

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../Core/ProjectContext.h"
#include "DisassemblyViewerPanel.h"
#include "IPanel.h"

namespace TitaniumRenderer {
class FunctionListingPanel : public IPanel {
public:
    FunctionListingPanel(bool defaultOpen = true)
        : IPanel("Functions", defaultOpen) {}

    virtual void OnImGuiRender() override;
    void SetClassesMap(
        const std::multimap<std::string, std::string>& classesMap);
    void DrawFunctionNames(const std::string& functionNames);
    virtual void OnProjectLoaded(const ProjectContext& context) override;

public:
    const std::string& GetSelectedFunction() { return m_SelectedFunction; }

    void SetDisassemblyViewerPanel(DisassemblyViewerPanel* viewerPanel) {
        m_DisassemblyViewerPanel = viewerPanel;
    }

private:
    void PopulateFunctionLineMapping();

private:
    std::string m_ClassName;
    std::multimap<std::string, std::string> m_ClassMap;
    std::string m_SelectedFunction;
    std::string m_LastSelectedFunction;
    DisassemblyViewerPanel* m_DisassemblyViewerPanel;
    std::multimap<std::string, int>
        m_FunctionLineMapping;  // Mapping of function names to line numbers
};
}  // namespace TitaniumRenderer
