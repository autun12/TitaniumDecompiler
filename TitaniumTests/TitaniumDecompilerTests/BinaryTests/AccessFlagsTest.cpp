#include <gtest/gtest.h>
#include "Platform/Linux/FileFormats/JVM/AccessFlags.h"

using namespace TitaniumDecompiler;

TEST(AccessFlagsTest, ParseJavaClassAccessFlag_CombinedFlags) {
    int access = ACC_PUBLIC | ACC_SUPER | ACC_FINAL;
    std::string result = ParseJavaClassAccessFlag(access);
    EXPECT_NE(result.find("public"), std::string::npos);
    EXPECT_NE(result.find("super"), std::string::npos);
    EXPECT_NE(result.find("final"), std::string::npos);
}

TEST(AccessFlagsTest, ParseJavaFieldAccessFlag_AllFlags) {
    int access = ACC_PUBLIC | ACC_STATIC | ACC_FINAL | ACC_ENUM;
    std::string result = ParseJavaFieldAccessFlag(access);
    EXPECT_NE(result.find("public"), std::string::npos);
    EXPECT_NE(result.find("static"), std::string::npos);
    EXPECT_NE(result.find("final"), std::string::npos);
    EXPECT_NE(result.find("enum"), std::string::npos);
}

TEST(AccessFlagsTest, ParseJavaMethodsAccessFlag_AllFlags) {
    int access = ACC_PUBLIC | ACC_STATIC | ACC_ABSTRACT | ACC_NATIVE | ACC_SYNCHRONIZED;
    std::string result = ParseJavaMethodsAccessFlag(access);
    EXPECT_NE(result.find("public"), std::string::npos);
    EXPECT_NE(result.find("static"), std::string::npos);
    EXPECT_NE(result.find("abstract"), std::string::npos);
    EXPECT_NE(result.find("native"), std::string::npos);
    EXPECT_NE(result.find("synchronized"), std::string::npos);
}

TEST(AccessFlagsTest, ParseJavaClassAccessFlag_Empty) {
    int access = 0;
    std::string result = ParseJavaClassAccessFlag(access);
    EXPECT_TRUE(result.empty());
}

TEST(AccessFlagsTest, ParseJavaFieldAccessFlag_Empty) {
    int access = 0;
    std::string result = ParseJavaFieldAccessFlag(access);
    EXPECT_TRUE(result.empty());
}

TEST(AccessFlagsTest, ParseJavaMethodsAccessFlag_Empty) {
    int access = 0;
    std::string result = ParseJavaMethodsAccessFlag(access);
    EXPECT_TRUE(result.empty());
}
