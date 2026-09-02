// FunctionTest.cpp

#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "FileFormats/JVM/Methods.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/Function.h"

// Test default initialization of a Function
TEST(FunctionTest, DefaultInitialization) {
    TitaniumDecompiler::Function func("InitialFunction");
    EXPECT_EQ(func.GetFunctionName(), "InitialFunction");
    // By default, CFG should be null.
    EXPECT_EQ(func.GetFunctionCFG(), nullptr);
    // And local variable map should be empty.
    EXPECT_TRUE(func.GetLocals().empty());
}

// Test updating the function name
TEST(FunctionTest, UpdateFunctionName) {
    TitaniumDecompiler::Function func("InitialFunction");
    func.SetFunctionName("UpdatedFunction");
    EXPECT_EQ(func.GetFunctionName(), "UpdatedFunction");
}

// Test setting and retrieving a CFG pointer
TEST(FunctionTest, SetAndGetCFG) {
    TitaniumDecompiler::Function func("TestFunction");
    // Create a dummy CFG instance.
    std::shared_ptr<TitaniumDecompiler::CFG> dummyCFG =
        std::make_shared<TitaniumDecompiler::CFG>();
    func.SetFunctionCFG(dummyCFG);
    EXPECT_EQ(func.GetFunctionCFG(), dummyCFG);
}

// Test setting and getting method properties using a Methods instance
TEST(FunctionTest, SetAndGetMethodProperties) {
    TitaniumDecompiler::Function func("TestFunction");
    TitaniumDecompiler::Methods method;
    // Set some test values.
    method.m_AccessFlags = 0x0010;
    method.m_NameIndex = 42;
    method.m_DescIndex = 84;

    func.SetMethod(method);
    func.SetMethodDesc("()V");
    func.SetMethodAccessFlags("public");

    TitaniumDecompiler::Methods retrievedMethod = func.GetMethod();
    EXPECT_EQ(retrievedMethod.m_AccessFlags, 0x0010);
    EXPECT_EQ(retrievedMethod.m_NameIndex, 42);
    EXPECT_EQ(retrievedMethod.m_DescIndex, 84);
    EXPECT_EQ(func.GetMethodDesc(), "()V");
    EXPECT_EQ(func.GetMethodAccessFlags(), "public");
}

// Test setting and getting maximum stack and locals
TEST(FunctionTest, SetAndGetStackAndLocals) {
    TitaniumDecompiler::Function func("TestFunction");
    func.SetMaxStack(10);
    func.SetMaxLocals(20);

    EXPECT_EQ(func.GetMaxStack(), 10);
    EXPECT_EQ(func.GetMaxLocals(), 20);
}

// Test inserting entries into the reference constant pool
TEST(FunctionTest, InsertIntoRefConstPool) {
    TitaniumDecompiler::Function func("TestFunction");
    func.InsertIntoRefConstPool(1, "const1");
    func.InsertIntoRefConstPool(2, "const2");

    auto refPool = func.GetRefConstPool();
    EXPECT_EQ(refPool.count(1), 1);
    EXPECT_EQ(refPool.count(2), 1);

    // Check that the inserted values match.
    auto range1 = refPool.equal_range(1);
    bool foundConst1 = false;
    for (auto it = range1.first; it != range1.second; ++it) {
        if (it->second == "const1") {
            foundConst1 = true;
            break;
        }
    }
    EXPECT_TRUE(foundConst1);

    auto range2 = refPool.equal_range(2);
    bool foundConst2 = false;
    for (auto it = range2.first; it != range2.second; ++it) {
        if (it->second == "const2") {
            foundConst2 = true;
            break;
        }
    }
    EXPECT_TRUE(foundConst2);
}
