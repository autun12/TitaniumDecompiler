#pragma once

#include <cstdint>
#include <string_view>

#include "AttributeStructures.h"
#include "ConstantPool.h"

namespace TitaniumDecompiler {
class Attributes {
public:
    Attributes();
    ~Attributes() = default;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            Attributes& instance,
                            const ConstantPool& constantPool);
    [[nodiscard]] const AttributeInfo* FindAttribute(
        std::string_view name, const ConstantPool& constantPool) const;
    [[nodiscard]] AttributeInfo* FindAttribute(
        std::string_view name, const ConstantPool& constantPool);
    template <typename T>
    [[nodiscard]] const T* GetAttributePayload(
        std::string_view name, const ConstantPool& constantPool) const {
        const AttributeInfo* attr = FindAttribute(name, constantPool);
        if (!attr || !attr->info) {
            return nullptr;
        }
        return dynamic_cast<const T*>(attr->info.get());
    }

public:
    uint16_t AttributesCount = 0;
    std::vector<AttributeInfo> m_Attributes;
};
}  // namespace TitaniumDecompiler
