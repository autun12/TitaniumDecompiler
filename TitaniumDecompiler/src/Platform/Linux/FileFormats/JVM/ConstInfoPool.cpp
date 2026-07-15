#include <TitaniumLogger/Logger/Log.h>

#include <iostream>
#include <vector>

#include "ConstPoolInfo.h"

namespace TitaniumDecompiler {

void ConstPoolInfo::Deserialize(BigEndianStreamReader *deserializer, ConstPoolInfo &instance) {
    deserializer->ReadRawBigEndian<Tags>(instance.Tag);
    instance.Info = instance.CreateConstPoolItemFromTag(instance);

    if (instance.Info == nullptr) {
        TD_DECOMP_ERROR("ERROR\n");
    }

    instance.Info->Deserialize(deserializer, *instance.Info);
}

std::shared_ptr<ConstPool> ConstPoolInfo::CreateConstPoolItemFromTag(ConstPoolInfo &instance) {
    switch (instance.Tag) {
        case TitaniumDecompiler::Tags::Utf8: {
            return std::make_shared<UTF8Info>();
        }
        case TitaniumDecompiler::Tags::Int: {
            return std::make_shared<IntegerInfo>();
        }
        case TitaniumDecompiler::Tags::Float: {
            return std::make_shared<FloatInfo>();
        }
        case TitaniumDecompiler::Tags::Long: {
            return std::make_shared<LongInfo>();
        }
        case TitaniumDecompiler::Tags::Double: {
            return std::make_shared<DoubleInfo>();
        }
        case TitaniumDecompiler::Tags::Class: {
            return std::make_shared<ClassInfo>();
        }
        case TitaniumDecompiler::Tags::String: {
            return std::make_shared<StringInfo>();
        }
        case TitaniumDecompiler::Tags::Field: {
            return std::make_shared<FieldRefInfo>();
        }
        case TitaniumDecompiler::Tags::Method: {
            return std::make_shared<MethodRefInfo>();
        }
        case TitaniumDecompiler::Tags::InterfaceMethod: {
            return std::make_shared<InterfaceMethodRef>();
        }
        case TitaniumDecompiler::Tags::NameAndType: {
            return std::make_shared<NameAndTypeInfo>();
        }
        case TitaniumDecompiler::Tags::MethodHandle: {
            return std::make_shared<MethodHandleInfo>();
        }
        case TitaniumDecompiler::Tags::MethodType: {
            return std::make_shared<MethodTypeInfo>();
        }
        case TitaniumDecompiler::Tags::Dynamic: {
            return std::make_shared<DynamicInfo>();
        }
        case TitaniumDecompiler::Tags::InvokeDynamic: {
            return std::make_shared<InvokeDynamicInfo>();
        }
        case TitaniumDecompiler::Tags::Module: {
            return std::make_shared<ModuleInfo>();
        }
        case TitaniumDecompiler::Tags::Package: {
            return std::make_shared<PackageInfo>();
        }
        default: {
            return nullptr;
        }
    }
}

}  // namespace TitaniumDecompiler