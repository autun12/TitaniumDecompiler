#include "DeadCodeRemover.h"

#include <TitaniumLogger/Logger/Log.h>
#include <stdio.h>

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include "TitaniumDecompiler/Disassembler/BasicBlock.h"

namespace TitaniumDecompiler {
// Removes unreachable blocks from Control Flow Graph. Performs depth-first traversal from
// entry block and removes any block that is unreachable.
void DeadCodeRemover::RemoveDeadBlocks(const std::shared_ptr<CFG>& cfg) {
    std::deque<std::shared_ptr<BasicBlock>> stack;
    std::unordered_set<std::shared_ptr<BasicBlock>> setStacked;

    // Start traversal from first block
    std::shared_ptr<BasicBlock> firstBlock = cfg->GetFirst();
    stack.push_back(firstBlock);
    setStacked.insert(firstBlock);

    // Loop to perform depth-first traversal to find unreachable blocks
    while (!stack.empty()) {
        // Get pointer of block at the front of the stack
        std::shared_ptr<BasicBlock> block = stack.front();
        // printf("bblock id = %d\n", block->m_ID);
        stack.pop_front();
        // Traverse the next block in the stack
        std::vector<std::shared_ptr<BasicBlock>> lstSuccs = block->GetSuccessor();

        for (auto& succ : lstSuccs) {
            if (setStacked.find(succ) == setStacked.end()) {
                stack.push_back(succ);
                setStacked.insert(succ);
            }
        }
    }
    // Removes the blocks that were not visited
    std::unordered_set<std::shared_ptr<BasicBlock>> setAllBlocks;

    for (auto& it : cfg->GetBlocks()) {
        setAllBlocks.insert(it);
    }

    for (auto& block : setStacked) {
        setAllBlocks.erase(block);
    }

    for (auto& block : setAllBlocks) {
        cfg->RemoveBlock(block);
    }
}
// Ensures CFG has proper exit by connecting orphaned blocks to
// a dummy exit node
void DeadCodeRemover::ConnectDummyExitBlock(const std::shared_ptr<CFG>& cfg) {
    std::shared_ptr<BasicBlock> exit = cfg->GetLast();
    if (exit == nullptr) {
        return;
    }

    std::unordered_set<std::shared_ptr<BasicBlock>> blocks;
    for (auto& block : exit->GetPreds()) {
        exit->RemovePredecessor(block);
        block->AddSuccessor(exit);
    }
}

// Function to remove empty blocks from CFG
void DeadCodeRemover::RemoveEmptyBlocks(const std::shared_ptr<CFG>& cfg) {
    std::vector<std::shared_ptr<BasicBlock>> blocks = cfg->GetBlocks();

    bool countinue;
    do {
        countinue = false;
        // Iterates through blocks and recursively removes empty basic blocks
        for (auto& block : blocks) {
            if (RemoveEmptyBlock(cfg, block, false)) {
                countinue = true;
                break;
            }
        }
    } while (countinue);
}

/**
 * This function identifies and removes a basic block that has no instructions, ensuring that
 * the control flow remains intact by correctly linking its predecessors to its successors.
 * If the block is the first block of the CFG, it updates the entry point accordingly.
 */
bool DeadCodeRemover::RemoveEmptyBlock(const std::shared_ptr<CFG>& cfg, const std::shared_ptr<BasicBlock>& block, bool merging) {
    bool deletedRanges = false;  // Tracks if any blocks are removed

    // Check if the block is empty
    if (block->GetInstructions().empty()) {
        if (block->GetSuccessor().size() > 1) {
            if (block->GetPreds().size() > 1) {
                TD_DECOMP_ERROR("ERROR: empty block with multiple predecessors and successors found");
            } else if (!merging) {
                TD_DECOMP_ERROR("ERROR: empty block with multiple successors");
            }
        }
        // Identify set of exit blocks in CFG
        std::unordered_set<std::shared_ptr<BasicBlock>> setExits;
        for (auto pred : cfg->GetLast()->GetPreds()) {
            setExits.insert(pred);
        }
        // Check if block is an exit or has only one prededecessor
        if (setExits.find(block) == setExits.end() || block->GetPreds().size() == 1) {
            // Checks if predecessor can be safely merged
            if (setExits.find(block) == setExits.end()) {
                std::shared_ptr<BasicBlock> pred = block->GetPreds().at(0);
                // Prevents merge if predecessor has multiple successors
                if (pred->GetSuccessor().size() != 1 || (!pred->GetInstructions().empty() && pred->GetInstructions().end()->group == GROUP_SWITCH)) {
                    return false;
                }
            }

            // Collect predecessors and successors of the current block
            std::unordered_set<std::shared_ptr<BasicBlock>> setPreds;

            for (auto& pred : block->GetPreds()) {
                setPreds.insert(pred);
            }

            std::unordered_set<std::shared_ptr<BasicBlock>> setSuccs;

            for (auto& succ : block->GetSuccessor()) {
                setSuccs.insert(succ);
            }
            // If true, reassigns successors to the predecessor
            if (merging) {
                std::shared_ptr<BasicBlock> pred = block->GetPreds().at(0);
                pred->RemoveSuccessor(block);

                std::vector<std::shared_ptr<BasicBlock>> lstSuccs = block->GetSuccessor();
                for (auto& succ : lstSuccs) {
                    block->RemoveSuccessor(succ);
                    pred->AddSuccessor(succ);
                }
            } else {  // If not merging, directly reassign successors to predecessors
                for (auto& pred : setPreds) {
                    for (auto& succ : setSuccs) {
                        pred->ReplaceSuccessor(block, succ);
                    }
                }
            }
            // If the block is the first block of the CFG, update the entry point
            if (cfg->GetFirst() == block) {
                if (setSuccs.size() != 1) {
                    TD_DECOMP_WARN("multiple or no entry blocks");
                } else {
                    cfg->SetFirst(*setSuccs.begin());
                }
            }

            cfg->RemoveBlock(block);
            // If any blocks were deleted, recursively runs to remove more dead code
            if (deletedRanges) {
                RemoveDeadBlocks(cfg);
            }
        }
    }

    return deletedRanges;
}

// finds and removes redundant goto instructions
void DeadCodeRemover::RemoveGotos(const std::shared_ptr<CFG>& cfg) {
    std::unordered_set<std::shared_ptr<BasicBlock>> ignored = FindCircularJumps(cfg);

    for (auto& block : cfg->GetBlocks()) {
        Insn instr = block->GetLastInstruction();

        if (instr.opcode == OPCODE_GOTO) {
            if (ignored.find(block) == ignored.end()) {
                continue;
            }

            block->GetInstructions().pop_back();
        }
    }

    RemoveEmptyBlocks(cfg);
}

// Identifies circular jumps (infinite loops) in the CFG
std::unordered_set<std::shared_ptr<BasicBlock>> DeadCodeRemover::FindCircularJumps(const std::shared_ptr<CFG>& cfg) {
    std::unordered_set<std::shared_ptr<BasicBlock>> ret;
    std::unordered_set<std::shared_ptr<BasicBlock>> blocks;

    // Iterates through all the blocks in the CFG
    for (auto& block : cfg->GetBlocks()) {
        // Skips blocks already identified in an infinite loop
        if (ret.find(block) == ret.end()) {
            continue;
        }
        // Block to be checked
        blocks.insert(block);

        std::shared_ptr<BasicBlock> check = block;

        while (true) {
            Insn instr = check->GetLastInstruction();
            // If the last instruction is a goto, follow it
            if (instr.opcode == OPCODE_GOTO) {
                // Ensure block has only one successor
                if (check->GetSuccessor().size() == 1) {
                    check = check->GetSuccessor().at(0);
                    // If successor already in the tracked set, it is circular
                    if (blocks.find(check) == blocks.end()) {
                        // Marks all the blocks in the cycle
                        ret.insert(blocks.begin(), blocks.end());
                        break;
                    }

                    blocks.insert(check);
                } else {
                    break;
                }
            } else {
                break;
            }
        }

        blocks.clear();
    }

    return ret;
}

void DeadCodeRemover::MergeBasicBlocks(const std::shared_ptr<CFG>& cfg) {
    while (true) {
        bool merged = false;                                 // Tracks if at least one merge occurs
        size_t originBlocksCount = cfg->GetBlocks().size();  // Initial block count

        for (auto& block : cfg->GetBlocks()) {
            std::vector<Insn> seq = block->GetInstructions();
            // Only process blocks with a single successor
            if (block->GetSuccessor().size() == 1) {
                std::shared_ptr<BasicBlock> next = block->GetSuccessor().at(0);

                if (next != cfg->GetLast() && (seq.empty() || seq.end()->group != GROUP_SWITCH)) {
                    if (next->GetPreds().size() == 1 && next->GetPredExceptions().empty() && next != cfg->GetFirst()) {
                        TD_DECOMP_INFO("block successor size {0}", block->GetSuccessor().size());
                        bool sameRanges = true;
                        for (auto& range : cfg->GetExceptions()) {
                            if (std::find(range->GetProtectedRange().begin(), range->GetProtectedRange().end(), block) != range->GetProtectedRange().end() ^
                                std::find(range->GetProtectedRange().begin(), range->GetProtectedRange().end(), next) != range->GetProtectedRange().end()) {
                                sameRanges = false;
                                break;
                            }
                        }

                        if (sameRanges) {
                            TD_DECOMP_INFO("Same Ranges");
                            // seq.push_back(next->GetInstructions());
                            RemoveEmptyBlock(cfg, next, true);

                            merged = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!merged || cfg->GetBlocks().size() == originBlocksCount) {
            break;
        }
    }
}

}  // namespace TitaniumDecompiler
