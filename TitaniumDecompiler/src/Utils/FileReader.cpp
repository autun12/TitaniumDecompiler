#include "FileReader.h"
#include "FileReaderError.h"

#include <cstdint>
#include <string>

FileReader::FileReader() {}

std::vector<uint8_t> FileReader::GetBuffer() {
    return m_Buffer;
}

void FileReader::Read(const std::filesystem::path& path) {
    std::string pathStr = path.string();
    auto length = std::filesystem::file_size(path);
    if(length == 0) {
        return;
    }

    std::vector<uint8_t> buffer(length);
    m_Buffer = buffer;
    m_BinaryFile.open(pathStr, std::ios_base::binary);
    m_BinaryFile.read(reinterpret_cast<char*>(m_Buffer.data()), length);
    m_BinaryFile.close();
}

std::vector<uint8_t> FileReader::GetBytes(size_t n) {
    if(n > m_Buffer.size()) {
        throw FileReaderError("end of data");
    }

    std::vector<uint8_t> result(m_Buffer.begin(), m_Buffer.begin() + n);
    m_Buffer.erase(m_Buffer.begin(), m_Buffer.begin() + n);
    return result;
}

uint8_t FileReader::ReadUInt8() {
    return GetBytes(1)[0];
}


int8_t FileReader::ReadInt8() {
    return static_cast<int8_t>(ReadUInt8());
}

uint16_t FileReader::ReadUInt16() {
    std::vector<uint8_t> bytes = GetBytes(2);
    return (bytes[0] << 8) | bytes[1];
}

int16_t FileReader::ReadInt16() {
    return static_cast<int16_t>(ReadUInt16());
}

uint32_t FileReader::ReadUInt32() {
    std::vector<uint8_t> bytes = GetBytes(4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

int32_t FileReader::ReadInt32() {
    return static_cast<int16_t>(ReadUInt32());
}

uint64_t FileReader::ReadUInt64() {
    std::vector<uint8_t> bytes = GetBytes(8);
    return (static_cast<uint64_t>(bytes[0]) << 56) |
            (static_cast<uint64_t>(bytes[1]) << 48) |
            (static_cast<uint64_t>(bytes[2]) << 40) |
            (static_cast<uint64_t>(bytes[3]) << 32) |
            (static_cast<uint64_t>(bytes[4]) << 24) |
            (static_cast<uint64_t>(bytes[5]) << 16) |
            (static_cast<uint64_t>(bytes[6]) << 8) |
            bytes[7];
}

int64_t FileReader::ReadInt64() {
    return static_cast<int16_t>(ReadUInt64());
}