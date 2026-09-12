#include "JVMInstructionDecoder.h"

#include "Core/Disassembly/IDisassembler.h"
#include "JVMInstructionSet.h"

namespace TitaniumDecompiler {
namespace {

uint32_t ReadU32BE(std::span<const uint8_t> bytes, size_t offset) {
    return (static_cast<uint32_t>(bytes[offset]) << 24) |
           (static_cast<uint32_t>(bytes[offset + 1]) << 16) |
           (static_cast<uint32_t>(bytes[offset + 2]) << 8) |
           static_cast<uint32_t>(bytes[offset + 3]);
}

int32_t ReadS32BE(std::span<const uint8_t> bytes, size_t offset) {
    return static_cast<int32_t>(ReadU32BE(bytes, offset));
}

size_t SwitchPadding(uint64_t address) {
    // Address is the address of the opcode.
    //
    // The first 4-byte operand must begin at the next
    // 4-byte boundary.
    return static_cast<size_t>((4 - ((address + 1) & 3)) & 3);
}
}  // namespace

bool JVMInstructionDecoder::DecodeInstruction(uint64_t address,
                                              std::span<const uint8_t> bytes,
                                              DecodedInsn& outInsn) {
    if (bytes.empty()) return false;

    const uint8_t opcode = bytes[0];
    const JVMOpcodeInfo& info = JVMInstructionSet::Get(opcode);

    if (info.Mnemonic == "invalid") return false;

    outInsn = {};
    outInsn.address = address;
    outInsn.mnemonic = info.Mnemonic;
    outInsn.controlFlow = info.ControlFlow;

    if (!DecodeOperands(info.OperandEncoding, address, bytes, outInsn))
        return false;

    return true;
}

bool JVMInstructionDecoder::DecodeOperands(JVMOperandEncoding encoding,
                                           uint64_t address,
                                           std::span<const uint8_t> bytes,
                                           DecodedInsn& outInsn) {
    switch (encoding) {
        case JVMOperandEncoding::None: {
            outInsn.length = 1;
            return true;
        }

        case JVMOperandEncoding::S8: {
            if (bytes.size() < 2) return false;
            const int8_t value = static_cast<int8_t>(bytes[1]);
            outInsn.operands.push_back(
                {OperandType::Immediate,
                 static_cast<uint64_t>(static_cast<int64_t>(value))});
            outInsn.length = 2;
            return true;
        }

        case JVMOperandEncoding::S16: {
            if (bytes.size() < 3) return false;

            const int16_t value =
                static_cast<int16_t>((static_cast<uint16_t>(bytes[1]) << 8) |
                                     static_cast<uint16_t>(bytes[2]));

            outInsn.operands.push_back(
                {OperandType::Immediate,
                 static_cast<uint64_t>(static_cast<int64_t>(value))});

            outInsn.length = 3;
            return true;
        }
        case JVMOperandEncoding::U8: {
            if (bytes.size() < 2) return false;
            outInsn.operands.push_back({OperandType::Immediate, bytes[1]});
            outInsn.length = 2;
            return true;
        }
        case JVMOperandEncoding::U16: {
            if (bytes.size() < 3) return false;

            const uint16_t value = (static_cast<uint16_t>(bytes[1]) << 8) |
                                   static_cast<uint16_t>(bytes[2]);

            outInsn.operands.push_back({OperandType::Immediate, value});

            outInsn.length = 3;
            return true;
        }
        case JVMOperandEncoding::ConstPoolU8: {
            if (bytes.size() < 2) return false;

            outInsn.operands.push_back({OperandType::Immediate, bytes[1]});

            outInsn.length = 2;
            return true;
            return true;
        }
        case JVMOperandEncoding::ConstPoolU16: {
            if (bytes.size() < 3) return false;

            const uint16_t index = (static_cast<uint16_t>(bytes[1]) << 8) |
                                   static_cast<uint16_t>(bytes[2]);

            outInsn.operands.push_back({OperandType::Immediate, index});

            outInsn.length = 3;
            return true;
            return true;
        }
        case JVMOperandEncoding::BranchS16: {
            if (bytes.size() < 3) return false;

            const int16_t displacement =
                static_cast<int16_t>((static_cast<uint16_t>(bytes[1]) << 8) |
                                     static_cast<uint16_t>(bytes[2]));

            const uint64_t target =
                address + static_cast<int64_t>(displacement);

            outInsn.operands.push_back({OperandType::BranchTarget, target});

            outInsn.length = 3;
            return true;
        }
        case JVMOperandEncoding::BranchS32: {
            if (bytes.size() < 5) return false;

            const int32_t displacement =
                (static_cast<int32_t>(bytes[1]) << 24) |
                (static_cast<int32_t>(bytes[2]) << 16) |
                (static_cast<int32_t>(bytes[3]) << 8) |
                static_cast<int32_t>(bytes[4]);

            const uint64_t target =
                address + static_cast<int64_t>(displacement);

            outInsn.operands.push_back({OperandType::BranchTarget, target});

            outInsn.length = 5;
            return true;
        }
        case JVMOperandEncoding::TableSwitch: {
            return DecodeTableSwitch(address, bytes, outInsn);
        }
        case JVMOperandEncoding::LookupSwitch: {
            return DecodeLookupSwitch(address, bytes, outInsn);
        }
        case JVMOperandEncoding::Wide: {
            return true;
        }
        case JVMOperandEncoding::IInc: {
            if (bytes.size() < 3) return false;

            const uint8_t localIndex = bytes[1];
            const int8_t amount = static_cast<int8_t>(bytes[2]);

            outInsn.operands.push_back({OperandType::Immediate, localIndex});

            outInsn.operands.push_back(
                {OperandType::Immediate,
                 static_cast<uint64_t>(static_cast<int64_t>(amount))});

            outInsn.length = 3;
            return true;
        }
        default:
            return false;
    }
    return false;
}

bool JVMInstructionDecoder::DecodeLookupSwitch(uint64_t address,
                                               std::span<const uint8_t> bytes,
                                               DecodedInsn& outInsn) {
    if (bytes.empty()) return false;

    const size_t padding = SwitchPadding(address);
    const size_t operandsOffset = 1 + padding;

    // default + npairs = 8 bytes
    if (bytes.size() < operandsOffset + 8) return false;

    size_t offset = operandsOffset;

    const int32_t defaultOffset = ReadS32BE(bytes, offset);
    offset += 4;

    const int32_t npairs = ReadS32BE(bytes, offset);
    offset += 4;

    // JVM requires npairs >= 0.
    if (npairs < 0) return false;

    // Each pair is:
    //
    //   match  (4 bytes)
    //   offset (4 bytes)
    //
    const uint64_t pairCount = static_cast<uint64_t>(npairs);

    if (pairCount > (bytes.size() - offset) / 8) return false;

    const uint64_t defaultTarget =
        address + static_cast<int64_t>(defaultOffset);

    outInsn.operands.push_back({OperandType::BranchTarget, defaultTarget});

    outInsn.operands.push_back(
        {OperandType::Immediate, static_cast<uint64_t>(pairCount)});

    for (uint64_t i = 0; i < pairCount; ++i) {
        const int32_t match = ReadS32BE(bytes, offset);
        offset += 4;

        const int32_t jumpOffset = ReadS32BE(bytes, offset);
        offset += 4;

        const uint64_t target = address + static_cast<int64_t>(jumpOffset);

        // match value
        outInsn.operands.push_back(
            {OperandType::Immediate,
             static_cast<uint64_t>(static_cast<int64_t>(match))});

        // target
        outInsn.operands.push_back({OperandType::BranchTarget, target});
    }

    outInsn.length = static_cast<uint32_t>(offset);

    return true;
}

bool JVMInstructionDecoder::DecodeTableSwitch(uint64_t address,
                                              std::span<const uint8_t> bytes,
                                              DecodedInsn& outInsn) {
    if (bytes.empty()) return false;

    const size_t padding = SwitchPadding(address);
    const size_t operandsOffset = 1 + padding;

    if (bytes.size() < operandsOffset + 12) return false;

    size_t offset = operandsOffset;
    const int32_t defaultOffset = ReadS32BE(bytes, offset);
    offset += 4;

    const int32_t low = ReadS32BE(bytes, offset);
    offset += 4;

    const int32_t high = ReadS32BE(bytes, offset);
    offset += 4;

    // JVM tableswitch requires high >= low.
    if (high < low) return false;

    // Avoid integer overflow / unreasonable allocations.
    const uint64_t count = static_cast<uint64_t>(static_cast<int64_t>(high) -
                                                 static_cast<int64_t>(low)) +
                           1;

    // Each entry is a signed 32-bit offset.
    if (count > (bytes.size() - offset) / 4) return false;

    // Default target.
    const uint64_t defaultTarget =
        address + static_cast<int64_t>(defaultOffset);

    outInsn.operands.push_back({OperandType::BranchTarget, defaultTarget});

    // Store low/high as immediates.
    outInsn.operands.push_back(
        {OperandType::Immediate,
         static_cast<uint64_t>(static_cast<int64_t>(low))});

    outInsn.operands.push_back(
        {OperandType::Immediate,
         static_cast<uint64_t>(static_cast<int64_t>(high))});

    // One branch target for every value from low through high.
    for (uint64_t i = 0; i < count; ++i) {
        const int32_t jumpOffset = ReadS32BE(bytes, offset);
        offset += 4;

        const uint64_t target = address + static_cast<int64_t>(jumpOffset);

        outInsn.operands.push_back({OperandType::BranchTarget, target});
    }

    outInsn.length = static_cast<uint32_t>(offset);

    return true;
}

bool JVMInstructionDecoder::DecodeWide(uint64_t address,
                                       std::span<const uint8_t> bytes,
                                       DecodedInsn& outInsn) {}
}  // namespace TitaniumDecompiler
