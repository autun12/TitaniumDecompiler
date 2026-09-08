#pragma once

#include <filesystem>
#include <span>

#include "Types.h"

namespace TitaniumDecompiler {
class IFormatParser {
public:
    virtual ~IFormatParser() = default;
    virtual bool Load(const std::filesystem::path& path) = 0;
    virtual bool CanParse(std::span<const uint8_t> data) const = 0;

    virtual std::string_view GetFormatName() const = 0;
    virtual std::vector<Section> GetSections() const = 0;
    virtual std::vector<Symbol> GetSymbols() const = 0;
};
}  // namespace TitaniumDecompiler
