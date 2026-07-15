#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ConstPoolInfo.h"
#include "Utils/FileReader.h"

namespace TitaniumDecompiler {

class ConstantPool {
public:
    ConstantPool();
    ConstantPool(std::vector<ConstPoolInfo> inf);
    ~ConstantPool() = default;

    static void Deserialize(BigEndianStreamReader* deserializer, ConstantPool& instance);

    //     std::string GetQualifiedName(uint16_t index);
    //     std::string CheckClassName(std::string className);
public:
    uint16_t ConstantPoolCount = 0;
    std::vector<ConstPoolInfo> m_ConstPoolInfo;
};

std::string GetConstantUTF8(int idx);
std::string GetConstantClass(int idx);
std::string GetNameAndType(int idx);

ConstPoolInfo GetConstant(int idx);

extern std::vector<ConstantPool> g_ConstPool;

}  // namespace TitaniumDecompiler
