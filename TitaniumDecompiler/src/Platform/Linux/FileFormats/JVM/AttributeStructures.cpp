#include "AttributeStructures.h"
#include <memory>
#include "Attributes.h"

namespace TitaniumDecompiler {

void AttributeInfo::Deserialize(BigEndianStreamReader *deserializer, AttributeInfo &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AttributeNameIndex);
    deserializer->ReadRawBigEndian<uint32_t>(instance.m_AttributeLength);
    std::string attrName = GetConstantUTF8(instance.m_AttributeNameIndex);
    instance.tag = instance.GetAttrTypeFromName(attrName);

    instance.info = instance.CreateAttributeFromTag(instance);
    if(instance.info == nullptr) {
        printf("ERROR");
    }

    instance.info->Deserialize(deserializer, *instance.info);
    // info->Deserialize(deserializer, *info);
}

AttributeTypes AttributeInfo::GetAttrTypeFromName(std::string name) {
    if(name == "ConstantValue") {
        tag = ConstantValueType;
    } else if(name == "Code") {
        tag = CodeType;
    } else if (name == "LineNumberTable") {
        tag = LineNumberTableType;
    } else if (name == "Exceptions") {
        tag = ExceptionsType;
    } else if (name == "BootstrapMethods") {
        tag = BootstrapMethodsType;
    } else if (name == "SourceFile") {
        tag = SourceFileType;
    }

    return tag;
}

std::shared_ptr<Attribute> AttributeInfo::CreateAttributeFromTag(AttributeInfo& instance) {
    switch(instance.tag) {
        case TitaniumDecompiler::AttributeTypes::ExceptionsType: {
            printf("EXCEPTIONS\n");
            return std::make_shared<ExceptionTable>();
        }
        case TitaniumDecompiler::AttributeTypes::LineNumberTableType: {
            printf("LINENUMBER\n");
            return std::make_shared<LineNumberTable>();
        }
        case TitaniumDecompiler::AttributeTypes::CodeType: {
            printf("CODETYPE\n");
            return std::make_shared<Code>();
        }
        case TitaniumDecompiler::AttributeTypes::SourceFileType: {
            printf("SOURCE\n");
            return std::make_shared<SourceFile>();
        }
        default: {
            return nullptr;
        }
    }
}

void LineNumTableItem::Deserialize(BigEndianStreamReader *deserializer, LineNumTableItem &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.startPc);
    printf("Start PC: %d\n", instance.startPc);
    deserializer->ReadRawBigEndian<uint16_t>(instance.lineNumber);
    printf("Line Number: %d\n", instance.lineNumber);
}

// Attribute::Attribute(FileReader& reader, ConstantPool& cpInfo) {
//     m_AttributeNameIndex = reader.ReadUInt16();
//     std::string attributeName = cpInfo.GetQualifiedName(m_AttributeNameIndex);
//     m_AttributeLength = reader.ReadUInt32();

//     if(attributeName == "ConstantValue") {
//         tag = ConstantValueType;
//     } else if(attributeName == "Code") {
//         tag = CodeType;
//     } else if (attributeName == "LineNumberTable") {
//         tag = LineNumberTableType;
//     } else if (attributeName == "Exceptions") {
//         tag = ExceptionsType;
//     } else if (attributeName == "BootstrapMethods") {
//         tag = BootstrapMethodsType;
//     } else if (attributeName == "SourceFile") {
//         tag = SourceFileType;
//     }
// }

// BootstrapMethodsInfo::BootstrapMethodsInfo(FileReader& reader) {
//     bootstrapMethodRef = reader.ReadUInt16();
//     numBootstrapArguments = reader.ReadUInt16();

//     for(size_t i = 0; i < numBootstrapArguments; i++) {
//         bootstrapArguments.push_back(reader.ReadUInt16());
//     }
// }

// ConstantValue::ConstantValue(FileReader& reader, ConstantPool& cpInfo) {
//     constantValueIndex = reader.ReadUInt16();
// }


// Exception::Exception(FileReader& reader, ConstantPool& cp) {
//     numberOfExceptions = reader.ReadUInt16();

//     for(size_t i = 1; i <= sizeof(uint16_t); i++) {
//         exceptionIndexTable.push_back(reader.ReadUInt16());
//     }
// }

// BootstrapMethods::BootstrapMethods(FileReader& reader, ConstantPool& cp) {
//     numBootstrapMethods = reader.ReadUInt16();

//     for(size_t i = 0; i < numBootstrapMethods; i++) {
//         bootstrapMethods.push_back(BootstrapMethodsInfo(reader));
//     }
// }

// SourceFile::SourceFile(FileReader& reader, ConstantPool& cp) {
//     sourceFileIndex = reader.ReadUInt16();
// }
}