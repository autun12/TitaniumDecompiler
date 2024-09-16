#include "Attributes.h"
#include "AttributeStructures.h"

#include <iostream>

namespace TitaniumDecompiler {
Attributes::Attributes() {}

void Attributes::Deserialize(BigEndianStreamReader *deserializer, Attributes &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.AttributesCount);
    deserializer->ReadArrayBigEndian(instance.m_Attributes, instance.AttributesCount);
    // for(size_t i = 0; i < instance.AttributesCount; i++) {
    //     std::shared_ptr<AttributeInfo> attribute = std::make_shared<AttributeInfo>();
    //     // printf("%x", attribute);
    //     attribute->Deserialize(deserializer, attribute);
    //     // instance.m_Attributes.push_back(attribute);
    //     // deserializer->ReadObject(instance.m_Attributes);
    // }
}
// Attributes::Attributes(FileReader& reader, ConstantPool& cp) 
//     : Attribute(reader, cp) {
    
//     switch(tag) {
//         case AttributeTypes::ConstantValueType: {
//             m_ConstantValue = new ConstantValue(reader, cp);
//             break;
//         }
//         case AttributeTypes::CodeType: {
//             m_Code = new Code(reader, cp);
//         }
//         case AttributeTypes::LineNumberTableType: {
//             m_LineNumberTable = new LineNumberTable(reader, cp);
//             break;
//         }
//         case AttributeTypes::ExceptionsType: {
//             m_Exception = new Exception(reader, cp);
//             break;
//         }
//         case AttributeTypes::BootstrapMethodsType: {
//             m_BootstrapMethods = new BootstrapMethods(reader, cp);
//             break;
//         }
//         case AttributeTypes::SourceFileType: {
//             m_SourceFile = new SourceFile(reader, cp);
//             break;
//         }
//     }
// }

// Attributes::~Attributes() {
//     switch (tag) {
//         case AttributeTypes::ConstantValueType: delete m_ConstantValue; break;
//         case AttributeTypes::CodeType: delete m_Code; break;
//         case AttributeTypes::LineNumberTableType: delete m_LineNumberTable; break;
//         case AttributeTypes::ExceptionsType: delete m_Exception; break;
//         case AttributeTypes::BootstrapMethodsType: delete m_BootstrapMethods; break;
//         case AttributeTypes::SourceFileType: delete m_SourceFile; break;
//     }
// }
}