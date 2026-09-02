#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "TitaniumDecompiler/src/TitaniumDecompiler/Decompiler/Codegen.h"

namespace TitaniumDecompiler {

class CodegenTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up any necessary preconditions for the tests
    }

    void TearDown() override {
        // Clean up after tests
    }
};

TEST_F(CodegenTest, TestIINCOutput) {
    // Redirect std::cout to capture the output
    std::ostringstream output;
    std::streambuf* oldCoutBuffer = std::cout.rdbuf(output.rdbuf());

    // Create a dummy instruction for OPCODE_IINC
    Insn instruction;
    instruction.opcode = OPCODE_IINC;
    instruction.Op2.value = (1 << 8) | 5;  // Example: localIdx = 1, amount = 5

    // Create a Codegen instance
    Codegen codegen;

    // Call the ReadInstruction method
    codegen.ReadInstruction(instruction);

    // Restore std::cout
    std::cout.rdbuf(oldCoutBuffer);

    // Check the captured output
    EXPECT_EQ(output.str(), "IINC\n");
}

}  // namespace TitaniumDecompiler
