#pragma once

#include <memory>
#include <unordered_set>

#include "TitaniumDecompiler/Disassembler/CFG.h"

namespace TitaniumDecompiler {
struct DeadCodeRemover {
public:
    void RemoveDeadBlocks(const std::shared_ptr<CFG>& cfg);
    void ConnectDummyExitBlock(const std::shared_ptr<CFG>& cfg);
    void RemoveEmptyBlocks(const std::shared_ptr<CFG>& cfg);
    void RemoveGotos(const std::shared_ptr<CFG>& cfg);
    void MergeBasicBlocks(const std::shared_ptr<CFG>& cfg);

private:
    std::unordered_set<std::shared_ptr<BasicBlock>> FindCircularJumps(const std::shared_ptr<CFG>& cfg);
    bool RemoveEmptyBlock(const std::shared_ptr<CFG>& cfg, const std::shared_ptr<BasicBlock>& block, bool merging);
};
}  // namespace TitaniumDecompiler
