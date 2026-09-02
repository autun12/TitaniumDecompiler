#include <gtest/gtest.h>

#include <cstring>
#include <memory>
#include <vector>

#include "FileFormats/JVM/AttributeStructures.h"
#include "FileFormats/JVM/Attributes.h"
#include "Utils/StreamReader.h"

using namespace TitaniumDecompiler;

// -------------------- Mock StreamReader --------------------

class MockStreamReader : public BigEndianStreamReader {
public:
    std::vector<uint8_t> buffer;
    size_t pos = 0;

    explicit MockStreamReader(std::vector<uint8_t> data)
        : buffer(std::move(data)) {}

    bool IsStreamGood() const override { return pos < buffer.size(); }
    uint64_t GetStreamPosition() override { return pos; }
    void SetStreamPosition(uint64_t p) override { pos = p; }

    bool ReadData(char* destination, size_t size) override {
        if (pos + size > buffer.size()) return false;
        std::memcpy(destination, buffer.data() + pos, size);
        pos += size;
        return true;
    }
};

// -------------------- Tests --------------------

TEST(AttributeStructuresTest, GetAttrTypeFromName_RecognizedAttributes) {
    AttributeInfo info;

    EXPECT_EQ(info.GetAttrTypeFromName("Code"), CodeType);
    EXPECT_EQ(info.GetAttrTypeFromName("LineNumberTable"), LineNumberTableType);
    EXPECT_EQ(info.GetAttrTypeFromName("Exceptions"), ExceptionsType);
    EXPECT_EQ(info.GetAttrTypeFromName("StackMapTable"), StackMapTableType);
    EXPECT_EQ(info.GetAttrTypeFromName("LocalVariableTable"),
              LocalVariableTableType);
    EXPECT_EQ(info.GetAttrTypeFromName("Signature"), SignatureType);
    EXPECT_EQ(info.GetAttrTypeFromName("Synthetic"), SyntheticType);
    EXPECT_EQ(info.GetAttrTypeFromName("AnnotationDefault"),
              AnnotationDefaultType);
    EXPECT_EQ(info.GetAttrTypeFromName("RuntimeVisibleAnnotations"),
              RuntimeVisibleAnnotationsType);
    EXPECT_EQ(info.GetAttrTypeFromName("NestMembers"), NestMembersType);
}

TEST(AttributeStructuresTest,
     GetAttrTypeFromName_UnrecognizedNameLeavesTagUnchanged) {
    AttributeInfo info;
    info.tag = ConstantValueType;

    AttributeTypes result = info.GetAttrTypeFromName("NotARealAttribute");

    EXPECT_EQ(result, ConstantValueType);  // tag should be unchanged
}

TEST(AttributeStructuresTest, Deserialize_InvalidNameFailsGracefully) {
    // Attribute name index = 1 (invalid)
    std::vector<uint8_t> bytes = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

    auto reader = std::make_unique<MockStreamReader>(bytes);
    AttributeInfo info;

    // This should fail silently (info.info remains null)
    EXPECT_NO_THROW(AttributeInfo::Deserialize(reader.get(), info));
    EXPECT_EQ(info.info, nullptr);
}
