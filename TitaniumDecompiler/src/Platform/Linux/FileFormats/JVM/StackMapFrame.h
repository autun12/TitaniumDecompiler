#pragma once

#include "Utils/FileReader.h"
// #include "AttributeStructures.h"
#include <vector>

#include "VerificationTypeInfo.h"

namespace TitaniumDecompiler {
struct StackMapFrameType {
    virtual ~StackMapFrameType() = default;
    virtual void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) = 0;
};

struct StackMapFrame {
public:
    StackMapFrame() : m_Type(0), m_StackFrameType(nullptr) {}
    StackMapFrame(uint8_t type, StackMapFrameType* stackFrameType) : m_Type(type), m_StackFrameType(stackFrameType) {}

    ~StackMapFrame() = default;

    static void Deserialize(BigEndianStreamReader* deserializer, StackMapFrame& instance);

private:
    std::shared_ptr<StackMapFrameType> CreateStackFrameFromTag(StackMapFrame& instance);

public:
    uint8_t m_Type;
    std::shared_ptr<StackMapFrameType> m_StackFrameType;
};

struct StackMapSame : StackMapFrameType {
    StackMapSame() {}
    ~StackMapSame() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapSame& stackMapSameInstance = static_cast<StackMapSame&>(instance);
    }
};

struct StackMapSameLocals1StackItemFrame : StackMapFrameType {
    StackMapSameLocals1StackItemFrame() {}
    ~StackMapSameLocals1StackItemFrame() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapSameLocals1StackItemFrame& stackMapSameLocals1StackItemFrameInstance = static_cast<StackMapSameLocals1StackItemFrame&>(instance);
        Deserialize(deserializer, stackMapSameLocals1StackItemFrameInstance);
    };

    static void Deserialize(BigEndianStreamReader* deserializer, StackMapSameLocals1StackItemFrame& instance) {
        deserializer->ReadArrayBigEndian(instance.stack, 1);
    }

    std::vector<VerificationTypeInfo> stack;
};

struct StackMapSameLocals1StackItemFrameExtended : StackMapFrameType {
    StackMapSameLocals1StackItemFrameExtended() {}
    ~StackMapSameLocals1StackItemFrameExtended() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapSameLocals1StackItemFrameExtended& stackMapSameLocals1StackItemFrameExtendedInstance =
            static_cast<StackMapSameLocals1StackItemFrameExtended&>(instance);
        Deserialize(deserializer, stackMapSameLocals1StackItemFrameExtendedInstance);
    };

    static void Deserialize(BigEndianStreamReader* deserializer, StackMapSameLocals1StackItemFrameExtended& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.offsetDelta);
        deserializer->ReadArrayBigEndian(instance.stack, 1);
    }

    uint16_t offsetDelta;
    std::vector<VerificationTypeInfo> stack;
};

struct StackMapChopFrame : StackMapFrameType {
    StackMapChopFrame() {}
    ~StackMapChopFrame() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapChopFrame& stackMapChopFrameInstance = static_cast<StackMapChopFrame&>(instance);
        Deserialize(deserializer, stackMapChopFrameInstance);
    };

    static void Deserialize(BigEndianStreamReader* deserializer, StackMapChopFrame& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.offsetDelta);
    }
    uint16_t offsetDelta;
};

struct StackMapSameFrameExtended : StackMapFrameType {
    StackMapSameFrameExtended() {}
    ~StackMapSameFrameExtended() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapSameFrameExtended& stackMapSameFrameExtendedInstance = static_cast<StackMapSameFrameExtended&>(instance);
        Deserialize(deserializer, stackMapSameFrameExtendedInstance);
    };

    static void Deserialize(BigEndianStreamReader* deserializer, StackMapSameFrameExtended& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.offsetDelta);
    }
    uint16_t offsetDelta;
};

struct StackMapAppendFrame : StackMapFrameType {
    StackMapAppendFrame(uint8_t frameType) { m_FrameType = frameType; }
    ~StackMapAppendFrame() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapAppendFrame& stackMapAppendFrame = static_cast<StackMapAppendFrame&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(stackMapAppendFrame.offsetDelta);
        if (m_FrameType - 251 > 0) {
            deserializer->ReadArrayBigEndian(stackMapAppendFrame.locals, m_FrameType - 251);
        }
    };

    uint8_t m_FrameType;
    uint16_t offsetDelta;
    std::vector<VerificationTypeInfo> locals;
};

struct StackMapFullFrame : StackMapFrameType {
    StackMapFullFrame() {}
    ~StackMapFullFrame() = default;

    void Deserialize(BigEndianStreamReader* deserializer, StackMapFrameType& instance) override {
        StackMapFullFrame& stackMapFullFrameInstance = static_cast<StackMapFullFrame&>(instance);
        Deserialize(deserializer, stackMapFullFrameInstance);
    };

    static void Deserialize(BigEndianStreamReader* deserializer, StackMapFullFrame& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.offsetDelta);
        deserializer->ReadRawBigEndian<uint16_t>(instance.numOfLocals);

        if (instance.numOfLocals > 0) {
            deserializer->ReadArrayBigEndian(instance.locals, instance.numOfLocals);
        }

        deserializer->ReadRawBigEndian<uint16_t>(instance.numOfStackItems);

        if (instance.numOfStackItems > 0) {
            deserializer->ReadArrayBigEndian(instance.stack, instance.numOfStackItems);
        }
    }
    uint16_t offsetDelta;
    uint16_t numOfLocals;
    std::vector<VerificationTypeInfo> locals;
    uint16_t numOfStackItems;
    std::vector<VerificationTypeInfo> stack;
};
}  // namespace TitaniumDecompiler
