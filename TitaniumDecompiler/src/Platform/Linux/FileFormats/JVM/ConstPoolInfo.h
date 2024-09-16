#pragma once

#include <cstdint>
// #include <sstream>
#include <vector>
#include <iostream> 

#include "../../../../Utils/FileReader.h"

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
    // Dynamic = 17,
    InvokeDynamic = 18,
    // Module = 19,
    // Package = 20
};

struct UTF8Info;
struct ClassInfo;

struct ConstPoolInfo {
    ConstPoolInfo() {}
	ConstPoolInfo(Tags tag) : Tag(tag) {}
	ConstPoolInfo(Tags tag, std::vector<uint8_t> info) : Tag(tag), Info(std::move(info)) {}

	Tags Tag;
	std::vector<uint8_t> Info;

    UTF8Info* asUTF8Info() { return Tag == Tags::Utf8 ? (UTF8Info*)this : nullptr; }
    ClassInfo* asClassInfo() { return Tag == Tags::Class ? (ClassInfo*)this : nullptr; }

    static void Deserialize(BigEndianStreamReader* deserializer, ConstPoolInfo &instance);
};

struct UTF8Info : public ConstPoolInfo {
	UTF8Info(uint16_t length, std::vector<uint8_t> bytes) : ConstPoolInfo(Tags::Utf8) {
        Info.push_back((length & 0xFF00) >> 8);
        Info.push_back(length & 0xFF);
        Info.insert(Info.end(), bytes.begin(), bytes.end());
    }

    uint16_t GetLength() {
        // printf("%x\n", Info.size());
        return Info.size();
    }

    uint8_t GetByte(int idx) {
        return Info[idx];
    }
};

struct IntegerInfo : public ConstPoolInfo {
	IntegerInfo(uint32_t bytes) : ConstPoolInfo(Tags::Int) {
        Info.push_back((bytes & 0xFF000000) >> 24);
        Info.push_back((bytes & 0xFF0000) >> 16);
        Info.push_back((bytes & 0xFF00) >> 8);
        Info.push_back(bytes & 0xFF);
    }
};

struct FloatInfo : public ConstPoolInfo {
	FloatInfo(uint32_t bytes) : ConstPoolInfo(Tags::Float) {
        Info.push_back((bytes & 0xFF000000) >> 24);
        Info.push_back((bytes & 0xFF0000) >> 16);
        Info.push_back((bytes & 0xFF00) >> 8);
        Info.push_back(bytes & 0xFF);
    }
	// uint32_t m_Bytes;
};

struct LongInfo : public ConstPoolInfo {
	LongInfo(uint64_t bytes) : ConstPoolInfo(Tags::Long) {
        Info.push_back((bytes & 0xFF00000000000000) >> 56);
        Info.push_back((bytes & 0xFF000000000000) >> 48);
        Info.push_back((bytes & 0xFF0000000000) >> 40);
        Info.push_back((bytes & 0xFF00000000) >> 32);
        Info.push_back((bytes & 0xFF000000) >> 24);
        Info.push_back((bytes & 0xFF0000) >> 16);
        Info.push_back((bytes & 0xFF00) >> 8);
        Info.push_back(bytes & 0xFF);
    }
};

struct DoubleInfo : public ConstPoolInfo {
	DoubleInfo(uint64_t bytes) : ConstPoolInfo(Tags::Double) {
        Info.push_back((bytes & 0xFF00000000000000) >> 56);
        Info.push_back((bytes & 0xFF000000000000) >> 48);
        Info.push_back((bytes & 0xFF0000000000) >> 40);
        Info.push_back((bytes & 0xFF00000000) >> 32);
        Info.push_back((bytes & 0xFF000000) >> 24);
        Info.push_back((bytes & 0xFF0000) >> 16);
        Info.push_back((bytes & 0xFF00) >> 8);
        Info.push_back(bytes & 0xFF);
    }
};

struct ClassInfo : public ConstPoolInfo {
	ClassInfo(uint16_t nameIndex) : ConstPoolInfo(Tags::Class) {
        Info.push_back((nameIndex & 0xFF00) >> 8);
        Info.push_back(nameIndex & 0xFF);
    }
};

struct StringInfo : public ConstPoolInfo {
	StringInfo(uint16_t stringIndex) : ConstPoolInfo(Tags::String) {
        Info.push_back((stringIndex & 0xFF00) >> 8);
        Info.push_back(stringIndex & 0xFF);
    }
};

struct FieldRefInfo : public ConstPoolInfo {
	FieldRefInfo(uint16_t classIndex, uint16_t nameAndTypeIndex) : ConstPoolInfo(Tags::Field) {
        Info.push_back((classIndex & 0xFF00) >> 8);
        Info.push_back(classIndex & 0xFF);

        Info.push_back((nameAndTypeIndex & 0xFF00) >> 8);
        Info.push_back(nameAndTypeIndex & 0xFF);
    }
};

struct MethodRefInfo : public ConstPoolInfo {
	MethodRefInfo(uint16_t classIndex, uint16_t nameAndTypeIndex) : ConstPoolInfo(Tags::Method) {
        Info.push_back((classIndex & 0xFF00) >> 8);
        Info.push_back(classIndex & 0xFF);

        Info.push_back((nameAndTypeIndex & 0xFF00) >> 8);
        Info.push_back(nameAndTypeIndex & 0xFF);  
    }
};

struct InterfaceMethodRef : public ConstPoolInfo {
	InterfaceMethodRef(uint16_t classIndex, uint16_t nameAndTypeIndex) : ConstPoolInfo(Tags::InterfaceMethod) {
        Info.push_back((classIndex & 0xFF00) >> 8);
        Info.push_back(classIndex & 0xFF);

        Info.push_back((nameAndTypeIndex & 0xFF00) >> 8);
        Info.push_back(nameAndTypeIndex & 0xFF);  
    }
};

struct NameAndTypeInfo : public ConstPoolInfo {
	NameAndTypeInfo(uint16_t nameIndex, uint16_t descriptorIndex) : ConstPoolInfo(Tags::NameAndType) {
        Info.push_back((nameIndex & 0xFF00) >> 8);
        Info.push_back(nameIndex & 0xFF);

        Info.push_back((descriptorIndex & 0xFF00) >> 8);
        Info.push_back(descriptorIndex & 0xFF);
    }
};

struct MethodHandleInfo : public ConstPoolInfo {
	MethodHandleInfo(uint8_t referenceKind, uint16_t referenceIndex) : ConstPoolInfo(Tags::MethodHandle) {
        Info.push_back(referenceKind);
        Info.push_back((referenceIndex & 0xFF00) >> 8);
        Info.push_back(referenceIndex & 0xFF); 
    }
};

struct MethodTypeInfo : public ConstPoolInfo {
	MethodTypeInfo(uint16_t descriptorIndex) : ConstPoolInfo(Tags::MethodType) {
        Info.push_back((descriptorIndex & 0xFF00) >> 8);
        Info.push_back(descriptorIndex & 0xFF);
    }
};

struct InvokeDynamicInfo : public ConstPoolInfo {
	InvokeDynamicInfo(uint16_t bootstrapMethodAttrIndex, uint16_t nameAndTypeIndex) : ConstPoolInfo(Tags::InvokeDynamic) {
        Info.push_back((bootstrapMethodAttrIndex & 0xFF00) >> 8);
        Info.push_back(bootstrapMethodAttrIndex & 0xFF);

        Info.push_back((nameAndTypeIndex & 0xFF00) >> 8);
        Info.push_back(nameAndTypeIndex & 0xFF);
    }
};

}