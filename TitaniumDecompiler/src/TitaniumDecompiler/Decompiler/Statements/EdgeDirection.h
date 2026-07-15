#pragma once

#include <stdint.h>

namespace TitaniumDecompiler {
enum class EdgeDirection : uint8_t {
    BACKWARD = 0,
    FORWARD = 1

};

enum class LastBasicType : uint8_t { IF = 0, SWITCH = 1, GENERAL = 2 };
}  // namespace TitaniumDecompiler
