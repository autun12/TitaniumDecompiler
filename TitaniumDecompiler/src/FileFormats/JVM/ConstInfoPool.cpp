#include <TitaniumLogger/Logger/Log.h>

#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

#include "ConstPoolInfo.h"

namespace TitaniumDecompiler {

void ConstPoolInfo::Deserialize(BigEndianStreamReader *deserializer,
                                ConstPoolInfo &instance) {
    deserializer->ReadRawBigEndian<Tags>(instance.Tag);
    switch (instance.Tag) {
        case Tags::Utf8:
            instance.Info.emplace<UTF8Info>();
            break;
        case Tags::Int:
            instance.Info.emplace<IntegerInfo>();
            break;
        case Tags::Float:
            instance.Info.emplace<FloatInfo>();
            break;
        case Tags::Long:
            instance.Info.emplace<LongInfo>();
            break;
        case Tags::Double:
            instance.Info.emplace<DoubleInfo>();
            break;
        case Tags::Class:
            instance.Info.emplace<ClassInfo>();
            break;
        case Tags::String:
            instance.Info.emplace<StringInfo>();
            break;
        case Tags::Field:
            instance.Info.emplace<FieldRefInfo>();
            break;
        case Tags::Method:
            instance.Info.emplace<MethodRefInfo>();
            break;
        case Tags::InterfaceMethod:
            instance.Info.emplace<InterfaceMethodRef>();
            break;
        case Tags::NameAndType:
            instance.Info.emplace<NameAndTypeInfo>();
            break;
        case Tags::MethodHandle:
            instance.Info.emplace<MethodHandleInfo>();
            break;
        case Tags::MethodType:
            instance.Info.emplace<MethodTypeInfo>();
            break;
        case Tags::Dynamic:
            instance.Info.emplace<DynamicInfo>();
            break;
        case Tags::InvokeDynamic:
            instance.Info.emplace<InvokeDynamicInfo>();
            break;
        case Tags::Module:
            instance.Info.emplace<ModuleInfo>();
            break;
        case Tags::Package:
            instance.Info.emplace<PackageInfo>();
            break;
        default:
            TD_DECOMP_ERROR("Unrecognized Constant Pool tag: 0x%02X\n",
                            static_cast<uint8_t>(instance.Tag));
            instance.Info.emplace<std::monostate>();
            return;
    }

    std::visit(
        [deserializer](auto &&item) {
            using T = std::decay_t<decltype(item)>;
            if constexpr (!std::is_same_v<T, std::monostate>) {
                T::Deserialize(deserializer, item);
            }
        },
        instance.Info);
    // instance.Info = instance.CreateConstPoolItemFromTag(instance);
    //
    // if (instance.Info == nullptr) {
    //     TD_DECOMP_ERROR("ERROR\n");
    // }
    //
    // instance.Info->Deserialize(deserializer, *instance.Info);
}

// std::unique_ptr<ConstPool> ConstPoolInfo::CreateConstPoolItemFromTag(
//     ConstPoolInfo &instance) {
//     switch (instance.Tag) {
//         case TitaniumDecompiler::Tags::Utf8: {
//             return std::make_unique<UTF8Info>();
//         }
//         case TitaniumDecompiler::Tags::Int: {
//             return std::make_unique<IntegerInfo>();
//         }
//         case TitaniumDecompiler::Tags::Float: {
//             return std::make_unique<FloatInfo>();
//         }
//         case TitaniumDecompiler::Tags::Long: {
//             return std::make_unique<LongInfo>();
//         }
//         case TitaniumDecompiler::Tags::Double: {
//             return std::make_unique<DoubleInfo>();
//         }
//         case TitaniumDecompiler::Tags::Class: {
//             return std::make_unique<ClassInfo>();
//         }
//         case TitaniumDecompiler::Tags::String: {
//             return std::make_unique<StringInfo>();
//         }
//         case TitaniumDecompiler::Tags::Field: {
//             return std::make_unique<FieldRefInfo>();
//         }
//         case TitaniumDecompiler::Tags::Method: {
//             return std::make_unique<MethodRefInfo>();
//         }
//         case TitaniumDecompiler::Tags::InterfaceMethod: {
//             return std::make_unique<InterfaceMethodRef>();
//         }
//         case TitaniumDecompiler::Tags::NameAndType: {
//             return std::make_unique<NameAndTypeInfo>();
//         }
//         case TitaniumDecompiler::Tags::MethodHandle: {
//             return std::make_unique<MethodHandleInfo>();
//         }
//         case TitaniumDecompiler::Tags::MethodType: {
//             return std::make_unique<MethodTypeInfo>();
//         }
//         case TitaniumDecompiler::Tags::Dynamic: {
//             return std::make_unique<DynamicInfo>();
//         }
//         case TitaniumDecompiler::Tags::InvokeDynamic: {
//             return std::make_unique<InvokeDynamicInfo>();
//         }
//         case TitaniumDecompiler::Tags::Module: {
//             return std::make_unique<ModuleInfo>();
//         }
//         case TitaniumDecompiler::Tags::Package: {
//             return std::make_unique<PackageInfo>();
//         }
//         default: {
//             return nullptr;
//         }
//     }
// }

}  // namespace TitaniumDecompiler
