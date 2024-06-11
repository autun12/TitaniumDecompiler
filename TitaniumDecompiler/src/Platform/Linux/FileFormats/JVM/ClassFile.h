#pragma once

#include <cstdint>
#include <filesystem>

#include "../../../../Utils/FileReader.h"

class ClassFile {
public:
    ClassFile(const std::filesystem::path& path);
    ~ClassFile() = default;
    void Parse(std::vector<uint8_t> data);
private:
    uint32_t m_Magic = 0;
    uint16_t m_Minor = 0;
    uint16_t m_Major = 0;
    FileReader m_Reader;
};