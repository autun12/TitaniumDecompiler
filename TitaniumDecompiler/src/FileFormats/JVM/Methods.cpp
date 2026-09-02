#include "Methods.h"

namespace TitaniumDecompiler {

Methods::Methods() {}
void Methods::Deserialize(BigEndianStreamReader* deserializer,
                          Methods& instance, const ConstantPool& constPool) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AccessFlags);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_NameIndex);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_DescIndex);
    deserializer->ReadObject(instance.m_Attr, constPool);
}
}  // namespace TitaniumDecompiler
