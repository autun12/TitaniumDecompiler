#include "FormatFactory.h"

#include <memory>

#include "FileFormats/JVM/JVMFormatParser.h"

namespace TitaniumDecompiler {
std::unique_ptr<IFormatParser> FormatFactory::CreateParserForFile(
    const std::filesystem::path &path) {
    auto ext = path.extension().string();
    if (ext == ".class") {
        return std::make_unique<JVMFormatParser>();
    }

    return nullptr;
}
}  // namespace TitaniumDecompiler
