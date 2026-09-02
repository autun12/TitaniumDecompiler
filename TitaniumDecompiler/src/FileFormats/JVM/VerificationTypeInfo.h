#pragma once

#include <memory>

#include "Utils/FileStream.h"

namespace TitaniumDecompiler {
enum VerificationTypeInfoTags : uint8_t {
    TYPE_TOP = 0,
    TYPE_INTEGER = 1,
    TYPE_FLOAT = 2,
    TYPE_DOUBLE = 3,
    TYPE_LONG = 4,
    TYPE_NULL = 5,
    TYPE_UNINITIALIZEDTHIS = 6,
    TYPE_OBJECT = 7,
    TYPE_UNINITIALIZED = 8,
    UNKNOWN = 9
};

struct VerificationType {
    virtual ~VerificationType() = default;
    virtual void Deserialize(BigEndianStreamReader* deserializer,
                             VerificationType& instance) = 0;
};

struct VerificationTypeInfo {
public:
    VerificationTypeInfo()
        : m_Tags(VerificationTypeInfoTags::UNKNOWN),
          m_VerificationType(nullptr) {}
    VerificationTypeInfo(VerificationTypeInfoTags tags, VerificationType* type)
        : m_Tags(tags), m_VerificationType(type) {}

    ~VerificationTypeInfo() = default;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            VerificationTypeInfo& instance);
    std::string ResolveType() const {
        switch (m_Tags) {
            case TYPE_TOP:
                return "top";  // Unused or uninitialized slot
            case TYPE_INTEGER:
                return "int";
            case TYPE_FLOAT:
                return "float";
            case TYPE_DOUBLE:
                return "double";
            case TYPE_LONG:
                return "long";
            case TYPE_NULL:
                return "null";
            case TYPE_UNINITIALIZEDTHIS:
                return "this";
            default:
                return "unknown";
        }
    }

private:
    std::shared_ptr<VerificationType> CreateVerificationTypeFromTag(
        VerificationTypeInfo& instance);

public:
    VerificationTypeInfoTags m_Tags;
    std::shared_ptr<VerificationType> m_VerificationType;
};

struct TopVariableInfo : VerificationType {
    TopVariableInfo() {}
    ~TopVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        TopVariableInfo& topVariableInfoInstance =
            static_cast<TopVariableInfo&>(instance);
    }
};

struct IntegerVariableInfo : VerificationType {
    IntegerVariableInfo() {}
    ~IntegerVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        IntegerVariableInfo& intVariableInfoInstance =
            static_cast<IntegerVariableInfo&>(instance);
    }
};

struct FloatVariableInfo : VerificationType {
    FloatVariableInfo() {}
    ~FloatVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        FloatVariableInfo& floatVariableInfoInstance =
            static_cast<FloatVariableInfo&>(instance);
    }
};

struct LongVariableInfo : VerificationType {
    LongVariableInfo() {}
    ~LongVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        LongVariableInfo& longVariableInfoInstance =
            static_cast<LongVariableInfo&>(instance);
    }
};

struct DoubleVariableInfo : VerificationType {
    DoubleVariableInfo() {}
    ~DoubleVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        DoubleVariableInfo& doubleVariableInfoInstance =
            static_cast<DoubleVariableInfo&>(instance);
    }
};

struct NullVariableInfo : VerificationType {
    NullVariableInfo() {}
    ~NullVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        NullVariableInfo& nullVariableInfoInstance =
            static_cast<NullVariableInfo&>(instance);
    }
};

struct UninitializedThisVariableInfo : VerificationType {
    UninitializedThisVariableInfo() {}
    ~UninitializedThisVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        UninitializedThisVariableInfo& uninitThisVariableInfoInstance =
            static_cast<UninitializedThisVariableInfo&>(instance);
    }
};

struct ObjectVariableInfo : VerificationType {
    ObjectVariableInfo() {}
    ~ObjectVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        ObjectVariableInfo& objectVariableInfoInstance =
            static_cast<ObjectVariableInfo&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            objectVariableInfoInstance.cpoolIndex);
    }

    uint16_t cpoolIndex;
};

struct UninitializedVariableInfo : VerificationType {
    UninitializedVariableInfo() {}
    ~UninitializedVariableInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer,
                     VerificationType& instance) override {
        UninitializedVariableInfo& unInitVariableInfoInstance =
            static_cast<UninitializedVariableInfo&>(instance);
        deserializer->ReadRawBigEndian<uint16_t>(
            unInitVariableInfoInstance.offset);
    }

    uint16_t offset;
};
}  // namespace TitaniumDecompiler
