#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace TitaniumDecompiler {
enum class OperandType { Register, Immediate, Memory, BranchTarget, Indirect };

struct DecodedOperand {
    OperandType type = OperandType::Immediate;
    uint64_t rawValue;
};

struct DecodedInsn {
    uint64_t address = 0;
    uint32_t length = 0;
    std::string mnemonic;
    std::vector<DecodedOperand> operands;

    bool isBranch = false;
    bool isCall = false;
    bool isReturn = false;
};

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
