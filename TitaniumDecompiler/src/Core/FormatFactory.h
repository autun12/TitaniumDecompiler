#pragma once

#include <filesystem>
#include <memory>

#include "IFormatParser.h"
namespace TitaniumDecompiler {
class FormatFactory {
public:
    static std::unique_ptr<IFormatParser> CreateParserForFile(
        const std::filesystem::path& path);
};
}  // namespace TitaniumDecompiler
