#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "FileFormats/JVM/Attributes.h"
#include "FileFormats/JVM/ClassFileParser.h"
#include "FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/Decompiler/SSA.h"
#include "TitaniumDecompiler/Disassembler/Disassembler.h"
#include "TitaniumDecompiler/Kernel/Loader.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/CFG.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/Function.h"

class DisassemblerTest : public ::testing::Test {
protected:
    TitaniumDecompiler::Disassembler disassembler;

    std::filesystem::path CreateTempFile(
        const std::string& content, const std::string& extension = ".class") {
        std::filesystem::path tempPath =
            std::filesystem::temp_directory_path() /
            ("temp_test_file" + extension);
        std::ofstream tempFile(tempPath, std::ios::binary);
        tempFile << content;
        tempFile.close();
        return tempPath;
    }
};
TEST_F(DisassemblerTest, DisassembleValidClassFile) {
    std::filesystem::path testFilePath = "JavaTestFiles/Test.class";

    // Load raw bytes
    std::ifstream inFile(testFilePath, std::ios::binary);
    ASSERT_TRUE(inFile.is_open()) << "Could not open test class file.";
    std::vector<uint8_t> classBytes((std::istreambuf_iterator<char>(inFile)),
                                    std::istreambuf_iterator<char>());

    // Parse and heap-allocate ClassFile
    auto parser = std::make_unique<TitaniumDecompiler::ClassFileParser>();
    std::shared_ptr<TitaniumDecompiler::ClassFile> classFile =
        std::make_shared<TitaniumDecompiler::ClassFile>(
            parser->Parse(classBytes));

    // Get class name safely
    std::string className =
        TitaniumDecompiler::GetConstantClass(classFile->m_ThisClass);

    // Disassemble
    TitaniumDecompiler::Disassembler dis;
    std::string output = dis.DisassembleClassFile(
        *classFile, className);  // Pass by ref to avoid copy

    EXPECT_FALSE(output.empty());
}

TEST_F(DisassemblerTest, DisassembleInvalidClassFile) {
    std::vector<unsigned char> invalidData = {0xCA, 0xFE, 0xBA, 0xBE};
    std::string corrupted(reinterpret_cast<const char*>(invalidData.data()),
                          invalidData.size());
    auto tempPath = CreateTempFile(corrupted);

    try {
        std::string output = disassembler.Disassemble(tempPath);
        EXPECT_TRUE(output.empty());
    } catch (const std::exception& ex) {
        SUCCEED() << "Caught expected exception: " << ex.what();
    }

    std::filesystem::remove(tempPath);
}

TEST_F(DisassemblerTest, DisassembleValidJarFile) {
    // Create a temporary valid JAR file
    // std::string validJarContent = "valid jar content"; // Replace with actual
    // valid jar content std::filesystem::path tempPath =
    // CreateTempFile(validJarContent);
    std::filesystem::path testFilePath =
        "JavaTestFiles/TestJarFile/TestJarFile.jar";

    std::string output = disassembler.Disassemble(testFilePath);
    EXPECT_FALSE(output.empty());

    std::filesystem::remove(testFilePath);
}

TEST_F(DisassemblerTest, DisassembleInvalidJarFile) {
    // Create a temporary invalid JAR file
    std::string invalidJarContent = "invalid jar content";
    std::filesystem::path tempPath = CreateTempFile(invalidJarContent);

    try {
        std::string output = disassembler.Disassemble(tempPath);
        EXPECT_TRUE(output.empty());
    } catch (const std::exception& ex) {
        SUCCEED() << "Caught expected exception: " << ex.what();
    }

    std::filesystem::remove(tempPath);
}

TEST_F(DisassemblerTest, CreateFunctions) {
    TitaniumDecompiler::ClassFile classFile;
    // Constant Pool entries
    auto methodNameUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    methodNameUtf8->length = 6;
    methodNameUtf8->bytes = {'m', 'e', 't', 'h', 'o', 'd'};
    TitaniumDecompiler::ConstPoolInfo nameEntry;
    nameEntry.Tag = TitaniumDecompiler::Tags::Utf8;
    nameEntry.Info = std::move(methodNameUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(nameEntry));  // index 0

    auto methodDescUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    methodDescUtf8->length = 3;
    methodDescUtf8->bytes = {'(', ')', 'V'};
    TitaniumDecompiler::ConstPoolInfo descEntry;
    descEntry.Tag = TitaniumDecompiler::Tags::Utf8;
    descEntry.Info = std::move(methodDescUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(descEntry));  // index 1

    // Code attribute with bytecode
    auto codeAttr = std::make_unique<TitaniumDecompiler::Code>();
    codeAttr->maxStack = 1;
    codeAttr->maxLocals = 1;
    codeAttr->code = {0xB1};  // return

    TitaniumDecompiler::AttributeInfo attr;
    attr.tag = TitaniumDecompiler::AttributeTypes::CodeType;
    attr.info = std::move(codeAttr);

    TitaniumDecompiler::Attributes methodAttrs;
    methodAttrs.m_Attributes.push_back(attr);

    TitaniumDecompiler::Methods method;
    method.m_NameIndex = 1;         // refers to index 0 (JVM-style 1-based)
    method.m_DescIndex = 2;         // refers to index 1
    method.m_AccessFlags = 0x0001;  // public
    method.m_Attr = methodAttrs;

    classFile.m_Methods.push_back(method);
    disassembler.CreateFunctions(classFile, "TestClassName");

    auto functions = disassembler.GetClassesToFunctions()["TestClassName"];
    ASSERT_FALSE(functions.empty());
    EXPECT_EQ(functions[0].GetFunctionName(), "method");
    EXPECT_EQ(functions[0].GetMethodDesc(), "()V");
    EXPECT_EQ(functions[0].GetMaxStack(), 1);
    EXPECT_EQ(functions[0].GetMaxLocals(), 1);
}

TEST_F(DisassemblerTest, ParseFunction) {
    TitaniumDecompiler::ClassFile classFile;
    TitaniumDecompiler::Function function("TestFunction");

    // Safe defaults
    function.SetMethodAccessFlags("public");
    function.SetMethodDesc("()V");
    function.SetMaxStack(1);
    function.SetMaxLocals(1);
    function.SetFunctionCFG(std::make_shared<TitaniumDecompiler::CFG>());

    // Create at least one dummy basic block
    auto block = std::make_shared<TitaniumDecompiler::BasicBlock>();
    function.GetFunctionCFG()->m_Blocks[0] = block;

    std::string output = disassembler.ParseFunction(classFile, function);
    EXPECT_FALSE(output.empty());
}

TEST_F(DisassemblerTest, ParseFunctionInstructions) {
    TitaniumDecompiler::ClassFile classFile;
    TitaniumDecompiler::Function function("TestFunction");

    // Setup function metadata required by ParseFunctionInstructions
    function.SetMaxStack(1);
    function.SetMaxLocals(1);
    function.SetMethodAccessFlags("public");
    function.SetMethodDesc("()V");

    // Set dummy CFG to avoid null accesses
    function.SetFunctionCFG(std::make_shared<TitaniumDecompiler::CFG>());

    // Create a mock instruction (e.g., BIPUSH)
    TitaniumDecompiler::Insn dummyInsn;
    dummyInsn.opcode = TitaniumDecompiler::OPCODE_BIPUSH;
    dummyInsn.opcodeName = "bipush";
    dummyInsn.Op1.value = 42;  // Push the value 42

    std::vector<TitaniumDecompiler::Insn> instructions = {dummyInsn};

    std::string output = disassembler.ParseFunctionInstructions(
        function, classFile, instructions);
    EXPECT_FALSE(output.empty());
}

TEST_F(DisassemblerTest, GetConstantUTF8FromClass) {
    TitaniumDecompiler::ClassFile classFile;

    // Create a mock UTF8 constant entry manually
    auto utf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    utf8->length = 5;
    utf8->bytes = {'H', 'e', 'l', 'l', 'o'};

    TitaniumDecompiler::ConstPoolInfo cpEntry;
    cpEntry.Tag = TitaniumDecompiler::Tags::Utf8;
    cpEntry.Info = std::move(utf8);

    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(std::move(cpEntry));

    std::string utf8String =
        disassembler.GetConstantUTF8FromClass(classFile, 1);
    EXPECT_EQ(utf8String, "Hello");
}

TEST_F(DisassemblerTest, GetConstantClassFromClass) {
    TitaniumDecompiler::ClassFile classFile;

    // Step 1: Add UTF8 entry for class name at index 1 (idx = 2 in pool)
    auto utf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    utf8->length = 4;
    utf8->bytes = {'T', 'e', 's', 't'};

    TitaniumDecompiler::ConstPoolInfo utf8Entry;
    utf8Entry.Tag = TitaniumDecompiler::Tags::Utf8;
    utf8Entry.Info = std::move(utf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(utf8Entry));  // index 0

    // Step 2: Add Class entry pointing to UTF8 index 1 (JVM uses 1-based
    // indexing)
    auto classInfo = std::make_unique<TitaniumDecompiler::ClassInfo>();
    classInfo->nameIndex = 1;

    TitaniumDecompiler::ConstPoolInfo classEntry;
    classEntry.Tag = TitaniumDecompiler::Tags::Class;
    classEntry.Info = std::move(classInfo);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(classEntry));  // index 1

    // Step 3: Use index 2 (points to entry at [1]) to get the class name
    std::string className =
        disassembler.GetConstantClassFromClass(nullptr, classFile, 2);
    EXPECT_EQ(className, "Test");
}

TEST_F(DisassemblerTest, GetNameAndTypeFromClass) {
    TitaniumDecompiler::ClassFile classFile;

    // Step 1: UTF8 for name (index 0)
    auto nameUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    nameUtf8->length = 4;
    nameUtf8->bytes = {'n', 'a', 'm', 'e'};

    TitaniumDecompiler::ConstPoolInfo nameUtf8Entry;
    nameUtf8Entry.Tag = TitaniumDecompiler::Tags::Utf8;
    nameUtf8Entry.Info = std::move(nameUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(nameUtf8Entry));  // index 0

    // Step 2: UTF8 for descriptor (index 1)
    auto descUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    descUtf8->length = 3;
    descUtf8->bytes = {'(', ')', 'V'};

    TitaniumDecompiler::ConstPoolInfo descUtf8Entry;
    descUtf8Entry.Tag = TitaniumDecompiler::Tags::Utf8;
    descUtf8Entry.Info = std::move(descUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(descUtf8Entry));  // index 1

    // Step 3: NameAndType (index 2, but referenced via idx = 3)
    auto nameAndType = std::make_unique<TitaniumDecompiler::NameAndTypeInfo>();
    nameAndType->nameIndex = 1;        // JVM 1-based → UTF8 "name"
    nameAndType->descriptorIndex = 2;  // JVM 1-based → UTF8 "()V"

    TitaniumDecompiler::ConstPoolInfo ntEntry;
    ntEntry.Tag = TitaniumDecompiler::Tags::NameAndType;
    ntEntry.Info = std::move(nameAndType);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(ntEntry));  // index 2

    // Step 4: Call with idx = 3, which points to index 2 (NameAndType)
    std::string nameAndTypeStr =
        disassembler.GetNameAndTypeFromClass(nullptr, classFile, 3);
    EXPECT_EQ(nameAndTypeStr, "name:()V");
}

TEST_F(DisassemblerTest, InstructionsWithReferences) {
    TitaniumDecompiler::ClassFile classFile;
    TitaniumDecompiler::Function function("TestFunction");

    // Step 1: UTF8 for class name
    auto classNameUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    classNameUtf8->length = 4;
    classNameUtf8->bytes = {'T', 'e', 's', 't'};
    TitaniumDecompiler::ConstPoolInfo classNameEntry;
    classNameEntry.Tag = TitaniumDecompiler::Tags::Utf8;
    classNameEntry.Info = std::move(classNameUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(classNameEntry));  // index 0

    // Step 2: ClassInfo pointing to UTF8 "Test"
    auto classInfo = std::make_unique<TitaniumDecompiler::ClassInfo>();
    classInfo->nameIndex = 1;
    TitaniumDecompiler::ConstPoolInfo classEntry;
    classEntry.Tag = TitaniumDecompiler::Tags::Class;
    classEntry.Info = std::move(classInfo);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(classEntry));  // index 1

    // Step 3: UTF8 for field name
    auto fieldNameUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    fieldNameUtf8->length = 4;
    fieldNameUtf8->bytes = {'n', 'a', 'm', 'e'};
    TitaniumDecompiler::ConstPoolInfo fieldNameEntry;
    fieldNameEntry.Tag = TitaniumDecompiler::Tags::Utf8;
    fieldNameEntry.Info = std::move(fieldNameUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(fieldNameEntry));  // index 2

    // Step 4: UTF8 for descriptor
    auto fieldDescUtf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    fieldDescUtf8->length = 1;
    fieldDescUtf8->bytes = {'I'};
    TitaniumDecompiler::ConstPoolInfo fieldDescEntry;
    fieldDescEntry.Tag = TitaniumDecompiler::Tags::Utf8;
    fieldDescEntry.Info = std::move(fieldDescUtf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(fieldDescEntry));  // index 3

    // Step 5: NameAndType -> nameIndex = 3, descriptorIndex = 4 (JVM 1-based)
    auto nameAndType = std::make_unique<TitaniumDecompiler::NameAndTypeInfo>();
    nameAndType->nameIndex = 3;
    nameAndType->descriptorIndex = 4;
    TitaniumDecompiler::ConstPoolInfo nameTypeEntry;
    nameTypeEntry.Tag = TitaniumDecompiler::Tags::NameAndType;
    nameTypeEntry.Info = std::move(nameAndType);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(nameTypeEntry));  // index 4

    // Step 6: FieldRefInfo -> classIndex = 2, nameAndTypeIndex = 5
    auto fieldRef = std::make_unique<TitaniumDecompiler::FieldRefInfo>();
    fieldRef->classIndex = 2;
    fieldRef->nameAndTypeIndex = 5;
    TitaniumDecompiler::ConstPoolInfo fieldRefEntry;
    fieldRefEntry.Tag = TitaniumDecompiler::Tags::Field;
    fieldRefEntry.Info = std::move(fieldRef);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(fieldRefEntry));  // index 5

    // Step 7: Set up instruction to reference the FieldRef at index 6
    // (JVM-style)
    TitaniumDecompiler::Insn insn;
    insn.opcode = TitaniumDecompiler::OPCODE_GETSTATIC;
    insn.opcodeName = "getstatic";
    insn.Op1.value = 6;  // Refers to vector index 5

    std::string output =
        disassembler.InstructionsWithReferences(function, classFile, insn);
    EXPECT_FALSE(output.empty());
    EXPECT_TRUE(output.find("Test.name:I") != std::string::npos);
}

TEST_F(DisassemblerTest, LoadConstFromPool) {
    TitaniumDecompiler::ClassFile classFile;
    TitaniumDecompiler::Function function("TestFunction");

    // Step 1: Add a UTF8Info with the string "Hello"
    auto utf8 = std::make_unique<TitaniumDecompiler::UTF8Info>();
    utf8->length = 5;
    utf8->bytes = {'H', 'e', 'l', 'l', 'o'};

    TitaniumDecompiler::ConstPoolInfo utf8Entry;
    utf8Entry.Tag = TitaniumDecompiler::Tags::Utf8;
    utf8Entry.Info = std::move(utf8);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(utf8Entry));  // index 0

    // Step 2: Add a StringInfo pointing to the UTF8 at index 1
    auto stringInfo = std::make_unique<TitaniumDecompiler::StringInfo>();
    stringInfo->stringIndex = 1;

    TitaniumDecompiler::ConstPoolInfo stringEntry;
    stringEntry.Tag = TitaniumDecompiler::Tags::String;
    stringEntry.Info = std::move(stringInfo);
    classFile.m_ConstantPool.m_ConstPoolInfo.push_back(
        std::move(stringEntry));  // index 1

    // Step 3: Setup insn to reference index 2 (JVM-style)
    TitaniumDecompiler::Insn insn;
    insn.opcode = TitaniumDecompiler::OPCODE_LDC;
    insn.opcodeName = "ldc";
    insn.Op1.value = 2;  // references index 1 (StringInfo)

    std::string output =
        disassembler.LoadConstFromPool(function, classFile, insn);
    EXPECT_EQ(output, "Hello");
}

TEST_F(DisassemblerTest, ParseIfStatement) {
    TitaniumDecompiler::Insn insn;
    insn.Op1.addr = 1234;  // mock a jump target address

    std::string output = disassembler.ParseIfStatement(insn);
    EXPECT_EQ(output, "1234");
}

TEST_F(DisassemblerTest, ParseIncrement) {
    TitaniumDecompiler::Insn insn;
    // High byte = 5, Low byte = 9 → value = (5 << 8) | 9 = 1289
    insn.Op2.value = (5 << 8) | 9;

    std::string output = disassembler.ParseIncrement(insn);
    EXPECT_EQ(output, " 5, 9");
}

TEST_F(DisassemblerTest, ParseGoto) {
    TitaniumDecompiler::Insn insn;
    insn.Op1.addr = 2048;

    std::string output = disassembler.ParseGoto(insn);
    EXPECT_EQ(output, "2048");
}

TEST_F(DisassemblerTest, ParseBiPushOpCode) {
    TitaniumDecompiler::Insn insn;
    insn.Op1.value = 0;
    std::string output = disassembler.ParseBiPushOpCode(insn);
    EXPECT_EQ(output, "0");
}
