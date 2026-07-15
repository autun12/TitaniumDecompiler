#pragma once

#include <stdint.h>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "CFG.h"
#include "Platform/Linux/FileFormats/JVM/ClassFileParser.h"
#include "Platform/Linux/FileFormats/JVM/Methods.h"
#include "TitaniumDecompiler/Decompiler/AST.h"

namespace TitaniumDecompiler {

struct FieldDescriptor {
    Type m_Ty;

    FieldDescriptor(Type type) : m_Ty(std::move(type)) {}

    static FieldDescriptor newDescriptor(const std::string& s) {
        auto it = s.begin();
        auto end = s.end();

        auto ty = parseSingleType(it, end);
        if (!ty) {
            throw std::runtime_error("Invalid return type");
        }

        return FieldDescriptor(*ty);
    }
};

struct MethodDescriptor {
    std::vector<Type> args;
    Type returnType;

    MethodDescriptor(std::vector<Type> arguments, Type ret_type) : args(std::move(arguments)), returnType(std::move(ret_type)) {}

    static MethodDescriptor newDescriptor(const std::string& s) {
        auto it = s.begin();
        auto end = s.end();

        if (it == end || *it != '(') {
            throw std::runtime_error("Invalid starting character in method descriptor");
        }
        ++it;

        std::vector<Type> arguments;
        while (it != end && *it != ')') {
            auto arg_type = parseSingleType(it, end);
            if (arg_type) {
                arguments.push_back(*arg_type);
            }
        }

        if (it == end || *it != ')') {
            throw std::runtime_error("Expected ')' in method descriptor");
        }
        ++it;

        if (it == end) {
            throw std::runtime_error("No return type specified");
        }

        auto return_type = parseSingleType(it, end);
        if (!return_type) {
            throw std::runtime_error("Invalid return type");
        }

        return MethodDescriptor(arguments, *return_type);
    }
};

struct FunctionHeader {
    std::string className;
    std::string name;
    MethodDescriptor descriptor;

    FunctionHeader(std::string className, std::string name, MethodDescriptor desc)
        : className(std::move(className)), name(std::move(name)), descriptor(std::move(desc)) {}
};

struct Function {
public:
    Function() = default;
    Function(const std::string& functionName);
    ~Function() = default;
    std::shared_ptr<CFG> GetFunctionCFG() const;
    std::string GetFunctionName();
    void SetFunctionCFG(const std::shared_ptr<CFG>& cfg);
    void SetFunctionName(const std::string& functionName);
    void SetMethod(const Methods& method);
    Methods GetMethod();
    void SetMethodDesc(const std::string& methodDesc);
    std::string GetMethodDesc();
    void SetMethodAccessFlags(const std::string& methodAccessFlags);
    std::string GetMethodAccessFlags();
    void SetMaxStack(uint16_t maxStack);
    uint16_t GetMaxStack();
    void SetMaxLocals(uint16_t maxLocals);
    StackMapTable GetStackMapTable() { return m_StackMapTable; };
    void SetStackMapTable(const StackMapTable& smt) { m_StackMapTable = smt; }
    uint16_t GetMaxLocals();

    // void GetLocals();
    void InsertIntoRefConstPool(uint64_t index, std::string constPoolName);
    std::multimap<uint64_t, std::string> GetLocals();
    std::multimap<uint64_t, std::string> GetRefConstPool();
    void SetFunctionClassFile(ClassFile classFile) { m_ClassFile = classFile; }
    ClassFile GetFunctionClassFile() { return m_ClassFile; }

private:
    std::string m_FunctionName;
    std::string m_MethodDesc;
    std::string m_MethodAccessFlags;

    std::shared_ptr<CFG> m_CFG;
    Methods m_Method;
    uint16_t m_MaxStack;
    uint16_t m_MaxLocals;
    std::multimap<uint64_t, std::string> m_LocalVars;
    std::multimap<uint64_t, std::string> m_RefConstPool;
    StackMapTable m_StackMapTable;
    ClassFile m_ClassFile;
};

}  // namespace TitaniumDecompiler
