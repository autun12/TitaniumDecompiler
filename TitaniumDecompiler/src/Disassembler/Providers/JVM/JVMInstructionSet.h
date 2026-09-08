#pragma once

#include <stdint.h>

#include <array>
#include <string_view>

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
    Wide
};

struct JVMOpcodeInfo {
    std::string_view Mnemonic;
    uint8_t Length;
    JVMOperandEncoding OperandEncoding;
};

class JVMInstructionSet {
public:
    static const JVMOpcodeInfo& Get(uint8_t opcode);

private:
    static const std::array<JVMOpcodeInfo, 256> m_Opcodes;
};
}  // namespace TitaniumDecompiler
