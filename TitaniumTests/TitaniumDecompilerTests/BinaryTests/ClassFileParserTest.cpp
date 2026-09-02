#include <gtest/gtest.h>

#include <fstream>
#include <vector>

#include "FileFormats/JVM/ClassFileParser.h"  // Include your actual header file
#include "Utils/FileStream.h"                 // Include necessary dependencies

class ClassFileParserTest : public ::testing::Test {
protected:
    TitaniumDecompiler::ClassFileParser parser;

    std::vector<uint8_t> ReadFileBytes(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
    }
};

TEST_F(ClassFileParserTest, ParsesValidData) {
    // Mock valid byte data representing a ClassFile
    std::vector<uint8_t> validData = ReadFileBytes("JavaTestFiles/Test4.class");

    EXPECT_NO_THROW(
        { TitaniumDecompiler::ClassFile result = parser.Parse(validData); });
}

TEST_F(ClassFileParserTest, ThrowsExceptionOnInvalidData) {
    // Mock invalid byte data
    std::vector<uint8_t> invalidData = {};

    EXPECT_THROW({ parser.Parse(invalidData); }, std::runtime_error);
}
