#include "AttributeStructures.h"

#include <TitaniumLogger/Logger/Log.h>

#include <memory>

#include "Attributes.h"

namespace TitaniumDecompiler {

void AttributeInfo::Deserialize(BigEndianStreamReader *deserializer, AttributeInfo &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AttributeNameIndex);
    deserializer->ReadRawBigEndian<uint32_t>(instance.m_AttributeLength);
    std::string attrName = GetConstantUTF8(instance.m_AttributeNameIndex);
    instance.tag = instance.GetAttrTypeFromName(attrName);
    instance.info = instance.CreateAttributeFromTag(instance);

    if (instance.info == nullptr) {
        TD_DECOMP_ERROR("AttributeInfo ERROR\n");
        return;
    }

    instance.info->Deserialize(deserializer, *instance.info);
}

AttributeTypes AttributeInfo::GetAttrTypeFromName(std::string name) {
    if (name == "ConstantValue") {
        tag = ConstantValueType;
    } else if (name == "Code") {
        tag = CodeType;
    } else if (name == "LineNumberTable") {
        tag = LineNumberTableType;
    } else if (name == "Exceptions") {
        tag = ExceptionsType;
    } else if (name == "Deprecated") {
        tag = DeprecatedType;
    } else if (name == "BootstrapMethods") {
        tag = BootstrapMethodsType;
    } else if (name == "SourceFile") {
        tag = SourceFileType;
    } else if (name == "StackMapTable") {
        tag = StackMapTableType;
    } else if (name == "LocalVariableTable") {
        tag = LocalVariableTableType;
    } else if (name == "LocalVariableTypeTable") {
        tag = LocalVariableTypeTableType;
    } else if (name == "InnerClasses") {
        tag = InnerClassesType;
    } else if (name == "Signature") {
        tag = SignatureType;
    } else if (name == "Synthetic") {
        tag = SyntheticType;
    } else if (name == "EnclosingMethod") {
        tag = EnclosingMethodType;
    } else if (name == "Deprecated") {
        tag = InnerClassesType;
    } else if (name == "MethodParameters") {
        tag = MethodParametersType;
    } else if (name == "RuntimeVisibleAnnotations") {
        tag = RuntimeVisibleAnnotationsType;
    } else if (name == "RuntimeInvisibleTypeAnnotations") {
        tag = MethodParametersType;
    } else if (name == "RuntimeVisibleParameterAnnotations") {
        tag = RuntimeVisibleParameterAnnotationsType;
    } else if (name == "RuntimeInvisibleParameterAnnotations") {
        tag = RuntimeInvisibleParameterAnnotationsType;
    } else if (name == "RuntimeVisibleTypeAnnotations") {
        tag = RuntimeVisibleTypeAnnotationsType;
    } else if (name == "RuntimeInvisibleTypeAnnotations") {
        tag = RuntimeInvisibleTypeAnnotationsType;
    } else if (name == "AnnotationDefault") {
        tag = AnnotationDefaultType;
    } else if (name == "NestHost") {
        tag = NestHostType;
    } else if (name == "NestMembers") {
        tag = NestMembersType;
    }

    return tag;
}

std::shared_ptr<Attribute> AttributeInfo::CreateAttributeFromTag(AttributeInfo &instance) {
    switch (instance.tag) {
        case TitaniumDecompiler::AttributeTypes::ExceptionsType: {
            return std::make_shared<ExceptionsAttr>();
        }
        case TitaniumDecompiler::AttributeTypes::LineNumberTableType: {
            return std::make_shared<LineNumberTable>();
        }
        case TitaniumDecompiler::AttributeTypes::CodeType: {
            return std::make_shared<Code>();
        }
        case TitaniumDecompiler::AttributeTypes::SourceFileType: {
            return std::make_shared<SourceFile>();
        }
        case TitaniumDecompiler::AttributeTypes::StackMapTableType: {
            return std::make_shared<StackMapTable>();
        }
        case TitaniumDecompiler::AttributeTypes::LocalVariableTableType: {
            return std::make_shared<LocalVariableTable>();
        }
        case TitaniumDecompiler::AttributeTypes::LocalVariableTypeTableType: {
            return std::make_shared<LocalVariableTypeTableAttr>();
        }
        case TitaniumDecompiler::AttributeTypes::DeprecatedType: {
            return std::make_shared<Deprecated>();
        }
        case TitaniumDecompiler::AttributeTypes::SyntheticType: {
            return std::make_shared<Synthetic>();
        }
        case TitaniumDecompiler::AttributeTypes::EnclosingMethodType: {
            return std::make_shared<EnclosingMethod>();
        }
        case TitaniumDecompiler::AttributeTypes::InnerClassesType: {
            return std::make_shared<InnerClasses>();
        }
        case TitaniumDecompiler::AttributeTypes::SignatureType: {
            return std::make_shared<Signature>();
        }
        case TitaniumDecompiler::AttributeTypes::ConstantValueType: {
            return std::make_shared<ConstantValue>();
        }
        case TitaniumDecompiler::AttributeTypes::BootstrapMethodsType: {
            return std::make_shared<BootstrapMethods>();
        }
        case TitaniumDecompiler::AttributeTypes::MethodParametersType: {
            return std::make_shared<MethodParameters>();
        }
        case TitaniumDecompiler::AttributeTypes::RuntimeVisibleAnnotationsType: {
            return std::make_shared<RuntimeVisibleAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::RuntimeInvisibleAnnotationsType: {
            return std::make_shared<RuntimeInvisibleAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::RuntimeVisibleParameterAnnotationsType: {
            return std::make_shared<RuntimeVisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::RuntimeInvisibleParameterAnnotationsType: {
            return std::make_shared<RuntimeInvisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::RuntimeVisibleTypeAnnotationsType: {
            return std::make_shared<RuntimeVisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::RuntimeInvisibleTypeAnnotationsType: {
            return std::make_shared<RuntimeInvisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::AnnotationDefaultType: {
            return std::make_shared<AnnotationDefault>();
        }
        case TitaniumDecompiler::AttributeTypes::NestHostType: {
            return std::make_shared<NestHost>();
        }
        case TitaniumDecompiler::AttributeTypes::NestMembersType: {
            return std::make_shared<NestMembers>();
        }
        default: {
            return nullptr;
        }
    }
}
}  // namespace TitaniumDecompiler