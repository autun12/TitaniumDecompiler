#pragma once

#include <filesystem>
#include <vector>

#include "AST.h"
#include "Codegen.h"
#include "DomHelper.h"
#include "Platform/Linux/FileFormats/JVM/Instruction.h"
#include "SSA.h"
#include "TitaniumDecompiler/CFGReducer/DeadCodeRemover.h"
#include "TitaniumDecompiler/Disassembler/Function.h"
#include "TitaniumDecompiler/Disassembler/JVMDisassembler.h"

namespace TitaniumDecompiler {
struct Decompiler {
public:
    Decompiler() = default;
    ~Decompiler() = default;

public:
    std::string Decompile();
    void ProcessFunctions(const std::vector<Function>& functions);

private:
    std::string GenerateJavaCode();
    std::string ConvertFunctionToJava(const Function& insn, std::unordered_map<int, StackEntry>& localVars);

    std::string GetConstantUTF8FromClass(const ClassFile& classFile, int idx);

    std::string GetConstantClassFromClass(const ClassFile& classFile, int idx);
    std::string GetNameAndTypeFromClass(const ClassFile& classFile, int idx);
    std::string GetConstantFromClass(const ClassFile& classFile, int idx);
    std::string ConvertConstToName(const ClassFile& classFile, int idx);

private:
    JVMDisassembler m_Disassembler;
    DeadCodeRemover m_DeadCodeRemover;
    std::vector<Function> m_Functions;
    std::string m_Output;
    // Codegen m_Codegen;
    AST m_AST;
};
}  // namespace TitaniumDecompiler
