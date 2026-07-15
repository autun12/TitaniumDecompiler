#include "Loader.h"

#include <algorithm>

namespace TitaniumDecompiler {
void Loader::LoadFile(const std::filesystem::path& filePath) {
    std::string extension = filePath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    if (extension == ".jar") {
        JarFileParser parser;
        m_JarFile = parser.LoadFromJar(filePath);
    } else if (extension == ".class") {
        ClassFileParser parser;
        m_ClassFile = parser.OpenClassFile(filePath);
    }
}

ClassFile Loader::GetClassFile() { return m_ClassFile; }

JarFile Loader::GetJarFile() { return m_JarFile; }
}  // namespace TitaniumDecompiler
