#pragma once

#include <memory>
#include <vector>

#include "TitaniumDecompiler/Disassembler/BasicBlock.h"

namespace TitaniumDecompiler {
class SSAGraph {
private:
    std::shared_ptr<BasicBlock> m_EntryBlock;
    std::shared_ptr<std::vector<BasicBlock>> m_Blocks;
};
}  // namespace TitaniumDecompiler