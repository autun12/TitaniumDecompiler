#pragma once

#include <stdint.h>

#include <array>
#include <string_view>

#include "Core/Disassembly/IDisassembler.h"

namespace TitaniumDecompiler {
enum class JVMOperandEncoding : uint8_t {
    None,
    S8,
    S16,
    U8,
    U16,
    ConstPoolU8,
    ConstPoolU16,
    BranchS16,
    BranchS32,
    TableSwitch,
    LookupSwitch,
    Wide,
    IInc,
};

struct JVMOpcodeInfo {
    std::string_view Mnemonic;

    uint8_t Length;

    JVMOperandEncoding OperandEncoding;
    ControlFlowType ControlFlow = ControlFlowType::None;
};

class JVMInstructionSet {
public:
    static const JVMOpcodeInfo& Get(uint8_t opcode) {
        return m_Opcodes[opcode];
    };

private:
    static const std::array<JVMOpcodeInfo, 256> m_Opcodes;
};
}  // namespace TitaniumDecompiler
