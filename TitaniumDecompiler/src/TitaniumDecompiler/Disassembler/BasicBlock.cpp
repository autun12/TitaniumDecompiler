#include "BasicBlock.h"

#include <algorithm>
#include <memory>

namespace TitaniumDecompiler {
void BasicBlock::AddPredecessor(std::shared_ptr<BasicBlock> basicBlock) {
    if (!basicBlock) return;
    m_Preds.push_back(basicBlock);
}

void BasicBlock::AddSuccessor(std::shared_ptr<BasicBlock> basicBlock) {
    if (!basicBlock) return;

    m_Succ.push_back(basicBlock);
    std::shared_ptr<BasicBlock> self = shared_from_this();
    basicBlock->AddPredecessor(self);
}

void BasicBlock::AddPredecessorException(std::shared_ptr<BasicBlock> basicBlock) {
    if (!basicBlock) return;
    m_PredExceptions.push_back(basicBlock);
}

void BasicBlock::AddSuccessorException(std::shared_ptr<BasicBlock> basicBlock) {
    if (!basicBlock) return;
    if (std::find(m_SuccExceptions.begin(), m_SuccExceptions.end(), basicBlock) != m_SuccExceptions.end()) {
        return;
    } else {
        m_SuccExceptions.push_back(basicBlock);
        std::shared_ptr<BasicBlock> self = shared_from_this();
        basicBlock->AddPredecessorException(self);
    }
}

std::vector<std::shared_ptr<BasicBlock>> BasicBlock::GetPreds() { return m_Preds; }

std::vector<std::shared_ptr<BasicBlock>> BasicBlock::GetSuccessor() { return m_Succ; }

std::vector<std::shared_ptr<BasicBlock>> BasicBlock::GetPredExceptions() { return m_PredExceptions; }

std::vector<std::shared_ptr<BasicBlock>> BasicBlock::GetSuccessorExceptions() { return m_SuccExceptions; }

std::vector<Insn> BasicBlock::GetInstructions() { return m_InstructionList; }

void BasicBlock::AddInstruction(Insn insns) { m_InstructionList.push_back(insns); }

Insn BasicBlock::GetLastInstruction() { return m_InstructionList.at(m_InstructionList.size() - 1); }

void BasicBlock::RemoveSuccessor(std::shared_ptr<BasicBlock> block) {
    auto it = std::remove_if(m_Succ.begin(), m_Succ.end(), [&block](const std::shared_ptr<BasicBlock>& succ) {
        return succ == block;  // Compare pointers
    });

    // If we found the block, erase it from the vector
    if (it != m_Succ.end()) {
        m_Succ.erase(it, m_Succ.end());
    }
    block->RemovePredecessor(shared_from_this());
}

void BasicBlock::RemoveSuccessorException(std::shared_ptr<BasicBlock> block) {
    auto it = std::remove_if(m_SuccExceptions.begin(), m_SuccExceptions.end(), [&block](const std::shared_ptr<BasicBlock>& succ) {
        return succ == block;  // Compare pointers
    });

    // If we found the block, erase it from the vector
    if (it != m_SuccExceptions.end()) {
        m_SuccExceptions.erase(it, m_SuccExceptions.end());
    }
    block->RemovePredecessorException(shared_from_this());
}

void BasicBlock::ReplaceSuccessor(std::shared_ptr<BasicBlock> old, std::shared_ptr<BasicBlock> newBlock) {
    if (old == newBlock) {
        return;
    }

    // Replace in m_Succ
    for (size_t i = 0; i < m_Succ.size(); ++i) {
        if (m_Succ[i]->m_ID == old->m_ID) {
            m_Succ[i] = newBlock;
            old->RemovePredecessor(shared_from_this());
            newBlock->AddPredecessor(shared_from_this());
        }
    }

    for (size_t i = 0; i < m_SuccExceptions.size(); ++i) {
        if (m_SuccExceptions[i]->m_ID == old->m_ID) {
            m_SuccExceptions[i] = newBlock;
            old->RemovePredecessorException(shared_from_this());
            newBlock->AddPredecessorException(shared_from_this());
        }
    }
}

void BasicBlock::RemovePredecessor(std::shared_ptr<BasicBlock> block) {
    auto it = std::remove_if(m_Preds.begin(), m_Preds.end(), [&block](const std::shared_ptr<BasicBlock>& pred) {
        return pred == block;  // Compare pointers
    });

    // If we found the block, erase it from the vector
    if (it != m_Preds.end()) {
        m_Preds.erase(it, m_Preds.end());
    }
}

void BasicBlock::RemovePredecessorException(std::shared_ptr<BasicBlock> block) {
    auto it = std::remove_if(m_PredExceptions.begin(), m_PredExceptions.end(), [&block](const std::shared_ptr<BasicBlock>& pred) {
        return pred == block;  // Compare pointers
    });

    // If we found the block, erase it from the vector
    if (it != m_PredExceptions.end()) {
        m_PredExceptions.erase(it, m_PredExceptions.end());
    }
}

}  // namespace TitaniumDecompiler
