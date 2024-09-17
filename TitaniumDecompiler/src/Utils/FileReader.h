// #pragma once

// #include <vector>
// #include <filesystem>
// #include <cstdint>
// #include <fstream>

// namespace TitaniumDecompiler {
// class FileReader {
// public:
//     FileReader();
//     void Read(const std::filesystem::path& path);
//     // void Read(std::vector<uint8_t>& data);
//     std::vector<uint8_t> GetBytes(size_t n);
//     uint8_t ReadUInt8();
//     int8_t ReadInt8();
//     uint16_t ReadUInt16();
//     int16_t ReadInt16();
//     uint32_t ReadUInt32();
//     int32_t ReadInt32();
//     uint64_t ReadUInt64();
//     int64_t ReadInt64();
//     std::vector<uint8_t> GetBuffer();
// private:
//     std::vector<uint8_t> m_Buffer;
//     std::ifstream m_BinaryFile;
//     size_t m_Position = 0;
// };
// }

#pragma once
#include "StreamReader.h"

#include <filesystem>
#include <fstream>

namespace TitaniumDecompiler {

class FileStreamReader : public BigEndianStreamReader {
public:
    FileStreamReader(const std::filesystem::path& path);
    FileStreamReader(const FileStreamReader&) = delete;
    ~FileStreamReader();

    bool IsStreamGood() const final { return m_Stream.good(); }
    uint64_t GetStreamPosition() override { return m_Stream.tellg(); }
    void SetStreamPosition(uint64_t position) override { m_Stream.seekg(position); }
    bool ReadData(char* destination, size_t size) override;

private:
    std::filesystem::path m_Path;
    std::ifstream m_Stream;
};

} // namespace Hazel