#include <gtest/gtest.h>

#include <cstring>

#include "FileFormats/JVM/AttributeInnerStructures.h"
#include "Utils/StreamReader.h"

using namespace TitaniumDecompiler;

class MockStreamReader : public TitaniumDecompiler::BigEndianStreamReader {
public:
    MockStreamReader(std::vector<uint8_t>& buffer)
        : m_Buffer(buffer), m_Pos(0) {}

    bool IsStreamGood() const override { return m_Pos < m_Buffer.size(); }

    uint64_t GetStreamPosition() override { return m_Pos; }

    void SetStreamPosition(uint64_t position) override { m_Pos = position; }

    bool ReadData(char* destination, size_t size) override {
        if (m_Pos + size > m_Buffer.size()) return false;
        std::memcpy(destination, m_Buffer.data() + m_Pos, size);
        m_Pos += size;
        return true;
    }

private:
    std::vector<uint8_t>& m_Buffer;
    size_t m_Pos;
};

class AttributeInnerStructuresTest : public ::testing::Test {
protected:
    std::vector<uint8_t> buffer;
    std::unique_ptr<BigEndianStreamReader> reader;

    void SetUpBuffer(const std::vector<uint8_t>& data) {
        buffer = data;
        reader = std::make_unique<MockStreamReader>(buffer);
    }
};

TEST_F(AttributeInnerStructuresTest, LineNumTableItemDeserialization) {
    SetUpBuffer({0x00, 0x10, 0x00, 0x1A});  // startPc = 16, lineNumber = 26
    LineNumTableItem item;
    LineNumTableItem::Deserialize(reader.get(), item);
    EXPECT_EQ(item.startPc, 16);
    EXPECT_EQ(item.lineNumber, 26);
}

TEST_F(AttributeInnerStructuresTest, LocalVariableTableEntryDeserialization) {
    SetUpBuffer({0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05});
    LocalVariableTableEntry entry;
    LocalVariableTableEntry::Deserialize(reader.get(), entry);
    EXPECT_EQ(entry.startPc, 1);
    EXPECT_EQ(entry.length, 2);
    EXPECT_EQ(entry.nameIndex, 3);
    EXPECT_EQ(entry.descIndex, 4);
    EXPECT_EQ(entry.index, 5);
}

TEST_F(AttributeInnerStructuresTest, ClassesDeserialization) {
    SetUpBuffer({0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04});
    Classes cls;
    Classes::Deserialize(reader.get(), cls);
    EXPECT_EQ(cls.innerClassInfoIdx, 1);
    EXPECT_EQ(cls.outerClassInfoIdx, 2);
    EXPECT_EQ(cls.innerNameIdx, 3);
    EXPECT_EQ(cls.innerClassAccessFlags, 4);
}

TEST_F(AttributeInnerStructuresTest, BootstrapMethodsInnerDeserialization) {
    SetUpBuffer({
        0x00, 0x02,             // bootstrapMethodRef
        0x00, 0x02,             // numBootstrapArgs
        0x00, 0x03, 0x00, 0x04  // bootstrapArgs[0]=3, [1]=4
    });
    BootstrapMethodsInner inner;
    BootstrapMethodsInner::Deserialize(reader.get(), inner);
    EXPECT_EQ(inner.bootstrapMethodRef, 2);
    EXPECT_EQ(inner.numBootstrapArgs, 2);
    ASSERT_EQ(inner.bootstrapArgs.size(), 2);
    EXPECT_EQ(inner.bootstrapArgs[0], 3);
    EXPECT_EQ(inner.bootstrapArgs[1], 4);
}

TEST_F(AttributeInnerStructuresTest, ParameterAnnotationsDeserialization) {
    SetUpBuffer({
        0x00,
        0x01,  // numAnnotations = 1
        0x00,
        0x02,  // typeIdx
        0x00,
        0x00,  // numElemValPairs = 0
    });
    ParameterAnnotations paramAnno;
    ParameterAnnotations::Deserialize(reader.get(), paramAnno);
    ASSERT_EQ(paramAnno.annotations.size(), 1);
    EXPECT_EQ(paramAnno.annotations[0].typeIdx, 2);
    EXPECT_EQ(paramAnno.annotations[0].numElemValPairs, 0);
}
