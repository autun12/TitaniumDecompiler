#include <gtest/gtest.h>

#include "TitaniumDecompiler/Decompiler/AST.h"
#include "TitaniumDecompiler/Decompiler/ASTNodes.h"
#include "TitaniumDecompiler/Decompiler/Codegen.h"
#include "TitaniumDecompiler/src/FileFormats/JVM/CodeConstants.h"
#include "TitaniumDecompiler/src/FileFormats/JVM/ConstPoolInfo.h"
#include "TitaniumDecompiler/src/FileFormats/JVM/ConstantPool.h"
#include "TitaniumDecompiler/src/FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/CFG.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/Function.h"

using namespace TitaniumDecompiler;

std::map<uint8_t, std::string> opcodes = {
    {OPCODE_BIPUSH, "bipush"},       {OPCODE_ICONST_1, "iconst_1"},
    {OPCODE_ICONST_2, "iconst_2"},   {OPCODE_IADD, "iadd"},
    {OPCODE_IRETURN, "ireturn"},     {OPCODE_ISTORE_1, "istore_1"},
    {OPCODE_FCONST_1, "fconst_1"},   {OPCODE_ILOAD_0, "iload_0"},
    {OPCODE_ISTORE_0, "istore_0"},   {OPCODE_ISUB, "isub"},
    {OPCODE_IF_ICMPEQ, "if_icmpeq"}, {OPCODE_INVOKESTATIC, "invokestatic"}};

class CodegenTest : public ::testing::Test {
protected:
    Function function;
    ClassFile classFile;
    Codegen codegen;

    CodegenTest()
        : function("mockMethod"), classFile(), codegen(classFile, {}) {}

    void SetUp() override {
        function.SetFunctionClassFile(classFile);

        auto cfg = std::make_shared<CFG>();
        auto block = std::make_shared<BasicBlock>();
        cfg->m_Blocks[0] = block;
        function.SetFunctionCFG(cfg);
    }

    Insn MakeInsn(uint8_t opcode, uint32_t op1 = 0, uint32_t op2 = 0) {
        Insn insn;
        insn.opcode = opcode;
        insn.opcodeName = opcodes[opcode];
        insn.Op1.value = op1;
        insn.Op2.value = op2;
        return insn;
    }
    void VisitAndAssertAST(const std::vector<AST>& ast) {
        ASSERT_FALSE(ast.empty());
        AstVisitor visitor;
        std::stringstream ss;
        for (const auto& node : ast) {
            visitor.visit(node, ss);  // now correct
        }
        EXPECT_FALSE(ss.str().empty());
    }
};

TEST_F(CodegenTest, HandlesBIPUSH) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_BIPUSH, 42));

    auto ast = codegen.GenJavaCode(function);
    EXPECT_TRUE(ast.empty());
}

TEST_F(CodegenTest, HandlesICONSTAndStore) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_ICONST_2));
    block->AddInstruction(MakeInsn(OPCODE_ISTORE_1));
    auto ast = codegen.GenJavaCode(function);
    VisitAndAssertAST(ast);
}

TEST_F(CodegenTest, HandlesIADD) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_ICONST_1));
    block->AddInstruction(MakeInsn(OPCODE_ICONST_2));
    block->AddInstruction(MakeInsn(OPCODE_IADD));

    auto ast = codegen.GenJavaCode(function);
    EXPECT_TRUE(ast.empty());
}

TEST_F(CodegenTest, HandlesIRETURN) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_ICONST_1));
    block->AddInstruction(MakeInsn(OPCODE_IRETURN));
    auto ast = codegen.GenJavaCode(function);
    VisitAndAssertAST(ast);
}

TEST_F(CodegenTest, HandlesFCONST1) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_FCONST_1));

    auto ast = codegen.GenJavaCode(function);
    EXPECT_TRUE(ast.empty());
}

TEST_F(CodegenTest, HandlesILOAD0AfterStore) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_ICONST_2));
    block->AddInstruction(MakeInsn(OPCODE_ISTORE_0));
    block->AddInstruction(MakeInsn(OPCODE_ILOAD_0));
    auto ast = codegen.GenJavaCode(function);
    VisitAndAssertAST(ast);
}
TEST_F(CodegenTest, HandlesISUB) {
    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_ICONST_2));
    block->AddInstruction(MakeInsn(OPCODE_ICONST_1));
    block->AddInstruction(MakeInsn(OPCODE_ISUB));

    auto ast = codegen.GenJavaCode(function);
    EXPECT_TRUE(ast.empty());
}

// TODO fix this test, fails to build when uncommented

// TEST_F(CodegenTest, HandlesIF_ICMPEQ) {
//     auto cfg = function.GetFunctionCFG();
//     auto block0 = cfg->GetBlocks().at(0);
//     auto block1 = std::make_shared<BasicBlock>();
//     cfg->m_Blocks[1] = block1;
//     block0->AddSuccessor(block1);

//     auto insn1 = MakeInsn(OPCODE_ICONST_1, 0, 0, 0);
//     auto insn2 = MakeInsn(OPCODE_ICONST_1, 0, 0, 1);
//     auto branch = MakeInsn(OPCODE_IF_ICMPEQ, 10, 0, 2); // jump to offset 10
//     auto insn3 = MakeInsn(OPCODE_ICONST_2, 0, 0, 10);

//     block0->AddInstruction(insn1);
//     block0->AddInstruction(insn2);
//     block0->AddInstruction(branch);
//     block1->AddInstruction(insn3);

//     auto ast = codegen.GenJavaCode(function);
//     EXPECT_GE(ast.size(), 1);
// }

TEST_F(CodegenTest, HandlesINVOKESTATIC) {
    // Create mock constant pool
    auto* methodRef = new MethodRefInfo();
    methodRef->classIndex = 2;
    methodRef->nameAndTypeIndex = 3;
    auto* classInfo = new ClassInfo();
    classInfo->nameIndex = 4;
    auto* nameAndType = new NameAndTypeInfo();
    nameAndType->nameIndex = 5;
    nameAndType->descriptorIndex = 6;
    auto* nameUtf = new UTF8Info();
    nameUtf->length = 4;
    nameUtf->bytes = {'m', 'a', 'i', 'n'};
    auto* descUtf = new UTF8Info();
    descUtf->length = 3;
    descUtf->bytes = {'(', ')', 'V'};
    auto* classNameUtf = new UTF8Info();
    classNameUtf->length = 4;
    classNameUtf->bytes = {'T', 'e', 's', 't'};

    classFile.m_ConstantPool.m_ConstPoolInfo.resize(6);
    classFile.m_ConstantPool.m_ConstPoolInfo[0] = {Tags::Method, methodRef};
    classFile.m_ConstantPool.m_ConstPoolInfo[1] = {Tags::Class, classInfo};
    classFile.m_ConstantPool.m_ConstPoolInfo[2] = {Tags::NameAndType,
                                                   nameAndType};
    classFile.m_ConstantPool.m_ConstPoolInfo[3] = {Tags::Utf8, classNameUtf};
    classFile.m_ConstantPool.m_ConstPoolInfo[4] = {Tags::Utf8, nameUtf};
    classFile.m_ConstantPool.m_ConstPoolInfo[5] = {Tags::Utf8, descUtf};

    auto block = function.GetFunctionCFG()->GetBlocks().at(0);
    block->AddInstruction(MakeInsn(OPCODE_ICONST_1));
    block->AddInstruction(MakeInsn(OPCODE_INVOKESTATIC, 1));
    auto ast = codegen.GenJavaCode(function);
    VisitAndAssertAST(ast);
}
