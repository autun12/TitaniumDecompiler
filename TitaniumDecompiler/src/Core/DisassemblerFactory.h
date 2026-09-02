#pragma once

#include <filesystem>
#include <memory>

#include "Core/IFormatParser.h"
#include "Disassembler/IDisassembler.h"

namespace TitaniumDecompiler {
class DisassemblerFactory {
public:
    static std::unique_ptr<IDisassembler> CreateForParser(
        const IFormatParser& parser);
};
}  // namespace TitaniumDecompiler
