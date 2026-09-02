#include <gtest/gtest.h>

#include "FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/BasicBlock.h"

using namespace TitaniumDecompiler;

class BasicBlockTest : public ::testing::Test {
protected:
    std::shared_ptr<BasicBlock> block;

    void SetUp() override { block = std::make_shared<BasicBlock>(); }
};

TEST_F(BasicBlockTest, DefaultIDIsZero) { EXPECT_EQ(block->m_ID, 0); }

TEST_F(BasicBlockTest, CanSetAndGetID) {
    block->m_ID = 42;
    EXPECT_EQ(block->m_ID, 42);
}

TEST_F(BasicBlockTest, CanAddInstruction) {
    Insn insn;
    insn.opcode = 0x60;  // iadd
    insn.opcodeName = "iadd";

    block->AddInstruction(insn);
    ASSERT_EQ(block->GetInstructions().size(), 1);
    EXPECT_EQ(block->GetInstructions()[0].opcodeName, "iadd");
}

TEST_F(BasicBlockTest, CanAddSuccessor) {
    auto succ = std::make_shared<BasicBlock>();
    block->AddSuccessor(succ);
    EXPECT_EQ(block->GetSuccessor().size(), 1);
    EXPECT_EQ(block->GetSuccessor()[0], succ);
}

TEST_F(BasicBlockTest, CanReplaceSuccessor) {
    auto succ1 = std::make_shared<BasicBlock>();
    auto succ2 = std::make_shared<BasicBlock>();
    block->AddSuccessor(succ1);
    block->ReplaceSuccessor(succ1, succ2);
    ASSERT_EQ(block->GetSuccessor().size(), 1);
    EXPECT_EQ(block->GetSuccessor()[0], succ2);
}

TEST_F(BasicBlockTest, ReplaceSuccessorWithNonexistentDoesNothing) {
    auto succ1 = std::make_shared<BasicBlock>();
    auto succ2 = std::make_shared<BasicBlock>();
    block->ReplaceSuccessor(succ1, succ2);  // no effect
    EXPECT_TRUE(block->GetSuccessor().empty());
}

TEST_F(BasicBlockTest, ReplaceSuccessorAlsoWorksForExceptionSuccessors) {
    auto oldBlock = std::make_shared<BasicBlock>();
    auto newBlock = std::make_shared<BasicBlock>();
    block->AddSuccessorException(oldBlock);
    block->ReplaceSuccessor(oldBlock, newBlock);

    EXPECT_EQ(block->GetSuccessorExceptions()[0], newBlock);
    EXPECT_TRUE(oldBlock->GetPredExceptions().empty());
    EXPECT_EQ(newBlock->GetPredExceptions()[0], block);
}

TEST_F(BasicBlockTest, AddPredecessorAddsCorrectly) {
    auto pred = std::make_shared<BasicBlock>();
    block->AddPredecessor(pred);
    EXPECT_EQ(block->GetPreds().size(), 1);
    EXPECT_EQ(block->GetPreds()[0], pred);
}

TEST_F(BasicBlockTest, AddSuccessorAlsoAddsToPredecessor) {
    auto succ = std::make_shared<BasicBlock>();
    block->AddSuccessor(succ);
    EXPECT_EQ(block->GetSuccessor().size(), 1);
    EXPECT_EQ(succ->GetPreds().size(), 1);
    EXPECT_EQ(succ->GetPreds()[0], block);
}

TEST_F(BasicBlockTest, AddSuccessorExceptionWorksAndAvoidsDuplicates) {
    auto exc = std::make_shared<BasicBlock>();
    block->AddSuccessorException(exc);
    EXPECT_EQ(block->GetSuccessorExceptions().size(), 1);
    block->AddSuccessorException(exc);  // Should not duplicate
    EXPECT_EQ(block->GetSuccessorExceptions().size(), 1);
    EXPECT_EQ(exc->GetPredExceptions().size(), 1);
}

TEST_F(BasicBlockTest, RemoveSuccessorRemovesCorrectlyAndUpdatesPred) {
    auto succ = std::make_shared<BasicBlock>();
    block->AddSuccessor(succ);
    EXPECT_EQ(block->GetSuccessor().size(), 1);
    block->RemoveSuccessor(succ);
    EXPECT_TRUE(block->GetSuccessor().empty());
    EXPECT_TRUE(succ->GetPreds().empty());
}

TEST_F(BasicBlockTest, RemoveSuccessorExceptionRemovesCorrectlyAndUpdatesPred) {
    auto succ = std::make_shared<BasicBlock>();
    block->AddSuccessorException(succ);
    block->RemoveSuccessorException(succ);
    EXPECT_TRUE(block->GetSuccessorExceptions().empty());
    EXPECT_TRUE(succ->GetPredExceptions().empty());
}

TEST_F(BasicBlockTest, RemovePredecessorWorks) {
    auto pred = std::make_shared<BasicBlock>();
    block->AddPredecessor(pred);
    block->RemovePredecessor(pred);
    EXPECT_TRUE(block->GetPreds().empty());
}

TEST_F(BasicBlockTest, RemovePredecessorExceptionWorks) {
    auto pred = std::make_shared<BasicBlock>();
    block->AddPredecessorException(pred);
    block->RemovePredecessorException(pred);
    EXPECT_TRUE(block->GetPredExceptions().empty());
}

TEST_F(BasicBlockTest, GetLastInstructionReturnsCorrectInsn) {
    Insn insn1;
    insn1.opcodeName = "iconst_0";
    Insn insn2;
    insn2.opcodeName = "iconst_1";
    block->AddInstruction(insn1);
    block->AddInstruction(insn2);
    Insn last = block->GetLastInstruction();
    EXPECT_EQ(last.opcodeName, "iconst_1");
}
