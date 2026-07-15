#pragma once

#include "TitaniumDecompiler/Disassembler/CFG.h"

namespace TitaniumDecompiler {
enum ExpressionType : uint8_t { ANNITATION, ARRAY, ASSERT, ASSIGNMENT, CONST, EXIT, FIELD, FUNCTION, IF, INVOCATION, MONITOR, NEW, SWITCH, VAR };

struct Expr {
public:
    Expr() = default;
    ~Expr() = default;

public:
    ExpressionType m_Type;
    uint32_t m_ID = 0;
};
}  // namespace TitaniumDecompiler
