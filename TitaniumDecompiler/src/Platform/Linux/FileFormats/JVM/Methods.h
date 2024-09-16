#pragma once

#include "../../../../Utils/FileReader.h"
#include "Attributes.h"
#include "ConstantPool.h"
#include "AttributeStructures.h"

#include <vector>
#include <memory>

namespace TitaniumDecompiler {
class Methods {
public:
    Methods();
    ~Methods() = default;
    static void Deserialize(BigEndianStreamReader* deserializer, Methods& instance);
public:
    uint16_t m_AccessFlags;
    uint16_t m_NameIndex;
    uint16_t m_DescIndex;
    Attributes m_Attr;
};
}