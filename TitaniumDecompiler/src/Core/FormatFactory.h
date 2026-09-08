#pragma once

#include <filesystem>
#include <functional>
#include <memory>

#include "IFormatParser.h"
namespace TitaniumDecompiler {
class FormatFactory {
public:
    using ParserFactory = std::function<std::unique_ptr<IFormatParser>()>;

    static std::unique_ptr<IFormatParser> CreateParserForFile(
        const std::filesystem::path& path);

private:
    static const std::vector<ParserFactory>& GetParserFactories();
};
}  // namespace TitaniumDecompiler
