#pragma once

#include "Core/Disassembly/DecodedInsn.h"
#include "TitaniumDecompiler/Disassembler/CFG.h"
namespace TitaniumDecompiler {
class JVMLifter : public ILifter {
public:
    bool Lift(const CFG& cfg, IR::Function& output) override;

private:
    bool LiftInstruction(const DecodedInsn& instruction, IR::BasicBlock& block);
};
}  // namespace TitaniumDecompiler
