#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "ConstPoolInfo.h"

namespace TitaniumDecompiler {

class ConstantPool {
public:
    ConstantPool();
    ConstantPool(std::vector<ConstPoolInfo> inf);
    ~ConstantPool() = default;

    static void Deserialize(BigEndianStreamReader* deserializer,
                            ConstantPool& instance);
    [[nodiscard]] std::string GetConstantUTF8(uint16_t idx) const;
    [[nodiscard]] std::string GetConstantClass(uint16_t idx) const;
    [[nodiscard]] std::pair<std::string, std::string> GetNameAndType(
        uint16_t idx) const;
    [[nodiscard]] std::pair<std::string, std::string> GetMethodRef(
        uint16_t idx) const;
    [[nodiscard]] const ConstPoolInfo* GetConstant(uint16_t idx) const;
    [[nodiscard]] size_t Size() const { return m_ConstPoolInfo.size(); };
    [[nodiscard]] std::vector<uint8_t> ToRawBytes() const;

public:
    uint16_t ConstantPoolCount = 0;
    std::vector<ConstPoolInfo> m_ConstPoolInfo;
};

std::string GetConstantClass(int idx);

}  // namespace TitaniumDecompiler
