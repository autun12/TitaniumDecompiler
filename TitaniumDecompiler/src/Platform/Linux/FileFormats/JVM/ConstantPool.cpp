#include "ConstantPool.h"
// #include "ConstPoolInfo.h"
#include "ConstPoolInfo.h"
#include <iostream>

namespace TitaniumDecompiler {
ConstantPool::ConstantPool() {}
void ConstantPool::Deserialize(BigEndianStreamReader *deserializer, ConstantPool &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.ConstantPoolCount);

    for(uint16_t i = 1; i < instance.ConstantPoolCount; i++) {
        ConstPoolInfo info;
        ConstPoolInfo::Deserialize(deserializer, info);
        if(info.Tag == Tags::Long || info.Tag == Tags::Double) {
            i++;
        }
        instance.m_ConstPoolInfo.push_back(info);
    }
}

std::string GetConstantUTF8(int idx) {
    int constantIdx = idx - 2;

    auto cpInfo = g_ConstPool.m_ConstPoolInfo.at(constantIdx);
    UTF8Info *utf8Info = cpInfo.asUTF8Info();

    std::stringstream s;
    for (int i = 0; i < utf8Info->GetLength(); i++) {
        s << utf8Info->GetByte(i);
    }

    return s.str();
}

ConstPoolInfo GetConstant(int idx) {
    return g_ConstPool.m_ConstPoolInfo.at(idx - 1);
}

// // std::string print(const ConstPoolInfo& cpinfo) {
// //     std::stringstream ss;
// //     ss << "ConstPoolInfo{tag=" << (int)cpinfo.tag << ", ";

// //     switch (cpinfo.tag) {
// //         case 1: // UTF8
// //             ss << "UTF8Info{length=" << cpinfo.m_UTF8Info->length << ", bytes=\"";
// //             for(auto bytes : cpinfo.m_UTF8Info->bytes) {
// //                 std::cout << bytes;
// //             }
// //             std::cout << "\"}";
// //             break;
// //         case 3: // Integer
// //             ss << "IntegerInfo{bytes=" << cpinfo.m_IntegerInfo->bytes << "}";
// //             break;
// //         case 4: // Float
// //             ss << "FloatInfo{bytes=" << cpinfo.m_FloatInfo->bytes << "}";
// //             break;
// //         case 5: // Long
// //             ss << "BigIntInfo{bytes=" << cpinfo.m_LongInfo->bytes << "}";
// //             break;
// //         case 6: // Double
// //             ss << "DoubleInfo{bytes=" << cpinfo.m_DoubleInfo->bytes << "}";
// //             break;
// //         case 7: // Class
// //             ss << "ClassInfo{name_index=" << cpinfo.m_ClassInfo->nameIndex << "}";
// //             break;
// //         case 8: // String
// //             ss << "StringInfo{string_index=" << cpinfo.m_StringInfo->stringIndex << "}";
// //             break;
// //         case 9: // Fieldref
// //         case 10: // Methodref
// //         case 11: // InterfaceMethodref
// //             ss << "RefInfo{class_index=" << cpinfo.m_FieldRefInfo->classIndex << ", name_and_type_index=" << cpinfo.m_FieldRefInfo->nameAndTypeIndex << "}";
// //             break;
// //         case 12: // NameAndType
// //             ss << "NameAndTypeInfo{name_index=" << cpinfo.m_NameAndTypeInfo->nameIndex << ", descriptor_index=" << cpinfo.m_NameAndTypeInfo->descriptorIndex << "}";
// //             break;
// //         case 15: // MethodHandle
// //             ss << "MethodHandleInfo{reference_kind=" << (int)cpinfo.m_MethodHandleInfo->referenceKind << ", reference_index=" << cpinfo.m_MethodHandleInfo->referenceIndex << "}";
// //             break;
// //         case 16: // MethodType
// //             ss << "MethodTypeInfo{descriptor_index=" << cpinfo.m_MethodTypeInfo->descriptorIndex << "}";
// //             break;
// //         case 18: // InvokeDynamic
// //             ss << "InvokeDynamicInfo{bootstrap_method_attr_index=" << cpinfo.m_InvokeDynamicInfo->bootstrapMethodAttrIndex << ", name_and_type_index=" << cpinfo.m_InvokeDynamicInfo->nameAndTypeIndex << "}";
// //             break;
// //         default:
// //             ss << "Unknown tag " << (int)cpinfo.tag;
// //             break;
// //     }

// //     ss << "}";
// //     return ss.str();
// // }

// std::string ConstantPool::GetQualifiedName(uint16_t index) {
//     std::string qualifiedName = "";
//     ConstPoolInfo* data = &m_ConstPoolInfo[index];
    
//     if(data->tag == Tags::Utf8) {
//         for(auto bytes : m_UTF8Info->bytes) {
//             qualifiedName += bytes;
//         }
//         std::cout << qualifiedName << std::endl;
//     }

//     return qualifiedName;
// }

// std::string ConstantPool::CheckClassName(std::string className) {
//     std::replace(className.begin(), className.end(), '/', '.');
//     size_t startPos = className.find("java.lang.");
//     if(startPos != std::string::npos) {
//         className.replace(startPos, 10, std::string());
//     }
//     return className;
// }

}