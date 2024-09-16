#include "ClassFileParser.h"
#include <sys/types.h>
#include <filesystem>
#include <vector>
#include "AttributeStructures.h"
#include "ConstantPool.h"

namespace TitaniumDecompiler {

ConstantPool g_ConstPool;

void ClassFile::Deserialize(BigEndianStreamReader* deserializer, ClassFile &instance) {
    deserializer->ReadRawBigEndian<uint32_t>(instance.m_Magic);
    
    if(instance.m_Magic != 0xCAFEBABE) {
        return;
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Minor);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Major);
    deserializer->ReadObject(instance.m_ConstantPool);
    g_ConstPool = instance.m_ConstantPool;
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AccessFlags);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_ThisClass);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_SuperClass);
    
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_InterfaceCount);
    if(instance.m_InterfaceCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Interfaces, instance.m_InterfaceCount);
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_FieldCount);
    if(instance.m_FieldCount > 0) {
        deserializer->ReadArrayBigEndian<Fields>(instance.m_Fields, instance.m_FieldCount);
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_MethodCount);
    if(instance.m_MethodCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Methods, instance.m_MethodCount);
    }

    deserializer->ReadObject(instance.m_Attributes);
    // deserializer->ReadRawBigEndian<uint16_t>(instance.m_AttributeCount);
    // if(instance.m_AttributeCount > 0) {
    //     deserializer->ReadArrayBigEndian(instance.m_Attributes, instance.m_AttributeCount);
    // }
}

ClassFileParser::ClassFileParser() {}

void ClassFileParser::OpenClassFile(const std::filesystem::path& path) {
    FileStreamReader stream(path);
    ClassFile classFile;

    if(!stream.IsStreamGood()) {
        return;
    }

    stream.ReadObject<ClassFile>(classFile);
    printf("Magic: %x\n", classFile.m_Magic);
    printf("Minor: %d\n", classFile.m_Minor);
    printf("Major: %d\n", classFile.m_Major);
    printf("Const Pool Count: %d\n", classFile.m_ConstantPool.ConstantPoolCount);
    printf("Access Flags: %d\n", classFile.m_AccessFlags);
    printf("Super Class: %d\n", classFile.m_SuperClass);
    printf("This Class: %d\n", classFile.m_ThisClass);
    printf("Interface Count: %d\n", classFile.m_InterfaceCount);
    printf("Field Count: %d\n", classFile.m_FieldCount);
    printf("Method Count: %d\n", classFile.m_MethodCount);
    for(size_t i = 0; i < classFile.m_Methods.size(); i++) {
        if(classFile.m_Methods.at(i).m_Attr.m_Attributes.at(0).tag == TitaniumDecompiler::AttributeTypes::CodeType) {
            auto info = classFile.m_Methods.at(i).m_Attr.m_Attributes.at(0).info;
            Code& codeInstance = static_cast<Code&>(*info);
            printf("Max Stack: %d\n", codeInstance.maxStack);
        }
    }
}
}