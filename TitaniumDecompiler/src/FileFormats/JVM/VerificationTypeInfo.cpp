#include "VerificationTypeInfo.h"

#include <TitaniumLogger/Logger/Log.h>

namespace TitaniumDecompiler {
void VerificationTypeInfo::Deserialize(BigEndianStreamReader *deserializer, VerificationTypeInfo &instance) {
    deserializer->ReadRawBigEndian<VerificationTypeInfoTags>(instance.m_Tags);
    instance.m_VerificationType = instance.CreateVerificationTypeFromTag(instance);

    if (instance.m_VerificationType == nullptr) {
        TD_DECOMP_ERROR("ERROR\n");
    }

    instance.m_VerificationType->Deserialize(deserializer, *instance.m_VerificationType);
}

std::shared_ptr<VerificationType> VerificationTypeInfo::CreateVerificationTypeFromTag(VerificationTypeInfo &instance) {
    switch (instance.m_Tags) {
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_TOP: {
            return std::make_shared<TopVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_INTEGER: {
            return std::make_shared<IntegerVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_FLOAT: {
            return std::make_shared<FloatVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_DOUBLE: {
            return std::make_shared<LongVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_LONG: {
            return std::make_shared<DoubleVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_NULL: {
            return std::make_shared<NullVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_UNINITIALIZEDTHIS: {
            return std::make_shared<UninitializedThisVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_OBJECT: {
            return std::make_shared<ObjectVariableInfo>();
        }
        case TitaniumDecompiler::VerificationTypeInfoTags::TYPE_UNINITIALIZED: {
            return std::make_shared<UninitializedVariableInfo>();
        }
        default: {
            return nullptr;
        }
    }
}
}  // namespace TitaniumDecompiler