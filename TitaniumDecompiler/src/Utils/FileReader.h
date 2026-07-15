#pragma once
#include <filesystem>
#include <fstream>

#include "StreamReader.h"

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

}  // namespace TitaniumDecompiler