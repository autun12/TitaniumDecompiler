#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include <string>
#include "../../../../Utils/FileReader.h"
#include "ConstPoolInfo.h"

namespace TitaniumDecompiler {


class ConstantPool {
public:
    ConstantPool();
    ConstantPool(std::vector<ConstPoolInfo> inf);
    ~ConstantPool() = default;

    static void Deserialize(BigEndianStreamReader* deserializer, ConstantPool& instance);

//     bool ReadConstPoolInfo(FileReader& reader);
//     ConstantPool newFromReader(FileReader& reader);

//     std::vector<ConstPoolInfo> GetConstPoolInfo();

//     std::string GetQualifiedName(uint16_t index);
//     std::string CheckClassName(std::string className);
//     std::string GetUtf8Content(UTF8Info& utf8);
public:
    uint16_t ConstantPoolCount = 0;
    std::vector<ConstPoolInfo> m_ConstPoolInfo;
};


std::string GetConstantUTF8(int idx);
static ConstPoolInfo GetConstant(int idx);

extern ConstantPool g_ConstPool;

}
