#pragma once

#include <stdint.h>

#include <limits>
#include <string>
#include <vector>

#include "Core/Disassembly/DecodedInsn.h"

namespace TitaniumDecompiler {
struct Symbol {
    std::string Name;
    std::string ParentScope;
    uint64_t Address;
    size_t Size;
    bool IsFunction;
};

struct Section {
    std::string Name;
    uint64_t VirtualAddress;
    std::vector<uint8_t> Bytes;
    bool IsExecutable;
};

struct MetadataRow {
    uint32_t Index;
    std::string Kind;
    std::string Details;
    std::string RawRepresentation;
};

struct MetadataTable {
    std::string TableName;
    std::vector<std::string> ColumnHeaders;
    std::vector<MetadataRow> Rows;
};

using InstructionId = uint32_t;
using BlockId = uint32_t;

constexpr InstructionId InvalidInstructionId =
    std::numeric_limits<InstructionId>::max();
constexpr BlockId InvalidBlockId = std::numeric_limits<BlockId>::max();

struct IBasicBlock {
    BlockId id;
    uint64_t startAddr;
    uint64_t endAddr;

    std::vector<InstructionId> instructions;
    std::vector<BlockId> successors;
    std::vector<BlockId> predecessors;
};

struct DecodedFunction {
    std::string name;
    std::string signature;

    std::vector<DecodedInsn> instructions;
    // CFG cfg;
};
}  // namespace TitaniumDecompiler
