#pragma once

#include <imgui.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace TitaniumRenderer {
struct ThemeStyle {
    float WindowRounding = 0.0f;
    float FrameRounding = 0.0f;
    float WindowBorderSize = 1.0f;
    float FrameBorderSize = 0.0f;
    ImVec2 FramePadding{8.0f, 8.0f};
    ImVec2 WindowPadding{8.0f, 8.0f};
};

class ScopedTheme {
public:
    ScopedTheme() = default;

    // Non-copyable
    ScopedTheme(const ScopedTheme&) = delete;
    ScopedTheme& operator=(const ScopedTheme&) = delete;

    ~ScopedTheme() {
        if (m_ColorCount > 0) ImGui::PopStyleColor(m_ColorCount);
        if (m_VarCount > 0) ImGui::PopStyleVar(m_VarCount);
    }

    void PushColor(ImGuiCol idx, const ImVec4& color) {
        ImGui::PushStyleColor(idx, color);
        m_ColorCount++;
    }

    void PushVar(ImGuiStyleVar idx, float val) {
        ImGui::PushStyleVar(idx, val);
        m_VarCount++;
    }

    void PushVar(ImGuiStyleVar idx, const ImVec2& val) {
        ImGui::PushStyleVar(idx, val);
        m_VarCount++;
    }

private:
    int m_ColorCount = 0;
    int m_VarCount = 0;
};

class ThemeManager {
public:
    static ThemeManager& Get();

    ThemeManager(const ThemeManager&) = default;
    ThemeManager& operator=(const ThemeManager&) = default;

    bool LoadTheme(const std::filesystem::path& path);
    const std::string& GetCurrentThemeName() const { return m_CurrentThemeName; }
    const ThemeStyle& GetStyleMetrics() const { return m_Style; }

private:
    ThemeManager() = default;
    ~ThemeManager() = default;

    ImVec4 ParseHexColor(const std::string& hex);
    void ApplyThemeToImGui();

private:
    std::string m_CurrentThemeName;
    ThemeStyle m_Style;

    std::unordered_map<int, ImVec4> m_ImGuiColors;
};
}  // namespace TitaniumRenderer
