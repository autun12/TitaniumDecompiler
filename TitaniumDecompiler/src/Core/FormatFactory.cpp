#include "FormatFactory.h"

#include <array>
#include <memory>

#include "FileFormats/JVM/JVMFormatParser.h"

namespace TitaniumDecompiler {

const std::vector<FormatFactory::ParserFactory>&
FormatFactory::GetParserFactories() {
    static const std::vector<ParserFactory> factories = {
        [] { return std::make_unique<JVMFormatParser>(); },
    };

    return factories;
}

std::unique_ptr<IFormatParser> FormatFactory::CreateParserForFile(
    const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) return nullptr;

    std::array<uint8_t, 16> probe;

    file.read(reinterpret_cast<char*>(probe.data()), probe.size());

    const auto bytesRead = file.gcount();

    const std::span<const uint8_t> data(probe.data(),
                                        static_cast<size_t>(bytesRead));

    for (const auto& factory : GetParserFactories()) {
        auto parser = factory();

        if (parser->CanParse(data)) {
            return parser;
        }
    }

    return nullptr;
}
}  // namespace TitaniumDecompiler
