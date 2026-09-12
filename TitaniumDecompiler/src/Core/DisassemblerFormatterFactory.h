#pragma once

#include <filesystem>
#include <memory>

#include "Core/IFormatParser.h"
#include "Formatting/Disassembly/IDisassemblerFormatter.h"

namespace TitaniumDecompiler {
class DisassemblerFormatterFactory {
public:
    static std::unique_ptr<IDisassemblyFormatter> CreateForParser(
        const IFormatParser& parser);
};
}  // namespace TitaniumDecompiler
