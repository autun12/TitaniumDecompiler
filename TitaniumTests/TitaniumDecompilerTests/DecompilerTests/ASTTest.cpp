#include <gtest/gtest.h>
#include "TitaniumDecompiler/src/TitaniumDecompiler/Decompiler/AST.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/Function.h"

using namespace TitaniumDecompiler;

TEST(ASTTest, GetType_ValidTypes) {
    EXPECT_EQ(getType(4).kind, Type::Boolean);
    EXPECT_EQ(getType(5).kind, Type::Char);
    EXPECT_EQ(getType(6).kind, Type::Float);
    EXPECT_EQ(getType(7).kind, Type::Double);
    EXPECT_EQ(getType(8).kind, Type::Byte);
    EXPECT_EQ(getType(9).kind, Type::Short);
    EXPECT_EQ(getType(10).kind, Type::Int);
    EXPECT_EQ(getType(11).kind, Type::Long);
}

TEST(ASTTest, GetType_InvalidTypeThrows) {
    EXPECT_THROW(getType(3), std::runtime_error);
    EXPECT_THROW(getType(42), std::runtime_error);
}

TEST(ASTTest, ParseSingleType_Primitives) {
    std::string input = "IZCBS";
    std::string::const_iterator it = input.begin();  // FIXED: use const_iterator
    EXPECT_EQ(parseSingleType(it, input.end())->kind, Type::Int);
    EXPECT_EQ(parseSingleType(it, input.end())->kind, Type::Boolean);
    EXPECT_EQ(parseSingleType(it, input.end())->kind, Type::Char);
    EXPECT_EQ(parseSingleType(it, input.end())->kind, Type::Byte);
    EXPECT_EQ(parseSingleType(it, input.end())->kind, Type::Short);
}

TEST(ASTTest, ParseSingleType_Void) {
    std::string input = "V";
    std::string::const_iterator it = input.begin();
    EXPECT_EQ(parseSingleType(it, input.end())->kind, Type::Void);
}

TEST(ASTTest, ParseSingleType_ClassType_DoesNotThrow) {
    std::string input = "Ljava/lang/String;";
    std::string::const_iterator it = input.begin();
    EXPECT_NO_THROW({
        auto parsed = parseSingleType(it, input.end());
        EXPECT_TRUE(parsed.has_value());
    });
}

TEST(ASTTest, ParseSingleType_ArrayType_DoesNotThrow) {
    std::string input = "[I";
    std::string::const_iterator it = input.begin();
    EXPECT_NO_THROW({
        auto parsed = parseSingleType(it, input.end());
        EXPECT_TRUE(parsed.has_value());
    });
}

TEST(ASTTest, ParseSingleType_InvalidArrayTypeThrows) {
    std::string input = "[";
    std::string::const_iterator it = input.begin();
    EXPECT_THROW(parseSingleType(it, input.end()), std::runtime_error);
}