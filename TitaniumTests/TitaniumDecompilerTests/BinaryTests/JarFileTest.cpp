#include <gtest/gtest.h>
#include "Platform/Linux/FileFormats/JVM/JarFile.h"  // Adjust the include path as necessary
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include "Utils/StreamReader.h"
#include "Utils/FileReader.h"

class JarFileParserTest : public ::testing::Test {
protected:
    TitaniumDecompiler::JarFileParser jarParser;

    std::filesystem::path GetJarFilePath(const std::string& filename) {
        return std::filesystem::path(filename);
    }
};


TEST_F(JarFileParserTest, ParsesValidJarFile) {
    // Assume "JavaTestFiles/TestJarFile.jar" is a valid jar file containing at least one .class file.
    std::filesystem::path jarFilePath = GetJarFilePath("JavaTestFiles/TestJarFile/TestJarFile.jar");
    EXPECT_NO_THROW({
        TitaniumDecompiler::JarFile jarFile = jarParser.LoadFromJar(jarFilePath);
        EXPECT_GT(jarFile.m_Classes.size(), 0);
    });
}

TEST_F(JarFileParserTest, ThrowsExceptionOnInvalidJarFile) {
    // Provide a path to an invalid or non-existent jar file.
    std::filesystem::path invalidJarPath = GetJarFilePath("JavaTestFiles/Invalid.jar");

    EXPECT_THROW({
        // This should throw an exception if the jar file is invalid.
        TitaniumDecompiler::JarFile jarFile = jarParser.LoadFromJar(invalidJarPath);
    }, std::runtime_error);
}
