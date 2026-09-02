#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "BasicBlock.h"
#include "ExceptionRangeCFG.h"
#include "FileFormats/JVM/ClassFileParser.h"
// #include "TitaniumDecompiler/Decompiler/SSA.h"

namespace TitaniumDecompiler {
struct CFG {
public:
    CFG() = default;
    ~CFG() = default;

    std::map<uint32_t, std::shared_ptr<BasicBlock>> CreateBasicBlocks(
        const InstrMap& instructions);

    // std::vector<std::shared_ptr<BasicBlock>> BuildBlocks(const InstrMap&
    // instructions);

    void BuildGraph();
    void ConnectBasicBlocks(
        const std::map<uint32_t, std::shared_ptr<BasicBlock>>& mapOfBlocks);
    void RemoveBlock(std::shared_ptr<BasicBlock> block);
    bool IsBranchInstruction(const Insn& insn);
    bool IsReturnInstruction(const Insn& insn);
    bool IsSwitchInstruction(const Insn& insn);
    bool IsConditionalBranch(const Insn& insn);
    bool IsUnconditionalJump(const Insn& insn);
    bool IsInvokeCall(const Insn& insn);
    void PrintBlock(const std::shared_ptr<BasicBlock>& block);
    void PrintBasicBlocks(
        const std::vector<std::shared_ptr<BasicBlock>>& basicBlocks);
    const Insn* FindInstructionByAddr(const InstrMap& instructions,
                                      uint64_t targetAddr);
    std::tuple<std::vector<SSAInstruction>, std::vector<SSAVariable>,
               std::map<uint32_t, SSAVariable>>
    TranslateBytecodeInstruction(
        const Insn& insn, std::vector<SSAVariable> stack,
        std::map<uint32_t, SSAVariable> localVarVersions);

    // std::set<size_t> FindBoundaries(const std::vector<Insn>& instructions);
    std::set<size_t> FindBoundaries(const InstrMap& instructions);
    std::map<uint32_t, std::shared_ptr<BasicBlock>> CreateBasicBlocks(
        std::vector<uint16_t> states, const std::vector<Insn>& instructions);
    static std::vector<uint16_t> FindStartInstructions(
        const std::vector<Insn>& instrs);
    size_t GetJumpTarget(const Insn& insn);
    std::map<uint32_t, std::shared_ptr<BasicBlock>> GetMapOfBlocks() {
        return m_Blocks;
    }
    std::shared_ptr<BasicBlock> GetBlockByInsn(const Insn& instruction);

public:
    std::vector<std::shared_ptr<BasicBlock>> GetBlocks() {
        std::vector<std::shared_ptr<BasicBlock>> blocks;
        for (auto& it : m_Blocks) {
            blocks.push_back(it.second);
        }
        return blocks;
    }

    std::shared_ptr<BasicBlock> GetFirst() { return m_First; }
    void SetFirst(const std::shared_ptr<BasicBlock>& first) { m_First = first; }
    std::shared_ptr<BasicBlock> GetLast() { return m_Last; }
    void SetLast(const std::shared_ptr<BasicBlock>& last) { m_Last = last; }
    std::vector<std::shared_ptr<ExceptionRangeCFG>> GetExceptions() {
        return m_Exceptions;
    }

    void SetCFGClassFile(ClassFile classFile) { m_ClassFile = classFile; }
    ClassFile GetCFGClassFile() { return m_ClassFile; }

public:
    ClassFile m_ClassFile;
    std::shared_ptr<SSACFG> m_SsaCfg;
    std::set<int> m_Boundaries;
    std::map<uint32_t, std::shared_ptr<BasicBlock>> m_Blocks;
    std::shared_ptr<BasicBlock> m_First;
    std::shared_ptr<BasicBlock> m_Last;
    std::vector<std::shared_ptr<ExceptionRangeCFG>> m_Exceptions;
};
}  // namespace TitaniumDecompiler
