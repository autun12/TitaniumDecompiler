#include "JVMDisassembler.h"

#include "JVMInstructionDecoder.h"

namespace TitaniumDecompiler {

JVMDisassembler::JVMDisassembler(const ConstantPool& constPool)
    : m_ConstantPool(constPool) {}

bool JVMDisassembler::DecodeInstruction(uint64_t address,
                                        std::span<const uint8_t> bytes,
                                        DecodedInsn& outInsn) {
    if (bytes.empty()) return false;
    return JVMInstructionDecoder::DecodeInstruction(address, bytes, outInsn);
}

std::vector<DecodedInsn> JVMDisassembler::DisassembleBlock(
    uint64_t startAddress, std::span<const uint8_t> bytes) {
    std::vector<DecodedInsn> block;
    uint32_t offset = 0;

    while (offset < bytes.size()) {
        std::span<const uint8_t> remainingBytes = bytes.subspan(offset);

        DecodedInsn insn;
        uint64_t currentAddr = startAddress + offset;
        if (!DecodeInstruction(currentAddr, remainingBytes, insn)) {
            break;
        }

        offset += insn.length;
        block.push_back(insn);
    }

    return block;
}

std::string_view JVMDisassembler::GetArchitectureName() const { return "JVM"; }
}  // namespace TitaniumDecompiler
