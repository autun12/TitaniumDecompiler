#pragma once

#include <vector>
#include <filesystem>
#include <cstdint>
#include <fstream>

class FileReader {
public:
    FileReader();
    void Read(const std::filesystem::path& path);
    std::vector<uint8_t> GetBytes(size_t n);
    uint8_t ReadUInt8();
    int8_t ReadInt8();
    uint16_t ReadUInt16();
    int16_t ReadInt16();
    uint32_t ReadUInt32();
    int32_t ReadInt32();
    uint64_t ReadUInt64();
    int64_t ReadInt64();
    std::vector<uint8_t> GetBuffer();
private:
    std::vector<uint8_t> m_Buffer;
    std::ifstream m_BinaryFile;
};