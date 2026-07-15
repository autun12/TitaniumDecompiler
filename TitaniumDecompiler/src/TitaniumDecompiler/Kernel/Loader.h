#pragma once

#include <filesystem>
#include <string>

#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/ClassFileParser.h"
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/JarFile.h"

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
