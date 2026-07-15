#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>  
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/ConstantPool.h"
#include "TitaniumDecompiler/src/Platform/Linux/FileFormats/JVM/ConstPoolInfo.h"
#include "TitaniumDecompiler/src/Utils/StreamReader.h"

using namespace TitaniumDecompiler;



class MockStreamReader : public BigEndianStreamReader {
    public:
        explicit MockStreamReader(std::stringstream& s) : stream(s) {}
    
        bool IsStreamGood() const {
            return stream.good();
        }
    
        uint64_t GetStreamPosition() {
            return stream.tellg();
        }
    
        void SetStreamPosition(uint64_t pos) {
            stream.seekg(pos);
        }
    
        bool ReadData(char* destination, size_t size) {
            stream.read(destination, size);
            return static_cast<size_t>(stream.gcount()) == size;
        }
    
    private:
        std::stringstream& stream;
    };
    class ConstPoolInfoTest : public ::testing::Test {
        protected:
            std::stringstream buffer;
            std::unique_ptr<MockStreamReader> reader;
        
            void SetUp() override {
                buffer = std::stringstream(std::ios::in | std::ios::out | std::ios::binary);
                reader = std::make_unique<MockStreamReader>(buffer);
            }
        
            void WriteBigEndianU16(uint16_t value) {
                uint16_t be = htons(value);
                buffer.write(reinterpret_cast<const char*>(&be), sizeof(be));
            }
        
            void WriteBigEndianI32(int32_t value) {
                int32_t be = htonl(value);
                buffer.write(reinterpret_cast<const char*>(&be), sizeof(be));
            }
        
            void WriteUtf8Entry() {
                buffer.put(static_cast<char>(Tags::Utf8));
                WriteBigEndianU16(5);
                buffer.write("Hello", 5);
                buffer.seekg(0);
            }
        
            void WriteIntegerEntry() {
                buffer.put(static_cast<char>(Tags::Int));
                WriteBigEndianI32(42);
                buffer.seekg(0);
            }
        };
        
        TEST_F(ConstPoolInfoTest, DeserializeUtf8Info) {
            WriteUtf8Entry();
        
            ConstPoolInfo info;
            ConstPoolInfo::Deserialize(reader.get(), info);
        
            auto utf8 = dynamic_cast<UTF8Info*>(info.Info.get());
            ASSERT_NE(utf8, nullptr);
            EXPECT_EQ(utf8->length, 5);
            EXPECT_EQ(std::string(utf8->bytes.begin(), utf8->bytes.end()), "Hello");
        }
        
        TEST_F(ConstPoolInfoTest, DeserializeIntegerInfo) {
            WriteIntegerEntry();
        
            ConstPoolInfo info;
            ConstPoolInfo::Deserialize(reader.get(), info);
        
            auto intInfo = dynamic_cast<IntegerInfo*>(info.Info.get());
            ASSERT_NE(intInfo, nullptr);
            EXPECT_EQ(intInfo->bytes, 42);
        }
        
        TEST_F(ConstPoolInfoTest, DeserializeInvalidTag) {
            buffer.put(static_cast<char>(0xFE)); // Unknown/invalid tag
            buffer.seekg(0);
        
            ConstPoolInfo info;
        
            // Manually deserialize just the tag
            Tags tag;
            reader->ReadRawBigEndian<Tags>(tag);
            EXPECT_EQ(tag, static_cast<Tags>(0xFE));
        }