#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/ExceptionRangeCFG.h"
#include "TitaniumDecompiler/src/TitaniumDecompiler/Disassembler/BasicBlock.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

using namespace TitaniumDecompiler;

#include <iostream>

//TODO : Fix this test, can't add exception through the constructor
// TEST(ExceptionRangeCFGTest, AddExceptionType_ValidType) {
//     auto handler = std::make_shared<BasicBlock>();
//     std::vector<std::shared_ptr<BasicBlock>> protectedRange = {std::make_shared<BasicBlock>()};

//     // Bypass broken constructor logic: create object, then add "seed" type manually
//     ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler, "Seed");

//     // Confirm it's there
//     auto initial = exceptionRangeCFG.GetExceptionTypes();
//     std::cout << "After seed: size = " << initial.size() << std::endl;

//     exceptionRangeCFG.AddExceptionType("IOException");

//     std::vector<std::string> exceptionTypes = exceptionRangeCFG.GetExceptionTypes();
//     std::cout << "Final size: " << exceptionTypes.size() << std::endl;
//     for (const auto& ex : exceptionTypes) {
//         std::cout << "  type: " << ex << std::endl;
//     }

//     ASSERT_EQ(exceptionTypes.size(), 2);
//     EXPECT_EQ(exceptionTypes[1], "IOException");
// }



TEST(ExceptionRangeCFGTest, AddExceptionType_EmptyType) {
    auto handler = std::make_shared<BasicBlock>();
    std::vector<std::shared_ptr<BasicBlock>> protectedRange = {std::make_shared<BasicBlock>()};
    ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler, std::vector<std::string>{});

    exceptionRangeCFG.AddExceptionType("");

    std::vector<std::string> exceptionTypes = exceptionRangeCFG.GetExceptionTypes();
    ASSERT_TRUE(exceptionTypes.empty());
}

TEST(ExceptionRangeCFGTest, IsCircular_True) {
    auto block = std::make_shared<BasicBlock>();
    std::vector<std::shared_ptr<BasicBlock>> protectedRange = {block};
    ExceptionRangeCFG exceptionRangeCFG(protectedRange, block, std::vector<std::string>{});

    EXPECT_TRUE(exceptionRangeCFG.IsCircular());
}

TEST(ExceptionRangeCFGTest, IsCircular_False) {
    auto handler = std::make_shared<BasicBlock>();
    std::vector<std::shared_ptr<BasicBlock>> protectedRange = {std::make_shared<BasicBlock>()};
    ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler, std::vector<std::string>{});

    EXPECT_FALSE(exceptionRangeCFG.IsCircular());
}

TEST(ExceptionRangeCFGTest, GetHandler) {
    auto handler = std::make_shared<BasicBlock>();
    std::vector<std::shared_ptr<BasicBlock>> protectedRange = {std::make_shared<BasicBlock>()};
    ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler, std::vector<std::string>{});

    EXPECT_EQ(exceptionRangeCFG.GetHandler(), handler);
}

TEST(ExceptionRangeCFGTest, SetHandler) {
    auto handler1 = std::make_shared<BasicBlock>();
    auto handler2 = std::make_shared<BasicBlock>();
    std::vector<std::shared_ptr<BasicBlock>> protectedRange = {std::make_shared<BasicBlock>()};
    ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler1, std::vector<std::string>{});

    exceptionRangeCFG.SetHandler(handler2);

    EXPECT_EQ(exceptionRangeCFG.GetHandler(), handler2);
}

TEST(ExceptionRangeCFGTest, GetProtectedRange) {
    auto block1 = std::make_shared<BasicBlock>();
    auto block2 = std::make_shared<BasicBlock>();
    std::vector<std::shared_ptr<BasicBlock>> protectedRange = {block1, block2};
    auto handler = std::make_shared<BasicBlock>();
    ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler, std::vector<std::string>{});

    std::vector<std::shared_ptr<BasicBlock>> result = exceptionRangeCFG.GetProtectedRange();
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], block1);
    EXPECT_EQ(result[1], block2);
}

// TODO: Fix test. Same issue with this test as AddExceptionType_ValidType, where the constructor logic is broken

// TEST(ExceptionRangeCFGTest, GetExceptionTypes) {
//     auto handler = std::make_shared<BasicBlock>();
//     auto block = std::make_shared<BasicBlock>();
//     std::vector<std::shared_ptr<BasicBlock>> protectedRange = {block};

//     // Seed with one dummy to allow AddExceptionType() to work
//     ExceptionRangeCFG exceptionRangeCFG(protectedRange, handler, "seed");

//     // Add real ones
//     exceptionRangeCFG.AddExceptionType("IOException");
//     exceptionRangeCFG.AddExceptionType("NullPointerException");

//     std::vector<std::string> result = exceptionRangeCFG.GetExceptionTypes();
//     ASSERT_EQ(result.size(), 3);  // Includes "seed"

//     // Use unordered checks to avoid order dependency
//     EXPECT_NE(std::find(result.begin(), result.end(), "IOException"), result.end());
//     EXPECT_NE(std::find(result.begin(), result.end(), "NullPointerException"), result.end());
// }


