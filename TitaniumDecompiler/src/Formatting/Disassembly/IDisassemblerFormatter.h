#pragma once
#include <string>
#include <vector>

#include "Core/Disassembly/DecodedInsn.h"
namespace TitaniumDecompiler {
class IDisassemblyFormatter {
public:
    virtual ~IDisassemblyFormatter() = default;
    virtual std::string FormatInstruction(const DecodedInsn& insn) = 0;
    virtual std::string FormatBlock(const std::vector<DecodedInsn>& block) = 0;
};
}  // namespace TitaniumDecompiler
