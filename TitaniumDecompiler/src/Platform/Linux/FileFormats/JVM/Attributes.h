#pragma once

#include <cstdint>
#include <memory>

#include "../../../../Utils/FileReader.h"
#include "AttributeStructures.h"
#include "ConstantPool.h"

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
}