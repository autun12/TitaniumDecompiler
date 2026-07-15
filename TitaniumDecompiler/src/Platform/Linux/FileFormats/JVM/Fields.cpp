#include "Fields.h"
// #include "ConstantPool.h"
#include "Attributes.h"

namespace TitaniumDecompiler {
Fields::Fields() {}

void Fields::Deserialize(BigEndianStreamReader *deserializer, Fields &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Access);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Name);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Desc);
    deserializer->ReadObject(instance.m_Attr);
}
}  // namespace TitaniumDecompiler