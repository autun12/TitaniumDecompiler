#include <gtest/gtest.h>
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/ConstantPool.h"
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/ConstPoolInfo.h"
#include "Utils/StreamReader.h"

using namespace TitaniumDecompiler;


class MockBigEndianStreamReader : public TitaniumDecompiler::BigEndianStreamReader {
    public:
        std::vector<uint8_t> buffer;
        size_t pos = 0;
    
        explicit MockBigEndianStreamReader(const std::vector<uint8_t>& data) : buffer(data), pos(0) {}
    
        bool IsStreamGood() const override {
            return pos <= buffer.size();
        }
    
        uint64_t GetStreamPosition() override {
            return pos;
        }
    
        void SetStreamPosition(uint64_t position) override {
            pos = position;
        }
    
        bool ReadData(char* destination, size_t size) override {
            if (pos + size > buffer.size()) return false;
            memcpy(destination, buffer.data() + pos, size);
            pos += size;
            return true;
        }
    };

// Mock UTF8Info helper
std::shared_ptr<UTF8Info> CreateMockUtf8(const std::string& value) {
    auto utf8 = std::make_shared<UTF8Info>();
    utf8->length = value.size();
    utf8->bytes = std::vector<uint8_t>(value.begin(), value.end());
    return utf8;
}

// Mock ClassInfo helper
std::shared_ptr<ClassInfo> CreateMockClassInfo(uint16_t nameIdx) {
    auto classInfo = std::make_shared<ClassInfo>();
    classInfo->nameIndex = nameIdx;
    return classInfo;
}

// Mock NameAndType helper
std::shared_ptr<NameAndTypeInfo> CreateMockNameAndTypeInfo(uint16_t nameIdx, uint16_t descIdx) {
    auto info = std::make_shared<NameAndTypeInfo>();
    info->nameIndex = nameIdx;
    info->descriptorIndex = descIdx;
    return info;
}

TEST(ConstantPoolTest, GetConstantUTF8_ReturnsExpectedString) {
    ConstantPool pool;
    ConstPoolInfo info;
    info.Tag = Tags::Utf8;
    info.Info = CreateMockUtf8("Hello");

    pool.m_ConstPoolInfo.push_back(info);  // index 0

    g_ConstPool = {pool};
    std::string result = GetConstantUTF8(1); // 1-based index
    EXPECT_EQ(result, "Hello");
}

TEST(ConstantPoolTest, GetConstantClass_ReturnsClassNameFromUtf8) {
    ConstantPool pool;

    // Mock constant pool layout:
    // 1 -> UTF8("MyClass")
    // 2 -> Class(nameIndex=1)
    ConstPoolInfo utf8;
    utf8.Tag = Tags::Utf8;
    utf8.Info = CreateMockUtf8("MyClass");

    ConstPoolInfo classInfo;
    classInfo.Tag = Tags::Class;
    classInfo.Info = CreateMockClassInfo(1); // Refers to idx 1

    pool.m_ConstPoolInfo.push_back(utf8);      // idx 0
    pool.m_ConstPoolInfo.push_back(classInfo); // idx 1

    g_ConstPool = {pool};
    std::string result = GetConstantClass(2);
    EXPECT_EQ(result, "MyClass");
}

TEST(ConstantPoolTest, GetNameAndType_ReturnsFormattedString) {
    ConstantPool pool;

    // Mock layout:
    // 1: UTF8("methodName")
    // 2: UTF8("()V")
    // 3: NameAndType(nameIdx=1, descIdx=2)
    ConstPoolInfo name;
    name.Tag = Tags::Utf8;
    name.Info = CreateMockUtf8("methodName");

    ConstPoolInfo desc;
    desc.Tag = Tags::Utf8;
    desc.Info = CreateMockUtf8("()V");

    ConstPoolInfo nameType;
    nameType.Tag = Tags::NameAndType;
    nameType.Info = CreateMockNameAndTypeInfo(1, 2);

    pool.m_ConstPoolInfo.push_back(name);     // 0
    pool.m_ConstPoolInfo.push_back(desc);     // 1
    pool.m_ConstPoolInfo.push_back(nameType); // 2

    g_ConstPool = {pool};
    std::string result = GetNameAndType(3); // index 3 (1-based)
    EXPECT_EQ(result, "methodName:()V");
}

TEST(ConstantPoolTest, GetConstant_ReturnsCorrectEntry) {
    ConstantPool pool;

    ConstPoolInfo info;
    info.Tag = Tags::Utf8;
    info.Info = CreateMockUtf8("Hello");

    pool.m_ConstPoolInfo.push_back(info); // idx 0

    g_ConstPool = {pool};

    ConstPoolInfo returned = GetConstant(0);
    auto utf8 = returned.asUTF8Info();
    EXPECT_EQ(utf8->length, 5);
    EXPECT_EQ(utf8->bytes[0], 'H');
}

TEST(ConstantPoolTest, GetConstantUTF8_ThrowsForInvalidIndex) {
    ConstantPool pool;
    g_ConstPool = {pool};
    EXPECT_THROW(GetConstantUTF8(1), std::out_of_range);
}

TEST(ConstantPoolTest, Deserialize_SkipsLongAndDoubleEntries) {
    using namespace TitaniumDecompiler;

    // ConstantPoolCount = 5 (encoded as big endian)
    std::vector<uint8_t> data = {
        0x00, 0x05,                         // ConstantPoolCount = 5
        Tags::Utf8, 0x00, 0x03, 'A', 'B', 'C',  // UTF8 string "ABC"
        Tags::Long, 0x00, 0x00, 0x00, 0x00,     // Long - high bytes
                     0x00, 0x00, 0x00, 0x00,     // Long - low bytes (total 8 bytes)
        Tags::Utf8, 0x00, 0x03, 'X', 'Y', 'Z',  // UTF8 "XYZ"
        Tags::Utf8, 0x00, 0x03, '1', '2', '3'   // UTF8 "123"
    };

    MockBigEndianStreamReader reader(data);
    ConstantPool pool;
    ConstantPool::Deserialize(&reader, pool);

    // FIXED: Expect only 3 entries — the Long tag occupies 2 slots, but only 1 is pushed
    EXPECT_EQ(pool.m_ConstPoolInfo.size(), 3);

    // Optional: check tags to ensure correct decoding
    EXPECT_EQ(pool.m_ConstPoolInfo[0].Tag, Tags::Utf8);
    EXPECT_EQ(pool.m_ConstPoolInfo[1].Tag, Tags::Long);
    EXPECT_EQ(pool.m_ConstPoolInfo[2].Tag, Tags::Utf8);
}