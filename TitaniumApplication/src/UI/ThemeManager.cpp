#include "ThemeManager.h"

#include <yaml-cpp/yaml.h>

#include <exception>
#include <sstream>

#include "TitaniumLogger/Logger/Log.h"

namespace TitaniumRenderer {
ThemeManager& ThemeManager::Get() {
    static ThemeManager instance;
    return instance;
}

bool ThemeManager::LoadTheme(const std::filesystem::path& path) {
    try {
        YAML::Node config = YAML::LoadFile(path.string());
        if (config["metadata"] && config["metadata"]["name"]) {
            m_CurrentThemeName = config["metadata"]["name"].as<std::string>();
        } else {
            m_CurrentThemeName = path.stem().string();
        }

        if (config["imgui_style"]) {
            auto styleNode = config["imgui_style"];

            if (styleNode["WindowRounding"]) m_Style.WindowRounding = styleNode["WindowRounding"].as<float>();
            if (styleNode["FrameRounding"]) m_Style.FrameRounding = styleNode["FrameRounding"].as<float>();
            if (styleNode["WindowBorderSize"]) m_Style.WindowBorderSize = styleNode["WindowBorderSize"].as<float>();
            if (styleNode["FrameBorderSize"]) m_Style.FrameBorderSize = styleNode["FrameBorderSize"].as<float>();

            if (styleNode["WindowPadding"]) {
                m_Style.WindowPadding = ImVec2(styleNode["WindowPadding"][0].as<float>(), styleNode["WindowPadding"][1].as<float>());
            }

            if (styleNode["FramePadding"]) {
                m_Style.FramePadding = ImVec2(styleNode["FramePadding"][0].as<float>(), styleNode["FramePadding"][1].as<float>());
            }

            TD_WARN("Window Padding {0}, {1}", m_Style.WindowPadding.x, m_Style.WindowPadding.y);
        }

        ApplyThemeToImGui();
        return true;
    } catch (std::exception) {
        return false;
    }
}

void ThemeManager::ApplyThemeToImGui() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Unpack layout geometry metrics
    style.WindowRounding = m_Style.WindowRounding;
    style.FrameRounding = m_Style.FrameRounding;
    style.WindowBorderSize = m_Style.WindowBorderSize;
    style.FrameBorderSize = m_Style.FrameBorderSize;
    style.WindowPadding = m_Style.WindowPadding;
    style.FramePadding = m_Style.FramePadding;

    // Direct translation mapping of colors to core global state context
    for (const auto& [imguiCol, color] : m_ImGuiColors) {
        style.Colors[imguiCol] = color;
    }
}

ImVec4 ThemeManager::ParseHexColor(const std::string& hex) {
    if (hex.empty() || hex[0] != '#') {
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    std::string cleanHex = hex.substr(1);
    unsigned int r = 0, g = 0, b = 0, a = 255;

    if (cleanHex.length() == 6) {
        std::sscanf(cleanHex.c_str(), "%02x%02x%02x", &r, &g, &b);
    } else if (cleanHex.length() == 8) {
        std::sscanf(cleanHex.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);
    }

    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}
}  // namespace TitaniumRenderer
