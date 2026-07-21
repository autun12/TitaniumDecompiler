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
        m_ImGuiColors.clear();
        m_TokenStyles.clear();
        m_Style = ThemeStyle();

        if (config["metadata"] && config["metadata"]["name"]) {
            m_CurrentThemeName = config["metadata"]["name"].as<std::string>();
        } else {
            m_CurrentThemeName = path.stem().string();
        }

        if (config["imgui_style"]) {
            auto styleNode = config["imgui_style"];

            if (styleNode["WindowRounding"])
                m_Style.WindowRounding =
                    styleNode["WindowRounding"].as<float>();
            if (styleNode["FrameRounding"])
                m_Style.FrameRounding = styleNode["FrameRounding"].as<float>();
            if (styleNode["WindowBorderSize"])
                m_Style.WindowBorderSize =
                    styleNode["WindowBorderSize"].as<float>();
            if (styleNode["FrameBorderSize"])
                m_Style.FrameBorderSize =
                    styleNode["FrameBorderSize"].as<float>();

            if (styleNode["WindowPadding"]) {
                m_Style.WindowPadding =
                    ImVec2(styleNode["WindowPadding"][0].as<float>(),
                           styleNode["WindowPadding"][1].as<float>());
            }

            if (styleNode["FramePadding"]) {
                m_Style.FramePadding =
                    ImVec2(styleNode["FramePadding"][0].as<float>(),
                           styleNode["FramePadding"][1].as<float>());
            }
            if (styleNode["ScrollbarSize"])
                m_Style.ScrollbarSize = styleNode["ScrollbarSize"].as<float>();
            if (styleNode["GrabRounding"])
                m_Style.GrabRounding = styleNode["GrabRounding"].as<float>();
            if (styleNode["TabRounding"])
                m_Style.TabRounding = styleNode["TabRounding"].as<float>();

            if (styleNode["colors"]) {
                auto colorsNode = styleNode["colors"];

                auto parseAndMap = [&](const char* yamlKey, ImGuiCol imguiCol) {
                    if (colorsNode[yamlKey]) {
                        m_ImGuiColors[imguiCol] = ParseHexColor(
                            colorsNode[yamlKey].as<std::string>());
                    }
                };

                // Base Window Panels
                parseAndMap("WindowBg", ImGuiCol_WindowBg);
                parseAndMap("ChildBg", ImGuiCol_ChildBg);
                parseAndMap("PopupBg", ImGuiCol_PopupBg);
                parseAndMap("Border", ImGuiCol_Border);

                // Window Header and Title Elements
                parseAndMap("TitleBg", ImGuiCol_TitleBg);
                parseAndMap("TitleBgActive", ImGuiCol_TitleBgActive);
                parseAndMap("TitleBgCollapsed", ImGuiCol_TitleBgCollapsed);

                // Core Window Widgets
                parseAndMap("Button", ImGuiCol_Button);
                parseAndMap("ButtonHovered", ImGuiCol_ButtonHovered);
                parseAndMap("ButtonActive", ImGuiCol_ButtonActive);
                parseAndMap("FrameBg", ImGuiCol_FrameBg);
                parseAndMap("FrameBgHovered", ImGuiCol_FrameBgHovered);
                parseAndMap("FrameBgActive", ImGuiCol_FrameBgActive);

                // Docking Architecture and Tab Elements (ImGui Docking branch)
                parseAndMap("Tab", ImGuiCol_Tab);
                parseAndMap("TabHovered", ImGuiCol_TabHovered);
                parseAndMap("TabActive", ImGuiCol_TabActive);
                parseAndMap("TabUnfocused", ImGuiCol_TabUnfocused);
                parseAndMap("TabUnfocusedActive", ImGuiCol_TabUnfocusedActive);
                parseAndMap("DockingPreview", ImGuiCol_DockingPreview);
                parseAndMap("DockingEmptyBg", ImGuiCol_DockingEmptyBg);

                // Panel Grips and Window Scrollbars
                parseAndMap("ResizeGrip", ImGuiCol_ResizeGrip);
                parseAndMap("ResizeGripHovered", ImGuiCol_ResizeGripHovered);
                parseAndMap("ResizeGripActive", ImGuiCol_ResizeGripActive);
                parseAndMap("ScrollbarBg", ImGuiCol_ScrollbarBg);
                parseAndMap("ScrollbarGrab", ImGuiCol_ScrollbarGrab);
                parseAndMap("ScrollbarGrabHovered",
                            ImGuiCol_ScrollbarGrabHovered);
                parseAndMap("ScrollbarGrabActive",
                            ImGuiCol_ScrollbarGrabActive);

                parseAndMap("Text", ImGuiCol_Text);
                parseAndMap("TextDisabled", ImGuiCol_TextDisabled);
                parseAndMap("TextSelectedBg", ImGuiCol_TextSelectedBg);
                parseAndMap("Button", ImGuiCol_Button);
                parseAndMap("ButtonHovered", ImGuiCol_ButtonHovered);
                parseAndMap("ButtonActive", ImGuiCol_ButtonActive);
            }
        }

        // 3. Parse Decompiler Syntax Highlighting Configurations
        if (config["decompiler_theme"]) {
            auto decompNode = config["decompiler_theme"];

            auto parseToken = [&](const char* yamlKey, TokenType type,
                                  bool defaultItalic = false) {
                if (decompNode[yamlKey]) {
                    DecompilerTokenStyle style;
                    style.Color =
                        ParseHexColor(decompNode[yamlKey].as<std::string>());
                    style.Italic = defaultItalic;
                    m_TokenStyles[type] = style;
                }
            };

            parseToken("Keyword", TokenType::Keyword);
            parseToken("Type", TokenType::Type);
            parseToken("Variable", TokenType::Variable);
            parseToken("Constant", TokenType::Constant);
            parseToken("Comment", TokenType::Comment,
                       true);  // Italic comment tokens by default
            parseToken("Address", TokenType::Address);
            parseToken("DefaultText", TokenType::DefaultText);
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

ImVec4 ThemeManager::GetImGuiColor(ImGuiCol val) const {
    auto it = m_ImGuiColors.find(val);
    if (it != m_ImGuiColors.end()) {
        return it->second;
    }
    // Safe context fallback directly to ImGui's built-in styles
    return ImGui::GetStyle().Colors[val];
}

const DecompilerTokenStyle& ThemeManager::GetTokenStyle(TokenType type) const {
    auto it = m_TokenStyles.find(type);
    if (it != m_TokenStyles.end()) {
        return it->second;
    }

    static DecompilerTokenStyle fallbackStyle;
    return fallbackStyle;
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

void ThemeManager::InitializeFonts(ImGuiIO& io) {
    float fontSize = 24.0f;

    m_BoldFont = io.Fonts->AddFontFromFileTTF(
        "TitaniumApplication/assets/fonts/jetbrainsmono/JetBrainsMono-Bold.ttf",
        fontSize);
    m_DefaultFont = io.Fonts->AddFontFromFileTTF(
        "TitaniumApplication/assets/fonts/jetbrainsmono/"
        "JetBrainsMono-Regular.ttf",
        fontSize);

    io.FontDefault = m_DefaultFont;
}
}  // namespace TitaniumRenderer
