#pragma once

#include <cstdint>
#include <filesystem>

#include "../../../../Utils/FileReader.h"

namespace TitaniumDecompiler {
class ClassFile {
public:
    ClassFile();
    ~ClassFile() = default;
    void OpenClassFile(const std::filesystem::path& path);
    void Parse(std::vector<uint8_t> data);
private:
    uint32_t m_Magic = 0;
    uint16_t m_Minor = 0;
    uint16_t m_Major = 0;
    FileReader m_Reader;
};
}