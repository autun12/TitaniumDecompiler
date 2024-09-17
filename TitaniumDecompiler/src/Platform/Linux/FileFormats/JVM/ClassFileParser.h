#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include "../../../../Utils/FileReader.h"
#include "../../../../Utils/StreamReader.h"
// #include "ConstPoolInfo.h"
#include "AttributeStructures.h"
#include "ConstantPool.h"
#include "Fields.h"
#include "Methods.h"
// #include "Methods.h"

namespace TitaniumDecompiler {
class ClassFile {
public:
    ~ClassFile() = default;
    static void Deserialize(BigEndianStreamReader* deserializer, ClassFile& instance);
public:
    uint32_t m_Magic = 0;
    uint16_t m_Minor = 0;
    uint16_t m_Major = 0;
    ConstantPool m_ConstantPool;
    uint16_t m_AccessFlags = 0;
    uint16_t m_ThisClass = 0;
    uint16_t m_SuperClass = 0;
    uint16_t m_InterfaceCount = 0;
    std::vector<uint16_t> m_Interfaces;
    uint16_t m_FieldCount = 0;
    std::vector<Fields> m_Fields;
    uint16_t m_MethodCount = 0;
    std::vector<Methods> m_Methods;
    Attributes m_Attributes;
};

class ClassFileParser {
public:
    ClassFileParser();
    ~ClassFileParser() = default;
    void OpenClassFile(const std::filesystem::path& path);
    void Parse(std::vector<uint8_t> data);
    // std::string ParseInterface();
// private:
//     uint32_t m_Magic = 0;
//     uint16_t m_Minor = 0;
//     uint16_t m_Major = 0;
//     uint16_t m_AccessFlags = 0;
//     uint16_t m_ThisClass = 0;
//     uint16_t m_SuperClass = 0;
    // ConstantPool m_ConstantPool;
    // FileReader m_Reader;
    // std::vector<std::string> m_Interfaces;
    // std::vector<Fields> m_Fields;
    // std::vector<Methods> m_Methods;
};
}