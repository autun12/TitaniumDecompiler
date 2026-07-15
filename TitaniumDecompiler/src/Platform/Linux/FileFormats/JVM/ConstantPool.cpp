#include "ConstantPool.h"
// #include "ConstPoolInfo.h"
#include <TitaniumLogger/Logger/Log.h>

#include <iostream>

#include "ConstPoolInfo.h"

namespace TitaniumDecompiler {
ConstantPool::ConstantPool() {}

void ConstantPool::Deserialize(BigEndianStreamReader *deserializer, ConstantPool &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.ConstantPoolCount);

    for (uint16_t i = 1; i < instance.ConstantPoolCount; i++) {
        ConstPoolInfo info;
        ConstPoolInfo::Deserialize(deserializer, info);

        if (info.Tag == Tags::Long || info.Tag == Tags::Double) {
            i++;
        }

        instance.m_ConstPoolInfo.push_back(info);
    }
}

std::string GetConstantUTF8(int idx) {
    int constantIdx = idx - 1;
    std::stringstream s;
    s.str("");
    s.clear();
    for (auto &constPool : g_ConstPool) {
        auto cpInfo = constPool.m_ConstPoolInfo.at(constantIdx);
        UTF8Info *utf8Info = cpInfo.asUTF8Info();

        for (int i = 0; i < utf8Info->length; i++) {
            s << utf8Info->bytes[i];
        }
    }
    return s.str();
}

std::string GetConstantClass(int idx) {
    TD_DECOMP_INFO("const idx = %x\n", idx);
    std::string className = "";
    for (auto &constPool : g_ConstPool) {
        auto cpInfo = constPool.m_ConstPoolInfo.at(idx - 1);
        ClassInfo *classInfo = cpInfo.asClassInfo();
        TD_DECOMP_INFO("name index = %d\n", classInfo->nameIndex);
        className = GetConstantUTF8(classInfo->nameIndex);
    }
    return className;
}

std::string GetNameAndType(int idx) {
    std::string output = "";
    for (auto &constPool : g_ConstPool) {
        auto cpInfo = constPool.m_ConstPoolInfo.at(idx - 1);
        NameAndTypeInfo *nameAndTypeInfo = cpInfo.asNameAndType();
        std::string name = GetConstantUTF8(nameAndTypeInfo->nameIndex);
        std::string desc = GetConstantUTF8(nameAndTypeInfo->descriptorIndex);
        output.append(name);
        output.append(":");
        output.append(desc);
    }
    return output;
}

ConstPoolInfo GetConstant(int idx) {
    ConstPoolInfo pool;
    for (auto &constPool : g_ConstPool) {
        pool = constPool.m_ConstPoolInfo.at(idx);
    }

    return pool;
}

// std::string ConstantPool::GetQualifiedName(uint16_t index) {
//     std::string qualifiedName = "";
//     ConstPoolInfo* data = &m_ConstPoolInfo[index];

//     if(data->tag == Tags::Utf8) {
//         for(auto bytes : m_UTF8Info->bytes) {
//             qualifiedName += bytes;
//         }
//         std::cout << qualifiedName << std::endl;
//     }

//     return qualifiedName;
// }

// std::string ConstantPool::CheckClassName(std::string className) {
//     std::replace(className.begin(), className.end(), '/', '.');
//     size_t startPos = className.find("java.lang.");
//     if(startPos != std::string::npos) {
//         className.replace(startPos, 10, std::string());
//     }
//     return className;
// }

}  // namespace TitaniumDecompiler
