#pragma once

#include <TitaniumRenderer.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "DisassemblyViewerPanel.h"

namespace TitaniumRenderer {
class FunctionListingPanel {
public:
    FunctionListingPanel() = default;

    void OnImGuiRender(bool* open);
    void SetClassesMap(const std::multimap<std::string, std::string>& classesMap);
    void DrawFunctionNames(const std::string& functionNames);

public:
    const std::string& GetSelectedFunction() { return m_SelectedFunction; }
    void SetDisassemblyViewerPanel(DisassemblyViewerPanel* viewerPanel) { m_DisassemblyViewerPanel = viewerPanel; }

private:
    void PopulateFunctionLineMapping();

private:
    std::string m_ClassName;
    std::multimap<std::string, std::string> m_ClassMap;
    std::string m_SelectedFunction;
    std::string m_LastSelectedFunction;
    DisassemblyViewerPanel* m_DisassemblyViewerPanel;
    std::multimap<std::string, int> m_FunctionLineMapping;  // Mapping of function names to line numbers
};
}  // namespace TitaniumRenderer
