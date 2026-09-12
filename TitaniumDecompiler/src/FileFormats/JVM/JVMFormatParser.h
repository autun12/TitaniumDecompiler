#pragma once

#include <filesystem>
#include <span>
#include <string>

#include "ClassFileParser.h"
#include "Core/IFormatParser.h"

namespace TitaniumDecompiler {

struct JVMMethodView {
    std::string name;
    std::string descriptor;
    uint16_t accessFlags = 0;
    std::vector<uint8_t> code;
    uint16_t maxStack = 0;
    uint16_t maxLocals = 0;
};

class JVMFormatParser : public IFormatParser {
public:
    JVMFormatParser() = default;

    bool Load(const std::filesystem::path& path) override;
    bool CanParse(const std::span<const uint8_t> bytes) const override;

    std::string_view GetFormatName() const override { return "JVM ClassFile"; }
    std::vector<Section> GetSections() const override;
    std::vector<Symbol> GetSymbols() const override;

    const ClassFile& GetClassFile() const { return m_ClassFile; }

private:
    void BuildMethodViews();

private:
    ClassFile m_ClassFile;
    ClassFileParser m_Parser;
    bool m_IsLoaded = false;
    std::vector<JVMMethodView> m_Methods;
};
}  // namespace TitaniumDecompiler
