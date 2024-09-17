#include "ConstPoolInfo.h"
#include <iostream>
#include <vector>

namespace TitaniumDecompiler {

// void ConstPoolInfo::Deserialize(BigEndianStreamReader *deserializer, ConstPoolInfo &instance) {
//     if(!deserializer) {
//         std::cerr << "Deserializer is null" << std::endl;
//         return;
//     }
//     deserializer->ReadRawBigEndian<Tags>(instance.Tag);
//     switch(instance.Tag) {
//         case Tags::Utf8: {
//             std::cout << "UTF8" << std::endl;
//             uint16_t length;
//             std::vector<uint8_t> bytes;
//             deserializer->ReadRawBigEndian<uint16_t>(length);
//             deserializer->ReadArrayBigEndian(bytes, length);
//             UTF8Info utf8Info(length, bytes);
//             deserializer->ReadObject(utf8Info);
//             instance.Info = bytes;
//             break;
//         }
//         case Tags::Int: {
//             uint32_t bytes;
//             deserializer->ReadRawBigEndian<uint32_t>(bytes);
//             IntegerInfo intInfo(bytes);
//             deserializer->ReadObject(intInfo);
//             instance.Info = intInfo.Info;
//             return;
//         }
//         case Float: {
//             uint32_t bytes;
//             deserializer->ReadRawBigEndian<uint32_t>(bytes);
//             FloatInfo floatInfo(bytes);
//             deserializer->ReadObject(floatInfo);
//             std::cout << "Float" << std::endl;
//             instance.Info = floatInfo.Info;
//             return;
//         }
//         case Long: {
//             uint64_t bytes;
//             deserializer->ReadRawBigEndian<uint64_t>(bytes);
//             LongInfo longInfo(bytes);
//             deserializer->ReadObject(longInfo);
//             std::cout << "Long" << std::endl;
//             instance.Info = longInfo.Info;
//             return;
//         }
//         case Double: {
//             uint64_t bytes;
//             deserializer->ReadRawBigEndian<uint64_t>(bytes);
//             DoubleInfo doubleInfo(bytes);
//             deserializer->ReadObject(doubleInfo);
//             std::cout << "Double" << std::endl;
//             return;
//         }
//         case Class: {
//             uint16_t bytes;
//             deserializer->ReadRawBigEndian<uint16_t>(bytes);
//             ClassInfo classInfo(bytes);
//             deserializer->ReadObject(classInfo);
//             std::cout << "Class" << std::endl;
//             return;
//         }
//         case String: {
//             uint16_t bytes;
//             deserializer->ReadRawBigEndian<uint16_t>(bytes);
//             StringInfo stringInfo(bytes);
//             deserializer->ReadObject(stringInfo);
//             std::cout << "String" << std::endl;
//             return;
//         }
//         case Field: {
//             uint16_t classIndex;
//             uint16_t nameAndTypeIndex;
//             deserializer->ReadRawBigEndian<uint16_t>(classIndex);
//             deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
//             FieldRefInfo fieldRefInfo(classIndex, nameAndTypeIndex);
//             deserializer->ReadObject(fieldRefInfo);
//             std::cout << "Field" << std::endl;
//             return;
//         }
//         case Method: {
//             uint16_t classIndex;
//             uint16_t nameAndTypeIndex;
//             deserializer->ReadRawBigEndian<uint16_t>(classIndex);
//             deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
//             MethodRefInfo methodRefInfo(classIndex, nameAndTypeIndex);
//             deserializer->ReadObject(methodRefInfo);
//             return;
//         }
//         case InterfaceMethod: {
//             uint16_t classIndex;
//             uint16_t nameAndTypeIndex;
//             deserializer->ReadRawBigEndian<uint16_t>(classIndex);
//             deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
//             InterfaceMethodRef interfaceMethodRef(classIndex, nameAndTypeIndex);
//             deserializer->ReadObject(interfaceMethodRef);
//             std::cout << "Interface" << std::endl;
//             return;
//         }
//         case NameAndType: {
//             uint16_t nameIndex;
//             uint16_t descriptorIndex;
//             deserializer->ReadRawBigEndian<uint16_t>(nameIndex);
//             deserializer->ReadRawBigEndian<uint16_t>(descriptorIndex);
//             NameAndTypeInfo nameAndTypeInfo(nameIndex, descriptorIndex);
//             deserializer->ReadObject(nameAndTypeInfo);
//             std::cout << "NameAndType" << std::endl;
//             return;
//         }
//         case MethodHandle: {
//             uint8_t referenceKind;
//             uint16_t referenceIndex;
//             deserializer->ReadRawBigEndian<uint8_t>(referenceKind);
//             deserializer->ReadRawBigEndian<uint16_t>(referenceIndex);
//             MethodHandleInfo methodHandleInfo(referenceKind, referenceIndex);
//             deserializer->ReadObject(methodHandleInfo);
//             std::cout << "MethodHandle" << std::endl;
//             return;
//         }
//         case MethodType: {
//             uint16_t descriptorIndex;
//             deserializer->ReadRawBigEndian<uint16_t>(descriptorIndex);
//             MethodTypeInfo methodTypeInfo(descriptorIndex);
//             deserializer->ReadObject(methodTypeInfo);
//             std::cout << "MethodType" << std::endl;
//             return;
//         }
//         // case Dynamic: {
//         //     std::cout << "Int" << std::endl;
//         //     return;
//         // }
//         case InvokeDynamic: {
//             uint8_t bootstrapMethodAttrIndex;
//             uint16_t nameAndTypeIndex;
//             deserializer->ReadRawBigEndian<uint8_t>(bootstrapMethodAttrIndex);
//             deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
//             InvokeDynamicInfo invokeDynamicInfo(bootstrapMethodAttrIndex, nameAndTypeIndex);
//             deserializer->ReadObject(invokeDynamicInfo);
//             return;
//         }
//         // case Module: {
//         //     std::cout << "Int" << std::endl;
//         //     return;
//         // }
//         // case Package:
//         //     return;
//     }

//     deserializer->ReadArrayBigEndian(instance.Info, instance.Info.size());
// }

void ConstPoolInfo::Deserialize(BigEndianStreamReader *deserializer, ConstPoolInfo &instance) {
    deserializer->ReadRawBigEndian<Tags>(instance.Tag);
    switch(instance.Tag) {
        case Tags::Utf8: {
            uint16_t length;
            deserializer->ReadRawBigEndian<uint16_t>(length);
            instance.Info.resize(length);
            deserializer->ReadArrayBigEndian(instance.Info, length);
            break;
        }
        case Tags::Int: {
            uint32_t bytes;
            deserializer->ReadRawBigEndian<uint32_t>(bytes);
            instance.Info.resize(4);
            instance.Info[0] = (bytes & 0xFF000000) >> 24;
            instance.Info[1] = (bytes & 0xFF0000) >> 16;
            instance.Info[2] = (bytes & 0xFF00) >> 8;
            instance.Info[3] = bytes & 0xFF;
            break;
        }
        case Tags::Float: {
            uint32_t bytes;
            deserializer->ReadRawBigEndian<uint32_t>(bytes);
            instance.Info.resize(4);
            instance.Info[0] = (bytes & 0xFF000000) >> 24;
            instance.Info[1] = (bytes & 0xFF0000) >> 16;
            instance.Info[2] = (bytes & 0xFF00) >> 8;
            instance.Info[3] = bytes & 0xFF;
            break;
        }
        case Tags::Long: {
            uint64_t bytes;
            deserializer->ReadRawBigEndian<uint64_t>(bytes);
            instance.Info.resize(8);
            instance.Info[0] = (bytes & 0xFF00000000000000) >> 56;
            instance.Info[1] = (bytes & 0xFF000000000000) >> 48;
            instance.Info[2] = (bytes & 0xFF0000000000) >> 40;
            instance.Info[3] = (bytes & 0xFF00000000) >> 32;
            instance.Info[4] = (bytes & 0xFF000000) >> 24;
            instance.Info[5] = (bytes & 0xFF0000) >> 16;
            instance.Info[6] = (bytes & 0xFF00) >> 8;
            instance.Info[7] = bytes & 0xFF;
            break;
        }
        case Tags::Double: {
            uint64_t bytes;
            deserializer->ReadRawBigEndian<uint64_t>(bytes);
            instance.Info.resize(8);
            instance.Info[0] = (bytes & 0xFF00000000000000) >> 56;
            instance.Info[1] = (bytes & 0xFF000000000000) >> 48;
            instance.Info[2] = (bytes & 0xFF0000000000) >> 40;
            instance.Info[3] = (bytes & 0xFF00000000) >> 32;
            instance.Info[4] = (bytes & 0xFF000000) >> 24;
            instance.Info[5] = (bytes & 0xFF0000) >> 16;
            instance.Info[6] = (bytes & 0xFF00) >> 8;
            instance.Info[7] = bytes & 0xFF;
            break;
        }
        case Tags::Class: {
            uint16_t nameIndex;
            deserializer->ReadRawBigEndian<uint16_t>(nameIndex);
            instance.Info.resize(2);
            instance.Info[0] = (nameIndex & 0xFF00) >> 8;
            instance.Info[1] = nameIndex & 0xFF;
            break;
        }
        case Tags::String: {
            uint16_t stringIndex;
            deserializer->ReadRawBigEndian<uint16_t>(stringIndex);
            instance.Info.resize(2);
            instance.Info[0] = (stringIndex & 0xFF00) >> 8;
            instance.Info[1] = stringIndex & 0xFF;
            break;
        }
        case Tags::Field: {
            uint16_t classIndex;
            uint16_t nameAndTypeIndex;
            deserializer->ReadRawBigEndian<uint16_t>(classIndex);
            deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
            instance.Info.resize(4);
            instance.Info[0] = (classIndex & 0xFF00) >> 8;
            instance.Info[1] = classIndex & 0xFF;
            instance.Info[2] = (nameAndTypeIndex & 0xFF00) >> 8;
            instance.Info[3] = nameAndTypeIndex & 0xFF;
            break;
        }
        case Tags::Method: {
            uint16_t classIndex;
            uint16_t nameAndTypeIndex;
            deserializer->ReadRawBigEndian<uint16_t>(classIndex);
            deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
            instance.Info.resize(4);
            instance.Info[0] = (classIndex & 0xFF00) >> 8;
            instance.Info[1] = classIndex & 0xFF;
            instance.Info[2] = (nameAndTypeIndex & 0xFF00) >> 8;
            instance.Info[3] = nameAndTypeIndex & 0xFF;
            break;
        }
        case Tags::InterfaceMethod: {
            uint16_t classIndex;
            uint16_t nameAndTypeIndex;
            deserializer->ReadRawBigEndian<uint16_t>(classIndex);
            deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
            instance.Info.resize(4);
            instance.Info[0] = (classIndex & 0xFF00) >> 8;
            instance.Info[1] = classIndex & 0xFF;
            instance.Info[2] = (nameAndTypeIndex & 0xFF00) >> 8;
            instance.Info[3] = nameAndTypeIndex & 0xFF;
            break;
        }
        case Tags::NameAndType: {
            uint16_t nameIndex;
            uint16_t descriptorIndex;
            deserializer->ReadRawBigEndian<uint16_t>(nameIndex);
            deserializer->ReadRawBigEndian<uint16_t>(descriptorIndex);
            instance.Info.resize(4);
            instance.Info[0] = (nameIndex & 0xFF00) >> 8;
            instance.Info[1] = nameIndex & 0xFF;
            instance.Info[2] = (descriptorIndex & 0xFF00) >> 8;
            instance.Info[3] = descriptorIndex & 0xFF;
            break;
        }
        case Tags::MethodHandle: {
            uint8_t referenceKind;
            uint16_t referenceIndex;
            deserializer->ReadRawBigEndian<uint8_t>(referenceKind);
            deserializer->ReadRawBigEndian<uint16_t>(referenceIndex);
            instance.Info.resize(3);
            instance.Info[0] = referenceKind;
            instance.Info[1] = (referenceIndex & 0xFF00) >> 8;
            instance.Info[2] = referenceIndex & 0xFF;
            break;
        }
        case Tags::MethodType: {
            uint16_t descriptorIndex;
            deserializer->ReadRawBigEndian<uint16_t>(descriptorIndex);
            instance.Info.resize(2);
            instance.Info[0] = (descriptorIndex & 0xFF00) >> 8;
            instance.Info[1] = descriptorIndex & 0xFF;
            break;
        }
        case Tags::InvokeDynamic: {
            uint16_t bootstrapMethodAttrIndex;
            uint16_t nameAndTypeIndex;
            deserializer->ReadRawBigEndian<uint16_t>(bootstrapMethodAttrIndex);
            deserializer->ReadRawBigEndian<uint16_t>(nameAndTypeIndex);
            instance.Info.resize(4);
            instance.Info[0] = (bootstrapMethodAttrIndex & 0xFF00) >> 8;
            instance.Info[1] = bootstrapMethodAttrIndex & 0xFF;
            instance.Info[2] = (nameAndTypeIndex & 0xFF00) >> 8;
            instance.Info[3] = nameAndTypeIndex & 0xFF;
            break;
        }
    }
}

// UTF8Info::UTF8Info(FileReader& reader) {
//     m_Length = reader.ReadUInt16();
    
//     for(size_t i = 0; i < m_Length; i++) {
//         m_Bytes.push_back(reader.ReadUInt8());
//     }
// }

// IntegerInfo::IntegerInfo(FileReader& reader) {
//     m_Bytes = reader.ReadUInt32();
// }

// FloatInfo::FloatInfo(FileReader& reader) {
//     m_Bytes = reader.ReadUInt32();
// }

// LongInfo::LongInfo(FileReader& reader) {
//     m_Bytes = reader.ReadUInt64();
// }

// DoubleInfo::DoubleInfo(FileReader& reader) {
//     m_Bytes = reader.ReadUInt64();
// }

// ClassInfo::ClassInfo(FileReader& reader) {
//     m_NameIndex = reader.ReadUInt16();
// }

// StringInfo::StringInfo(FileReader& reader) {
//     m_StringIndex = reader.ReadUInt16();
// }

// FieldRefInfo::FieldRefInfo(FileReader& reader) {
//     m_ClassIndex = reader.ReadUInt16();
//     m_NameAndTypeIndex = reader.ReadUInt16();
// }

// MethodRefInfo::MethodRefInfo(FileReader& reader) {
//     m_ClassIndex = reader.ReadUInt16();
//     m_NameAndTypeIndex = reader.ReadUInt16();
// }

// InterfaceMethodRef::InterfaceMethodRef(FileReader& reader) {
//     m_ClassIndex = reader.ReadUInt16();
//     m_NameAndTypeIndex = reader.ReadUInt16();
// }

// NameAndTypeInfo::NameAndTypeInfo(FileReader& reader) {
//     m_NameIndex = reader.ReadUInt16();
//     m_DescriptorIndex = reader.ReadUInt16();
// }

// MethodHandleInfo::MethodHandleInfo(FileReader& reader) {
//     m_ReferenceKind = reader.ReadUInt8();
//     m_ReferenceIndex = reader.ReadUInt16();
// }

// MethodTypeInfo::MethodTypeInfo(FileReader& reader) {
//     m_DescriptorIndex = reader.ReadUInt16();
// }

// InvokeDynamicInfo::InvokeDynamicInfo(FileReader& reader) {
//     m_BootstrapMethodAttrIndex = reader.ReadUInt16();
//     m_NameAndTypeIndex = reader.ReadUInt16();
// }
}