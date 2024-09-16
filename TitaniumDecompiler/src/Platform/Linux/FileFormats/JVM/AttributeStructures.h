#pragma once

#include "../../../../Utils/FileReader.h"

// #include "ConstantPool.h"

#include <memory>
#include <vector>

namespace TitaniumDecompiler {

enum AttributeTypes {
    // AnnotationDefaultType,
    BootstrapMethodsType,
    CodeType,
    ConstantValueType,
    // DeprecatedType,
    // EnclosingMethodType,
    ExceptionsType,
    // InnerClassesType,
    LineNumberTableType,
    // LocalVariableTableType,
    // LocalVariableTypeTableType,
    // MethodParametersType,
    // ModulesType,
    // NestHostType,
    // NestMembersType,
    // PermittedSubclassesType,
    // RecordType,

    // RuntimeInvisibleAnnotationsType,
    // RuntimeInvisibleParameterAnnotationsType,
    // RuntimeInvisibleTypeAnnotationsType,
    // RuntimeVisibleAnnotationsType,
    // RuntimeVisibleParameterAnnotationsType,
    // RuntimeVisibleTypeAnnotationsType,

    // SignatureType,
    SourceFileType,
    // SyntheticType,
    Unknown = 60
};

struct Attribute {
    virtual ~Attribute() = default;
    virtual void Deserialize(BigEndianStreamReader* deserializer, Attribute &instance) = 0;
};

struct AttributeInfo {
public:
    AttributeInfo() : m_AttributeNameIndex(0), m_AttributeLength(0), tag(AttributeTypes::Unknown), info(nullptr) {}
    AttributeInfo(uint16_t attributeNameIndex, uint32_t attributeLength, Attribute* info) : m_AttributeNameIndex(attributeNameIndex), m_AttributeLength(attributeLength), info(info) {}

    ~AttributeInfo() = default;

    AttributeTypes GetAttrTypeFromName(std::string name);
    
    static void Deserialize(BigEndianStreamReader* deserializer, AttributeInfo& instance);
private:
    std::shared_ptr<Attribute> CreateAttributeFromTag(AttributeInfo& instance);
public:
    uint16_t m_AttributeNameIndex;
    uint32_t m_AttributeLength;
    AttributeTypes tag;
    std::shared_ptr<Attribute> info;
};

struct ExceptionTable : Attribute {
    ExceptionTable() {}
    ~ExceptionTable() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance) override {
        ExceptionTable& exceptionTableInstance = static_cast<ExceptionTable&>(instance);
        Deserialize(deserializer, exceptionTableInstance);
    };

    static void Deserialize(BigEndianStreamReader* deserializer, ExceptionTable& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.startPc);
        deserializer->ReadRawBigEndian<uint16_t>(instance.endPc);
        deserializer->ReadRawBigEndian<uint16_t>(instance.handlerPc);
        deserializer->ReadRawBigEndian<uint16_t>(instance.catchType);
    }
    uint16_t startPc;
    uint16_t endPc;
    uint16_t handlerPc;
    uint16_t catchType;
};

// struct BootstrapMethodsInfo {
//     BootstrapMethodsInfo(FileReader& reader);
//     uint16_t bootstrapMethodRef;
//     uint16_t numBootstrapArguments;
//     std::vector<uint16_t> bootstrapArguments;
// };

// struct ConstantValue {
//     ConstantValue(FileReader& reader, ConstantPool& cpInfo);
//     uint16_t constantValueIndex;
// };

struct Code : Attribute {
    Code() {}
    ~Code() = default;
    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance) override {
        Code& codeInstance = static_cast<Code&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.maxStack);
        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.maxLocals);
        deserializer->ReadRawBigEndian<uint32_t>(codeInstance.codeLength);
        deserializer->ReadArrayBigEndian(codeInstance.code, codeInstance.codeLength);
        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.exceptionTableLength);
        if(codeInstance.exceptionTableLength > 0) {
            deserializer->ReadArrayBigEndian(codeInstance.exceptionTable, codeInstance.exceptionTableLength);
        }
        deserializer->ReadRawBigEndian<uint16_t>(codeInstance.attributesCount);
        deserializer->ReadArrayBigEndian(codeInstance.attributes, codeInstance.attributesCount);
    }

    uint16_t maxStack;
    uint16_t maxLocals;
    uint32_t codeLength;
    std::vector<uint8_t> code;
    uint16_t exceptionTableLength;
    std::vector<ExceptionTable> exceptionTable;
    uint16_t attributesCount;
    std::vector<AttributeInfo> attributes;
};

struct LineNumTableItem {
    LineNumTableItem() {}
    ~LineNumTableItem() = default;
    uint16_t startPc;
    uint16_t lineNumber;

    static void Deserialize(BigEndianStreamReader* deserializer, LineNumTableItem &instance);
};

struct LineNumberTable : Attribute {
    LineNumberTable() {}
    ~LineNumberTable() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance) override {
        LineNumberTable& lineNumberTableInstance = static_cast<LineNumberTable&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(lineNumberTableInstance.lineNumberTableLength);
        deserializer->ReadArrayBigEndian(lineNumberTableInstance.lineNumberTable, lineNumberTableInstance.lineNumberTableLength);
    }

    uint16_t lineNumberTableLength;
    std::vector<LineNumTableItem> lineNumberTable;
};

// struct Exception {
//     Exception(FileReader& reader, ConstantPool& cpInfo);
//     uint16_t numberOfExceptions;
//     std::vector<uint16_t> exceptionIndexTable;
// };

// struct BootstrapMethods {
//     BootstrapMethods(FileReader& reader, ConstantPool& cpInfo);
//     uint16_t numBootstrapMethods;
//     std::vector<BootstrapMethodsInfo> bootstrapMethods;
// };

struct SourceFile : Attribute {
    SourceFile() {}
    ~SourceFile() = default;

    void Deserialize(BigEndianStreamReader* deserializer, Attribute& instance) override {
        SourceFile& sourceFileInstance = static_cast<SourceFile&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(sourceFileInstance.sourceFileIndex);
    }

    uint16_t sourceFileIndex;
};

// struct Unknown {
//     Unknown(FileReader& reader, uint32_t value);
//     std::vector<uint8_t> info;
// };
}