#pragma once

#include <memory>
#include <vector>

#include "AttributeStructures.h"
#include "Attributes.h"
#include "ConstantPool.h"
#include "Utils/FileStream.h"

namespace TitaniumDecompiler {
class Methods {
public:
    Methods();
    ~Methods() = default;
    static void Deserialize(BigEndianStreamReader* deserializer,
                            Methods& instance, const ConstantPool& constPool);

public:
    uint16_t m_AccessFlags;
    uint16_t m_NameIndex;
    uint16_t m_DescIndex;
    Attributes m_Attr;
};
}  // namespace TitaniumDecompiler
