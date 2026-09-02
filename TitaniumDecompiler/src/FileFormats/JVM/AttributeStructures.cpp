#include "AttributeStructures.h"

#include <TitaniumLogger/Logger/Log.h>

#include <memory>
#include <string_view>

#include "Attributes.h"

namespace TitaniumDecompiler {

static const std::unordered_map<std::string, AttributeTypes>
    g_AttributeNameToType = {
        {"ConstantValue", AttributeTypes::ConstantValueType},
        {"Code", AttributeTypes::CodeType},
        {"StackMapTable", AttributeTypes::StackMapTableType},
        {"Exceptions", AttributeTypes::ExceptionsType},
        {"InnerClasses", AttributeTypes::InnerClassesType},
        {"EnclosingMethod", AttributeTypes::EnclosingMethodType},
        {"Synthetic", AttributeTypes::SyntheticType},
        {"Signature", AttributeTypes::SignatureType},
        {"SourceFile", AttributeTypes::SourceFileType},
        {"LineNumberTable", AttributeTypes::LineNumberTableType},
        {"LocalVariableTable", AttributeTypes::LocalVariableTableType},
        {"LocalVariableTypeTable", AttributeTypes::LocalVariableTypeTableType},
        {"Deprecated", AttributeTypes::DeprecatedType},
        {"RuntimeVisibleAnnotations",
         AttributeTypes::RuntimeVisibleAnnotationsType},
        {"RuntimeInvisibleAnnotations",
         AttributeTypes::RuntimeInvisibleAnnotationsType},
        {"RuntimeVisibleParameterAnnotations",
         AttributeTypes::RuntimeVisibleParameterAnnotationsType},
        {"RuntimeInvisibleParameterAnnotations",
         AttributeTypes::RuntimeInvisibleParameterAnnotationsType},
        {"RuntimeVisibleTypeAnnotations",
         AttributeTypes::RuntimeVisibleTypeAnnotationsType},
        {"RuntimeInvisibleTypeAnnotations",
         AttributeTypes::RuntimeInvisibleTypeAnnotationsType},
        {"AnnotationDefault", AttributeTypes::AnnotationDefaultType},
        {"BootstrapMethods", AttributeTypes::BootstrapMethodsType},
        {"MethodParameters", AttributeTypes::MethodParametersType},
        {"NestHost", AttributeTypes::NestHostType},
        {"NestMembers", AttributeTypes::NestMembersType}};

void AttributeInfo::Deserialize(BigEndianStreamReader *deserializer,
                                AttributeInfo &instance,
                                const ConstantPool &constantPool) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AttributeNameIndex);
    deserializer->ReadRawBigEndian<uint32_t>(instance.m_AttributeLength);
    std::string attrName =
        constantPool.GetConstantUTF8(instance.m_AttributeNameIndex);
    instance.tag = instance.GetAttrTypeFromName(attrName);
    instance.info = instance.CreateAttributeFromTag(instance);

    if (instance.info == nullptr) {
        TD_DECOMP_ERROR("AttributeInfo ERROR");
        return;
    }

    instance.info->Deserialize(deserializer, *instance.info, constantPool);
}

AttributeTypes AttributeInfo::GetAttrTypeFromName(const std::string &name) {
    auto it = g_AttributeNameToType.find(name);
    if (it != g_AttributeNameToType.end()) {
        return it->second;
    }
    return AttributeTypes::Unknown;
}

std::shared_ptr<Attribute> AttributeInfo::CreateAttributeFromTag(
    AttributeInfo &instance) {
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
        case TitaniumDecompiler::AttributeTypes::
            RuntimeVisibleAnnotationsType: {
            return std::make_shared<RuntimeVisibleAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::
            RuntimeInvisibleAnnotationsType: {
            return std::make_shared<RuntimeInvisibleAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::
            RuntimeVisibleParameterAnnotationsType: {
            return std::make_shared<RuntimeVisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::
            RuntimeInvisibleParameterAnnotationsType: {
            return std::make_shared<RuntimeInvisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::
            RuntimeVisibleTypeAnnotationsType: {
            return std::make_shared<RuntimeVisibleParameterAnnotations>();
        }
        case TitaniumDecompiler::AttributeTypes::
            RuntimeInvisibleTypeAnnotationsType: {
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
