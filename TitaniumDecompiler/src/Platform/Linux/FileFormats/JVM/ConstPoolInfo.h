#pragma once

#include <cstdint>
// #include <sstream>
#include <iostream>
#include <vector>

#include "Utils/FileReader.h"

namespace TitaniumDecompiler {
enum Tags : uint8_t {
    // Null,
    Utf8 = 1,
    Int = 3,
    Float = 4,
    Long = 5,
    Double = 6,
    Class = 7,
    String = 8,
    Field = 9,
    Method = 10,
    InterfaceMethod = 11,
    NameAndType = 12,
    MethodHandle = 15,
    MethodType = 16,
    Dynamic = 17,
    InvokeDynamic = 18,
    Module = 19,
    Package = 20
};

struct ConstPool {
    virtual ~ConstPool() = default;
    virtual void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) = 0;
};

struct UTF8Info : public ConstPool {
    UTF8Info() {}
    ~UTF8Info() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        UTF8Info& utf8InfoInstance = static_cast<UTF8Info&>(instance);
        Deserialize(deserializer, utf8InfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, UTF8Info& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.length);
        if (instance.length > 0) {
            deserializer->ReadArrayBigEndian(instance.bytes, instance.length);
        }
    }

    uint16_t length = 0;
    std::vector<uint8_t> bytes = {};
};

struct IntegerInfo : public ConstPool {
    IntegerInfo() {}
    ~IntegerInfo() = default;
    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        IntegerInfo& intInfoInstance = static_cast<IntegerInfo&>(instance);
        Deserialize(deserializer, intInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, IntegerInfo& instance) { deserializer->ReadRawBigEndian<uint32_t>(instance.bytes); }

    uint32_t bytes;
};

struct FloatInfo : public ConstPool {
    FloatInfo() {}
    ~FloatInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        FloatInfo& floatInfoInstance = static_cast<FloatInfo&>(instance);
        Deserialize(deserializer, floatInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, FloatInfo& instance) { deserializer->ReadRawBigEndian<uint32_t>(instance.bytes); }

    uint32_t bytes;
};

struct LongInfo : public ConstPool {
    LongInfo() {}
    ~LongInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        LongInfo& longInfoInstance = static_cast<LongInfo&>(instance);
        Deserialize(deserializer, longInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, LongInfo& instance) { deserializer->ReadRawBigEndian<uint64_t>(instance.bytes); }

    uint64_t bytes;
};

struct DoubleInfo : public ConstPool {
    DoubleInfo() {}
    ~DoubleInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        DoubleInfo& doubleInfoInstance = static_cast<DoubleInfo&>(instance);
        Deserialize(deserializer, doubleInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, DoubleInfo& instance) { deserializer->ReadRawBigEndian<uint64_t>(instance.bytes); }

    uint64_t bytes;
};

struct ClassInfo : public ConstPool {
    ClassInfo() {}
    ~ClassInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        ClassInfo& classInfoInstance = static_cast<ClassInfo&>(instance);
        Deserialize(deserializer, classInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, ClassInfo& instance) { deserializer->ReadRawBigEndian<uint16_t>(instance.nameIndex); }

    uint16_t nameIndex;
};

struct StringInfo : public ConstPool {
    StringInfo() {}
    ~StringInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        StringInfo& stringInfoInstance = static_cast<StringInfo&>(instance);
        Deserialize(deserializer, stringInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, StringInfo& instance) { deserializer->ReadRawBigEndian<uint16_t>(instance.stringIndex); }

    uint16_t stringIndex;
};

struct FieldRefInfo : public ConstPool {
    FieldRefInfo() {}
    ~FieldRefInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        FieldRefInfo& fieldInfoInstance = static_cast<FieldRefInfo&>(instance);
        Deserialize(deserializer, fieldInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, FieldRefInfo& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.classIndex);
        deserializer->ReadRawBigEndian<uint16_t>(instance.nameAndTypeIndex);
    }

    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
};

struct MethodRefInfo : public ConstPool {
    MethodRefInfo() {}
    ~MethodRefInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        MethodRefInfo& methodInfoInstance = static_cast<MethodRefInfo&>(instance);
        Deserialize(deserializer, methodInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, MethodRefInfo& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.classIndex);
        deserializer->ReadRawBigEndian<uint16_t>(instance.nameAndTypeIndex);
    }

    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
};

struct InterfaceMethodRef : public ConstPool {
    InterfaceMethodRef() {}
    ~InterfaceMethodRef() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        InterfaceMethodRef& interfaceMethodInfoInstance = static_cast<InterfaceMethodRef&>(instance);
        Deserialize(deserializer, interfaceMethodInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, InterfaceMethodRef& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.classIndex);
        deserializer->ReadRawBigEndian<uint16_t>(instance.nameAndTypeIndex);
    }

    uint16_t classIndex;
    uint16_t nameAndTypeIndex;
};

struct NameAndTypeInfo : public ConstPool {
    NameAndTypeInfo() {}
    ~NameAndTypeInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        NameAndTypeInfo& nameAndTypeInfoInstance = static_cast<NameAndTypeInfo&>(instance);
        Deserialize(deserializer, nameAndTypeInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, NameAndTypeInfo& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.nameIndex);
        deserializer->ReadRawBigEndian<uint16_t>(instance.descriptorIndex);
    }

    uint16_t nameIndex;
    uint16_t descriptorIndex;
};

struct MethodHandleInfo : public ConstPool {
    MethodHandleInfo() {}
    ~MethodHandleInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        MethodHandleInfo& methodHandleInfoInstance = static_cast<MethodHandleInfo&>(instance);
        Deserialize(deserializer, methodHandleInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, MethodHandleInfo& instance) {
        deserializer->ReadRawBigEndian<uint8_t>(instance.referenceKind);
        deserializer->ReadRawBigEndian<uint16_t>(instance.referenceIndex);
    }

    uint8_t referenceKind;
    uint16_t referenceIndex;
};

struct MethodTypeInfo : public ConstPool {
    MethodTypeInfo() {}
    ~MethodTypeInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        MethodTypeInfo& methodTypeInfoInstance = static_cast<MethodTypeInfo&>(instance);
        Deserialize(deserializer, methodTypeInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, MethodTypeInfo& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.descriptorIndex);
    }

    uint16_t descriptorIndex;
};

struct DynamicInfo : public ConstPool {
    DynamicInfo() {}
    ~DynamicInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        DynamicInfo& dynamicInfoInstance = static_cast<DynamicInfo&>(instance);
        Deserialize(deserializer, dynamicInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, DynamicInfo& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.bootstrapMethodAttrIndex);
        deserializer->ReadRawBigEndian<uint16_t>(instance.nameAndTypeIndex);
    }

    uint16_t bootstrapMethodAttrIndex;
    uint16_t nameAndTypeIndex;
};

struct InvokeDynamicInfo : public ConstPool {
    InvokeDynamicInfo() {}
    ~InvokeDynamicInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        InvokeDynamicInfo& invokeDynamicInfoInstance = static_cast<InvokeDynamicInfo&>(instance);
        Deserialize(deserializer, invokeDynamicInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, InvokeDynamicInfo& instance) {
        deserializer->ReadRawBigEndian<uint16_t>(instance.bootstrapMethodAttrIndex);
        deserializer->ReadRawBigEndian<uint16_t>(instance.nameAndTypeIndex);
    }

    uint16_t bootstrapMethodAttrIndex;
    uint16_t nameAndTypeIndex;
};

struct ModuleInfo : public ConstPool {
    ModuleInfo() {}
    ~ModuleInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        ModuleInfo& moduleInstance = static_cast<ModuleInfo&>(instance);
        Deserialize(deserializer, moduleInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, ModuleInfo& instance) { deserializer->ReadRawBigEndian<uint16_t>(instance.nameIndex); }

    uint16_t nameIndex;
};

struct PackageInfo : public ConstPool {
    PackageInfo() {}
    ~PackageInfo() = default;

    void Deserialize(BigEndianStreamReader* deserializer, ConstPool& instance) override {
        PackageInfo& packageInfoInstance = static_cast<PackageInfo&>(instance);
        Deserialize(deserializer, packageInfoInstance);
    }

    static void Deserialize(BigEndianStreamReader* deserializer, PackageInfo& instance) { deserializer->ReadRawBigEndian<uint16_t>(instance.nameIndex); }

    uint16_t nameIndex;
};

struct ConstPoolInfo {
public:
    ConstPoolInfo() {}
    ConstPoolInfo(Tags tag) : Tag(tag) {}
    ConstPoolInfo(Tags tag, ConstPool* info) : Tag(tag), Info(std::move(info)) {}

    UTF8Info* asUTF8Info() { return Tag == Tags::Utf8 ? dynamic_cast<UTF8Info*>(Info.get()) : nullptr; }
    ClassInfo* asClassInfo() { return Tag == Tags::Class ? dynamic_cast<ClassInfo*>(Info.get()) : nullptr; }
    NameAndTypeInfo* asNameAndType() { return Tag == Tags::NameAndType ? dynamic_cast<NameAndTypeInfo*>(Info.get()) : nullptr; }
    StringInfo* asStringInfo() { return Tag == Tags::String ? dynamic_cast<StringInfo*>(Info.get()) : nullptr; }
    IntegerInfo* asIntInfo() { return Tag == Tags::Int ? dynamic_cast<IntegerInfo*>(Info.get()) : nullptr; }
    FloatInfo* asFloatInfo() { return Tag == Tags::Float ? dynamic_cast<FloatInfo*>(Info.get()) : nullptr; }
    MethodRefInfo* asMethodRefInfo() { return Tag == Tags::Method ? dynamic_cast<MethodRefInfo*>(Info.get()) : nullptr; }
    // ClassInfo* asClassInfo() { return Tag == Tags::Class ? dynamic_cast<ClassInfo*>(Info.get()) : nullptr; }
    // ClassInfo* asClassInfo() { return Tag == Tags::Class ? dynamic_cast<ClassInfo*>(Info.get()) : nullptr; }
    // ClassInfo* asClassInfo() { return Tag == Tags::Class ? dynamic_cast<ClassInfo*>(Info.get()) : nullptr; }
    InvokeDynamicInfo* asInvokeDynamicInfo() { return Tag == Tags::InvokeDynamic ? dynamic_cast<InvokeDynamicInfo*>(Info.get()) : nullptr; }

    static void Deserialize(BigEndianStreamReader* deserializer, ConstPoolInfo& instance);

public:
    Tags Tag;
    std::shared_ptr<ConstPool> Info;

private:
    std::shared_ptr<ConstPool> CreateConstPoolItemFromTag(ConstPoolInfo& instance);
};

}  // namespace TitaniumDecompiler