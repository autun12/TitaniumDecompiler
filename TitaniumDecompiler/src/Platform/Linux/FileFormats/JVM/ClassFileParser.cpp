#include "ClassFileParser.h"

#include <sys/types.h>

#include <filesystem>
#include <vector>

#include "AttributeStructures.h"
#include "ConstantPool.h"
#include "Instruction.h"

namespace TitaniumDecompiler {

std::vector<ConstantPool> g_ConstPool;

void ClassFile::Deserialize(BigEndianStreamReader* deserializer, ClassFile& instance) {
    deserializer->ReadRawBigEndian<uint32_t>(instance.m_Magic);
    g_ConstPool.clear();
    if (instance.m_Magic != MAGIC_NUMBER) {
        return;
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Minor);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Major);
    deserializer->ReadObject(instance.m_ConstantPool);
    g_ConstPool.push_back(instance.m_ConstantPool);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AccessFlags);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_ThisClass);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_SuperClass);

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_InterfaceCount);
    if (instance.m_InterfaceCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Interfaces, instance.m_InterfaceCount);
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_FieldCount);
    if (instance.m_FieldCount > 0) {
        deserializer->ReadArrayBigEndian<Fields>(instance.m_Fields, instance.m_FieldCount);
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_MethodCount);
    if (instance.m_MethodCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Methods, instance.m_MethodCount);
    }

    deserializer->ReadObject(instance.m_Attributes);
}

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
