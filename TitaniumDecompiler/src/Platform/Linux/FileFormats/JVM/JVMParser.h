#pragma once

#include <filesystem>
#include <cstdint>

namespace TitaniumDecompiler {
class JVMParser {
public:
    JVMParser();
    ~JVMParser();
    uint8_t readUint8();
    int8_t readInt8();
    uint8_t readUint16();
    int8_t readInt16();
    uint8_t readUint32();
    int8_t readInt32();
    uint8_t readUint64();
    int8_t readInt64();
};
}