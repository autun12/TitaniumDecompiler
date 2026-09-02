#include "Attributes.h"

#include "AttributeStructures.h"

namespace TitaniumDecompiler {
Attributes::Attributes() {}

void Attributes::Deserialize(BigEndianStreamReader* deserializer,
                             Attributes& instance,
                             const ConstantPool& constantPool) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.AttributesCount);

    if (instance.AttributesCount > 0) {
        deserializer->ReadArrayBigEndian(
            instance.m_Attributes, instance.AttributesCount, constantPool);
    }
}

const AttributeInfo* Attributes::FindAttribute(
    std::string_view name, const ConstantPool& constantPool) const {
    for (const auto& attr : m_Attributes) {
        if (constantPool.GetConstantUTF8(attr.m_AttributeNameIndex) == name) {
            return &attr;
        }
    }
    return nullptr;
}

AttributeInfo* Attributes::FindAttribute(std::string_view name,
                                         const ConstantPool& constantPool) {
    for (auto& attr : m_Attributes) {
        if (constantPool.GetConstantUTF8(attr.m_AttributeNameIndex) == name) {
            return &attr;
        }
    }
    return nullptr;
}
}  // namespace TitaniumDecompiler
