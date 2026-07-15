#include <gtest/gtest.h>
#include "Platform/Linux/FileFormats/JVM/Attributes.h"
#include "Platform/Linux/FileFormats/JVM/AttributeStructures.h"
#include <vector>
#include <memory>
#include <cstring>

using namespace TitaniumDecompiler;

// Mock concrete implementation of BigEndianStreamReader for testing
class MockStreamReader : public BigEndianStreamReader {
public:
    explicit MockStreamReader(const std::vector<uint8_t>& data)
        : m_Data(data), m_Pos(0) {}

    bool IsStreamGood() const override {
        return m_Pos < m_Data.size();
    }

    uint64_t GetStreamPosition() override {
        return m_Pos;
    }

    void SetStreamPosition(uint64_t position) override {
        m_Pos = position;
    }

    bool ReadData(char* destination, size_t size) override {
        if (m_Pos + size > m_Data.size()) return false;
        std::memcpy(destination, m_Data.data() + m_Pos, size);
        m_Pos += size;
        return true;
    }

private:
    std::vector<uint8_t> m_Data;
    size_t m_Pos;
};

class AttributesTest : public ::testing::Test {
protected:
    std::unique_ptr<MockStreamReader> MakeReaderFromVector(const std::vector<uint8_t>& data) {
        return std::make_unique<MockStreamReader>(data);
    }
};

TEST_F(AttributesTest, DeserializeZeroAttributes) {
    std::vector<uint8_t> data = {0x00, 0x00};  // AttributesCount = 0
    auto reader = MakeReaderFromVector(data);

    Attributes attrs;
    Attributes::Deserialize(reader.get(), attrs);

    EXPECT_EQ(attrs.AttributesCount, 0);
    EXPECT_TRUE(attrs.m_Attributes.empty());
}

TEST_F(AttributesTest, DeserializeSingleAttribute) {
    std::vector<uint8_t> data = {
        0x00, 0x01,         // AttributesCount = 1
        0x00, 0x01,         // tag = 1 (mock tag)
        0x00, 0x00, 0x00, 0x00  // length = 0
    };
    auto reader = MakeReaderFromVector(data);

    Attributes attrs;
    EXPECT_NO_THROW(Attributes::Deserialize(reader.get(), attrs));
    EXPECT_EQ(attrs.AttributesCount, 1);
    EXPECT_EQ(attrs.m_Attributes.size(), 1);
}

TEST_F(AttributesTest, DeserializeMultipleAttributes) {
    std::vector<uint8_t> data = {
        0x00, 0x02,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00,  // attr 1
        0x00, 0x02, 0x00, 0x00, 0x00, 0x00   // attr 2
    };
    auto reader = MakeReaderFromVector(data);

    Attributes attrs;
    EXPECT_NO_THROW(Attributes::Deserialize(reader.get(), attrs));
    EXPECT_EQ(attrs.AttributesCount, 2);
    EXPECT_EQ(attrs.m_Attributes.size(), 2);
}

TEST_F(AttributesTest, GracefullyHandlesTruncatedStream) {
    std::vector<uint8_t> data = {
        0x00, 0x01,             // AttributesCount = 1
        0x00, 0x01,             // tag = 1 (fake tag)
        0x00, 0x00, 0x00        // incomplete length (only 3 bytes instead of 4)
    };

    auto reader = MakeReaderFromVector(data);
    Attributes attrs;

    EXPECT_NO_THROW(Attributes::Deserialize(reader.get(), attrs));
    EXPECT_EQ(attrs.AttributesCount, 1);
    EXPECT_EQ(attrs.m_Attributes.size(), 1);

    // Check that the attribute is null or a placeholder object
    EXPECT_EQ(attrs.m_Attributes[0].info, nullptr);
}



