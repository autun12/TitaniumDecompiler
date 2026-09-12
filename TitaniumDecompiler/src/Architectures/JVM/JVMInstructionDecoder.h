#pragma once

#include <span>

#include "Core/Disassembly/IDisassembler.h"
#include "JVMInstructionSet.h"
namespace TitaniumDecompiler {

class JVMInstructionDecoder {
public:
    static bool DecodeInstruction(uint64_t address,
                                  std::span<const uint8_t> bytes,
                                  DecodedInsn& outInsn);

private:
    static bool DecodeOperands(JVMOperandEncoding encoding, uint64_t address,
                               std::span<const uint8_t> bytes,
                               DecodedInsn& outInsn);

    static bool DecodeLookupSwitch(uint64_t address,
                                   std::span<const uint8_t> bytes,
                                   DecodedInsn& outInsn);

    static bool DecodeTableSwitch(uint64_t address,
                                  std::span<const uint8_t> bytes,
                                  DecodedInsn& outInsn);

    static bool DecodeWide(uint64_t address, std::span<const uint8_t> bytes,
                           DecodedInsn& outInsn);
};
}  // namespace TitaniumDecompiler
