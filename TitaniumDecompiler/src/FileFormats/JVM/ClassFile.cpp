#include "ClassFile.h"

#include "TitaniumLogger/Logger/Log.h"

namespace TitaniumDecompiler {

void ClassFile::Deserialize(BigEndianStreamReader* deserializer,
                            ClassFile& instance) {
    deserializer->ReadRawBigEndian<uint32_t>(instance.m_Magic);
    if (instance.m_Magic != MAGIC_NUMBER) {
        return;
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Minor);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_Major);
    deserializer->ReadObject(instance.m_ConstantPool);

    const ConstantPool& cpRef = instance.m_ConstantPool;
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_AccessFlags);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_ThisClass);
    deserializer->ReadRawBigEndian<uint16_t>(instance.m_SuperClass);

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_InterfaceCount);
    if (instance.m_InterfaceCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Interfaces,
                                         instance.m_InterfaceCount);
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_FieldCount);
    if (instance.m_FieldCount > 0) {
        deserializer->ReadArrayBigEndian<Fields>(instance.m_Fields,
                                                 instance.m_FieldCount, cpRef);
    }

    deserializer->ReadRawBigEndian<uint16_t>(instance.m_MethodCount);
    if (instance.m_MethodCount > 0) {
        deserializer->ReadArrayBigEndian(instance.m_Methods,
                                         instance.m_MethodCount, cpRef);
    }

    deserializer->ReadObject(instance.m_Attributes, cpRef);
}
}  // namespace TitaniumDecompiler
