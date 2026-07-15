// #include <gtest/gtest.h>
// #include "TitaniumDecompiler/Decompiler/Decompiler.h"
// #include "TitaniumDecompiler/Disassembler/Function.h"
// #include "Platform/Linux/FileFormats/JVM/ConstPoolInfo.h"
// #include "Platform/Linux/FileFormats/JVM/ConstantPool.h"

// namespace TitaniumDecompiler { //

// class DecompilerTest : public ::testing::Test {
// protected:
//     Decompiler decompiler;
// };

// TEST_F(DecompilerTest, DecompileReturnsNonEmptyOutputAfterProcessing) {
//     Function mockFunc("testMethod");
//     mockFunc.SetMethodAccessFlags("public static");
//     mockFunc.SetMethodDesc("()V");
//     mockFunc.SetFunctionCFG(std::make_shared<CFG>());
//     std::vector<Function> funcs = { mockFunc };
//     EXPECT_NO_THROW(decompiler.ProcessFunctions(funcs));
//     std::string output = decompiler.Decompile();
//     EXPECT_FALSE(output.empty());
// }

// TEST_F(DecompilerTest, ConvertInstructionToIrGeneratesLoadStatement) {
//     Function func("testFunc");
//     Insn insn;
//     insn.opcode = OPCODE_ILOAD_1;
//     insn.Op1.addr = 1;
//     std::string result = decompiler.ConvertInstructionToIr(func, insn);
//     EXPECT_TRUE(result.find("Load int var1") != std::string::npos);
// }

// TEST_F(DecompilerTest, ConvertInstructionToIrHandlesGoto) {
//     Function func("testFunc");
//     Insn insn;
//     insn.opcode = OPCODE_GOTO;
//     insn.Op1.addr = 10;
//     std::string result = decompiler.ConvertInstructionToIr(func, insn);
//     EXPECT_EQ(result, "goto L10:");
// }

// TEST_F(DecompilerTest, ConvertInstructionToIrHandlesReturn) {
//     Function func("testFunc");
//     Insn insn;
//     insn.opcode = OPCODE_RETURN;
//     std::string result = decompiler.ConvertInstructionToIr(func, insn);
//     EXPECT_EQ(result, "return;");
// }

// TEST_F(DecompilerTest, ConvertInstructionToIrHandlesInvokeStatic) {
//     Function func("testFunc");
//     ClassFile mockClassFile;
//     func.SetFunctionClassFile(mockClassFile);

//     // === Construct valid constant pool ===
//     auto methodRef = std::make_shared<MethodRefInfo>();
//     methodRef->classIndex = 2;
//     methodRef->nameAndTypeIndex = 3;
//     ConstPoolInfo methodInfo;
//     methodInfo.Tag = Tags::Method;
//     methodInfo.Info = methodRef;

//     auto classInfo = std::make_shared<ClassInfo>();
//     classInfo->nameIndex = 4;
//     ConstPoolInfo classCP;
//     classCP.Tag = Tags::Class;
//     classCP.Info = classInfo;

//     auto nameAndTypeInfo = std::make_shared<NameAndTypeInfo>();
//     nameAndTypeInfo->nameIndex = 4;
//     nameAndTypeInfo->descriptorIndex = 4;
//     ConstPoolInfo nameAndTypeCP;
//     nameAndTypeCP.Tag = Tags::NameAndType;
//     nameAndTypeCP.Info = nameAndTypeInfo;

//     auto utf8Name = std::make_shared<UTF8Info>();
//     utf8Name->length = 4;
//     utf8Name->bytes = {'n','a','m','e'};
//     ConstPoolInfo utf8NameCP;
//     utf8NameCP.Tag = Tags::Utf8;
//     utf8NameCP.Info = utf8Name;

//     auto utf8Desc = std::make_shared<UTF8Info>();
//     utf8Desc->length = 3;
//     utf8Desc->bytes = {'(',')','V'};
//     ConstPoolInfo utf8DescCP;
//     utf8DescCP.Tag = Tags::Utf8;
//     utf8DescCP.Info = utf8Desc;

//     mockClassFile.m_ConstantPool.m_ConstPoolInfo.resize(5);
//     mockClassFile.m_ConstantPool.m_ConstPoolInfo[0] = methodInfo;
//     mockClassFile.m_ConstantPool.m_ConstPoolInfo[1] = classCP;
//     mockClassFile.m_ConstantPool.m_ConstPoolInfo[2] = nameAndTypeCP;
//     mockClassFile.m_ConstantPool.m_ConstPoolInfo[3] = utf8NameCP;
//     mockClassFile.m_ConstantPool.m_ConstPoolInfo[4] = utf8DescCP;

//     func.SetFunctionClassFile(mockClassFile);

//     Insn insn;
//     insn.opcode = OPCODE_INVOKESTATIC;
//     insn.Op1.value = 1;

//     std::string output = decompiler.ConvertInstructionToIr(func, insn);
//     EXPECT_TRUE(output.find("invokeStatic(") != std::string::npos);
// }

// }  // namespace TitaniumDecompiler
