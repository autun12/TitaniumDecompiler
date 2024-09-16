#include "Fields.h"
// #include "ConstantPool.h"
#include "Attributes.h"

namespace TitaniumDecompiler {
    Fields::Fields() {}

    void Fields::Deserialize(BigEndianStreamReader *deserializer, Fields &instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.m_Access);
        deserializer->ReadRawBigEndian<uint16_t>(instance.m_Name);
        deserializer->ReadRawBigEndian<uint16_t>(instance.m_Desc);
        // deserializer->ReadObject(instance.m_Attr);
    }
    // Fields::Fields(FileReader& reader, ConstantPool& cp) {
    //     m_Access = reader.ReadUInt16();
    //     std::cout << m_Access << std::endl;
    //     m_Name = reader.ReadUInt16();
    //     m_Desc = reader.ReadUInt16();
    //     m_AttrCount = reader.ReadUInt16();

    //     for(size_t i = 0; i < m_AttrCount; i++) {
    //         std::shared_ptr<Attribute> newAttr(new Attributes(reader, cp));
    //         m_Attr.push_back(newAttr);
    //     }
    // }
}