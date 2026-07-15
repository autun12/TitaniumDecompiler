#include "AST.h"

namespace TitaniumDecompiler {
Type getType(int ty) {
    switch (ty) {
        case 4:
            return Type::Boolean;  // int
        case 5:
            return Type::Char;  // int
        case 6:
            return Type::Float;  // float
        case 7:
            return Type::Double;  // double
        case 8:
            return Type::Byte;  // int
        case 9:
            return Type::Short;  // int
        case 10:
            return Type::Int;  // int
        case 11:
            return Type::Long;  // long
        default:
            throw std::runtime_error("unexpected NewArray type");
    }
}

std::optional<Type> parseSingleType(std::string::const_iterator& it, const std::string::const_iterator& end) {
    if (it == end) return std::nullopt;

    char c = *it++;
    switch (c) {
        case 'B':
            return Type(Type::Byte);
        case 'C':
            return Type(Type::Char);
        case 'D':
            return Type(Type::Double);
        case 'F':
            return Type(Type::Float);
        case 'I':
            return Type(Type::Int);
        case 'J':
            return Type(Type::Long);
        case 'S':
            return Type(Type::Short);
        case 'Z':
            return Type(Type::Boolean);
        case 'V':
            return Type(Type::Void);
        case 'L': {
            std::string name;
            while (it != end && *it != ';') {
                name.push_back(*it++);
            }
            if (it != end) ++it;  // Skip ';'
            return Type::ClassNameType(name);
        }
        case '[': {
            auto inner_type = parseSingleType(it, end);
            if (!inner_type) {
                throw std::runtime_error("Invalid array type");
            }
            return Type::ReferenceType(std::make_unique<Type>(std::move(*inner_type)));
        }
        default: {
            std::string name(1, c);
            name.append(it, end);
            return Type::ClassNameType(name);
        }
    }
}
}  // namespace TitaniumDecompiler
