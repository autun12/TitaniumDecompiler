#pragma once

#include <cstdint>
#include <memory>

#include "AttributeStructures.h"
#include "ConstantPool.h"
#include "Utils/FileReader.h"

namespace TitaniumDecompiler {
class Attributes {
public:
    Attributes();
    ~Attributes() = default;

    static void Deserialize(BigEndianStreamReader* deserializer, Attributes& instance);

public:
    uint16_t AttributesCount = 0;
    std::vector<AttributeInfo> m_Attributes;
};
}  // namespace TitaniumDecompiler