#include "FunctionListingPanel.h"

#include <typeinfo>
#include <vector>

#include "TitaniumLogger/Logger/Log.h"
#include "imgui.h"

namespace TitaniumRenderer {

void FunctionListingPanel::OnImGuiRender() {
    if (!m_IsOpen) return;

    if (ImGui::Begin(GetName().c_str(), GetOpenPtr())) {
        if (!m_ClassMap.empty()) {
            ImGuiTreeNodeFlags flag =
                ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;

            std::string currentClassName;

            for (auto it = m_ClassMap.begin(); it != m_ClassMap.end();) {
                const std::string& className = it->first;

                if (className != currentClassName) {
                    currentClassName = className;
                    bool opened = ImGui::TreeNodeEx(className.c_str(), flag);
                    if (opened) {
                        if (ImGui::IsItemClicked()) {
                        }
                        auto range = m_ClassMap.equal_range(className);
                        for (auto funcIt = range.first; funcIt != range.second;
                             ++funcIt) {
                            DrawFunctionNames(funcIt->second);
                        }

                        ImGui::TreePop();
                    }
                }

                it = m_ClassMap.upper_bound(className);
            }
        }
    }

    ImGui::End();
}

void FunctionListingPanel::SetClassesMap(
    const std::multimap<std::string, std::string>& classesMap) {
    m_ClassMap = classesMap;
}

void FunctionListingPanel::DrawFunctionNames(const std::string& functionNames) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
                               ImGuiTreeNodeFlags_Leaf |
                               ImGuiTreeNodeFlags_FramePadding;
    bool opened = ImGui::TreeNodeEx((void*)typeid(functionNames).hash_code(),
                                    flags, functionNames.c_str());
    if (ImGui::IsItemClicked()) {
        m_SelectedFunction = functionNames;
        int lineNum = 0;
        if (m_SelectedFunction != m_LastSelectedFunction) {
            m_LastSelectedFunction = m_SelectedFunction;
        }

        auto range = m_FunctionLineMapping.equal_range(m_SelectedFunction);
        for (auto it = range.first; it != range.second; ++it) {
            lineNum = it->second;
        }

        // m_DisassemblyViewerPanel->SetCursorPositionToFunction(lineNum);
    }

    if (opened) {
        ImGui::TreePop();
    }
}

void FunctionListingPanel::PopulateFunctionLineMapping() {
    m_FunctionLineMapping.clear();

    std::string disassemblyText;
    if (m_DisassemblyViewerPanel != nullptr) {
        std::string disassemblyText =
            m_DisassemblyViewerPanel->GetDisassemblyText();
        std::istringstream stream(disassemblyText);
        std::string line;
        int lineNumber = 0;

        // std::unordered_set<std::string> addedFunctionNames;

        while (std::getline(stream, line, '\n')) {
            for (const auto& it : m_ClassMap) {
                if (line.find(it.second) != std::string::npos) {
                    m_FunctionLineMapping.emplace(it.second, lineNumber);
                    // if (addedFunctionNames.find(it.second) ==
                    // addedFunctionNames.end()) {
                    //     m_FunctionLineMapping[it.second] = lineNumber;  //
                    //     Map function name to line number
                    //     addedFunctionNames.insert(it.second);
                    // }
                }
            }
            lineNumber++;
        }
    } else {
        TD_ERROR("Error: m_DisassemblyViewerPanel is null!");
    }
}

void FunctionListingPanel::OnProjectLoaded(
    const TitaniumRenderer::ProjectContext& context) {
    m_ClassMap = context.GetClassesMap();
}
}  // namespace TitaniumRenderer
