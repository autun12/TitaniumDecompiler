#include "CFG.h"

#include <TitaniumLogger/Logger/Log.h>

#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>

#include "BasicBlock.h"
#include "Platform/Linux/FileFormats/JVM/CodeConstants.h"
#include "TitaniumDecompiler/Decompiler/AST.h"
#include "TitaniumDecompiler/Decompiler/Codegen.h"
// #include "TitaniumDecompiler/Decompiler/Function.h"

namespace TitaniumDecompiler {

const Insn* CFG::FindInstructionByAddr(const InstrMap& instructions, uint64_t targetAddr) {
    for (const auto& [addr, instr] : instructions) {
        if (addr == targetAddr) {
            return &instr;
        }
    }
    return nullptr;
}

std::set<size_t> CFG::FindBoundaries(const InstrMap& instructions) {
    std::set<size_t> boundaries;
    boundaries.insert(0);

    int len = instructions.size();

    size_t i = 0;

    for (auto it = instructions.begin(); it != instructions.end(); ++it, ++i) {
        const Insn& insn = it->second;

        switch (insn.group) {
            case GROUP_JUMP: {
                uint64_t jumpTargetAddr = insn.Op1.addr;
                const Insn* targetInstr = FindInstructionByAddr(instructions, jumpTargetAddr);

                // If target found, add jump target as a boundary
                if (targetInstr) {
                    boundaries.insert(jumpTargetAddr);
                } else {
                    TD_DECOMP_WARN("Jump target at addr {0} not found in instruction map.", jumpTargetAddr);
                }

                // Set boundary at the next instruction (end of current basic block)
                if (std::next(it) != instructions.end()) {
                    boundaries.insert(std::next(it)->first);
                }
                break;
            }
            case GROUP_RETURN: {
                // Set boundary at the next instruction if there is one
                if (std::next(it) != instructions.end()) {
                    boundaries.insert(std::next(it)->first);
                }
                break;
            }
            default:
                break;
        }
    }
    return boundaries;
}

std::map<uint32_t, std::shared_ptr<BasicBlock>> CFG::CreateBasicBlocks(const InstrMap& instructions) {
    std::map<uint32_t, std::shared_ptr<BasicBlock>> mapInstrBlocks;
    std::vector<std::shared_ptr<BasicBlock>> basicBlocks;
    std::map<uint32_t, SSAVariable> localVarVersions;  // Track local variable versions
    std::vector<SSAVariable> currentStack;             // Conceptual operand stack

    // Step 1: Identify target addresses of all jumps and add them as boundaries
    std::set<size_t> boundaries = FindBoundaries(instructions);

    // Step 2: Create blocks using boundaries
    std::shared_ptr<BasicBlock> currentBlock = std::make_shared<BasicBlock>();
    int blockID = 0;  // Block ID counter
    std::shared_ptr<SSACFG> ssaCFG = std::make_shared<SSACFG>();
    std::map<int, std::shared_ptr<SSABasicBlock>> blockIdToSSABlock;

    // Iterate over the instructions (the multimap is ordered by offset)
    for (auto& [offset, insn] : instructions) {
        // If the offset is a boundary and the current block has instructions, finalize the block
        if (boundaries.find(offset) != boundaries.end() && !currentBlock->GetInstructions().empty()) {
            currentBlock->m_ID = blockID++;  // Assign ID and increment
            basicBlocks.push_back(currentBlock);
            m_Blocks[currentBlock->m_ID] = currentBlock;
            currentBlock = std::make_shared<BasicBlock>();
        }

        auto [ssaInstrs, updatedStack, updatedLocalVars] = TranslateBytecodeInstruction(insn, currentStack, localVarVersions);
        std::shared_ptr<SSABasicBlock> ssaBlock;
        if (blockIdToSSABlock.find(currentBlock->m_ID) == blockIdToSSABlock.end()) {
            ssaBlock = std::make_shared<SSABasicBlock>(currentBlock->m_ID);
            blockIdToSSABlock[currentBlock->m_ID] = ssaBlock;
            ssaCFG->AddBlock(ssaBlock);
        } else {
            ssaBlock = blockIdToSSABlock[currentBlock->m_ID];
        }

        ssaBlock->AddInstructions(ssaInstrs);
        currentStack = updatedStack;
        localVarVersions = updatedLocalVars;
        // Add the current instruction to the current block
        currentBlock->AddInstruction(insn);

        // End the block if it’s an unconditional jump
        if (IsUnconditionalJump(insn)) {
            currentBlock->m_ID = blockID++;
            basicBlocks.push_back(currentBlock);
            m_Blocks[currentBlock->m_ID] = currentBlock;
            currentBlock = std::make_shared<BasicBlock>();
        } else if (IsConditionalBranch(insn)) {
            size_t targetOff = GetJumpTarget(insn);
            currentBlock->m_ID = blockID++;
            basicBlocks.push_back(currentBlock);
            m_Blocks[currentBlock->m_ID] = currentBlock;
            currentBlock = std::make_shared<BasicBlock>();
        }

        mapInstrBlocks.insert({offset, currentBlock});
    }

    // Add the final block if it contains any instructions
    if (!currentBlock->GetInstructions().empty()) {
        currentBlock->m_ID = blockID++;
        basicBlocks.push_back(currentBlock);
        m_Blocks[currentBlock->m_ID] = currentBlock;
    }

    m_First = basicBlocks.at(0);
    m_Last = std::make_shared<BasicBlock>();

    mapInstrBlocks.insert({instructions.size(), m_Last});
    for (auto& block : basicBlocks) {
        if (blockIdToSSABlock.find(block->m_ID) != blockIdToSSABlock.end()) {
            block->ssaBlock = blockIdToSSABlock[block->m_ID];
        }
    }
    m_SsaCfg = std::move(ssaCFG);

    return mapInstrBlocks;
}

// std::vector<std::shared_ptr<BasicBlock>> CFG::BuildBlocks(const InstrMap& instructions) {
//     std::map<uint32_t, std::shared_ptr<BasicBlock>> mapInstrBlocks;
//     std::vector<std::shared_ptr<BasicBlock>> basicBlocks;

//     // Step 1: Identify target addresses of all jumps and add them as boundaries
//     std::set<size_t> boundaries = FindBoundaries(instructions);

//     // Step 2: Create blocks using boundaries
//     std::shared_ptr<BasicBlock> currentBlock = std::make_shared<BasicBlock>();
//     int blockID = 0;  // Block ID counter

//     // Iterate over the instructions (the multimap is ordered by offset)
//     for (auto& [offset, insn] : instructions) {
//         // If the offset is a boundary and the current block has instructions, finalize the block
//         if (boundaries.find(offset) != boundaries.end() && !currentBlock->GetInstructions().empty()) {
//             currentBlock->m_ID = blockID++;  // Assign ID and increment
//             basicBlocks.push_back(currentBlock);
//             m_Blocks[currentBlock->m_ID] = currentBlock;
//             currentBlock = std::make_shared<BasicBlock>();
//         }

//         // Add the current instruction to the current block
//         currentBlock->AddInstruction(insn);

//         // End the block if it’s an unconditional jump
//         if (IsUnconditionalJump(insn)) {
//             currentBlock->m_ID = blockID++;
//             basicBlocks.push_back(currentBlock);
//             m_Blocks[currentBlock->m_ID] = currentBlock;
//             currentBlock = std::make_shared<BasicBlock>();
//         } else if (IsConditionalBranch(insn)) {
//             size_t targetOff = GetJumpTarget(insn);
//             currentBlock->m_ID = blockID++;
//             basicBlocks.push_back(currentBlock);
//             m_Blocks[currentBlock->m_ID] = currentBlock;
//             currentBlock = std::make_shared<BasicBlock>();
//         }
//     }

//     // Add the final block if it contains any instructions
//     if (!currentBlock->GetInstructions().empty()) {
//         currentBlock->m_ID = blockID++;
//         basicBlocks.push_back(currentBlock);
//         m_Blocks[currentBlock->m_ID] = currentBlock;
//     }

//     return basicBlocks;
// }

size_t CFG::GetJumpTarget(const Insn& insn) {
    size_t targetAddr = insn.Op1.addr;

    if (targetAddr == 0) {
        TD_DECOMP_ERROR("Jump target address is invalid");
    }

    return targetAddr;
}

void CFG::PrintBasicBlocks(const std::vector<std::shared_ptr<BasicBlock>>& basicBlocks) {
    TD_DECOMP_INFO("CFG Basic Blocks and Connections:");
    for (const auto& block : basicBlocks) {
        TD_DECOMP_INFO("Block ID: {0}", block->m_ID);
        TD_DECOMP_INFO("Instructions in Block:");
        for (const auto& insn : block->GetInstructions()) {
            TD_DECOMP_INFO(" - Opcode: {0}, Name: {1}", (int)insn.opcode, insn.opcodeName);
        }

        TD_DECOMP_INFO("Successors:");
        for (const auto& succ : block->GetSuccessor()) {
            TD_DECOMP_INFO(" - Block ID: {0}", succ->m_ID);
        }

        TD_DECOMP_INFO("Predecessors:");
        for (const auto& pred : block->GetPreds()) {
            TD_DECOMP_INFO(" - Block ID: {0}", pred->m_ID);
        }
        TD_DECOMP_INFO("\n");
    }
}

void CFG::PrintBlock(const std::shared_ptr<BasicBlock>& block) {
    TD_DECOMP_INFO("Block ID: {0}", block->m_ID);
    TD_DECOMP_INFO("Instructions in Block:");
    for (const auto& insn : block->GetInstructions()) {
        TD_DECOMP_INFO(" - Opcode: {0}, Name: {1}", (int)insn.opcode, insn.opcodeName);
    }

    TD_DECOMP_INFO("Successors:");
    for (const auto& succ : block->GetSuccessor()) {
        TD_DECOMP_INFO(" - Block ID: {0}", succ->m_ID);
    }

    TD_DECOMP_INFO("Predecessors:");
    for (const auto& pred : block->GetPreds()) {
        TD_DECOMP_INFO(" - Block ID: {0}", pred->m_ID);
    }
    TD_DECOMP_INFO("\n");
}

void CFG::ConnectBasicBlocks(const std::map<uint32_t, std::shared_ptr<BasicBlock>>& mapInstrBlocks) {
    for (auto& it : m_Blocks) {
        std::shared_ptr<BasicBlock> block = it.second;
        Insn instr = block->GetLastInstruction();

        switch (instr.group) {
            case GROUP_JUMP: {
                size_t dest = instr.Op1.addr;
                block->AddSuccessor(mapInstrBlocks.at(dest));

                if (instr.CanFallThrough()) {
                    if (it.first >= m_Blocks.size() - 1) {
                        continue;
                    }
                    block->AddSuccessor(m_Blocks.at(it.first + 1));
                }

                break;
            }
            case GROUP_RETURN: {
                if (instr.opcode != OPCODE_RET) {
                    m_Last->AddPredecessor(block);
                }
                break;
            }
            case GROUP_GENERAL:
            case GROUP_INVOCATION:
            case GROUP_FIELDACCESS: {
                if (it.first >= m_Blocks.size() - 1) {
                    continue;
                }
                std::shared_ptr<BasicBlock> nextBlock = m_Blocks.at(it.first + 1);
                block->AddSuccessor(nextBlock);
                break;
            }
            default: {
                TD_DECOMP_ERROR("{0}", instr.group);
                TD_DECOMP_ERROR("Unknown instruction group");
            }
        }
    }
}

void CFG::BuildGraph() {
    for (auto& [index, block] : m_Blocks) {
        Insn lastInsn = block->GetLastInstruction();

        switch (lastInsn.group) {
            case GROUP_JUMP: {
                size_t dest = lastInsn.Op1.addr;
                block->AddSuccessor(m_Blocks.at(dest));
                break;
            }
        }
        // If it’s an unconditional jump, connect to the jump target
        // if (IsUnconditionalJump(lastInsn)) {
        // size_t targetIndex = lastInsn.Op1.addr;  // Target of the jump (now stored in Op1.addr)
        // printf("target index %d\n", targetIndex);
        // printf("targetIndex block %d\n", m_Blocks.end()->first);
        // if (m_Blocks.find(targetIndex) != m_Blocks.end()) {
        // auto targetBlock = m_Blocks[targetIndex];
        // block->AddSuccessor(targetBlock);
        // printf("target block = %d\n", targetBlock->m_ID);
        // }
        // }
        // For branch instructions, connect both the branch target and the next sequential block
        // else if (IsBranchInstruction(lastInsn)) {
        // size_t targetIndex = lastInsn.Op1.addr;
        // if (m_Blocks.find(targetIndex) != m_Blocks.end()) {
        // auto targetBlock = m_Blocks[targetIndex];
        // block->AddSuccessor(targetBlock);
        // }
        // if (m_Blocks.find(index + 1) != m_Blocks.end()) {  // Next sequential block
        // auto nextBlock = m_Blocks[index + 1];
        // block->AddSuccessor(nextBlock);
        // }
        // }
        // For all other instructions, connect to the next sequential block if not a return
        if (!IsReturnInstruction(lastInsn)) {
            if (m_Blocks.find(index + 1) != m_Blocks.end()) {
                auto nextBlock = m_Blocks[index + 1];
                block->AddSuccessor(nextBlock);
            }
        }
    }

    if (!m_Blocks.empty()) {
        m_First = m_Blocks.begin()->second;
        m_Last = m_Blocks.rbegin()->second;
    }
}

void CFG::RemoveBlock(std::shared_ptr<BasicBlock> block) {
    while (block->GetSuccessor().size() > 0) {
        block->RemoveSuccessor(block->GetSuccessor().at(0));
    }

    while (block->GetSuccessorExceptions().size() > 0) {
        block->RemoveSuccessorException(block->GetSuccessorExceptions().at(0));
    }

    while (block->GetPreds().size() > 0) {
        block->GetPreds().at(0)->RemoveSuccessor(block);
    }

    while (block->GetPredExceptions().size() > 0) {
        block->RemovePredecessorException(block->GetPredExceptions().at(0));
    }

    m_Last->RemovePredecessor(block);

    auto it = m_Blocks.find(block->m_ID);

    if (it != m_Blocks.end()) {
        m_Blocks.erase(it);
    }

    // for(int i = exce)
}

bool CFG::IsBranchInstruction(const Insn& insn) { return insn.group == GROUP_JUMP; }

bool CFG::IsReturnInstruction(const Insn& insn) { return insn.group == GROUP_RETURN; }

bool CFG::IsSwitchInstruction(const Insn& insn) { return (insn.opcode == OPCODE_TABLESWITCH || insn.opcode == OPCODE_LOOKUPSWITCH); }

bool CFG::IsConditionalBranch(const Insn& insn) {
    return (insn.opcode == OPCODE_IFEQ || insn.opcode == OPCODE_IFNE || insn.opcode == OPCODE_IFLT || insn.opcode == OPCODE_IFGE ||
            insn.opcode == OPCODE_IFGT || insn.opcode == OPCODE_IFLE || insn.opcode == OPCODE_IF_ICMPEQ || insn.opcode == OPCODE_IF_ICMPNE ||
            insn.opcode == OPCODE_IF_ICMPLT || insn.opcode == OPCODE_IF_ICMPGE || insn.opcode == OPCODE_IF_ICMPGT || insn.opcode == OPCODE_IF_ICMPLE ||
            insn.opcode == OPCODE_IF_ACMPEQ || insn.opcode == OPCODE_IF_ACMPNE || insn.opcode == OPCODE_IFNULL || insn.opcode == OPCODE_IFNONNULL);
}

bool CFG::IsUnconditionalJump(const Insn& insn) { return (insn.opcode == OPCODE_GOTO || insn.opcode == OPCODE_GOTO_W); }

bool CFG::IsInvokeCall(const Insn& insn) {
    return (insn.opcode == OPCODE_INVOKEVIRTUAL || insn.opcode == OPCODE_INVOKEINTERFACE || insn.opcode == OPCODE_INVOKESPECIAL);
}

std::shared_ptr<BasicBlock> CFG::GetBlockByInsn(const Insn& instruction) {
    // Iterate over all blocks in the CFG
    for (auto& blockEntry : m_Blocks) {
        // Get the list of instructions in the block
        auto& block = blockEntry.second;
        auto instructions = block->GetInstructions();
        // Iterate over instructions in the current block
        for (const auto& insn : instructions) {
            // Check if the current instruction matches the given instruction
            if (&insn == &instruction) {
                
                // Return the block if the instruction matches
                return block;
            }
        }
    }

    // Return nullptr if the instruction was not found in any block
    return nullptr;
}

std::tuple<std::vector<SSAInstruction>, std::vector<SSAVariable>, std::map<uint32_t, SSAVariable>> CFG::TranslateBytecodeInstruction(
    const Insn& insn, std::vector<SSAVariable> stack, std::map<uint32_t, SSAVariable> localVarVersions) {
    std::vector<SSAInstruction> ssaInstructions;
    std::vector<SSAVariable> updatedStack = stack;                        // Start with the current stack
    std::map<uint32_t, SSAVariable> updatedLocalVars = localVarVersions;  // Start with current local variable versions

    switch (insn.opcode) {
        case OPCODE_BIPUSH: {
            int32_t byteValue = static_cast<int32_t>(insn.Op1.value);  // Get the byte value
            SSAVariable result(Type::BasicType(Type::Int, "int"),      // BIPUSH pushes an int
                               SSAVariable::Origin::CONSTANT,
                               "bipush_const_" + std::to_string(byteValue),       // Unique name
                               -1);                                               // Original number not relevant for constants here.
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});  // Use LOAD_CONST
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ALOAD: {
            uint32_t index = insn.Op1.value;
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                // Need to get the type of the object.
                localVariable = SSAVariable(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }
            SSAVariable result(localVariable.type, SSAVariable::Origin::OPERATION, "aload_result");
            ssaInstructions.push_back({SSAInstruction::LOAD_LOCAL, {localVariable}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ALOAD_0: {
            uint32_t index = 0;
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                // Need to get the type of the object.
                localVariable = SSAVariable(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }
            SSAVariable result(localVariable.type, SSAVariable::Origin::OPERATION, "aload_result");
            ssaInstructions.push_back({SSAInstruction::LOAD_LOCAL, {localVariable}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ALOAD_1: {
            uint32_t index = 1;
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                // Need to get the type of the object.
                localVariable = SSAVariable(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }
            SSAVariable result(localVariable.type, SSAVariable::Origin::OPERATION, "aload_result");
            ssaInstructions.push_back({SSAInstruction::LOAD_LOCAL, {localVariable}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ALOAD_2: {
            uint32_t index = 2;
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                // Need to get the type of the object.
                localVariable = SSAVariable(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }
            SSAVariable result(localVariable.type, SSAVariable::Origin::OPERATION, "aload_result");
            ssaInstructions.push_back({SSAInstruction::LOAD_LOCAL, {localVariable}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ALOAD_3: {
            uint32_t index = 3;
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                // Need to get the type of the object.
                localVariable = SSAVariable(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }
            SSAVariable result(localVariable.type, SSAVariable::Origin::OPERATION, "aload_result");
            ssaInstructions.push_back({SSAInstruction::LOAD_LOCAL, {localVariable}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_IADD: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iadd_result");  // Unique name
            SSAInstruction addInstr(SSAInstruction::ADD, {operand1, operand2}, result);
            ssaInstructions.push_back(addInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_M1: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_m1", -1);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_0: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_0", 0);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_1: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_1", 1);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_2: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_2", 2);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_3: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_3", 3);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_4: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_4", 4);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ICONST_5: {
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "iconst_5", 5);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FCONST_0: {
            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::CONSTANT, "fconst_0", 0.0f);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FCONST_1: {
            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::CONSTANT, "fconst_1", 1.0f);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FCONST_2: {
            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::CONSTANT, "fconst_2", 2.0f);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_DCONST_0: {
            SSAVariable result(Type::BasicType(Type::Double, "double"), SSAVariable::Origin::CONSTANT, "dconst_0", 0.0);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_DCONST_1: {
            SSAVariable result(Type::BasicType(Type::Double, "double"), SSAVariable::Origin::CONSTANT, "dconst_1", 1.0);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LCONST_0: {
            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::CONSTANT, "lconst_0", 0L);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LCONST_1: {
            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::CONSTANT, "lconst_1", 1L);
            ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ASTORE_0: {
            uint32_t index = 0;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ASTORE_1: {
            uint32_t index = 1;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ASTORE_2: {
            uint32_t index = 2;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ASTORE_3: {
            uint32_t index = 3;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ISTORE_0: {
            uint32_t index = 0;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ISTORE_1: {
            uint32_t index = 1;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ISTORE_2: {
            uint32_t index = 2;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_ISTORE_3: {
            uint32_t index = 3;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_LSTORE_0: {
            uint32_t index = 0;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_LSTORE_1: {
            uint32_t index = 1;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_LSTORE_2: {
            uint32_t index = 2;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_LSTORE_3: {
            uint32_t index = 3;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_FSTORE_0: {
            uint32_t index = 0;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_FSTORE_1: {
            uint32_t index = 1;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_FSTORE_2: {
            uint32_t index = 2;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_FSTORE_3: {
            uint32_t index = 3;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;
            break;
        }
        case OPCODE_INVOKEVIRTUAL: {
            MethodRefStack methStack = GetMethodRefFromIndex(m_ClassFile, insn.Op1.value);
            std::vector<SSAVariable> args;
            for(auto& descArgs : methStack.desc.args) {
                args.push_back(updatedStack.back());
                updatedStack.pop_back();
            }
            reverse(args.begin(), args.end());  // correct the order.
            SSAVariable result(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::OPERATION,
                               "invokevirtual_" + methStack.m_Class + "." + methStack.m_Name);  //  Determine the actual return type.
            ssaInstructions.push_back({SSAInstruction::CALL, args, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_INVOKESPECIAL: {
            // Placeholder for invoke special logic
            // std::string methodName = insn.Op2.string_operand;
            // int argCount = insn.Op2.int_operand;
            MethodRefStack methStack = GetMethodRefFromIndex(m_ClassFile, insn.Op1.value);
            std::vector<SSAVariable> args;
            for(auto& descArgs : methStack.desc.args) {
                args.push_back(updatedStack.back());
                updatedStack.pop_back();
            }
            //   reverse(args.begin(), args.end()); //correct the order.
            SSAVariable result(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::OPERATION,
                               "invokespecial_" + methStack.m_Class + "." + methStack.m_Name);  //  Determine the actual return type.
            ssaInstructions.push_back({SSAInstruction::CALL, args, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_INVOKESTATIC: {
            MethodRefStack methStack = GetMethodRefFromIndex(m_ClassFile, insn.Op1.value);
            std::vector<SSAVariable> args;
            for(auto& descArgs : methStack.desc.args) {
                args.push_back(updatedStack.back());
                updatedStack.pop_back();
            }
            // Placeholder for invoke special logic
            //  std::string methodName = insn.Op2.string_operand;
            // int argCount = insn.Op2.int_operand;
            // Pop arguments from the stack.
            // for (int i = 0; i < argCount; ++i) {
            //     args.push_back(updatedStack.back());
            //     updatedStack.pop_back();
            // }
            reverse(args.begin(), args.end());  // correct the order.
            SSAVariable result(Type::ClassNameType("java/lang/Object"), SSAVariable::Origin::OPERATION,
                               "invokestatic_" + methStack.m_Class + "." + methStack.m_Name);  //  Determine the actual return type.
            ssaInstructions.push_back({SSAInstruction::CALL, args, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ILOAD: {
            uint32_t index = insn.Op1.addr;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_I2S: {
            SSAVariable valueToConvert = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Int, "short"), SSAVariable::Origin::OPERATION, "i2s_result");
            SSAInstruction castInstr(SSAInstruction::CAST, {valueToConvert}, result);
            ssaInstructions.push_back(castInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ILOAD_0: {
            uint32_t index = 0;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ILOAD_1: {
            uint32_t index = 1;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ILOAD_2: {
            uint32_t index = 2;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ILOAD_3: {
            uint32_t index = 3;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FLOAD_0: {
            uint32_t index = 0;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::OPERATION, "fload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FLOAD_1: {
            uint32_t index = 1;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::OPERATION, "fload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FLOAD_2: {
            uint32_t index = 2;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::OPERATION, "fload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FLOAD_3: {
            uint32_t index = 3;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::OPERATION, "fload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LLOAD_0: {
            uint32_t index = 0;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::OPERATION, "lload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LLOAD_1: {
            uint32_t index = 1;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::OPERATION, "lload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LLOAD_2: {
            uint32_t index = 2;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::OPERATION, "lload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LLOAD_3: {
            uint32_t index = 3;  // Get local variable index
            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::OPERATION, "lload_result");
            SSAInstruction loadInstr(SSAInstruction::LOAD_LOCAL, {localVariable}, result);
            ssaInstructions.push_back(loadInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ISTORE: {
            uint32_t index = insn.Op1.addr;
            SSAVariable valueToStore = updatedStack.back();
            updatedStack.pop_back();
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {valueToStore});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = valueToStore;  // Update local variable version
            break;
        }
        case OPCODE_FADD: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::OPERATION, "fadd_result");
            SSAInstruction addInstr(SSAInstruction::ADD, {operand1, operand2}, result);
            ssaInstructions.push_back(addInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LADD: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::OPERATION, "ladd_result");
            SSAInstruction addInstr(SSAInstruction::ADD, {operand1, operand2}, result);
            ssaInstructions.push_back(addInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_DADD: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Double, "double"), SSAVariable::Origin::OPERATION, "dadd_result");
            SSAInstruction addInstr(SSAInstruction::ADD, {operand1, operand2}, result);
            ssaInstructions.push_back(addInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ISUB: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "isub_result");
            SSAInstruction subInstr(SSAInstruction::SUB, {operand1, operand2}, result);
            ssaInstructions.push_back(subInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_IMUL: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "imul_result");
            SSAInstruction mulInstr(SSAInstruction::MUL, {operand1, operand2}, result);
            ssaInstructions.push_back(mulInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_IDIV: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "idiv_result");
            SSAInstruction divInstr(SSAInstruction::DIV, {operand1, operand2}, result);
            ssaInstructions.push_back(divInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_IREM: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "irem_result");
            SSAInstruction remInstr(SSAInstruction::REM, {operand1, operand2}, result);
            ssaInstructions.push_back(remInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_FREM: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::OPERATION, "frem_result");
            SSAInstruction remInstr(SSAInstruction::REM, {operand1, operand2}, result);
            ssaInstructions.push_back(remInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_DREM: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Double, "double"), SSAVariable::Origin::OPERATION, "drem_result");
            SSAInstruction remInstr(SSAInstruction::REM, {operand1, operand2}, result);
            ssaInstructions.push_back(remInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_LREM: {
            SSAVariable operand2 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable operand1 = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Long, "long"), SSAVariable::Origin::OPERATION, "lrem_result");
            SSAInstruction remInstr(SSAInstruction::REM, {operand1, operand2}, result);
            ssaInstructions.push_back(remInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_IFNE: {
            SSAVariable conditionVar = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable result(Type::BasicType(Type::Boolean, "boolean"), SSAVariable::Origin::OPERATION, "ifne_result");
            SSAInstruction ifneInstr(SSAInstruction::ICMP_NE_ZERO, {conditionVar}, result);
            ssaInstructions.push_back(ifneInstr);
            SSAInstruction ifInstr(SSAInstruction::IF, {result});
            ssaInstructions.push_back(ifInstr);
            break;
        }
        case OPCODE_IINC: {
            uint32_t index = (insn.Op2.value >> 8) & 0xFF;
            int32_t increment = static_cast<int32_t>(insn.Op2.value & 0xFF);

            SSAVariable localVariable;
            if (updatedLocalVars.find(index) != updatedLocalVars.end()) {
                localVariable = updatedLocalVars[index];
            } else {
                localVariable = SSAVariable(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::LOCAL, "local" + std::to_string(index), index);
            }

            SSAVariable incrementVar(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "increment_const_" + std::to_string(increment), -1);
            SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::OPERATION, "iinc_result");

            SSAInstruction addInstr(SSAInstruction::ADD, {localVariable, incrementVar}, result);
            ssaInstructions.push_back(addInstr);
            SSAInstruction storeInstr(SSAInstruction::STORE_LOCAL, {result});
            ssaInstructions.push_back(storeInstr);
            updatedLocalVars[index] = result;
            break;
        }
        case OPCODE_NEWARRAY: {
            SSAVariable size = updatedStack.back();
            updatedStack.pop_back();
            //  In a real scenario, the type would be determined by insn.Op1.value
            Type arrayType = getType(insn.Op1.value);
            Type arr = Type::ReferenceType(std::make_unique<Type>(arrayType));
            SSAVariable result(arr, SSAVariable::Origin::OPERATION, "newarray_result");
            SSAInstruction newArrayInstr(SSAInstruction::NEW_ARRAY, {size}, result);
            ssaInstructions.push_back(newArrayInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_ANEWARRAY: {
            SSAVariable size = updatedStack.back();
            updatedStack.pop_back();
            // In a real scenario, the type would be determined by insn.Op1.value
            // For simplicity, we'll assume the type is java.lang.Object.  In a full
            // implementation, you'd use the type information from the constant pool.
            auto ty = FieldDescriptor::newDescriptor(GetConstantClassFromClass(m_ClassFile, insn.Op1.value)).m_Ty;
            Type arrayType = Type::ReferenceType(std::make_unique<Type>(Type::ClassNameType("java/lang/Object")));
            SSAVariable result(ty, SSAVariable::Origin::OPERATION, "anewarray_result");
            SSAInstruction newArrayInstr(SSAInstruction::NEW_ARRAY, {size}, result);
            ssaInstructions.push_back(newArrayInstr);
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_AASTORE: {
            SSAVariable value = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable index = updatedStack.back();
            updatedStack.pop_back();
            SSAVariable arrayRef = updatedStack.back();
            updatedStack.pop_back();

            SSAInstruction arrayStoreInstr(SSAInstruction::ARRAY_STORE, {arrayRef, index, value});
            ssaInstructions.push_back(arrayStoreInstr);
            break;
        }
        case OPCODE_LDC: {
            ConstPoolInfo cpi = m_ClassFile.m_ConstantPool.m_ConstPoolInfo.at(insn.Op1.value - 1);
            if (cpi.Tag == Tags::String) {
                auto str = static_cast<StringInfo*>(cpi.Info.get());
                std::string constStr = GetConstantUTF8FromClass(m_ClassFile, str->stringIndex);
                SSAVariable result(Type::ClassNameType("java/lang/String"), SSAVariable::Origin::CONSTANT, "ldc_string_" + constStr, -1);
                ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
                updatedStack.push_back(result);
            } else if(cpi.Tag == Tags::Int) {
                int32_t value = static_cast<int32_t>(insn.Op1.value);
                SSAVariable result(Type::BasicType(Type::Int, "int"), SSAVariable::Origin::CONSTANT, "ldc_int_" + std::to_string(value), -1);
                ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
                updatedStack.push_back(result);
            } else if(cpi.Tag == Tags::Float) {
                float value = static_cast<float>(insn.Op1.value);
                SSAVariable result(Type::BasicType(Type::Float, "float"), SSAVariable::Origin::CONSTANT, "ldc_float_" + std::to_string(value), -1);
                ssaInstructions.push_back({SSAInstruction::LOAD_CONST, {}, result});
                updatedStack.push_back(result);
            }
            break;
        }
        case OPCODE_GETSTATIC: {
            auto fieldRefStack = GetFieldRefFromIndex(m_ClassFile, insn.Op1.value);
            SSAVariable result = SSAVariable(fieldRefStack.m_type, SSAVariable::Origin::OPERATION, "getstatic_" + fieldRefStack.m_Class + "." + fieldRefStack.m_Name, -1);
            ssaInstructions.push_back({SSAInstruction::GET_STATIC, {}, result});
            updatedStack.push_back(result);
            break;
        }
        case OPCODE_GETFIELD: {
            auto fieldRefStack = GetFieldRefFromIndex(m_ClassFile, insn.Op1.value);
            auto obj = updatedStack.back();
            updatedStack.pop_back();
            // if(obj.isIdentifier()) {
            //     StackEntry::Field field{std::make_shared<StackEntry>(obj), fieldRefStack.m_Name, fieldRefStack.m_type};
            //     m_Stack.emplace_back(field);
            // }
            break;
        }
        case OPCODE_GOTO: {
            uint16_t branchOffset = insn.Op1.addr;
            
            SSAInstruction gotoInstr(SSAInstruction::GOTO, {}, std::nullopt, branchOffset); // No result
            gotoInstr.branchTarget = branchOffset;
            ssaInstructions.push_back(gotoInstr);
            break;
        }
        case OPCODE_RETURN: {
            if (!updatedStack.empty()) {
                SSAVariable returnValue = updatedStack.back();
                updatedStack.pop_back();
                SSAInstruction returnInstr(SSAInstruction::RETURN, {returnValue});
                ssaInstructions.push_back(returnInstr);
            } else {
                SSAInstruction returnInstr(SSAInstruction::RETURN, {});
                ssaInstructions.push_back(returnInstr);
            }

            break;
        }
        // ... (handle other opcodes) ...
        default:
            // Handle unknown or unsupported opcodes
            ssaInstructions.emplace_back(SSAInstruction::UNKNOWN);
            break;
    }
    return std::make_tuple(ssaInstructions, updatedStack, updatedLocalVars);
    //     case OPCODE_SIPUSH: { // short integer push
    //         int32_t value = static_cast<int32_t>(instruction.Op1.value);
    //         m_Stack.emplace_back(value);
    //         break;
    //     }

    //     // case OPCODE_LDC2_W: {
    //     // }

    //     case OPCODE_ACONST_NULL: //NULL reference
    //         m_Stack.emplace_back(StackEntry::Null());
    //         return std::make_shared<AST>(std::monostate{});
    //     // FCONST pushes predefined float constants onto stack

    //     case OPCODE_ISTORE:
    //     case OPCODE_ASTORE:
    //     case OPCODE_DSTORE:
    //     case OPCODE_LSTORE:
    //     case OPCODE_FSTORE: {
    //         ASTPtr store = std::make_shared<AST>(std::move(Store(instruction.Op1.value).value()));
    //         return store;
    //     }
    //     // Array storage operations
    //     case OPCODE_AASTORE: //Reference Array
    //     case OPCODE_BASTORE: //Byte or boolean
    //     case OPCODE_CASTORE: //char
    //     case OPCODE_DASTORE: //double
    //     case OPCODE_FASTORE: //float
    //     case OPCODE_IASTORE: //int
    //     case OPCODE_LASTORE: //long
    //     case OPCODE_SASTORE: { //short
    //         StackEntry val = PopStack();
    //         StackEntry index = PopStack();
    //         StackEntry array = PopStack();
    //         if(array.isArray()) {
    //             StackEntry::Array arr = array.getArrayValue();
    //             arr.elements.emplace_back(val);
    //             m_Stack.emplace_back(StackEntry::Array{arr.type, arr.length, arr.elements});
    //         } else {
    //             //if array is not explicitly an array, assume it is an object array.
    //             auto entry = ConvertStackEntryToAST(array);
    //             AST::ArrayIndex arrIndex{entry, ConvertStackEntryToAST(index), array.ty()};
    //             AST::ReAssignment reAssign{ std::make_shared<AST>(arrIndex), ConvertStackEntryToAST(val)};
    //             ASTPtr reAssignment = std::make_shared<AST>(std::move(reAssign));
    //             return reAssignment;
    //         }
    //         break;
    //     }

    //     case OPCODE_ISHL:
    //     case OPCODE_LSHL:
    //         BinaryOper(BinaryOp::Shl);
    //         break;
    //     //Shift right a >> b
    //     case OPCODE_ISHR:
    //     case OPCODE_LSHR:
    //         BinaryOper(BinaryOp::Shr);
    //         break;
    //     //Unsigned shift right a >>> b
    //     case OPCODE_IUSHR:
    //     case OPCODE_LUSHR:
    //         BinaryOper(BinaryOp::UShr);
    //         break;
    //     //Bitwise logical operations
    //     case OPCODE_IXOR:
    //     case OPCODE_LXOR:
    //         BinaryOper(BinaryOp::Xor);
    //         break;
    //     case OPCODE_IAND:
    //     case OPCODE_LAND:
    //         BinaryOper(BinaryOp::And);
    //         break;
    //     case OPCODE_IOR:
    //     case OPCODE_LOR:
    //         BinaryOper(BinaryOp::Or);
    //         break;
    //     case OPCODE_FCMPG:
    //     case OPCODE_DCMPG:
    //         BinaryOper(BinaryOp::GreaterThan);
    //         break;
    //     case OPCODE_FCMPL:
    //     case OPCODE_DCMPL:
    //         BinaryOper(BinaryOp::LessThan);
    //         break;
    //     case OPCODE_INEG:
    //     case OPCODE_FNEG:
    //     case OPCODE_DNEG:
    //     case OPCODE_LNEG: {
    //         StackEntry val = PopStack();
    //         m_Stack.emplace_back(StackEntry::UnaryOperation{UnaryOp::Neg, std::make_unique<StackEntry>(std::move(val))});
    //         break;
    //     }
    //     //Process opcode for type checking
    //     case OPCODE_INSTANCEOF: {
    //         StackEntry obj1 = PopStack(); //pop object reference
    //         std::string obj2 = GetConstantClassFromClass(m_ClassFile, instruction.Op1.value); //Retrieve class name from constant pool
    //         StackEntry::Class obj2Class{obj2};
    //         StackEntry::BinaryOperation binOp{std::make_shared<StackEntry>(obj1), BinaryOp::InstanceOf, std::make_shared<StackEntry>(obj2Class)};
    //         m_Stack.emplace_back(binOp);
    //         break;
    //     }
    //     case OPCODE_LCMP: {
    //         break;
    //     }
    //     case OPCODE_ARETURN:
    //     case OPCODE_IRETURN:
    //     case OPCODE_FRETURN:
    //     case OPCODE_DRETURN:
    //     case OPCODE_LRETURN: {
    //         AST::Return ret{ConvertStackEntryToAST(PopStack())};
    //         ASTPtr returnAST = std::make_shared<AST>(std::move(ret));

    //         return returnAST;
    //     }
    //     //Process opcode for dynamic method calls
    //     case OPCODE_INVOKEDYNAMIC: {
    //         auto methStack = GetDynamicInvokeRefFromIndex(m_ClassFile, instruction.Op1.value);

    //         if(methStack.m_Class == "StringConcatFactory" && methStack.m_Name == "makeConcatWithConstants") {
    //             auto args = methStack.unparsed_args;
    //             StringConcatFactory(args);
    //             // return nullptr;
    //             break;
    //         }

    //         if(methStack.desc.args.empty()) {
    //             break;
    //         }

    //         std::vector<StackEntry> args;

    //         for(int i = 0; i < methStack.desc.args.size(); i++) {
    //             args.emplace_back(PopStack());
    //         }

    //         std::string name = methStack.m_Class + "." + methStack.m_Name;

    //         StackEntry::Function func{getLastClassNamePart(name), args, methStack.desc.returnType};
    //         m_Stack.emplace_back(func);
    //         break;
    //     }
    //     //Process opcodes for interface method calls
    //     case OPCODE_INVOKEINTERFACE: {
    //         auto methStack = GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);

    //         if(methStack.desc.args.empty()) {
    //             break;
    //         }

    //         std::vector<StackEntry> args;

    //         for(int i = 0; i < methStack.desc.args.size(); i++) {
    //             args.emplace_back(PopStack());
    //         }

    //         std::string name = methStack.m_Class + "." + methStack.m_Name;

    //         StackEntry::Function func{getLastClassNamePart(name), args, methStack.desc.returnType};
    //         m_Stack.emplace_back(func);
    //         break;
    //     }
    //     //Process opcode for constructors and private method calls
    //     case OPCODE_INVOKESPECIAL: {
    //         MethodRefStack methStack = GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
    //         std::vector<StackEntry> argsStack;
    //         std::vector<ASTPtr> argsAST;
    //         for(auto& descArgs : methStack.desc.args) {
    //             argsStack.emplace_back(PopStack());
    //             // Convert each argument StackEntry to an ASTPtr
    //             // argsAST.push_back(ConvertStackEntryToAST(argsStack.back()));
    //         }

    //         auto objStackEntry = PopStack();
    //         std::string methodName = "";
    //         Type returnType = Type::Void; // Default return type

    //         if(methStack.m_Name == "<init>" || methStack.m_Name == "<clinit>") {
    //             if(objStackEntry.isClass()) {
    //                 methodName = objStackEntry.getClassValue().m_ClassName;
    //                 returnType = Type::ClassNameType(methStack.m_Class); // Constructor returns the object type
    //                 return std::make_shared<AST>(AST::Object{methodName}); // Represent constructor call as object creation? This depends on your semantics
    //             } else if(objStackEntry.isIdentifier()) {
    //                 methodName = objStackEntry.getIdentifierValue().name;
    //                 returnType = Type::ClassNameType(methStack.m_Class); // Constructor returns the object type
    //                 // return std::make_shared<AST>(AST::Object{methodName}); // Represent constructor call as object creation? This depends on your
    //                 semantics
    //             }
    //         } else {
    //             if(objStackEntry.isClass()) {
    //                 methodName = objStackEntry.getClassValue().m_ClassName + "." + methStack.m_Name;
    //                 returnType = Type::ClassNameType(methStack.m_Class); // Assuming method returns the class type for now - adjust based on
    //                 methStack.desc.return_type
    //             }
    //         }

    //         if (!methodName.empty()) {
    //             return std::make_shared<AST>(AST::FunctionCall{methodName, argsAST, returnType});
    //         } else {
    //             // Handle cases where method name couldn't be determined
    //             std::cerr << "Warning: Could not determine method name for INVOKESPECIAL" << std::endl;
    //             return nullptr; // Or some other appropriate error handling
    //         }
    //         break;
    //     }
    //     //Process opcode for static method calls
    //     case OPCODE_INVOKESTATIC: {
    //         MethodRefStack methStack = GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
    //         std::vector<StackEntry> args;
    //         for(auto& descArgs : methStack.desc.args) {
    //             args.emplace_back(PopStack());
    //         }

    //         std::string funcName = methStack.m_Class + "." + methStack.m_Name;
    //         StackEntry::Function func{funcName, args, methStack.desc.returnType};

    //         if(methStack.desc.returnType.kind == Type::Void) {
    //             AST::Object object{methStack.m_Class};
    //             std::vector<ASTPtr> astArgs;
    //             for(auto& stacks: args) {
    //                 astArgs.emplace_back(ConvertStackEntryToAST(stacks));
    //             }
    //             AST::FunctionCall funcCall{methStack.m_Name, astArgs, methStack.desc.returnType};
    //             AST::MethodCall call{std::make_shared<AST>(object), std::make_shared<AST>(funcCall)};
    //             ASTPtr methodCall = std::make_shared<AST>(std::move(call));
    //             return methodCall;
    //         }
    //         m_Stack.emplace_back(func);
    //         break;
    //     }
    //     //Process opcode for virtual method calls
    //     case OPCODE_INVOKEVIRTUAL: {
    //         MethodRefStack methStack = GetMethodRefFromIndex(m_ClassFile, instruction.Op1.value);
    //         std::vector<StackEntry> args;
    //         for(auto& descArgs : methStack.desc.args) {
    //             args.emplace_back(PopStack());
    //         }

    //         auto object = PopStack();
    //         std::string name = "";
    //         if(object.isClass()) {
    //             name = object.getClassValue().m_ClassName;
    //         } else if(object.isField()) {
    //             name = object.getFieldValue().field_name;
    //         }

    //         std::string funcName = name + "." + methStack.m_Name;
    //         StackEntry::Function func{funcName, args, methStack.desc.returnType};
    //         if(methStack.desc.returnType.kind == Type::Void) {
    //             auto obj = ConvertStackEntryToAST(object);
    //             std::vector<ASTPtr> astArgs;
    //             for(auto& stacks: args) {
    //                 astArgs.emplace_back(ConvertStackEntryToAST(stacks));
    //             }
    //             AST::FunctionCall funcCall{methStack.m_Name, astArgs, methStack.desc.returnType};
    //             AST::MethodCall call{std::make_shared<AST>(obj), std::make_shared<AST>(funcCall)};
    //             ASTPtr methodCall = std::make_shared<AST>(std::move(call));

    //             return methodCall;
    //         }
    //         m_Stack.emplace_back(func);
    //         break;
    //     }
    //     case OPCODE_GETFIELD: {
    //         auto fieldRefStack = GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
    //         auto obj = PopStack();
    //         if(obj.isIdentifier()) {
    //             StackEntry::Field field{std::make_shared<StackEntry>(obj), fieldRefStack.m_Name, fieldRefStack.m_type};
    //             m_Stack.emplace_back(field);
    //         }
    //         break;
    //     }

    //     case OPCODE_PUTSTATIC: {
    //         auto val = PopStack();
    //         FieldRefStack fieldRefStack = GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);
    //         AST::Object obj{fieldRefStack.m_Class};
    //         AST::FieldAssignment fieldAssign{std::make_shared<AST>(obj), fieldRefStack.m_Name, fieldRefStack.m_type, ConvertStackEntryToAST(val)};
    //         ASTPtr fieldAssignment = std::make_shared<AST>(std::move(fieldAssign));
    //         return fieldAssignment;
    //     }
    //     case OPCODE_PUTFIELD: {
    //         auto val = PopStack();
    //         auto obj = PopStack();
    //         FieldRefStack fieldRefStack = GetFieldRefFromIndex(m_ClassFile, instruction.Op1.value);

    //         if(m_InsideInit) {
    //             // m_Fie
    //         }
    //         // AST::Object obj{fieldRefStack.m_Class};
    //         AST::FieldAssignment fieldAssign{ConvertStackEntryToAST(obj), fieldRefStack.m_Name, fieldRefStack.m_type, ConvertStackEntryToAST(val)};
    //         ASTPtr fieldAssignment = std::make_shared<AST>(std::move(fieldAssign));
    //         return fieldAssignment;
    //     }
    //     case OPCODE_MULTIANEWARRAY: {
    //         break;
    //     }
    //     case OPCODE_ARRAYLENGTH: {
    //         StackEntry val = PopStack();
    //         StackEntry::UnaryOperation unaryOp{UnaryOp::ArrayLength, std::make_shared<StackEntry>(val)};
    //         m_Stack.emplace_back(unaryOp);
    //         break;
    //     }
    //     //No operation
    //     case OPCODE_NOP: {
    //         break;
    //     }
    //     //pop value from stack
    //     case OPCODE_POP: {
    //         auto value = m_Stack.back();
    //         m_Stack.pop_back();
    //         return ConvertStackEntryToAST(value);
    //     }
    //     //pop 2 values from stack
    //     case OPCODE_POP2: {
    //         auto val1 = PopStack();
    //         if(val1.ty().kind == Type::Long || val1.ty().kind == Type::Double) {
    //         } else {
    //             return ConvertStackEntryToAST(PopStack());

    //         }
    //         return ConvertStackEntryToAST(val1);
    //     }
    //     case OPCODE_IF_ACMPEQ: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if reference are not equal
    //     case OPCODE_IF_ACMPNE: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if equal to zero
    //     case OPCODE_IFEQ: {
    //         auto cond = ConvertStackEntryToAST(PopStack());
    //         int64_t pos = m_CurrentPos + instruction.Op1.addr;
    //         std::vector<ASTPtr> then;
    //         std::vector<Insn>::iterator currToken = funcInstructions.begin();
    //         std::vector<Insn>::iterator endToken = funcInstructions.end();

    //         while(currToken != endToken) {
    //             Insn insn = *currToken++;

    //             std::optional<AST> ast = ConvertInstructionToAST(insn);
    //             if(ast.has_value()) {
    //                 then.emplace_back(std::make_unique<AST>(std::move(*ast)));
    //             } else {
    //                 continue;
    //             }

    //             if(m_CurrentPos <= pos) {
    //                 break;
    //             }
    //         }

    //         if(then.size() == 1) {
    //             if (then[0]->isIfStatement()) { // Check if AST is an If condition
    //                 auto ifAst = then[0]->getIfNode();
    //                 auto this_cond = ifAst.cond;
    //                 auto this_then = ifAst.then;
    //                 then = this_then;
    //                 AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd, this_cond};
    //                 cond = std::make_shared<AST>(binOp);
    //                 // cond = std::make_shared<AST>(AST::BinaryOperation(cond, BinaryOp::LogicalAnd, ifAst->cond));
    //             }
    //         }
    //         AST::If ifStatement{cond, then};
    //         return std::make_shared<AST>(ifStatement);
    //     }
    //     //Process opcode for branch if greater than or equal to zero
    //     case OPCODE_IFGE: {

    //         auto cond = ConvertStackEntryToAST(PopStack());
    //         int64_t pos = m_CurrentPos + instruction.Op1.addr;
    //         std::vector<ASTPtr> then;
    //         std::vector<Insn>::iterator currToken = funcInstructions.begin();
    //         std::vector<Insn>::iterator endToken = funcInstructions.end();

    //         while(currToken != endToken) {
    //             Insn insn = *currToken++;

    //             std::optional<AST> ast = ConvertInstructionToAST(insn);
    //             if(ast.has_value()) {
    //                 then.emplace_back(std::make_unique<AST>(std::move(*ast)));
    //             } else {
    //                 continue;
    //             }

    //             if(m_CurrentPos <= pos) {
    //                 break;
    //             }
    //         }

    //         if(then.size() == 1) {
    //             if (then[0]->isIfStatement()) { // Check if AST is an If condition
    //                 auto ifAst = then[0]->getIfNode();
    //                 auto this_cond = ifAst.cond;
    //                 auto this_then = ifAst.then;
    //                 then = this_then;
    //                 AST::BinaryOperation binOp{cond, BinaryOp::GreaterEqualThan, this_cond};
    //                 cond = std::make_shared<AST>(binOp);
    //                 // cond = std::make_shared<AST>(AST::BinaryOperation(cond, BinaryOp::LogicalAnd, ifAst->cond));
    //             }
    //         }
    //         AST::If ifStatement{cond, then};
    //         return std::make_shared<AST>(ifStatement);
    //     }
    //     //Process opcode for branch if greater than zero
    //     case OPCODE_IFGT: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if less than or equal to zero
    //     case OPCODE_IFLE: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if less than zero
    //     case OPCODE_IFLT: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //opcode for branch if not equal to zero
    //     case OPCODE_IFNE: {
    //         auto raw_cond = PopStack();
    //         int64_t pos = m_CurrentPos + instruction.Op1.addr;
    //         std::vector<ASTPtr> then;
    //         std::vector<Insn>::iterator currToken = funcInstructions.begin();
    //         std::vector<Insn>::iterator endToken = funcInstructions.end();

    //         while(currToken != endToken) {
    //             Insn insn = *currToken++;

    //             std::optional<AST> ast = ConvertInstructionToAST(insn);
    //             if(ast.has_value()) {
    //                 then.emplace_back(std::make_unique<AST>(std::move(*ast)));
    //             } else {
    //                 continue;
    //             }
    //             if(m_CurrentPos <= pos) {
    //                 break;
    //             }
    //         }

    //         ASTPtr cond;
    //         if(then.size() != 1) {
    //             AST::UnaryOperation unOp{UnaryOp::Negate, ConvertStackEntryToAST(raw_cond)};
    //             cond = std::make_shared<AST>(unOp);
    //         } else {
    //             AST first_then = then[0];
    //             if (first_then.isIfStatement()) {
    //                auto this_if = first_then.getIfNode();
    //                then = this_if.then;
    //                AST::BinaryOperation binOp{ConvertStackEntryToAST(raw_cond), BinaryOp::LogicalOr, std::move(this_if.cond)};
    //                cond = std::make_unique<AST>(binOp);
    //             } else {
    //                 AST::UnaryOperation unOp{UnaryOp::Negate, ConvertStackEntryToAST(raw_cond)};
    //                 cond = std::make_unique<AST>(unOp);
    //             }
    //         }
    //         AST::If ifStatement{cond, then};
    //         return std::make_shared<AST>(ifStatement);
    //     }
    //     //Process opcode for branch if equal, val1 == val2
    //     case OPCODE_IF_ICMPEQ: {
    //         StackEntry val2 = PopStack();
    //         StackEntry val1 = PopStack();
    //         AST::BinaryOperation binaryOp{ConvertStackEntryToAST(val1), BinaryOp::NotEqual, ConvertStackEntryToAST(val2)};
    //         ASTPtr cond = std::make_shared<AST>(binaryOp);
    //         int64_t pos = m_CurrentPos + instruction.Op1.addr;
    //         std::vector<ASTPtr> then;

    //         std::vector<Insn>::iterator currToken = funcInstructions.begin();
    //         std::vector<Insn>::iterator endToken = funcInstructions.end();

    //         while(currToken != endToken) {
    //             Insn insn = *currToken++;

    //             std::optional<AST> ast = ConvertInstructionToAST(insn);
    //             if(ast.has_value()) {
    //                 then.emplace_back(std::make_unique<AST>(std::move(*ast)));
    //             } else {
    //                 continue;
    //             }
    //             if(m_CurrentPos <= pos) {
    //                 break;
    //             }
    //         }

    //         if(then.size() == 1) {
    //             if (then[0]->isIfStatement()) { // Check if AST is an If condition
    //                 auto ifAst = then[0]->getIfNode();
    //                 auto this_cond = ifAst.cond;
    //                 auto this_then = ifAst.then;
    //                 then = this_then;
    //                 AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd, this_cond};
    //                 cond = std::make_shared<AST>(binOp);
    //             }
    //         }

    //         AST::If ifStatement{cond, then};
    //         return std::make_shared<AST>(ifStatement);
    //     }
    //     //Process opcode for branch if Not equal, val1 != val2
    //     case OPCODE_IF_ICMPNE: {
    //         StackEntry val2 = PopStack();
    //         StackEntry val1 = PopStack();
    //         AST::BinaryOperation binaryOp{ConvertStackEntryToAST(val1), BinaryOp::Equal, ConvertStackEntryToAST(val2)};
    //         ASTPtr cond = std::make_shared<AST>(binaryOp);
    //         int64_t pos = m_CurrentPos + instruction.Op1.addr;
    //         std::vector<ASTPtr> then;

    //         std::vector<Insn>::iterator currToken = funcInstructions.begin();
    //         std::vector<Insn>::iterator endToken = funcInstructions.end();

    //         while(currToken != endToken) {
    //             Insn insn = *currToken++;

    //             std::optional<AST> ast = ConvertInstructionToAST(insn);
    //             if(ast.has_value()) {
    //                 then.emplace_back(std::make_unique<AST>(std::move(*ast)));
    //             } else {
    //                 continue;
    //             }
    //             if(m_CurrentPos <= pos) {
    //                 break;
    //             }
    //         }

    //         if(then.size() == 1) {
    //             if (then[0]->isIfStatement()) { // Check if AST is an If condition
    //                 auto ifAst = then[0]->getIfNode();
    //                 auto this_cond = ifAst.cond;
    //                 auto this_then = ifAst.then;
    //                 then = this_then;
    //                 AST::BinaryOperation binOp{cond, BinaryOp::LogicalAnd, this_cond};
    //                 cond = std::make_shared<AST>(binOp);
    //             }
    //         }

    //         AST::If ifStatement{cond, then};
    //         return std::make_shared<AST>(ifStatement);
    //     }
    //     //Process opcode for branch if greater than equal to, val1 >= val2
    //     case OPCODE_IF_ICMPGE: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if greater than, val1 > val2
    //     case OPCODE_IF_ICMPGT: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if less than or equal to, val1 <= val2
    //     case OPCODE_IF_ICMPLE: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if less than, val1 < val2
    //     case OPCODE_IF_ICMPLT: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if not null
    //     case OPCODE_IFNONNULL: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch if null
    //     case OPCODE_IFNULL: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch always instrucion
    //     case OPCODE_GOTO: {
    //         int targetBlockID = instruction.Op1.value;
    //         // size_t currentBlockID = block->m_ID;
    //         // auto block = funct.GetFunctionCFG()->GetBlockByInsn(instruction);
    //         // if(IsBackEdge(instruction, m_Dominators, currentBlockID)) {
    //         //     std::cout << "IS BACK EDGE" << std::endl;
    //         //     // auto loopNode = BuildLoopAST(insn, domTree);
    //         // } else {
    //         //     std::cout << "NOT A BACK EDGE" << std::endl;
    //         // }
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for branch always (wide index) instruction
    //     case OPCODE_GOTO_W: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for jump subroutine
    //     case OPCODE_JSR: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for jump subroutine (wide index)
    //     case OPCODE_JSR_W: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for return from subroutine
    //     case OPCODE_RET: {
    //         // TODO: Not Implemented
    //         break;
    //     }
    //     //Process opcode for convert int to byte
    //     case OPCODE_I2B: {
    //         Cast(Type::Byte);
    //         break;
    //     }
    //     //Process opcode for convert int to char
    //     case OPCODE_I2C: {
    //         Cast(Type::Char);
    //         break;
    //     }
    //     //Process opcode for convert into to short
    //     case OPCODE_I2S: {
    //         Cast(Type::Short);
    //         break;
    //     }
    //     //Process opcodes for converting to double
    //     case OPCODE_I2D:
    //     case OPCODE_F2D:
    //     case OPCODE_L2D: {
    //         Cast(Type::Double);
    //         break;
    //     }
    //     //Process opcodes for converting to long
    //     case OPCODE_I2L:
    //     case OPCODE_F2L:
    //     case OPCODE_D2L: {
    //         Cast(Type::Long);
    //         break;
    //     }
    //     //Process opcodes for converting to int
    //     case OPCODE_F2I:
    //     case OPCODE_D2I:
    //     case OPCODE_L2I: {
    //         Cast(Type::Int);
    //         break;
    //     }
    //     //Process opcodes for converting to float
    //     case OPCODE_I2F:
    //     case OPCODE_D2F:
    //     case OPCODE_L2F: {
    //         Cast(Type::Float);
    //         break;
    //     }
    //     //Process opcode for creating new objects
    //     case OPCODE_NEW: {
    //         auto obj = GetConstantClassFromClass(m_ClassFile, instruction.Op1.value);
    //         StackEntry::New newObj{obj};
    //         m_Stack.emplace_back(StackEntry(newObj));
    //         break;
    //     }
    //     //Process opcode for duplicating top stack value
    //     case OPCODE_DUP: {
    //         StackEntry val = PopStack();
    //         if(val.isArray() || val.isNew()) {
    //             m_Stack.emplace_back(val);
    //         } else {
    //             m_Stack.emplace_back(val);
    //             m_Stack.emplace_back(val);
    //         }
    //         break;
    //     }
    //     //Process opcode for duplicating the top value and inserting it below the second value
    //     case OPCODE_DUP_X1: {
    //         StackEntry val1 = PopStack();
    //         StackEntry val2 = PopStack();
    //         m_Stack.emplace_back(StackEntry(val1));
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         break;
    //     }
    //     //Process opcode for Duplicating the top value and inserting it below the third value
    //     case OPCODE_DUP_X2: {
    //         StackEntry val1 = PopStack();
    //         StackEntry val2 = PopStack();
    //         StackEntry val3 = PopStack();
    //         m_Stack.emplace_back(StackEntry(val1));
    //         m_Stack.emplace_back(StackEntry(val3));
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         break;
    //     }
    //     //Process opcode for duplicating the top two values
    //     case OPCODE_DUP2: {
    //         StackEntry val1 = PopStack();
    //         StackEntry val2 = PopStack();
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         break;
    //     }
    //     //Process opcode for duplicating the top two values and inserting them below the third value
    //     case OPCODE_DUP2_X1: {
    //         StackEntry val1 = PopStack();
    //         StackEntry val2 = PopStack();
    //         StackEntry val3 = PopStack();
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         m_Stack.emplace_back(StackEntry(val3));
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         break;
    //     }
    //     //Inserts the duplicated top two values below the fourth value
    //     case OPCODE_DUP2_X2: {
    //         StackEntry val1 = PopStack();
    //         StackEntry val2 = PopStack();
    //         StackEntry val3 = PopStack();
    //         StackEntry val4 = PopStack();
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         m_Stack.emplace_back(StackEntry(val4));
    //         m_Stack.emplace_back(StackEntry(val3));
    //         m_Stack.emplace_back(StackEntry(val2));
    //         m_Stack.emplace_back(StackEntry(val1));
    //         break;
    //     }
    //     //Swaps the top two stack values
    //     case OPCODE_SWAP: {
    //         StackEntry val1 = PopStack();
    //         StackEntry val2 = PopStack();
    //         m_Stack.emplace_back(StackEntry(val1));
    //         m_Stack.emplace_back(StackEntry(val2));
    //         break;
    //     }
    //     default:
    //         // Handle other instructions
    //         return nullptr;
    // }
}

}  // namespace TitaniumDecompiler
