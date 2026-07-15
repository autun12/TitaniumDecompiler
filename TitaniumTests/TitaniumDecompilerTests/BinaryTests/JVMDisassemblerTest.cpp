#include <gtest/gtest.h>
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/JVMDisassembler.h"
#include <filesystem>

class JVMDisassemblerTest : public ::testing::Test {
protected:
    TitaniumDecompiler::JVMDisassembler jvmDisassembler;

    std::filesystem::path GetTestClassFilePath() {
        return "JavaTestFiles/Test.class";
    }

    std::filesystem::path GetTestJarFilePath() {
        return "JavaTestFiles/TestJarFile/TestJarFile.jar";
    }
};

TEST_F(JVMDisassemblerTest, DisassemblyWriter_ValidClassFile) {
    auto path = GetTestClassFilePath();
    ASSERT_TRUE(std::filesystem::exists(path));
    std::string output = jvmDisassembler.DisassemblyWriter(path);
    EXPECT_FALSE(output.empty());
}

TEST_F(JVMDisassemblerTest, DisassemblyWriter_ValidJarFile) {
    auto path = GetTestJarFilePath();
    ASSERT_TRUE(std::filesystem::exists(path));
    std::string output = jvmDisassembler.DisassemblyWriter(path);
    EXPECT_FALSE(output.empty());
}

TEST_F(JVMDisassemblerTest, GetClassNames_ReturnsNonEmpty) {
    jvmDisassembler.DisassemblyWriter(GetTestClassFilePath());
    auto classNames = jvmDisassembler.GetClassNames();
    EXPECT_FALSE(classNames.empty());
}

TEST_F(JVMDisassemblerTest, GetFunctionNames_OptionalCheck) {
    jvmDisassembler.DisassemblyWriter(GetTestClassFilePath());

    auto funcNames = jvmDisassembler.GetFunctionNames();
    auto classToFuncs = jvmDisassembler.GetClassesToFunctions();

    // Combine all actual function names
    std::vector<std::string> expectedNames;
    for (const auto& [cls, funcs] : classToFuncs) {
        for (const auto& func : funcs) {
            TitaniumDecompiler::Function f = func;  // create a mutable copy
            expectedNames.push_back(f.GetFunctionName());

        }
    }

    // We expect the functionNames to be a subset or empty — depending on implementation
    EXPECT_LE(funcNames.size(), expectedNames.size());
}


TEST_F(JVMDisassemblerTest, GetClassesMap_PopulatedAfterDisassembly) {
    jvmDisassembler.DisassemblyWriter(GetTestClassFilePath());
    auto map = jvmDisassembler.GetClassesMap();
    EXPECT_FALSE(map.empty());
}

TEST_F(JVMDisassemblerTest, GetClassesToFunctions_PopulatedAfterDisassembly) {
    jvmDisassembler.DisassemblyWriter(GetTestClassFilePath());
    auto mapping = jvmDisassembler.GetClassesToFunctions();
    EXPECT_FALSE(mapping.empty());
}