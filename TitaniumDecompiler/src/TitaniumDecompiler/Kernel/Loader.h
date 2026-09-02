#pragma once

#include <filesystem>
#include <string>

#include "FileFormats/JVM/ClassFileParser.h"
#include "FileFormats/JVM/JarFile.h"

namespace TitaniumDecompiler {
struct Loader {
public:
    Loader() = default;
    ~Loader() = default;
    void LoadFile(const std::filesystem::path& filePath);
    ClassFile GetClassFile();
    JarFile GetJarFile();

private:
    std::string m_FileName;
    JarFile m_JarFile;
    ClassFile m_ClassFile;
};
}  // namespace TitaniumDecompiler
