#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "AST.h"
#include "Platform/Linux/FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/Disassembler/Function.h"

namespace TitaniumDecompiler {
struct PoolKindTypeNameField {
    std::string m_Name;
    Type m_Type;
};

struct PoolKindTypeNameMethod {
    std::string m_Name;
    MethodDescriptor m_Type;
};

struct MethodRefStack {
    std::string m_Class;
    std::string m_Name;
    MethodDescriptor desc;
};

struct DynamicInvokeRefStack {
    std::string m_Class;
    std::string m_Name;
    MethodDescriptor desc;
    std::vector<uint16_t> unparsed_args;
};

class Codegen {
public:
    Codegen(ClassFile& classFile, std::unordered_map<int, StackEntry> localVars)
        : m_ClassFile(classFile), m_LocalVariables(localVars), m_CurrentPos(0), m_InsideInit(false) {}

    std::vector<AST> GenJavaCode(const Function& function);
    std::optional<ASTPtr> Store(uint8_t idx);
    void Cast(const Type& ty);
    void Load(uint8_t idx);
    ASTPtr LoadLocalAST(uint8_t idx);
    void BinaryOper(BinaryOp op);
    StackEntry PopStack();

private:
    ClassFile& m_ClassFile;
    std::vector<StackEntry> m_Stack;
    std::unordered_map<int, StackEntry> m_LocalVariables;
    std::vector<AST> ast_;
    size_t m_CurrentPos;
    bool m_InsideInit;
    std::vector<Insn> funcInstructions;
    std::map<size_t, std::set<size_t>> m_Dominators;

    uint32_t FindStartNodeId(const std::vector<std::shared_ptr<BasicBlock>>& blocks);
    std::optional<AST> ReadInstruction(const Function& function, const Insn& instruction);
    ASTPtr ConvertStackEntryToAST(StackEntry entry);
    void StringConcatFactory(const std::vector<uint16_t>& unparsedArgs);
    ASTPtr CreateForLoop(std::shared_ptr<BasicBlock> loopHeader, const std::map<int, std::shared_ptr<BasicBlock>>& cfgBlocks,
                         const std::map<uint32_t, uint32_t>& dominators);
    ASTPtr ConvertConditionToAST(const Insn& instruction, std::vector<StackEntry>& stack);
    ASTPtr ConvertCFGToAST(std::shared_ptr<BasicBlock> block, const std::map<int, std::shared_ptr<BasicBlock>>& cfgBlocks,
                           const std::map<uint32_t, uint32_t>& dominators);
    bool IsBackEdge(const Insn& instruction, const std::map<size_t, std::set<size_t>>& dominatorTree, size_t currentBlockID);
    ASTPtr ConvertUpdateToAST(const Insn& instruction);
    ASTPtr ConvertInstructionToAST(const Insn& instruction);
};


std::string GetConstantUTF8FromClass(const ClassFile& classFile, int idx);
std::string GetConstantClassFromClass(const ClassFile& classFile, int idx);
PoolKindTypeNameField GetNameAndTypeFromClassField(const ClassFile& classFile, int idx);
PoolKindTypeNameMethod GetNameAndTypeFromClassMethod(const ClassFile& classFile, int idx);
FieldRefStack GetFieldRefFromIndex(const ClassFile& classFile, int idx);
MethodRefStack GetMethodRefFromIndex(const ClassFile& classFile, int idx);
DynamicInvokeRefStack GetDynamicInvokeRefFromIndex(const ClassFile& classFile, int idx);
BootstrapMethods GetBootstrapMethod(const ClassFile& classFile);
std::string GetStringFromIndex(const ClassFile& classFile, int idx);

}  // namespace TitaniumDecompiler
