#include "ConstantPool.h"

#include <TitaniumLogger/Logger/Log.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <type_traits>
#include <variant>

#include "ConstPoolInfo.h"

namespace TitaniumDecompiler {
ConstantPool::ConstantPool() {}

void ConstantPool::Deserialize(BigEndianStreamReader *deserializer,
                               ConstantPool &instance) {
    deserializer->ReadRawBigEndian<uint16_t>(instance.ConstantPoolCount);

    instance.m_ConstPoolInfo.clear();
    instance.m_ConstPoolInfo.reserve(instance.ConstantPoolCount);
    instance.m_ConstPoolInfo.emplace_back();

    for (uint16_t i = 1; i < instance.ConstantPoolCount; i++) {
        ConstPoolInfo info;
        ConstPoolInfo::Deserialize(deserializer, info);

        instance.m_ConstPoolInfo.push_back(info);

        if (info.Tag == Tags::Long || info.Tag == Tags::Double) {
            ConstPoolInfo dummySlot{};
            dummySlot.Tag = Tags::Null;
            instance.m_ConstPoolInfo.push_back(dummySlot);

            i++;
        }
    }
}

std::string ConstantPool::GetConstantUTF8(uint16_t idx) const {
    if (idx == 0 || idx >= m_ConstPoolInfo.size()) {
        return "";
    }

    int constIdx = idx;
    if (constIdx < 0) {
        return "";
    }

    const auto &cpInfo = m_ConstPoolInfo[constIdx];
    if (const auto *utf8Info = cpInfo.GetAs<UTF8Info>()) {
        return std::string(utf8Info->bytes.begin(), utf8Info->bytes.end());
    }

    TD_DECOMP_WARN("Constant Pool index {0} is not a UTF8 entry.\n", idx);
    return "";
}

std::string ConstantPool::GetConstantClass(uint16_t idx) const {
    if (idx == 0 || idx >= m_ConstPoolInfo.size()) {
        return "";
    }

    const auto &cpInfo = m_ConstPoolInfo[idx];
    if (const auto *classInfo = cpInfo.GetAs<ClassInfo>()) {
        return GetConstantUTF8(classInfo->nameIndex);
    }

    TD_DECOMP_WARN("Constant Pool index {0} is not a Class entry.\n", idx);
    return "";
}

std::pair<std::string, std::string> ConstantPool::GetMethodRef(
    uint16_t idx) const {
    if (idx == 0 || idx >= m_ConstPoolInfo.size()) {
        return {};
    }

    const auto &cpInfo = m_ConstPoolInfo[idx];
    if (const auto *nameAndTypeInfo = cpInfo.GetAs<MethodRefInfo>()) {
        std::string name = GetConstantUTF8(nameAndTypeInfo->classIndex);
        std::string desc = GetConstantUTF8(nameAndTypeInfo->nameAndTypeIndex);
        return {name, desc};
    }

    TD_DECOMP_WARN("Constant Pool index %d is not a MethodRef entry.\n", idx);
    return {};
}

std::pair<std::string, std::string> ConstantPool::GetNameAndType(
    uint16_t idx) const {
    if (idx == 0 || idx >= m_ConstPoolInfo.size()) {
        return {};
    }

    const auto &cpInfo = m_ConstPoolInfo[idx];
    if (const auto *nameAndTypeInfo = cpInfo.GetAs<NameAndTypeInfo>()) {
        std::string name = GetConstantUTF8(nameAndTypeInfo->nameIndex);
        std::string desc = GetConstantUTF8(nameAndTypeInfo->descriptorIndex);
        return {name, desc};
    }

    TD_DECOMP_WARN("Constant Pool index %d is not a NameAndType entry.\n", idx);
    return {};
}

const ConstPoolInfo *ConstantPool::GetConstant(uint16_t idx) const {
    if (idx == 0 || idx >= m_ConstPoolInfo.size()) {
        return nullptr;
    }

    return &m_ConstPoolInfo[idx];
}

static void Write16(std::vector<uint8_t> &buf, uint16_t val) {
    buf.push_back(static_cast<uint8_t>(val >> 8));
    buf.push_back(static_cast<uint8_t>(val));
}

static void Write32(std::vector<uint8_t> &buf, uint32_t val) {
    buf.push_back(static_cast<uint8_t>(val >> 24));
    buf.push_back(static_cast<uint8_t>(val >> 16));
    buf.push_back(static_cast<uint8_t>(val >> 8));
    buf.push_back(static_cast<uint8_t>(val));
}

static void Write64(std::vector<uint8_t> &buf, uint64_t val) {
    Write32(buf, static_cast<uint32_t>(val >> 32));
    Write32(buf, static_cast<uint32_t>(val));
}

std::vector<uint8_t> ConstantPool::ToRawBytes() const {
    std::vector<uint8_t> bytes;

    // Write pool count
    Write16(bytes, static_cast<uint16_t>(m_ConstPoolInfo.size()));

    for (size_t i = 1; i < m_ConstPoolInfo.size(); ++i) {
        const auto &entry = m_ConstPoolInfo[i];

        // Skip dummy entries for Long/Double 2nd slots
        if (std::holds_alternative<std::monostate>(entry.Info)) {
            continue;
        }

        bytes.push_back(static_cast<uint8_t>(entry.Tag));

        std::visit(
            [&bytes](auto &&item) {
                using T = std::decay_t<decltype(item)>;

                if constexpr (std::is_same_v<T, UTF8Info>) {
                    Write16(bytes, item.length);
                    bytes.insert(bytes.end(), item.bytes.begin(),
                                 item.bytes.end());
                } else if constexpr (std::is_same_v<T, IntegerInfo> ||
                                     std::is_same_v<T, FloatInfo>) {
                    Write32(bytes, item.bytes);
                } else if constexpr (std::is_same_v<T, LongInfo> ||
                                     std::is_same_v<T, DoubleInfo>) {
                    Write64(bytes, item.bytes);
                } else if constexpr (std::is_same_v<T, ClassInfo> ||
                                     std::is_same_v<T, StringInfo> ||
                                     std::is_same_v<T, MethodTypeInfo> ||
                                     std::is_same_v<T, ModuleInfo> ||
                                     std::is_same_v<T, PackageInfo>) {
                    // Write16(
                    //     bytes,
                    //     item.nameIndex);  // (Or stringIndex /
                    //     descriptorIndex)
                } else if constexpr (std::is_same_v<T, FieldRefInfo> ||
                                     std::is_same_v<T, MethodRefInfo> ||
                                     std::is_same_v<T, InterfaceMethodRef> ||
                                     std::is_same_v<T, NameAndTypeInfo> ||
                                     std::is_same_v<T, DynamicInfo> ||
                                     std::is_same_v<T, InvokeDynamicInfo>) {
                    // Write16(bytes, item.classIndex);
                    // Write16(bytes, item.nameAndTypeIndex);
                } else if constexpr (std::is_same_v<T, MethodHandleInfo>) {
                    bytes.push_back(item.referenceKind);
                    Write16(bytes, item.referenceIndex);
                }
            },
            entry.Info);
    }

    return bytes;
}

}  // namespace TitaniumDecompiler
