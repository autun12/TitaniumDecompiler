#include "StackMapFrame.h"

#include <TitaniumLogger/Logger/Log.h>

namespace TitaniumDecompiler {
void StackMapFrame::Deserialize(BigEndianStreamReader *deserializer, StackMapFrame &instance) {
    deserializer->ReadRawBigEndian<uint8_t>(instance.m_Type);
    instance.m_StackFrameType = instance.CreateStackFrameFromTag(instance);

    if (instance.m_StackFrameType == nullptr) {
        TD_DECOMP_ERROR("ERROR\n");
    }

    instance.m_StackFrameType->Deserialize(deserializer, *instance.m_StackFrameType);
}

std::shared_ptr<StackMapFrameType> StackMapFrame::CreateStackFrameFromTag(StackMapFrame &instance) {
    if (instance.m_Type >= 0 && instance.m_Type <= 63) {
        return std::make_shared<StackMapSame>();
    } else if (instance.m_Type >= 64 && instance.m_Type <= 127) {
        return std::make_shared<StackMapSameLocals1StackItemFrame>();
    } else if (instance.m_Type == 247) {
        return std::make_shared<StackMapSameLocals1StackItemFrameExtended>();
    } else if (instance.m_Type >= 248 && instance.m_Type <= 250) {
        return std::make_shared<StackMapChopFrame>();
    } else if (instance.m_Type == 251) {
        return std::make_shared<StackMapSameFrameExtended>();
    } else if (instance.m_Type >= 252 && instance.m_Type <= 254) {
        return std::make_shared<StackMapAppendFrame>(instance.m_Type);
    } else if (instance.m_Type == 255) {
        return std::make_shared<StackMapFullFrame>();

    } else {
        return nullptr;
    }
}
}  // namespace TitaniumDecompiler