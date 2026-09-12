#pragma once

#include <string>
#include <vector>

namespace TitaniumDecompiler {
enum class OperandType { Register, Immediate, Memory, BranchTarget, Indirect };

enum class ControlFlowType {
    None,
    ConditionalBranch,
    UnconditionalBranch,
    IndirectBranch,
    Call,
    Return,
    Switch,
    Throw
};

struct DecodedOperand {
    OperandType type = OperandType::Immediate;
    uint64_t rawValue;
};

struct DecodedInsn {
    uint64_t address = 0;
    uint32_t length = 0;
    std::string mnemonic;
    std::vector<DecodedOperand> operands;

    ControlFlowType controlFlow = ControlFlowType::None;
};

}  // namespace TitaniumDecompiler
