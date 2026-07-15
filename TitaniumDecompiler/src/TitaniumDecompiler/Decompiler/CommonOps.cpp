#include "CommonOps.h"
namespace TitaniumDecompiler {
UnaryOpPosition Position(UnaryOp unary) {
    switch (unary) {
        case Neg:
            return UnaryOpPosition::Prefix;
        case ArrayLength:
            return UnaryOpPosition::Postfix;
        case PlusPlus:
            return UnaryOpPosition::Postfix;
        case MinusMinus:
            return UnaryOpPosition::Postfix;
        case Negate:
            return UnaryOpPosition::Prefix;
    }
}

bool IsPrefix(UnaryOp unary) { return Position(unary) == UnaryOpPosition::Prefix; }

std::string UnaryOpToString(UnaryOp op) {
    switch (op) {
        case Neg:
            return "-";
        case ArrayLength:
            return ".length";
        case PlusPlus:
            return "++;";
        case MinusMinus:
            return "--;";
        case Negate:
            return "!";
    }
}

std::string BinOpToString(BinaryOp op) {
    switch (op) {
        case Add:
            return "+";
        case Sub:
            return "-";
        case Mul:
            return "*";
        case Div:
            return "/";
        case Rem:
            return "%";
        case Shl:
            return "<<";
        case Shr:
            return ">>";
        case UShr:
            return ">>>";
        case And:
            return "&";
        case Or:
            return "|";
        case LogicalAnd:
            return "&&";
        case LogicalOr:
            return "||";
        case Xor:
            return "^";
        case InstanceOf:
            return "instanceof";
        case Equal:
            return "==";
        case NotEqual:
            return "!=";
        case GreaterThan:
            return ">";
        case LessThan:
            return "<";
        case GreaterEqualThan:
            return ">=";
        case LessEqualThan:
            return "<=";
        default:
            return "Unknown";
    }
}
}  // namespace TitaniumDecompiler
