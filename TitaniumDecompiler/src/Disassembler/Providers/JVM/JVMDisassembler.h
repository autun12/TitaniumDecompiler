#pragma once

#include "../../IDisassembler.h"
#include "FileFormats/JVM/ConstantPool.h"
#include "JVMInstructionSet.h"

namespace TitaniumDecompiler {
class JVMDisassembler : public IDisassembler {
public:
    explicit JVMDisassembler(const ConstantPool& ConstantPool);

    virtual bool DecodeInstruction(uint64_t address,
                                   std::span<const uint8_t> bytes,
                                   DecodedInsn& outInsn) override;
    virtual std::vector<DecodedInsn> DisassembleBlock(
        uint64_t startAddress, std::span<const uint8_t> bytes) override;
    virtual std::string_view GetArchitectureName() const override;

private:
    bool DecodeOperands(uint64_t address, std::span<const uint8_t> bytes,
                        const JVMOpcodeInfo& info, DecodedInsn& outInsn);

private:
    const ConstantPool& m_ConstantPool;
};
}  // namespace TitaniumDecompiler
