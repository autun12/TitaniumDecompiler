#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "ClassFileParser.h"

namespace TitaniumDecompiler {

struct JarFile {
public:
    JarFile() = default;
    ~JarFile() = default;

public:
    std::vector<ClassFile> m_Classes;
};

struct JarFileParser {
public:
    JarFileParser() = default;
    ~JarFileParser() = default;
    JarFile LoadFromJar(const std::filesystem::path& jarPath);

public:
    JarFile m_JarFile;

private:
    ClassFileParser m_ClassParser;
};

}  // namespace TitaniumDecompiler
