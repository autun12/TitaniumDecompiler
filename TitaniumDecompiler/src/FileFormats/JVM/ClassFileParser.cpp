#include "ClassFileParser.h"

#include <sys/types.h>

#include <filesystem>
#include <vector>

namespace TitaniumDecompiler {

ClassFileParser::ClassFileParser() {}

ClassFile ClassFileParser::OpenClassFile(const std::filesystem::path& path) {
    FileStreamReader stream(path);
    ClassFile classFile;

    if (!stream.IsStreamGood()) {
        throw std::runtime_error("Stream is not good");
    }

    stream.ReadObject<ClassFile>(classFile);
    return classFile;
}

ClassFile ClassFileParser::Parse(const std::vector<uint8_t>& data) {
    MemoryStreamReader stream(data);
    ClassFile classFile;

    if (!stream.IsStreamGood()) {
        throw std::runtime_error("Stream is not good");
    }

    stream.ReadObject<ClassFile>(classFile);
    return classFile;
}
}  // namespace TitaniumDecompiler
