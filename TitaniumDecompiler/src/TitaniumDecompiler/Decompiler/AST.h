#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>

#include "CommonOps.h"
#include "Utils/Utils.h"

namespace TitaniumDecompiler {

// Overloaded visitor helper
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

class InvokeDynamicArgs {
private:
    std::vector<std::string> inner;

public:
    enum class ArgType { Stack, Pool };

    struct ParsedArg {
        ArgType type;
        std::string value;
    };

    explicit InvokeDynamicArgs(const std::string& s) {
        size_t last = 0;
        size_t index;
        while ((index = s.find('\u0001', last)) != std::string::npos) {
            if (last != index) {
                inner.push_back(s.substr(last, index - last));
            }
            inner.push_back("\u0001");
            last = index + 1;
        }
        if (last < s.size()) {
            inner.push_back(s.substr(last));
        }
    }

    std::optional<ParsedArg> next() {
        if (inner.empty()) return std::nullopt;
        std::string s = inner.back();
        inner.pop_back();
        if (s == "\u0001") {
            return ParsedArg{ArgType::Stack, ""};
        } else {
            return ParsedArg{ArgType::Pool, s};
        }
    }
};

// Forward declarations
struct Type {
    enum Kind { Byte, Char, Short, Boolean, Void, ClassName, Int, Float, Double, Long, Reference } kind;
    std::string class_name;
    std::unique_ptr<Type> ref_type;

    // Constructor
    Type(Kind kind, const std::string& class_name = "", std::unique_ptr<Type> ref_type = nullptr)
        : kind(kind), class_name(class_name), ref_type(std::move(ref_type)) {}

    // Copy Constructor
    Type(const Type& other) : kind(other.kind), class_name(other.class_name), ref_type(other.ref_type ? std::make_unique<Type>(*other.ref_type) : nullptr) {}

    // Move Constructor (Default)
    Type(Type&& other) noexcept = default;

    // Copy Assignment Operator
    Type& operator=(const Type& other) {
        if (this == &other) return *this;
        kind = other.kind;
        class_name = other.class_name;
        ref_type = other.ref_type ? std::make_unique<Type>(*other.ref_type) : nullptr;
        return *this;
    }

    // Move Assignment Operator (Default)
    Type& operator=(Type&& other) noexcept = default;

    static Type ClassNameType(const std::string& name) { return {ClassName, name, nullptr}; }

    static Type ReferenceType(std::unique_ptr<Type> ref) { return {Reference, "", std::move(ref)}; }

    static Type BasicType(Kind k, std::string className) { return {k, className, nullptr}; }

    std::string toString() const {
        switch (kind) {
            case Byte:
                return "byte";
            case Char:
                return "char";
            case Short:
                return "short";
            case Boolean:
                return "boolean";
            case Void:
                return "void";
            case ClassName: {
                if (class_name.rfind("java/lang/", 0) == 0) {
                    return class_name.substr(10);  // Strip "java/lang/"
                }
                return class_name;
            }
            case Int:
                return "int";
            case Float:
                return "float";
            case Double:
                return "double";
            case Long:
                return "long";
            case Reference:
                return ref_type->toString() + "[]";
            default:
                return "Unknown";
        }
    }

    bool operator==(const Type& other) const {
        if (kind != other.kind) {
            return false;
        }
        if (class_name != other.class_name) {
            return false;
        }
        if ((ref_type == nullptr) != (other.ref_type == nullptr)) {
            return false;
        }
        if (ref_type && other.ref_type) {
            return *ref_type == *other.ref_type;
        }
        return true;
    }
};

Type getType(int ty);

struct FieldRefStack {
    std::string m_Class;
    std::string m_Name;
    Type m_type;
};

// Function to parse a single type from a string
std::optional<Type> parseSingleType(std::string::const_iterator& it, const std::string::const_iterator& end);

// Enum-like variant for StackEntry
struct StackEntry {
    struct Null {};

    struct Array {
        Type type;
        std::shared_ptr<StackEntry> length;
        std::vector<StackEntry> elements;
    };

    struct Index {
        std::shared_ptr<StackEntry> array;
        std::shared_ptr<StackEntry> index;
        Type array_type;
    };

    struct Cast {
        Type target_type;
        std::shared_ptr<StackEntry> value;
    };

    struct UnaryOperation {
        UnaryOp op;
        std::shared_ptr<StackEntry> value;
    };

    struct BinaryOperation {
        std::shared_ptr<StackEntry> lhs;
        BinaryOp op;
        std::shared_ptr<StackEntry> rhs;
    };

    struct Identifier {
        std::string name;
        Type type;
    };

    struct Function {
        std::string name;
        std::vector<StackEntry> arguments;
        Type return_type;
    };

    struct Field {
        std::shared_ptr<StackEntry> obj;
        std::string field_name;
        Type field_type;
    };

    struct Class {
        std::string m_ClassName;
    };

    struct New {
        std::string m_ClassName;
    };

    struct String {
        std::string value;
    };

    using Variant = std::variant<Null, int32_t, float, double, int64_t, Class, New, Array, String, Index, Cast, UnaryOperation, BinaryOperation, Identifier,
                                 Function, Field>;

    Variant value;

    StackEntry() : value(Null{}) {}

    template <typename T, typename = std::enable_if_t<std::is_constructible_v<Variant, std::decay_t<T>>>>
    explicit StackEntry(T&& val) : value(std::forward<T>(val)) {}

    // Determine the type of the StackEntry
    Type ty() const {
        return std::visit(
            overloaded{[](const Null&) { return Type::ClassNameType("null"); }, [](int32_t) { return Type::BasicType(Type::Int, "int"); },
                       [](float) { return Type::BasicType(Type::Float, "float"); }, [](double) { return Type::BasicType(Type::Double, "double"); },
                       [](int64_t) { return Type::BasicType(Type::Long, "long"); },
                       [](const Array& arr) { return Type::ReferenceType(std::make_unique<Type>(arr.type)); },
                       [](const New& newStack) { return Type::ClassNameType(newStack.m_ClassName); },
                       [](const Class& classStack) { return Type::ClassNameType(classStack.m_ClassName); },
                       [](const String&) { return Type::ClassNameType("String"); }, [](const Index& idx) { return idx.array_type; },
                       [](const Cast& cast) { return cast.target_type; }, [](const UnaryOperation& unary) { return unary.value->ty(); },
                       [](const BinaryOperation& binary) { return binary.lhs->ty(); }, [](const Identifier& ident) { return ident.type; },
                       [](const Function& func) { return func.return_type; }, [](const Field& field) { return field.field_type; }},
            value);
    }

    // Formatting for display
    friend std::ostream& operator<<(std::ostream& os, const StackEntry& entry) {
        std::visit(
            overloaded{[&os](const Null&) { os << "null"; }, [&os](int32_t val) { os << val; }, [&os](float val) { os << val << "f"; },
                       [&os](double val) { os << val << "d"; }, [&os](int64_t val) { os << val << "l"; },
                       [&os](const Array& arr) {
                           os << "{ ";
                           for (const auto& elem : arr.elements) os << elem << " ";
                           os << "}";
                       },
                       [&os](const New& newStack) { os << "new " << newStack.m_ClassName; }, [&os](const Class& classStack) { os << classStack.m_ClassName; },
                       [&os](const String& str) { os << "\"" << str.value << "\""; }, [&os](const Index& idx) { os << *idx.array << "[" << *idx.index << "]"; },
                       [&os](const Cast& cast) { os << "(" << cast.target_type.toString() << ") " << *cast.value; },
                       [&os](const UnaryOperation& unary) {
                           if (IsPrefix(unary.op))
                               os << UnaryOpToString(unary.op) << *unary.value;
                           else
                               os << *unary.value << UnaryOpToString(unary.op);
                       },
                       [&os](const BinaryOperation& binary) { os << "(" << *binary.lhs << " " << BinOpToString(binary.op) << " " << *binary.rhs << ")"; },
                       [&os](const Identifier& ident) { os << ident.name; },
                       [&os](const Function& func) {
                           os << func.name << "(";
                           for (const auto& arg : func.arguments) os << arg << ", ";
                           os << ")";
                       },
                       [&os](const Field& field) { os << field.field_name; }},
            entry.value);
        return os;
    }

    bool isInteger() const { return std::holds_alternative<int32_t>(value) || std::holds_alternative<int64_t>(value); }

    int64_t getIntegerValue() const {
        if (std::holds_alternative<int32_t>(value)) {
            return std::get<int32_t>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        }
        throw std::runtime_error("Value is not a integer");
    }

    bool isFloat() const { return std::holds_alternative<float>(value); }

    float getFloatValue() const {
        if (std::holds_alternative<float>(value)) {
            return std::get<float>(value);
        }
        throw std::runtime_error("Value is not a float");
    }

    bool isString() const { return std::holds_alternative<String>(value); }

    std::string getStringValue() const {
        if (std::holds_alternative<String>(value)) {
            return std::get<String>(value).value;
        }
        throw std::runtime_error("Value is not a string");
    }

    bool isClass() const { return std::holds_alternative<Class>(value); }

    Class getClassValue() const {
        if (std::holds_alternative<Class>(value)) {
            return std::get<Class>(value);
        }
        throw std::runtime_error("Value is not a class");
    }

    bool isArrayIndex() const { return std::holds_alternative<Index>(value); }

    Index getArrayIndexValue() const {
        if (std::holds_alternative<Index>(value)) {
            return std::get<Index>(value);
        }
        throw std::runtime_error("Value is not a index for an array");
    }

    bool isArray() const { return std::holds_alternative<Array>(value); }

    Array getArrayValue() const {
        if (std::holds_alternative<Array>(value)) {
            return std::get<Array>(value);
        }
        throw std::runtime_error("Value is not an array");
    }

    bool isNew() const { return std::holds_alternative<New>(value); }

    New getNewValue() const {
        if (std::holds_alternative<New>(value)) {
            return std::get<New>(value);
        }
        throw std::runtime_error("Value is not new");
    }

    bool isFunction() const { return std::holds_alternative<Function>(value); }

    Function getFunctionValue() const {
        if (std::holds_alternative<Function>(value)) {
            return std::get<Function>(value);
        }
        throw std::runtime_error("Value is not a Function");
    }

    bool isField() const { return std::holds_alternative<Field>(value); }

    Field getFieldValue() const {
        if (std::holds_alternative<Field>(value)) {
            return std::get<Field>(value);
        }
        throw std::runtime_error("Value is not a field");
    }

    bool isBinaryOp() const { return std::holds_alternative<BinaryOperation>(value); }

    BinaryOperation getBinaryOpValue() const {
        if (std::holds_alternative<BinaryOperation>(value)) {
            return std::get<BinaryOperation>(value);
        }
        throw std::runtime_error("Value is not a binary operation");
    }

    bool isUnaryOp() const { return std::holds_alternative<UnaryOperation>(value); }

    UnaryOperation getUnaryOpValue() const {
        if (std::holds_alternative<UnaryOperation>(value)) {
            return std::get<UnaryOperation>(value);
        }
        throw std::runtime_error("Value is not a unary operation");
    }

    bool isIdentifier() const { return std::holds_alternative<Identifier>(value); }

    Identifier getIdentifierValue() const {
        if (std::holds_alternative<Identifier>(value)) {
            return std::get<Identifier>(value);
        }
        throw std::runtime_error("Value is not a Identifier");
    }

    bool isCast() const { return std::holds_alternative<Cast>(value); }

    Cast getCastValue() const {
        if (std::holds_alternative<Cast>(value)) {
            return std::get<Cast>(value);
        }
        throw std::runtime_error("Value is not a Cast");
    }
};

// Forward declarations
class AST;
using ASTPtr = std::shared_ptr<AST>;

// Enum to represent AST nodes
class AST {
public:
    struct Array {
        Type ty;
        ASTPtr num_of_els;
        std::vector<ASTPtr> els;
    };

    struct ArrayIndex {
        ASTPtr arr;
        ASTPtr index;
        Type arr_type;
    };

    struct MethodCall {
        ASTPtr type1;
        ASTPtr type2;
    };

    struct FunctionCall {
        std::string name;
        std::vector<ASTPtr> args;
        Type return_type;
    };

    struct FieldAccess {
        ASTPtr obj;
        std::string field_name;
        Type ty;
    };

    struct FieldAssignment {
        ASTPtr obj;
        std::string field_name;
        Type ty;
        ASTPtr val;
    };

    struct If {
        ASTPtr cond;
        std::vector<ASTPtr> then;
    };

    struct Loop {
        ASTPtr cond;
        std::vector<ASTPtr> body;
    };

    struct ReAssignment {
        ASTPtr var;
        ASTPtr val;
    };

    struct Assignment {
        Type type;
        std::string varName;
        ASTPtr val;
    };

    struct UnaryOperation {
        UnaryOp op;
        ASTPtr val;
    };

    struct BinaryOperation {
        ASTPtr variable;
        BinaryOp op;
        ASTPtr val;
    };

    struct Identifier {
        std::string name;
        Type type;
    };

    struct Return {
        ASTPtr ret;
    };

    struct Object {
        std::string objName;
    };

    struct Cast {
        Type type;
        ASTPtr val;
    };

    struct StatementList {
        std::vector<ASTPtr> statements;
    };

    using ASTVariant = std::variant<std::monostate,  // Null
                                    int32_t,         // Int
                                    float,           // Float
                                    double,          // Double
                                    int64_t,         // Long
                                    std::string,     // String
                                    Object,
                                    Array,       // Array
                                    ArrayIndex,  // ArrayIndex
                                    Cast,        // Cast
                                    Identifier,  // Ident
                                    // std::string,            // New
                                    MethodCall,       // MethodCall
                                    FunctionCall,     // FunctionCall
                                    FieldAccess,      // FieldAccess
                                    FieldAssignment,  // FieldAssignment
                                    Assignment,       // Assignment
                                    ReAssignment,     // ReAssignment
                                    BinaryOperation,  // BinaryOp
                                    UnaryOperation,   // UnaryOp
                                    If,               // If
                                    Return,           // Return
                                    Loop, StatementList, std::optional<ASTPtr>>;

    ASTVariant node;

    AST() = default;
    template <typename T>
    AST(T value) : node(std::move(value)) {}

    bool isInteger() const { return std::holds_alternative<int32_t>(node) || std::holds_alternative<int64_t>(node); }

    int64_t getIntegerNode() const {
        if (std::holds_alternative<int32_t>(node)) {
            return std::get<int32_t>(node);
        } else if (std::holds_alternative<int64_t>(node)) {
            return std::get<int64_t>(node);
        }
        throw std::runtime_error("Node is not a integer");
    }

    bool isFloat() const { return std::holds_alternative<float>(node); }

    float getFloatNode() const {
        if (std::holds_alternative<float>(node)) {
            return std::get<float>(node);
        }
        throw std::runtime_error("Node is not a float");
    }

    // bool isString() const {
    //     return std::holds_alternative<String>(node);
    // }

    // std::string getStringNode() const {
    //     if (std::holds_alternative<String>(node)) {
    //         return std::get<String>(node).Node;
    //     }
    //     throw std::runtime_error("Node is not a string");
    // }

    // bool isClass() const {
    //     return std::holds_alternative<Class>(node);
    // }

    // Class getClassNode() const {
    //     if (std::holds_alternative<Class>(node)) {
    //         return std::get<Class>(node);
    //     }
    //     throw std::runtime_error("Node is not a class");
    // }

    bool isArrayIndex() const { return std::holds_alternative<ArrayIndex>(node); }

    ArrayIndex getArrayIndexNode() const {
        if (std::holds_alternative<ArrayIndex>(node)) {
            return std::get<ArrayIndex>(node);
        }
        throw std::runtime_error("Node is not a index for an array");
    }

    bool isArray() const { return std::holds_alternative<Array>(node); }

    Array getArrayNode() const {
        if (std::holds_alternative<Array>(node)) {
            return std::get<Array>(node);
        }
        throw std::runtime_error("Node is not an array");
    }

    bool isMethodCall() const { return std::holds_alternative<MethodCall>(node); }

    MethodCall getMethodCall() const {
        if (std::holds_alternative<MethodCall>(node)) {
            return std::get<MethodCall>(node);
        }
        throw std::runtime_error("Node is not new");
    }

    bool isFunctionCall() const { return std::holds_alternative<FunctionCall>(node); }

    FunctionCall getFunctionCallNode() const {
        if (std::holds_alternative<FunctionCall>(node)) {
            return std::get<FunctionCall>(node);
        }
        throw std::runtime_error("Node is not a Function");
    }

    bool isIfStatement() const { return std::holds_alternative<If>(node); }

    If getIfNode() const {
        if (std::holds_alternative<If>(node)) {
            return std::get<If>(node);
        }
        throw std::runtime_error("Node is not a field");
    }

    // bool isBinaryOp() const {
    //     return std::holds_alternative<BinaryOperation>(node);
    // }

    // BinaryOperation getBinaryOpNode() const {
    //     if (std::holds_alternative<BinaryOperation>(node)) {
    //         return std::get<BinaryOperation>(node);
    //     }
    //     throw std::runtime_error("Node is not a binary operation");
    // }

    // bool isIdentifier() const {
    //     return std::holds_alternative<Identifier>(node);
    // }

    // Identifier getIdentifierNode() const {
    //     if (std::holds_alternative<Identifier>(node)) {
    //         return std::get<Identifier>(node);
    //     }
    //     throw std::runtime_error("Node is not a Identifier");
    // }
};

// Visitor class for AST
class AstVisitor {
public:
    AstVisitor() = default;
    void visit(const AST& ast, std::stringstream& out) const {
        std::visit(
            [&out, this](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    out << "null";
                } else if constexpr (std::is_same_v<T, int32_t>) {
                    out << arg;
                } else if constexpr (std::is_same_v<T, float>) {
                    out << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    out << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    out << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    out << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, AST::Object>) {
                    out << arg.objName;
                } else if constexpr (std::is_same_v<T, AST::Array>) {
                    if (arg.els.empty()) {
                        out << "new " << arg.ty.toString() << "[";
                        visit(*arg.num_of_els, out);
                        out << "]";
                    } else {
                        out << "{ ";
                        for (size_t i = 0; i < arg.els.size(); ++i) {
                            visit(*arg.els[i], out);
                            if (i != arg.els.size() - 1) {
                                out << ", ";
                            }
                        }
                        out << " }";
                    }
                } else if constexpr (std::is_same_v<T, AST::ArrayIndex>) {
                    visit(*arg.arr, out);
                    out << "[";
                    visit(*arg.index, out);
                    out << "]";
                } else if constexpr (std::is_same_v<T, AST::Cast>) {
                    out << "(" << arg.type.class_name << ") ";
                    visit(*arg.val, out);
                } else if constexpr (std::is_same_v<T, AST::Identifier>) {
                    out << arg.name;
                } else if constexpr (std::is_same_v<T, AST::MethodCall>) {
                    visit(*arg.type1, out);
                    out << ".";
                    visit(*arg.type2, out);
                } else if constexpr (std::is_same_v<T, AST::FunctionCall>) {
                    out << arg.name << "(";
                    for (size_t i = 0; i < arg.args.size(); ++i) {
                        visit(*arg.args[i], out);
                        if (i != arg.args.size() - 1) {
                            out << ", ";
                        }
                    }
                    out << ")";
                } else if constexpr (std::is_same_v<T, AST::FieldAccess>) {
                    visit(*arg.obj, out);
                    out << "." << arg.field_name;
                } else if constexpr (std::is_same_v<T, AST::FieldAssignment>) {
                    visit(*arg.obj, out);
                    out << "." << arg.field_name << " = ";
                    visit(*arg.val, out);
                    out << ";";
                } else if constexpr (std::is_same_v<T, AST::UnaryOperation>) {
                    if (IsPrefix(arg.op)) {
                        out << UnaryOpToString(arg.op);
                        visit(*arg.val, out);
                    } else {
                        visit(*arg.val, out);
                        out << UnaryOpToString(arg.op);
                    }
                } else if constexpr (std::is_same_v<T, AST::BinaryOperation>) {
                    out << "(";
                    visit(*arg.variable, out);
                    out << " " << BinOpToString(arg.op) << " ";
                    visit(*arg.val, out);
                    out << ")";
                } else if constexpr (std::is_same_v<T, AST::Assignment>) {
                    out << arg.type.toString() << " " << arg.varName << " = ";
                    visit(*arg.val, out);
                    out << ";";
                } else if constexpr (std::is_same_v<T, AST::ReAssignment>) {
                    visit(*arg.var, out);
                    out << " = ";
                    visit(*arg.val, out);
                    out << ";";
                } else if constexpr (std::is_same_v<T, AST::Return>) {
                    if (arg.ret) {
                        out << "return ";
                        visit(*arg.ret, out);
                        out << ";";
                    } else {
                        out << "return;";
                    }
                } else if constexpr (std::is_same_v<T, std::optional<ASTPtr>>) {
                    if (arg) {
                        visit(**arg, out);
                    }
                } else if constexpr (std::is_same_v<T, AST::If>) {
                    out << "if (";
                    visit(*arg.cond, out);
                    out << ") {\n";
                    for (const auto& stmt : arg.then) {
                        visit(*stmt, out);
                        out << "\n";
                    }
                    out << "}";
                } else if constexpr (std::is_same_v<T, AST::Loop>) {
                    out << "while (";
                    visit(*arg.cond, out);
                    out << ") { \n";
                    for (const auto& stmt : arg.body) {
                        visit(*stmt, out);
                    }
                    out << "}\n";
                } else if constexpr (std::is_same_v<T, AST::StatementList>) {
                    for (const auto& stmt : arg.statements) {
                        visit(*stmt, out);
                        out << "\n";  // Or some other separator
                    }
                }
            },
            ast.node);
    }
};

}  // namespace TitaniumDecompiler
