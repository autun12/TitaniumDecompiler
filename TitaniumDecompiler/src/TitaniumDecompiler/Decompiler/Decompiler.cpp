#include "Decompiler.h"

#include <TitaniumLogger/Logger/Log.h>
#include <stdio.h>

#include "ASTNodes.h"
#include "FileFormats/JVM/CodeConstants.h"
#include "GraphUtils.h"
#include "SSA.h"

namespace TitaniumDecompiler {

std::string Decompiler::Decompile() {
    m_Output = GenerateJavaCode();
    return m_Output;
}

void Decompiler::ProcessFunctions(const std::vector<Function>& functions) {
    for (auto& func : functions) {
        Function funct = func;
        m_DeadCodeRemover.RemoveDeadBlocks(funct.GetFunctionCFG());
        m_DeadCodeRemover.ConnectDummyExitBlock(funct.GetFunctionCFG());

        m_DeadCodeRemover.RemoveGotos(funct.GetFunctionCFG());
        m_DeadCodeRemover.MergeBasicBlocks(funct.GetFunctionCFG());
        TD_DECOMP_WARN("ssa cfg: {0}",
                       funct.GetFunctionCFG()->m_SsaCfg->toString());
        // for (auto& block : funct.GetFunctionCFG()->GetBlocks()) {
        //     funct.GetFunctionCFG()->PrintBlock(block);
        // }

        // DomHelper domHelper(*funct.GetFunctionCFG());
        // domHelper.Compute();

        // std::unordered_map<int, std::shared_ptr<BasicBlock>> basicBlocks;
        // // std::cout << "Number of blocks in CFG: " <<
        // funct.GetFunctionCFG()->GetBlocks().size() << std::endl;
        // // domHelper.Print();

        // for (auto& block : funct.GetFunctionCFG()->GetBlocks()) {
        //     basicBlocks[block->m_ID] = block;
        //     // funct.GetFunctionCFG()->PrintBlock(block);
        // }

        // std::map<size_t, std::set<size_t>> doms = domHelper.GetDoms();
        // std::shared_ptr<ASTNode> root =
        // domHelper.BuildASTFromDomTree(funct.GetFunctionCFG()->GetFirst(),
        // doms, basicBlocks);

        // domHelper.PrintAST(root);
        // std::cout << domHelper.GenerateStructuredCode(root, 0) << std::endl;

        m_Functions.push_back(funct);
    }
}

std::string Decompiler::GenerateJavaCode() {
    std::string javaCode = "";

    std::unordered_map<int, StackEntry> localVars;
    int argsOffset = 1;
    // javaCode = ""
    for (auto& func : m_Functions) {
        MethodDescriptor desc =
            MethodDescriptor::newDescriptor(func.GetMethodDesc());
        std::stringstream ss;
        for (size_t i = 0; i < desc.args.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "arg: " << (i + argsOffset) << desc.args[i].toString();
        }
        std::string methodDesc = ss.str();
        // std::cout << args[0] << std::endl;
        javaCode += func.GetMethodAccessFlags() + " fun " +
                    func.GetFunctionName() + "(" + methodDesc + ") {\n";
        javaCode += ConvertFunctionToJava(func, localVars);
        javaCode += "}\n\n";
        //     for (auto& block : func.GetFunctionCFG()->GetBlocks()) {
        //         std::vector<Insn> instr = block->GetInstructions();

        //         for (auto& insn : instr) {
        //             std::string line = ConvertInstructionToJava(insn);
        //             javaCode += line + "\n";
        //         }

        //         std::vector<std::shared_ptr<BasicBlock>> successors =
        //         block->GetSuccessor(); if (!successors.empty()) {
        //             if (successors.size() == 1) {
        //                 javaCode += "// Jump to next block\n";
        //             } else {
        //                 javaCode += "// Conditional jump to next block\n";
        //             }
        //         }
        //     }
        //     std::cout << javaCode << std::endl;
    }

    return javaCode;
}

std::string Decompiler::ConvertFunctionToJava(
    const Function& func, std::unordered_map<int, StackEntry>& localVars) {
    std::string javaCode = "";
    Function funct = func;

    uint32_t arg_offset;

    if (funct.GetMethodAccessFlags().find("static") != std::string::npos) {
        arg_offset = 0;
    } else {
        std::string className =
            GetConstantClassFromClass(funct.GetFunctionClassFile(),
                                      funct.GetFunctionClassFile().m_ThisClass);
        std::string thisType = "this";
        StackEntry::Identifier ident{thisType, Type::ClassNameType(className)};
        localVars[0] = StackEntry(ident);
        arg_offset = 1;
    }
    // for (auto& block : funct.GetFunctionCFG()->GetBlocks()) {
    //     std::vector<Insn> instr = block->GetInstructions();
    //     uint32_t arg_offset;
    //     if(funct.GetMethodAccessFlags().find("static") != std::string::npos)
    //     {
    //         arg_offset = 0;
    //     } else {
    //         std::string className =
    //         GetConstantClassFromClass(funct.GetFunctionClassFile(),
    //         funct.GetFunctionClassFile().m_ThisClass); std::string thisType =
    //         "this"; StackEntry::Identifier ident { thisType,
    //         Type::ClassNameType(className) }; localVars[0] =
    //         StackEntry(ident); arg_offset = 1;
    //     }

    //     for(auto& it : localVars) {
    //         printf("%d\n", it.first);
    //     }

    auto classFile = funct.GetFunctionClassFile();
    Codegen codegen(classFile, localVars);

    auto ast = codegen.GenJavaCode(funct);

    AstVisitor astVisitor;
    std::stringstream ss;
    std::ostringstream os;

    for (auto& lines : ast) {
        astVisitor.visit(lines, ss);
        ss << "\n";
    }

    javaCode += ss.str();
    return javaCode;
}

std::string Decompiler::GetConstantUTF8FromClass(const ClassFile& classFile,
                                                 int idx) {
    int constantIdx = idx - 1;

    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* utf8Info = cpInfo.GetAs<UTF8Info>();

    std::stringstream s;
    for (int i = 0; i < utf8Info->length; i++) {
        s << utf8Info->bytes[i];
    }

    return s.str();
}

std::string Decompiler::GetConstantClassFromClass(const ClassFile& classFile,
                                                  int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* classInfo = cpInfo.GetAs<ClassInfo>();
    std::string className =
        GetConstantUTF8FromClass(classFile, classInfo->nameIndex);
    // std::cout << "ClassName = " << className << std::endl;
    return className;
}

std::string Decompiler::GetNameAndTypeFromClass(const ClassFile& classFile,
                                                int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* nameAndTypeInfo = cpInfo.GetAs<NameAndTypeInfo>();
    std::string output = "";
    std::string name =
        GetConstantUTF8FromClass(classFile, nameAndTypeInfo->nameIndex);
    std::string desc =
        GetConstantUTF8FromClass(classFile, nameAndTypeInfo->descriptorIndex);
    output.append(name);
    output.append(":");
    output.append(desc);
    return output;
}

std::string Decompiler::GetConstantFromClass(const ClassFile& classFile,
                                             int idx) {
    int constantIdx = idx - 1;
    auto cpInfo = classFile.m_ConstantPool.m_ConstPoolInfo.at(constantIdx);
    auto* classInfo = cpInfo.GetAs<ClassInfo>();
    std::string className =
        GetConstantUTF8FromClass(classFile, classInfo->nameIndex);
    return className;
}

std::string Decompiler::ConvertConstToName(const ClassFile& classFile,
                                           int idx) {
    std::string output = "";
    InsnTempWrapper tempInsn;

    ConstPoolInfo info = classFile.m_ConstantPool.m_ConstPoolInfo.at(idx - 1);
    if (info.Tag == Tags::Field) {
        auto field = info.GetAs<FieldRefInfo>();
        // static_cast<FieldRefInfo*>(info.Info.get());
        std::string classInf =
            GetConstantClassFromClass(classFile, field->classIndex);
        std::string nameInf =
            GetNameAndTypeFromClass(classFile, field->nameAndTypeIndex);

        output.append(classInf);
        output.append(".");
        output.append(nameInf);
    } else if (info.Tag == Tags::Method) {
        auto method = info.GetAs<MethodRefInfo>();
        // static_cast<MethodRefInfo*>(info.Info.get());
        std::string classInf =
            GetConstantClassFromClass(classFile, method->classIndex);
        std::string nameInf =
            GetNameAndTypeFromClass(classFile, method->nameAndTypeIndex);

        output.append(classInf);
        output.append(".");
        output.append(nameInf);
    } else if (info.Tag == Tags::Class) {
        std::string classInf = GetConstantClassFromClass(classFile, idx);
        tempInsn.m_ClassInf = classInf;

        output.append(classInf);
    }
    return output;
}
}  // namespace TitaniumDecompiler
