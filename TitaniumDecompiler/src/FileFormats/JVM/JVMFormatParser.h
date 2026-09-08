#pragma once

#include <filesystem>
#include <span>

#include "ClassFileParser.h"
#include "Core/IFormatParser.h"

namespace TitaniumDecompiler {
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
    ClassFile m_ClassFile;
    ClassFileParser m_Parser;
    bool m_IsLoaded = false;
};
}  // namespace TitaniumDecompiler
