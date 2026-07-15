#include "Attributes.h"

#include <iostream>

#include "AttributeStructures.h"

namespace TitaniumDecompiler {
Attributes::Attributes() {}

void Attributes::Deserialize(BigEndianStreamReader *deserializer, Attributes &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.AttributesCount);
    if (instance.AttributesCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Attributes, instance.AttributesCount);
    }
}
}  // namespace TitaniumDecompiler