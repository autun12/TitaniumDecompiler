#pragma once

#include <memory>
#include <vector>

#include "FileFormats/JVM/Instruction.h"
#include "TitaniumDecompiler/Decompiler/AST.h"
#include "TitaniumDecompiler/Decompiler/SSA.h"

namespace TitaniumDecompiler {
struct BasicBlock : public std::enable_shared_from_this<BasicBlock> {
public:
    BasicBlock() = default;
    ~BasicBlock() {}
    Insn GetLastInstruction();

    void AddPredecessor(std::shared_ptr<BasicBlock> block);
    void RemovePredecessor(std::shared_ptr<BasicBlock> block);
    void AddSuccessor(std::shared_ptr<BasicBlock> block);
    void RemoveSuccessor(std::shared_ptr<BasicBlock> block);
    void AddPredecessorException(std::shared_ptr<BasicBlock> block);
    void RemovePredecessorException(std::shared_ptr<BasicBlock> block);
    void AddSuccessorException(std::shared_ptr<BasicBlock> block);
    void RemoveSuccessorException(std::shared_ptr<BasicBlock> block);
    void ReplaceSuccessor(std::shared_ptr<BasicBlock> old,
                          std::shared_ptr<BasicBlock> newBlock);

    std::vector<std::shared_ptr<BasicBlock>> GetPreds();
    std::vector<std::shared_ptr<BasicBlock>> GetSuccessor();

    std::vector<std::shared_ptr<BasicBlock>> GetPredExceptions();
    std::vector<std::shared_ptr<BasicBlock>> GetSuccessorExceptions();

    std::vector<Insn> GetInstructions();
    void AddInstruction(Insn insns);

    size_t GetJump(const Insn& insn);

public:
    int m_ID = 0;
    std::shared_ptr<SSABasicBlock> ssaBlock;

private:
    std::vector<Insn> m_InstructionList;
    std::vector<std::shared_ptr<BasicBlock>> m_Succ;
    std::vector<std::shared_ptr<BasicBlock>> m_Preds;

    std::vector<std::shared_ptr<BasicBlock>> m_SuccExceptions;
    std::vector<std::shared_ptr<BasicBlock>> m_PredExceptions;
};
}  // namespace TitaniumDecompiler
