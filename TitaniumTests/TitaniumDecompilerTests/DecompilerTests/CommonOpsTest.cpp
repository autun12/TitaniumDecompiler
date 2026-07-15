#include <gtest/gtest.h>
#include "TitaniumDecompiler/src/TitaniumDecompiler/Decompiler/CommonOps.h"

using namespace TitaniumDecompiler;

TEST(CommonOpsTest, UnaryOpPosition_ReturnsCorrectPosition) {
    EXPECT_EQ(Position(UnaryOp::Neg), UnaryOpPosition::Prefix);
    EXPECT_EQ(Position(UnaryOp::ArrayLength), UnaryOpPosition::Postfix);
    EXPECT_EQ(Position(UnaryOp::PlusPlus), UnaryOpPosition::Postfix);
    EXPECT_EQ(Position(UnaryOp::MinusMinus), UnaryOpPosition::Postfix);
    EXPECT_EQ(Position(UnaryOp::Negate), UnaryOpPosition::Prefix);
}

TEST(CommonOpsTest, IsPrefix_ReturnsTrueForPrefixOps) {
    EXPECT_TRUE(IsPrefix(UnaryOp::Neg));
    EXPECT_TRUE(IsPrefix(UnaryOp::Negate));
}

TEST(CommonOpsTest, IsPrefix_ReturnsFalseForPostfixOps) {
    EXPECT_FALSE(IsPrefix(UnaryOp::ArrayLength));
    EXPECT_FALSE(IsPrefix(UnaryOp::PlusPlus));
    EXPECT_FALSE(IsPrefix(UnaryOp::MinusMinus));
}

TEST(CommonOpsTest, UnaryOpToString_ProducesCorrectStrings) {
    EXPECT_EQ(UnaryOpToString(UnaryOp::Neg), "-");
    EXPECT_EQ(UnaryOpToString(UnaryOp::ArrayLength), ".length");
    EXPECT_EQ(UnaryOpToString(UnaryOp::PlusPlus), "++;");
    EXPECT_EQ(UnaryOpToString(UnaryOp::MinusMinus), "--;");
    EXPECT_EQ(UnaryOpToString(UnaryOp::Negate), "!");
}

TEST(CommonOpsTest, BinOpToString_ProducesExpectedSymbols) {
    EXPECT_EQ(BinOpToString(BinaryOp::Add), "+");
    EXPECT_EQ(BinOpToString(BinaryOp::Sub), "-");
    EXPECT_EQ(BinOpToString(BinaryOp::Mul), "*");
    EXPECT_EQ(BinOpToString(BinaryOp::Div), "/");
    EXPECT_EQ(BinOpToString(BinaryOp::Rem), "%");
    EXPECT_EQ(BinOpToString(BinaryOp::Shl), "<<");
    EXPECT_EQ(BinOpToString(BinaryOp::Shr), ">>");
    EXPECT_EQ(BinOpToString(BinaryOp::UShr), ">>>");
    EXPECT_EQ(BinOpToString(BinaryOp::And), "&");
    EXPECT_EQ(BinOpToString(BinaryOp::Or), "|");
    EXPECT_EQ(BinOpToString(BinaryOp::LogicalAnd), "&&");
    EXPECT_EQ(BinOpToString(BinaryOp::LogicalOr), "||");
    EXPECT_EQ(BinOpToString(BinaryOp::Xor), "^");
    EXPECT_EQ(BinOpToString(BinaryOp::InstanceOf), "instanceof");
    EXPECT_EQ(BinOpToString(BinaryOp::Equal), "==");
    EXPECT_EQ(BinOpToString(BinaryOp::NotEqual), "!=");
    EXPECT_EQ(BinOpToString(BinaryOp::GreaterThan), ">");
    EXPECT_EQ(BinOpToString(BinaryOp::LessThan), "<");
    EXPECT_EQ(BinOpToString(BinaryOp::GreaterEqualThan), ">=");
    EXPECT_EQ(BinOpToString(BinaryOp::LessEqualThan), "<=");
}

TEST(CommonOpsTest, BinOpToString_ReturnsUnknownForInvalidEnum) {
    BinaryOp invalidOp = static_cast<BinaryOp>(999);
    EXPECT_EQ(BinOpToString(invalidOp), "Unknown");
}
