#pragma once

#include "../../IDisassemblerFormatter.h"
#include "FileFormats/JVM/ConstantPool.h"

namespace TitaniumDecompiler {
class JVMFormatter : public IDisassemblyFormatter {
public:
    explicit JVMFormatter(const ConstantPool& ConstantPool);

    virtual std::string FormatInstruction(const DecodedInsn& insn) override;
    virtual std::string FormatBlock(
        const std::vector<DecodedInsn>& block) override;
    std::string ResolveConstantPoolEntry(uint16_t idx) const;

    bool IsConstantPoolInstruction(std::string_view mnemonic) const;

private:
    const ConstantPool& m_ConstantPool;
};
}  // namespace TitaniumDecompiler
