#pragma once

#include <string_view>
// #include "Core/CFG/CFG.h"

namespace TitaniumDecompiler {
class ILifter {
public:
    virtual ~ILifter() = default;

    // virtual bool Lift(const CFG& cfg, std::span<const DecodedInsn>
    // instructions,
    //                   Lifted::Function& output) = 0;
    virtual std::string_view GetArchitectureName() const = 0;
};
}  // namespace TitaniumDecompiler
