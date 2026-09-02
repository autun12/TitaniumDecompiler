#pragma once

#include <cstdint>
#include <memory>
// #include <string>
#include <vector>

#include "Attributes.h"
#include "ConstantPool.h"
#include "Utils/FileStream.h"

namespace TitaniumDecompiler {
class Fields {
public:
    Fields();
    ~Fields() = default;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            Fields& instance, const ConstantPool& cp);

public:
    uint16_t m_Access = 0;
    uint16_t m_Name = 0;
    uint16_t m_Desc = 0;
    Attributes m_Attr;
};
}  // namespace TitaniumDecompiler
