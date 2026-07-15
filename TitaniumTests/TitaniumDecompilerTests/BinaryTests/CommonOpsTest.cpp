#include "gtest/gtest.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Decompiler/CommonOps.h"

using namespace TitaniumDecompiler;

TEST(CommonOpsTest, PositionReturnsCorrectValues) {
    EXPECT_EQ(Position(Neg), UnaryOpPosition::Prefix);
    EXPECT_EQ(Position(ArrayLength), UnaryOpPosition::Postfix);
    EXPECT_EQ(Position(PlusPlus), UnaryOpPosition::Postfix);
    EXPECT_EQ(Position(MinusMinus), UnaryOpPosition::Postfix);
    EXPECT_EQ(Position(Negate), UnaryOpPosition::Prefix);
}

TEST(CommonOpsTest, IsPrefixReturnsCorrectValues) {
    EXPECT_TRUE(IsPrefix(Neg));
    EXPECT_TRUE(IsPrefix(Negate));
    EXPECT_FALSE(IsPrefix(ArrayLength));
    EXPECT_FALSE(IsPrefix(PlusPlus));
    EXPECT_FALSE(IsPrefix(MinusMinus));
}

TEST(CommonOpsTest, UnaryOpToStringReturnsCorrectValues) {
    EXPECT_EQ(UnaryOpToString(Neg), "-");
    EXPECT_EQ(UnaryOpToString(ArrayLength), ".length");
    EXPECT_EQ(UnaryOpToString(PlusPlus), "++;");
    EXPECT_EQ(UnaryOpToString(MinusMinus), "--;");
    EXPECT_EQ(UnaryOpToString(Negate), "!");
}

TEST(CommonOpsTest, BinOpToStringReturnsCorrectValues) {
    EXPECT_EQ(BinOpToString(Add), "+");
    EXPECT_EQ(BinOpToString(Sub), "-");
    EXPECT_EQ(BinOpToString(Mul), "*");
    EXPECT_EQ(BinOpToString(Div), "/");
    EXPECT_EQ(BinOpToString(Rem), "%");
    EXPECT_EQ(BinOpToString(Shl), "<<");
    EXPECT_EQ(BinOpToString(Shr), ">>");
    EXPECT_EQ(BinOpToString(UShr), ">>>");
    EXPECT_EQ(BinOpToString(And), "&");
    EXPECT_EQ(BinOpToString(Or), "|");
    EXPECT_EQ(BinOpToString(LogicalAnd), "&&");
    EXPECT_EQ(BinOpToString(LogicalOr), "||");
    EXPECT_EQ(BinOpToString(Xor), "^");
    EXPECT_EQ(BinOpToString(InstanceOf), "instanceof");
    EXPECT_EQ(BinOpToString(Equal), "==");
    EXPECT_EQ(BinOpToString(NotEqual), "!=");
    EXPECT_EQ(BinOpToString(GreaterThan), ">");
    EXPECT_EQ(BinOpToString(LessThan), "<");
    EXPECT_EQ(BinOpToString(GreaterEqualThan), ">=");
    EXPECT_EQ(BinOpToString(LessEqualThan), "<=");
    // Verify that an undefined BinaryOp returns "Unknown"
    EXPECT_EQ(BinOpToString(static_cast<BinaryOp>(999)), "Unknown");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
