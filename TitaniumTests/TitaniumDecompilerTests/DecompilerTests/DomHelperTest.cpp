#include <gtest/gtest.h>

#include "TitaniumDecompiler/src/FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Decompiler/ASTNodes.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Decompiler/DomHelper.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/BasicBlock.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/CFG.h"

using namespace TitaniumDecompiler;

class DomHelperTest : public ::testing::Test {
protected:
    CFG cfg;
    std::shared_ptr<BasicBlock> b0, b1, b2, b3;

    void SetUp() override {
        b0 = std::make_shared<BasicBlock>();
        b0->m_ID = 0;
        b1 = std::make_shared<BasicBlock>();
        b1->m_ID = 1;
        b2 = std::make_shared<BasicBlock>();
        b2->m_ID = 2;
        b3 = std::make_shared<BasicBlock>();
        b3->m_ID = 3;

        b0->AddSuccessor(b1);
        b1->AddSuccessor(b2);
        b2->AddSuccessor(b3);
        b3->AddSuccessor(b1);  // back edge: loop

        // Add mock instructions to populate AST
        Insn i1;
        i1.opcodeName = "iadd";
        i1.group = GROUP_GENERAL;
        Insn i2;
        i2.opcodeName = "istore_1";
        i2.group = GROUP_GENERAL;
        Insn i3;
        i3.opcodeName = "if_icmpge";
        i3.group = GROUP_JUMP;
        Insn i4;
        i4.opcodeName = "return";
        i4.group = GROUP_RETURN;

        b0->AddInstruction(i1);
        b1->AddInstruction(i2);
        b2->AddInstruction(i3);
        b3->AddInstruction(i4);

        cfg.m_Blocks[0] = b0;
        cfg.m_Blocks[1] = b1;
        cfg.m_Blocks[2] = b2;
        cfg.m_Blocks[3] = b3;
    }
};

TEST_F(DomHelperTest, ComputeDominatorTreeSetsCorrectly) {
    DomHelper helper(cfg);
    helper.Compute();

    auto doms = helper.GetDoms();
    EXPECT_TRUE(doms[0] == std::set<size_t>{0});
    EXPECT_TRUE(doms[1].count(0));
    EXPECT_TRUE(doms[2].count(1));
    EXPECT_TRUE(doms[3].count(2));
}

TEST_F(DomHelperTest, LoopDetectionFindsBackEdge) {
    DomHelper helper(cfg);
    helper.Compute();

    auto backEdges = helper.FindBackEdges();
    ASSERT_EQ(backEdges.size(), 1);

    std::pair<size_t, size_t> expectedEdge(3, 1);
    EXPECT_EQ(backEdges[0], expectedEdge);
}

TEST_F(DomHelperTest, FindLoopIncludesCorrectBlocks) {
    DomHelper helper(cfg);
    helper.Compute();

    auto loop = helper.FindLoop(1, 3);
    std::set<size_t> expected = {1, 2, 3};
    for (size_t b : expected) {
        EXPECT_TRUE(loop.count(b));
    }
}

TEST_F(DomHelperTest, GenerateStructuredCodeReturnsFormattedCode) {
    DomHelper helper(cfg);
    helper.Compute();

    std::unordered_map<int, std::shared_ptr<BasicBlock>> blockMap = {
        {0, b0}, {1, b1}, {2, b2}, {3, b3}};

    auto domTree = helper.GetDoms();
    auto ast = helper.BuildASTFromDomTree(b0, domTree, blockMap);
    std::string code = helper.GenerateStructuredCode(ast, 0);

    ASSERT_FALSE(code.empty());
    EXPECT_NE(code.find("iadd;"), std::string::npos);
    EXPECT_NE(code.find("istore_1;"), std::string::npos);
    EXPECT_NE(code.find("if_icmpge"), std::string::npos);
    EXPECT_NE(code.find("return return;"), std::string::npos);
}
