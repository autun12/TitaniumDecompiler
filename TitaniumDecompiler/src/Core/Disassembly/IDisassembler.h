#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "DecodedInsn.h"

namespace TitaniumDecompiler {
class IDisassembler {
public:
    virtual ~IDisassembler() = default;
    virtual bool DecodeInstruction(uint64_t address,
                                   std::span<const uint8_t> bytes,
                                   DecodedInsn& outInsn) = 0;
    virtual std::vector<DecodedInsn> DisassembleBlock(
        uint64_t startAddress, std::span<const uint8_t> bytes) = 0;
    virtual std::string_view GetArchitectureName() const = 0;
};
}  // namespace TitaniumDecompiler
