#include <gtest/gtest.h>

#include "TitaniumDecompiler/src/Disassembler/Providers/JVM/JVMDisassembler.h"
#include "TitaniumDecompiler/src/FileFormats/JVM/Instruction.h"

using namespace TitaniumDecompiler;

TEST(InstructionTest, DecodeSimpleNopInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_NOP};
    InstrMap instructions = ReadInstructions(bytes);

    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "nop");
}

TEST(InstructionTest, DecodeBipushInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_BIPUSH, 42};
    InstrMap instructions = ReadInstructions(bytes);

    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "bipush");
    EXPECT_EQ(insn.Op1.value, 42);
    EXPECT_EQ(insn.group, GROUP_GENERAL);
}

TEST(InstructionTest, DecodeGotoInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_GOTO, 0x00, 0x03};
    InstrMap instructions = ReadInstructions(bytes);

    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "goto");
    EXPECT_EQ(insn.group, GROUP_JUMP);
    EXPECT_EQ(insn.Op1.addr, 3);  // current PC = 0, offset = 3
}

TEST(InstructionTest, DecodeIfInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_IFEQ, 0x00, 0x05};
    InstrMap instructions = ReadInstructions(bytes);

    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "ifeq");
    EXPECT_EQ(insn.group, GROUP_JUMP);
    EXPECT_EQ(insn.Op1.addr, 5);  // jump offset from PC
}

TEST(InstructionTest, DecodeInvokeStaticReference) {
    std::vector<uint8_t> bytes = {OPCODE_INVOKESTATIC, 0x00, 0x02};
    InstrMap instructions = ReadInstructions(bytes);

    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "invokestatic");
    EXPECT_EQ(insn.Op1.value, 2);  // constant pool reference
    EXPECT_EQ(insn.group, GROUP_INVOCATION);
}

TEST(InstructionTest, IncrementInstructionExtractsCorrectValue) {
    std::vector<uint8_t> bytes = {OPCODE_IINC, 0x01, 0x02};
    InstrMap instructions = ReadInstructions(bytes);

    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "iinc");
    EXPECT_EQ(insn.group, GROUP_GENERAL);
    EXPECT_EQ(insn.Op2.value, 0x0102);  // high: 0x01, low: 0x02
}

TEST(InstructionTest, LoadDataConstantValidIndex) {
    std::vector<uint8_t> bytes = {OPCODE_LDC, 0x0A};
    uint8_t value = LoadDataConstant(bytes, 0);
    EXPECT_EQ(value, 0x0A);
}

TEST(InstructionTest, ReferencesInstructionReturnsCorrectValue) {
    std::vector<uint8_t> bytes = {OPCODE_GETSTATIC, 0x01, 0x02};
    uint64_t ref = ReferencesInstruction(bytes, 0);
    EXPECT_EQ(ref, 0x0102);
}

TEST(InstructionTest, GotoInstructionComputesTargetCorrectly) {
    std::vector<uint8_t> bytes = {OPCODE_GOTO, 0x00, 0x06};
    int16_t addr = GotoInstruction(bytes, 0);
    EXPECT_EQ(addr, 6);
}

TEST(InstructionTest, BiPushInstructionReadsCorrectByte) {
    std::vector<uint8_t> bytes = {OPCODE_BIPUSH, 0x2A};
    uint8_t val = BiPushInstruction(bytes, 0);
    EXPECT_EQ(val, 0x2A);
}
// ---------- LoadDataConstantWide ----------
TEST(InstructionTest, LoadDataConstantWideReadsCorrectValue) {
    std::vector<uint8_t> bytes = {
        0x00, 0x01,
        0x2A};  // index1 = 0x01, index2 = 0x2A → 298, but clipped to 42
    uint8_t val = LoadDataConstantWide(bytes, 0);
    EXPECT_EQ(
        val,
        42);  // because 298 gets clipped to 42 (0x2A) by uint8_t return type
}

TEST(InstructionTest, LoadDataConstantWideTooShortReturnsZero) {
    std::vector<uint8_t> bytes = {0x01};
    uint8_t val = LoadDataConstantWide(bytes, 0);
    EXPECT_EQ(val, 0);
}

// ---------- Error Conditions ----------
TEST(InstructionTest, GotoInstructionTooShortReturnsZero) {
    std::vector<uint8_t> bytes = {OPCODE_GOTO};
    int16_t addr = GotoInstruction(bytes, 0);
    EXPECT_EQ(addr, 0);
}

TEST(InstructionTest, IfStatementTooShortReturnsZero) {
    std::vector<uint8_t> bytes = {OPCODE_IFEQ};
    int16_t offset = IfStatement(bytes, 0);
    EXPECT_EQ(offset, 0);
}

TEST(InstructionTest, ReferencesInstructionTooShortReturnsZero) {
    std::vector<uint8_t> bytes = {OPCODE_GETSTATIC};
    uint64_t ref = ReferencesInstruction(bytes, 0);
    EXPECT_EQ(ref, 0);
}

// ---------- Tableswitch ----------
TEST(InstructionTest, DecodeTableSwitchInstructionMinimalPadded) {
    std::vector<uint8_t> bytes = {
        OPCODE_TABLESWITCH,
        0x00,
        0x00,  // padding
        0x00,
        0x00,
        0x00,
        0x0A,  // default
        0x00,
        0x00,
        0x00,
        0x01,  // low
        0x00,
        0x00,
        0x00,
        0x01,  // high
        0x00,
        0x00,
        0x00,
        0x01  // jump offset
    };
    InstrMap instructions = ReadInstructions(bytes);
    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "tableswitch");
}

// ---------- Multianewarray ----------
TEST(InstructionTest, DecodeMultiANewArrayInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_MULTIANEWARRAY, 0x00, 0x01, 0x02};
    InstrMap instructions = ReadInstructions(bytes);
    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.group, GROUP_GENERAL);
    EXPECT_EQ(insn.opcodeName, "multianewarray");
}

// ---------- ANewArray ----------
TEST(InstructionTest, DecodeANewArrayInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_ANEWARRAY, 0x00, 0x02};
    InstrMap instructions = ReadInstructions(bytes);
    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "anewarray");
    EXPECT_EQ(insn.Op1.value, 2);
    EXPECT_EQ(insn.group, GROUP_GENERAL);
}

// ---------- NewArray ----------
TEST(InstructionTest, DecodeNewArrayInstruction) {
    std::vector<uint8_t> bytes = {OPCODE_NEWARRAY, 0x01};
    InstrMap instructions = ReadInstructions(bytes);
    ASSERT_EQ(instructions.size(), 1);
    auto insn = instructions.begin()->second;
    EXPECT_EQ(insn.opcodeName, "newarray");
    EXPECT_EQ(insn.Op1.value, 1);
    EXPECT_EQ(insn.group, GROUP_GENERAL);
}

// ---------- LoadIndex helper ----------
TEST(InstructionTest, LoadIndexAssignsAddressCorrectly) {
    std::vector<uint8_t> bytes = {0x00, 0x42};  // dummy
    Insn insn;
    bool result = LoadIndex(insn, bytes, 0);
    EXPECT_TRUE(result);
    EXPECT_EQ(insn.Op1.addr, 0x42);
}
