#pragma once

#include <vector>

#include "Core/Types.h"

namespace TitaniumDecompiler {
struct CFG {
    std::vector<IBasicBlock> blocks;
    BlockId entryBlock = InvalidBlockId;
};
}  // namespace TitaniumDecompiler
