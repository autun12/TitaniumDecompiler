#pragma once

#include <string>

namespace TitaniumDecompiler {
enum UnaryOp { Neg, ArrayLength, PlusPlus, MinusMinus, Negate };

enum UnaryOpPosition { Prefix, Postfix };

enum BinaryOp {
    Add,
    Sub,
    Mul,
    Div,
    Rem,
    Shl,
    /// arithmetic shift right
    Shr,
    /// logical shift right
    UShr,
    /// bitwise and
    And,
    Or,
    LogicalAnd,
    LogicalOr,
    Xor,
    InstanceOf,
    Equal,
    NotEqual,
    GreaterThan,
    LessThan,
    GreaterEqualThan,
    LessEqualThan,
};

UnaryOpPosition Position(UnaryOp unary);
bool IsPrefix(UnaryOp unary);
std::string UnaryOpToString(UnaryOp op);
std::string BinOpToString(BinaryOp op);
}  // namespace TitaniumDecompiler
